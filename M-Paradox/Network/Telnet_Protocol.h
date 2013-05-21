//
//  Telnet_Protocol.h
//  M-Paradox
//
//  Created by Jonathan Rumion on 5/20/13.
//  Copyright (c) 2013 TAP. All rights reserved.
//

#ifndef __M_Paradox__Telnet_Protocol__
#define __M_Paradox__Telnet_Protocol__

#include "TCP_session.h"
#include "../Messages/IAC.h"
#include "TelnetOptionHandler.h"
#include "TelnetHandlers.h"
#include "Boost_Net_Asio.h"
#include <string>
#include <list>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/pool/pool.hpp>

const int BUFFSIZE = 512;

class Telnet_Protocol {
    
public:
    
    bool WillDoNAWS;
    bool sendSubnegotiation;
    
    Telnet_Protocol(int BUFFSIZE) : currentState(STATE_DATA), pool(BUFFSIZE + 1), // + 1 for '\0'
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
    ~Telnet_Protocol()
    {
        for(int i = 0; i < MAX_OPTION_VALUE + 1; ++i)
        {
            if(optionHandlers[i] != NULL)
            {
                delete optionHandlers[i];
            }
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
    
    unsigned char* COMMAND_WILL_PACKET;
    
private:
    
    
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
                    SendSubnegotiation(true);
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
                    SendSubnegotiation(true);
                }
            }
        }
    }
    
    void SendSubnegotiation(bool toSend){
        sendSubnegotiation = toSend;
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
                //SendDo(option);
            }
            else
            {
                ++doResponse[option];
                //SendDont(option);
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
                //SendDont(option);
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
                    //SendWill(option);
                }
                else
                {
                    ++willResponse[option];
                    //SendWont(option);
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
                //SendWont(option);
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
                //SendSubnegotiation(responseSuboption);
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


#endif /* defined(__M_Paradox__Telnet_Protocol__) */
