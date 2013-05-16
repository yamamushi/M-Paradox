//
//  ServerInit.h
//  M-Paradox
//
//  Created by Jonathan Rumion on 5/16/13.
//  Copyright (c) 2013 TAP. All rights reserved.
//

#ifndef __M_Paradox__ServerInit__
#define __M_Paradox__ServerInit__

#include <string>

void ServerInit(int argc, char *argv[]);
void DirectoryInit(std::string rootFSPath);
void NetInit(int port);

#endif /* defined(__M_Paradox__ServerInit__) */
