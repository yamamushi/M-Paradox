//
//  AccountRegistration.cpp
//  M-Paradox
//
//  Created by Jonathan Rumion on 5/21/13.
//  Copyright (c) 2013 TAP. All rights reserved.
//

#include "AccountRegistration.h"
#include "../Accounting/UserDB.h"


std::string AccountRegistration::Register(TCP_Participant* conn, std::string username){
    
    // make sure name is valid
    if( !UserDatabase::IsValidName( username ) ) {
        success = false;
        return std::string("Sorry, that is an invalid username.\r\nPlease enter another username: ");
    }
    
    // make sure name doesn't exist already
    if( UserDatabase::HasUser( username ) ) {
        success = false;
        return std::string("Sorry, that name is already in use.\r\nPlease enter another username: ");
    }
    else{
        UserDatabase::AddUser( conn, username );
        success = true;
        
        return std::string("Thank you for joining us, " + username + "\r\n");
    }
    
}