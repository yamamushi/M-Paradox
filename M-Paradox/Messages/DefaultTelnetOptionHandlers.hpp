// A modified version of toftlib net source
// Original author at https://code.google.com/p/toftlib/
//

#pragma once

#include "TelnetOptionHandler.h"

namespace toftlib
{

    template<typename Type>
    std::vector<Type> GetEmptyVector()
    {
        static std::vector<Type> empty;

        return empty;
    }

    /**
    A default echo handler. Send DO and accept DO for ECHO option
    */
    class DefaultEchoHandler : public TelnetOptionHandler
    {
    public:
        DefaultEchoHandler() : TelnetOptionHandler(OPTION_ECHO, false, true, true, true)
        {
        }
        
        virtual std::vector<unsigned char> AnswerSubnegotiation(const char* suboption, unsigned int suboptionLength)
        {
            return GetEmptyVector<unsigned char>();
        }

        virtual std::vector<unsigned char> StartSubnegotiationLocal()
        {
            return GetEmptyVector<unsigned char>();
        }

        virtual std::vector<unsigned char> StartSubnegotiationRemote()
        {
            return GetEmptyVector<unsigned char>();
        }
    };

    /**
    A default suppress go ahead handler. Send DO and accept DO for SUPPRESS_GO_AHEAD option
    */
    class SuppressGAOptionHandler : public TelnetOptionHandler
    {
    public:
        SuppressGAOptionHandler() : TelnetOptionHandler(OPTION_SUPPRESS_GO_AHEAD, true, true, true, true)
        {
        }
        virtual std::vector<unsigned char> AnswerSubnegotiation(const char* suboption, unsigned int suboptionLength)
        {
            return GetEmptyVector<unsigned char>();
        }
        virtual std::vector<unsigned char> StartSubnegotiationLocal()
        {
            return GetEmptyVector<unsigned char>();
        }
        virtual std::vector<unsigned char> StartSubnegotiationRemote()
        {
            return GetEmptyVector<unsigned char>();
        }
    };

    /**
    A VT102 terminal type handler. Use terminal type VT102
    */
    class TerminalTypeOptionVT102Handler : public TelnetOptionHandler
    {
    public:
        TerminalTypeOptionVT102Handler() : TelnetOptionHandler(OPTION_TERMINAL_TYPE, true, false, true, false)
        {
        }
        virtual std::vector<unsigned char> AnswerSubnegotiation(const char* suboption, unsigned int suboptionLength)
        {
            
            const static unsigned char COMMAND_IS_PACKET[] = {OPTION_TERMINAL_TYPE, 0};
            const static size_t isPacketLen = sizeof(COMMAND_IS_PACKET);
            const static std::string DEFAULT_TERMINAL_TYPE = "VT102";

            size_t length = isPacketLen + DEFAULT_TERMINAL_TYPE.length();
            std::vector<unsigned char> data(length);
            unsigned char* index = &data[0];
            memcpy(index, COMMAND_IS_PACKET, isPacketLen);
            index += isPacketLen;
            memcpy(index, DEFAULT_TERMINAL_TYPE.c_str(), DEFAULT_TERMINAL_TYPE.length());

            return data;
        }
        virtual std::vector<unsigned char> StartSubnegotiationLocal()
        {
            return GetEmptyVector<unsigned char>();
        }
        virtual std::vector<unsigned char> StartSubnegotiationRemote()
        {
            return GetEmptyVector<unsigned char>();
        }
    };
}
