//
//  banner.h
//  M-Paradox
//
//  Created by Jonathan Rumion on 5/16/13.
//  Copyright (c) 2013 TAP. All rights reserved.
//
//  A very simple class for loading up a custom banner to be presented at login.

#ifndef __M_Paradox__banner__
#define __M_Paradox__banner__

#include <string>

class Banner {
  
public:
    
    Banner(std::string data_dir) : data_dir(data_dir){};
    std::string print_banner();
    
private:
    
    std::string data_dir;
    
};


#endif /* defined(__M_Paradox__banner__) */
