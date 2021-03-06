//
//  TelnetOptionHandler.h
//  M-Paradox
//
//  Created by Jonathan Rumion on 5/21/13.
//  Copyright (c) 2013 TAP. All rights reserved.
//

#ifndef M_Paradox_TelnetOptionHandler_h
#define M_Paradox_TelnetOptionHandler_h

class TelnetOptionHandler
{
public:
    /**
     * Constructor for the TelnetOptionHandler. Allows defining desired
     * initial setting for local/remote activation of this option and
     * behaviour in case a local/remote activation request for this
     * option is received.
     *
     * @param optcode [in] Option code.
     * @param initlocal [in] if set to true, a WILL is sent upon connection.
     * @param initremote [in] if set to true, a DO is sent upon connection.
     * @param acceptlocal [in] if set to true, any DO request is accepted.
     * @param acceptremote [in] if set to true, any WILL request is accepted.
     */
    TelnetOptionHandler(unsigned char optcode, bool initlocal, bool initremote,
                        bool acceptlocal, bool acceptremote)
    : optionCode(optcode), initialLocal(initlocal),
    initialRemote(initremote), acceptLocal(acceptlocal),
    acceptRemote(acceptremote)
    {
    }
    
    virtual ~TelnetOptionHandler()
    {
    }
    
    virtual std::vector<unsigned char> AnswerSubnegotiation(const char* suboption, unsigned int suboptionLength) = 0;
    virtual std::vector<unsigned char> StartSubnegotiationLocal() = 0;
    virtual std::vector<unsigned char> StartSubnegotiationRemote() = 0;
    
    
    /**
     Option code
     */
    unsigned char optionCode;
    /**
     True if the option should be activated on the local side
     */
    bool initialLocal;
    /**
     True if the option should be activated on the remote side
     */
    bool initialRemote;
    /**
     True if the option should be accepted on the local side
     */
    bool acceptLocal;
    /**
     True if the option should be accepted on the remote side
     */
    bool acceptRemote;
    /**
     True if the option is active on the local side
     */
    bool doFlag;
    /**
     True if the option is active on the remote side
     */
    bool willFlag;
    
    
};

#endif
