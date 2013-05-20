// A modified version of toftlib net source
// Original author at https://code.google.com/p/toftlib/
//

#pragma once

#include "TelnetDefines.hpp"
#include "TelnetOptionHandler.h"
#include "DefaultTelnetOptionHandlers.hpp"
#include <string>
#include <list>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/pool/pool.hpp>

namespace toftlib
{
    template<unsigned int BUFFSIZE>
    class TelnetClient : public AsyncTcpClient
    {
    public:
        TelnetClient() : currentState(STATE_DATA), pool(BUFFSIZE + 1), // + 1 for '\0'
            asyncError(false), sendComplete(false)
        {
            memset(doResponse, 0, MAX_OPTION_VALUE + 1);
            memset(willResponse, 0, MAX_OPTION_VALUE + 1);
            memset(options, 0, MAX_OPTION_VALUE + 1);
            memset(optionHandlers, 0, sizeof(TelnetOptionHandler*) * (MAX_OPTION_VALUE + 1));

            // Set some default option handlers
            optionHandlers[OPTION_ECHO] = new DefaultEchoHandler();
            optionHandlers[OPTION_SUPPRESS_GO_AHEAD] = new SuppressGAOptionHandler();
            optionHandlers[OPTION_TERMINAL_TYPE] = new TerminalTypeOptionVT102Handler();
        }
        /**
        Deconstructor. Will release all the option handler registered.
        */
        ~TelnetClient()
        {
            for(int i = 0; i < MAX_OPTION_VALUE + 1; ++i)
            {
                if(optionHandlers[i] != NULL)
                {
                    delete optionHandlers[i];
                }
            }
        }

        /**
        Add a TelnetOptionHandler handler. If the optionCode member in the optionHandler
        is larger than MAX_OPTION_VALUE, will return false. If false returned, caller must
        release the optionHandler object. If true returned, do not release this object.
        
        @param optionHandler [in] A pointer to a TelnetOptionHandler object
        @return if add successed
        */
        bool AddOptionHandler(TelnetOptionHandler* optionHandler)
        {
            unsigned char optionCode = optionHandler->optionCode;
            if(optionCode > MAX_OPTION_VALUE)
            {
                return false;
            }
            if(optionHandlers[optionCode] != NULL)
            {
                delete optionHandlers[optionCode];
            }
            optionHandlers[optionCode] = optionHandler;

            return true;
        }

        /**
        Connect to telent server

        @param destination [in] Address of telnet server
        @param port [in] Port for telnet service on the server
        */
        virtual void Connect(const char* destination, const unsigned int port)
        {
            this->AsyncTcpClient::Connect(destination, port);

            /* Clear option and open TelnetOptionHandler functionality on a new connection */
            for (int i = 0; i < MAX_OPTION_VALUE + 1; ++i)
            {
                doResponse[i] = 0;
                willResponse[i] = 0;
                options[i] = 0;
                if (optionHandlers[i] != NULL)
                {
                    optionHandlers[i]->doFlag = false;
                    optionHandlers[i]->willFlag = false;

                    if(optionHandlers[i]->initialLocal)
                    {
                        RequestWill(optionHandlers[i]->optionCode);
                    }

                    if (optionHandlers[i]->initialRemote)
                    {
                        RequestDo(optionHandlers[i]->optionCode);
                    }
                }
            }               
        }
        
        /**
        Read until a given string is encountered or until timeout seconds have passed.

        @param target [in] expected string
        @param timeout [in] timeout seconds, with a default value 0. If this value 
        is less than or equal to 0, will wait infinitely
        */
        std::string ReadUntil(const std::string& target, const unsigned int timeout = 0)
        {
            try
            {
                std::string response = "";
                char* data = NULL;
                if(timeout <= 0)
                {
                    while(true)
                    {
                        size_t readSize = boostSocket.read_some(boost::asio::buffer(&syncBuff[0], BUFFSIZE));

                        ProcessTelnetProtocol(&syncBuff[0], readSize, &data);

                        if(NULL != data)
                        {
                            response += data;
                            pool.free(data);

                            if(response.find(target) != std::string::npos)
                            {
                                break;
                            }
                        }

                        Sleep(10);

                    }

                    return response;
                }
                else
                {
                    asyncError = false;
                    PostNextRead();
                    unsigned int timeCounter = 0;
                    unsigned int timeInMillionSecs = timeout * 1000;

                    while(true)
                    {
                        if(asyncError)
                        {
                            BOOST_THROW_EXCEPTION( toftlib::NetException(lastError) );
                        }
                        mutex.lock();
                        if(!responseList.empty())
                        {
                            response += responseList.front();
                            responseList.pop_front();

                            if(response.find(target) != std::string::npos)
                            {
                                mutex.unlock();
                                break;
                            }

                            Sleep(10);
                            timeCounter += 10;
                            if(timeCounter >= timeInMillionSecs)
                            {
                                mutex.unlock();
                                break;
                            }
                        }
                        mutex.unlock();
                    }

                    return response;
                }
            }
            catch(boost::system::system_error& se)
            {
                BOOST_THROW_EXCEPTION( toftlib::NetException(se) );

                return "";
            }
        }

        /**
        Send data to telnet server.

        @param target [in] expected string
        @param timeout [in] timeout seconds, with a default value 0. If this value 
        is less than or equal to 0, will wait infinitely
        */
        void SendData(const char* data, const unsigned int timeout = 0)
        {
            try
            {
                if(timeout <= 0)
                {
                    boost::asio::write(boostSocket, boost::asio::buffer(data, strlen(data)));
                }
                else
                {
                    asyncError = false;
                    sendComplete = false;
                    unsigned int timeCounter = 0;
                    unsigned int timeInMillionSecs = timeout * 1000;
                    boost::asio::async_write(boostSocket,
                            boost::asio::buffer(data, strlen(data)),
                            boost::bind(&TelnetClient::HandleDataWrite, this, boost::asio::placeholders::error));
                    while(true)
                    {
                        if(sendComplete)
                        {
                            break;
                        }
                        if(asyncError)
                        {
                            BOOST_THROW_EXCEPTION( toftlib::NetException(lastError) );
                        }

                        Sleep(10);
                        timeCounter += 10;
                        if(timeCounter >= timeInMillionSecs)
                        {
                            break;
                        }
                    }
                }   
            }
            catch(boost::system::system_error& se)
            {
                BOOST_THROW_EXCEPTION( toftlib::NetException(se) );
            }
        }

        void Disconnect()
        {
            CloseClient();
        }

    private:
        /**
        Handle function for our command write request, will release the memory for output data
        @param error [in] error code object
        @param buff [in] pointer to output memory
        */
        void HandleCommandWrite(const boost::system::error_code& error, void* buff)
        {
            pool.free(buff);
        }

        void HandleDataWrite(const boost::system::error_code& error)
        {
            sendComplete = true;
        }

        void HandleRead(const boost::system::error_code& error, std::size_t bytes_transferred)
        {
            char* data = NULL;
            if(!error)
            {
                ProcessTelnetProtocol(&buff[0], bytes_transferred, &data);

                if(NULL != data)
                {
                    mutex.lock();
                    responseList.push_back(std::string(data));
                    mutex.unlock();
                    pool.free(data);
                }

                return;
            }
            else
            {
                lastError = error;
                asyncError = true;
                CloseClient();

                return;
            }
        }

        void ProcessTelnetProtocol(unsigned char* buffer, size_t length, char** telnetData)
        {
            char* data = NULL;
            unsigned int dataLen = 0;
            // +1 for '\0'
            data = (char*)pool.malloc();
            for(unsigned int currentPos = 0; currentPos < length; ++currentPos)
            {
                unsigned char readChar = buffer[currentPos];

                switch(currentState)
                {
                case STATE_CR:
                    if (readChar == '\0')
                    {
                        // Strip null
                        continue;
                    }
                case STATE_DATA:
                    if(readChar == COMMAND_IAC)
                    {
                        currentState = STATE_IAC;

                        continue;
                    }
                    if (readChar == '\r')
                    {
                        if(RequestedDont(OPTION_BINARY))
                        {
                            currentState = STATE_CR;
                        }
                        else
                        {
                            currentState = STATE_DATA;
                        }

                    }
                    else
                    {
                        currentState = STATE_DATA;
                    }
                    break;
                case STATE_IAC:
                    switch (readChar)
                    {
                    case COMMAND_WILL:
                        currentState = STATE_WILL;
                        continue;
                    case COMMAND_WONT:
                        currentState = STATE_WONT;
                        continue;
                    case COMMAND_DO:
                        currentState = STATE_DO;
                        continue;
                    case COMMAND_DONT:
                        currentState = STATE_DONT;
                        continue;
                    case COMMAND_SB:
                        suboptionCount = 0;
                        currentState = STATE_SB;
                        continue;
                    case COMMAND_IAC:
                        currentState = STATE_DATA;
                        break;
                    default:
                        break;
                    }
                    currentState = STATE_DATA;
                    continue;
                case STATE_WILL:
                    ProcessWill(readChar);
                    currentState = STATE_DATA;
                    continue;
                case STATE_WONT:
                    ProcessWont(readChar);
                    currentState = STATE_DATA;
                    continue;
                case STATE_DO:
                    ProcessDo(readChar);
                    currentState = STATE_DATA;
                    continue;
                case STATE_DONT:
                    ProcessDont(readChar);
                    currentState = STATE_DATA;
                    continue;
                case STATE_SB:
                    switch (readChar)
                    {
                    case COMMAND_IAC:
                        currentState = STATE_IAC_SB;
                        continue;
                    default:
                        // store suboption char
                        suboptions[suboptionCount++] = readChar;
                        break;
                    }

                    currentState = STATE_SB;
                    continue;
                case STATE_IAC_SB:
                    switch (readChar)
                    {
                    case COMMAND_SE:
                        ProcessSuboption(&suboptions[0], suboptionCount);
                        currentState = STATE_DATA;
                        continue;
                    default:
                        currentState = STATE_SB;
                        break;
                    }
                    currentState = STATE_DATA;
                    continue;
                }

                //Get data char
                data[dataLen++] = readChar;
            }

            if(0 == dataLen)
            {
                pool.free(data);
                *telnetData = NULL;
            }
            else
            {
                data[dataLen] = '\0';
                *telnetData = data;
            }
        }

        void PostNextRead()
        {
            boostSocket.async_read_some(boost::asio::buffer(&buff[0], BUFFSIZE),
                boost::bind(&TelnetClient::HandleRead, this, 
                    boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        }

        void PostCommandWrite(unsigned char* data, size_t length)
        {
            boost::asio::async_write(boostSocket,
                boost::asio::buffer(data, length),
                boost::bind(&TelnetClient::HandleCommandWrite, this, boost::asio::placeholders::error, data));
        }

        void RequestWill(unsigned char option)
        {
            if ((willResponse[option] == 0 && StateIsWill(option))
                || RequestedWill(option))
            {
                return ;
            }

            SetWantWill(option);
            ++doResponse[option];
            SendWill(option);
        }

        void RequestDo(unsigned char option)
        {
            if ((doResponse[option] == 0 && StateIsDo(option))
                || RequestedDo(option))
            {
                return ;
            }

            SetWantDo(option);
            ++doResponse[option];
            SendDo(option);
        }


        bool RequestedDont(unsigned char option)
        {
            return !RequestedDo(option);
        }

        /**
        * Looks for the state of the option.
        * @param option [in] option code to be looked up.
        * @return returns true if a will has been reuqested
        */
        bool RequestedWill(unsigned char option)
        {
            return ((options[option] & REQUESTED_WILL_MASK) != 0);
        }

        /**
        * Looks for the state of the option.
        * @param option [in] option code to be looked up.
        * @return returns true if a wont has been reuqested
        * 
        */
        bool RequestedWont(unsigned char option)
        {
            return !RequestedWill(option);
        }

        /***
        * Looks for the state of the option.
        * @param option [in] option code to be looked up.
        * @return returns true if a do has been reuqested
        */
        bool RequestedDo(unsigned char option)
        {
            return ((options[option] & REQUESTED_DO_MASK) != 0);
        }

        bool StateIsDo(unsigned char option)
        {
            return ((options[option] & DO_MASK) != 0);
        }

        bool StateIsDont(unsigned char option)
        {
            return !StateIsDo(option);
        }

        bool StateIsWill(unsigned char option)
        {
            return ((options[option] & WILL_MASK) != 0);
        }

        bool StateIsWont(unsigned char option)
        {
            return !StateIsWill(option);
        }

        /**
        * Sets the state of the option to WANT DO.
        * @param option [in] option code to be set.
        */
        void SetWantDo(unsigned char option)
        {
            options[option] |= REQUESTED_DO_MASK;
        }

        /**
        * Sets the state of the option to WANT DONT.
        * @param option [in] option code to be set.
        */
        void SetWantDont(unsigned char option)
        {
            options[option] &= ~REQUESTED_DO_MASK;
        }


        /**
        * Sets the state of the option to DO.
        * 
        * @param option [in] option code to be set.
        */
        void SetDo(unsigned char option)
        {
            options[option] |= DO_MASK;

            if (RequestedDo(option))
            {
                if (optionHandlers[option] != NULL)
                {
                    optionHandlers[option]->doFlag = true;

                    std::vector<unsigned char> subneg =
                        optionHandlers[option]->StartSubnegotiationRemote();

                    if (!subneg.empty())
                    {
                        SendSubnegotiation(subneg);
                    }
                }
            }
        }

        /**
        * Sets the  WILL state of the option.
        * 
        * @param option [in] option code to be set.
        */
        void SetWill(unsigned char option)
        {
            options[option] |= WILL_MASK;

            if (RequestedWill(option))
            {
                if (optionHandlers[option] != NULL)
                {
                    optionHandlers[option]->willFlag = true;

                    std::vector<unsigned char> subneg =
                        optionHandlers[option]->StartSubnegotiationLocal();

                    if (!subneg.empty())
                    {
                        SendSubnegotiation(subneg);
                    }
                }
            }
        }

        /**
        * Sets the state of the option to WONT.
        * 
        * @param option [in] option code to be set.
        */
        void SetWont(unsigned char option)
        {
            options[option] &= ~WILL_MASK;

            if (optionHandlers[option] != NULL)
            {
                optionHandlers[option]->willFlag = false;
            }
        }

        /**
        * Sets the state of the option to DONT.
        * 
        * @param option [in] option code to be set.
        */
        void SetDont(unsigned char option)
        {
            options[option] &= ~DO_MASK;

            if (optionHandlers[option] != NULL)
            {
                optionHandlers[option]->doFlag = false;
            }
        }

        /**
        * Sets the state of the option to WANT WILL.
        * @param option [in] option code to be set.
        */
        void SetWantWill(unsigned char option)
        {
            options[option] |= REQUESTED_WILL_MASK;
        }

        /**
        * Sets the state of the option to WANT WONT.
        * @param option [in] option code to be set.
        */
        void SetWantWont(unsigned char option)
        {
            options[option] &= ~REQUESTED_WILL_MASK;
        }

        /**
        * Sends a DO command.
        *
        * @param option [in] Option code.
        **/
        void SendDo(unsigned char option)
        {
            const static unsigned char COMMAND_DO_PACKET[] = {COMMAND_IAC, COMMAND_DO, 0};
            unsigned char* data = (unsigned char*)pool.malloc();
            memcpy(data, COMMAND_DO_PACKET, COMMAND_PACKET_LENGTH);
            data[COMMAND_PACKET_LENGTH - 1] = option;
            PostCommandWrite(data, COMMAND_PACKET_LENGTH);
        }

        /**
        * Sends a DONT command.
        *
        * @param option [in] Option code.
        **/
        void SendDont(unsigned char option)
        {
            const static unsigned char COMMAND_DONT_PACKET[] = {COMMAND_IAC, COMMAND_DONT, 0};


            unsigned char* data = (unsigned char*)pool.malloc();
            memcpy(data, COMMAND_DONT_PACKET, COMMAND_PACKET_LENGTH);
            data[COMMAND_PACKET_LENGTH - 1] = option;
            PostCommandWrite(data, COMMAND_PACKET_LENGTH);
        }

        /**
        * Sends a WILL command.
        *
        * @param option [in] Option code.
        **/
        void SendWill(unsigned char option)
        {
            const static unsigned char COMMAND_WILL_PACKET[] = {COMMAND_IAC, COMMAND_WILL, 0};

            unsigned char* data = (unsigned char*)pool.malloc();
            memcpy(data, COMMAND_WILL_PACKET, COMMAND_PACKET_LENGTH);
            data[COMMAND_PACKET_LENGTH - 1] = option;
            PostCommandWrite(data, COMMAND_PACKET_LENGTH);
        }

        /**
        * Sends a WONT command.
        *
        * @param option [in] Option code.
        **/
        void SendWont(unsigned char option)
        {
            const static unsigned char COMMAND_WONT_PACKET[] = {COMMAND_IAC, COMMAND_WONT, 0};
            unsigned char* data = (unsigned char*)pool.malloc();
            memcpy(data, COMMAND_WONT_PACKET, COMMAND_PACKET_LENGTH);
            data[COMMAND_PACKET_LENGTH - 1] = option;
            PostCommandWrite(data, COMMAND_PACKET_LENGTH);
        }

        /**
        Sends sub negotiation command
        */
        void SendSubnegotiation(std::vector<unsigned char>& subn)
        {
            if (!subn.empty())
            {
                const static unsigned char COMMAND_SB_PACKET[] = {COMMAND_IAC, COMMAND_SB};
                const static unsigned char COMMAND_SE_PACKET[] = {COMMAND_IAC, COMMAND_SE};
                size_t length = sizeof(COMMAND_SB_PACKET) + sizeof(COMMAND_SE_PACKET) + subn.size();
                // +2 for COMMAND_SB and COMMAND_SE
                unsigned char* byteresp = (unsigned char*)pool.malloc();

                unsigned char* index = byteresp;
                memcpy(index, COMMAND_SB_PACKET, COMMAND_PACKET_LENGTH - 1);
                index += COMMAND_PACKET_LENGTH - 1;
                memcpy(index, &subn[0], subn.size());
                index += subn.size();
                memcpy(index, COMMAND_SE_PACKET, COMMAND_PACKET_LENGTH - 1);

                PostCommandWrite(byteresp, length);
            }
        }

        /**
        Process the command char come after WILL command
        @param option [in] command char
        */
        void ProcessWill(unsigned char option)
        {
            bool acceptNewState = false;
            if (optionHandlers[option] != NULL)
            {
                acceptNewState = optionHandlers[option]->acceptRemote;
            }
            
            if (doResponse[option] > 0)
            {
                --doResponse[option];
                if (doResponse[option] > 0 && StateIsDo(option))
                {
                    --doResponse[option];
                }
            }

            if (doResponse[option] == 0 && RequestedDont(option))
            {
                if (acceptNewState)
                {
                    SetWantDo(option);
                    SendDo(option);
                }
                else
                {
                    ++doResponse[option];
                    SendDont(option);
                }
            }

            SetDo(option);
        }

        /**
        * Processes a WONT request.
        * 
        * @param option [in] option code to be set.
        */
        void ProcessWont(unsigned char option)
        {

            if (doResponse[option] > 0)
            {
                --doResponse[option];
                if (doResponse[option] > 0 && StateIsDont(option))
                {
                    --doResponse[option];
                }
            }

            if (doResponse[option] == 0 && RequestedDo(option))
            {
                if ((StateIsDo(option)) || (RequestedDo(option)))
                {
                    SendDont(option);
                }

                SetWantDont(option);
            }

            SetDont(option);
        }

        /**
        * Processes a DO request.
        * 
        * @param option [in] option code to be set.
        **/
        void ProcessDo(unsigned char option)
        {

            bool acceptNewState = false;
            /* open TelnetOptionHandler functionality (start)*/
            if (optionHandlers[option] != NULL)
            {
                acceptNewState = optionHandlers[option]->acceptLocal;
            }
            else
            {
                /* TERMINAL-TYPE option (start)*/
                if (option == OPTION_TERMINAL_TYPE)
                {
                    acceptNewState = true;
                }
                /* TERMINAL-TYPE option (end)*/
            }

            if (willResponse[option] > 0)
            {
                --willResponse[option];
                if (willResponse[option] > 0 && StateIsWill(option))
                {
                    --willResponse[option];
                }
            }

            if (willResponse[option] == 0)
            {
                if (RequestedWont(option))
                {

                    if (acceptNewState)
                    {
                        SetWantWill(option);
                        SendWill(option);
                    }
                    else
                    {
                        ++willResponse[option];
                        SendWont(option);
                    }
                }
            }

            SetWill(option);

        }

        /**
        * Processes a DONT request.
        * 
        * @param option [in] option code to be set.
        **/
        void ProcessDont(unsigned char option)
        {
            if (willResponse[option] > 0)
            {
                --willResponse[option];
                if (willResponse[option] > 0 && StateIsWont(option))
                {
                    --willResponse[option];
                }
            }

            if (willResponse[option] == 0 && RequestedWill(option))
            {

                if ((StateIsWill(option)) || (RequestedWill(option)))
                {
                    SendWont(option);
                }

                SetWantWont(option);
            }

            SetWont(option);
        }

        /**
        * Processes a suboption negotiation.
        * 
        * @param suboption [in] subnegotiation data received
        * @param suboptionLength [in] length of data received
        **/
        void ProcessSuboption(const char* suboption, int suboptionLength)
        {
            if (suboptionLength > 0)
            {
                if (optionHandlers[suboption[0]] != NULL)
                {
                    std::vector<unsigned char> responseSuboption =
                        optionHandlers[suboption[0]]->AnswerSubnegotiation(suboption,
                        suboptionLength);
                    SendSubnegotiation(responseSuboption);
                }
            }
        }

        unsigned char buff[BUFFSIZE];
        unsigned char syncBuff[BUFFSIZE];
        unsigned char suboptionCount;
        std::list< std::string > responseList;
        bool sendComplete;
        bool asyncError;
        boost::pool<> pool;

        TELNET_STATUS currentState;

        /**
        Mutex object to pretend telnet response buffer
        */
        boost::mutex mutex;

        char doResponse[MAX_OPTION_VALUE + 1];
        char willResponse[MAX_OPTION_VALUE + 1];
        char options[MAX_OPTION_VALUE + 1];
        char suboptions[MAX_OPTION_VALUE + 1];
        TelnetOptionHandler* optionHandlers[MAX_OPTION_VALUE + 1];

        enum COMMAND_MASKS
        {
            WILL_MASK = 1,
            DO_MASK = 2,
            REQUESTED_WILL_MASK = 4,
            REQUESTED_DO_MASK = 8
        };

        enum BUFF_LENGTH
        {
            COMMAND_PACKET_LENGTH = 3
        };
        
    };

}
