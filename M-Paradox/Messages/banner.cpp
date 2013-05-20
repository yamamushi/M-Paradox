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
#include "DefaultMessages.h"
#include "banner.h"


std::string Banner::print_banner(){
    
    std::ifstream file( data_dir+"/config/banner.txt" );
    
    if ( file )
    {
        std::stringstream buffer;
        
        buffer << file.rdbuf();
        
        file.close();
        
        return buffer.str();
        
    }
    else {
        return defaultBanner_MSG;
    }
    
}