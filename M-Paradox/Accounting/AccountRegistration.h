//
//  AccountRegistration.h
//  M-Paradox
//
//  Created by Jonathan Rumion on 5/21/13.
//  Copyright (c) 2013 TAP. All rights reserved.
//

#ifndef __M_Paradox__AccountRegistration__
#define __M_Paradox__AccountRegistration__

#include "../Network/TCP_Participant.h"

class AccountRegistration {
    
public:
    
    std::string Register(TCP_Participant* conn, std::string username);
    bool checkRegistrationStatus(){return success;};
    
private:
    
    bool success = false;
    
};

#endif /* defined(__M_Paradox__AccountRegistration__) */
