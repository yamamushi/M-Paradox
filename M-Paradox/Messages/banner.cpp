//
//  banner.cpp
//  M-Paradox
//
//  Created by Jonathan Rumion on 5/16/13.
//  Copyright (c) 2013 TAP. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "banner.h"


std::string Banner::print_banner(){
    
    std::ifstream file( data_dir+"/config/banner.txt" );
    
    if ( file )
    {
        std::stringstream buffer;
        
        buffer << file.rdbuf();
        
        file.close();
        
        //std::cout << buffer.str() << std::endl;
        return buffer.str();
        
    }
    else {
        return "Server is missing a banner, please contact the Server Administrator about this issue";
    }
    
}