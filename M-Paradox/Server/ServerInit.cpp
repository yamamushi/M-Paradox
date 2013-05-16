//
//  ServerInit.cpp
//  M-Paradox
//
//  Created by Jonathan Rumion on 5/16/13.
//  Copyright (c) 2013 TAP. All rights reserved.
//

#include <locale>
#include "ServerInit.h"
#include "../utils/FileLogger.h"
#include "../utils/Filesystem.h"
#include "../parsers/ServerConfig.h"

#include "../Network/Boost_Net_Asio.h"
#include "../Network/TCP_Handler.h"
#include "../utils/ConsoleLog.h"
#include "../utils/FileLogger.h"
#include "../utils/Filesystem.h"
#include "../parsers/ServerConfig.h"
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

FileLogger *fileLogger;

void ServerInit(int argc, char *argv[]){
    
    setlocale(LC_ALL,"");
    
    std::shared_ptr<ServerConfigParser> serverConfig(new ServerConfigParser( argc, argv));
    serverConfig->Parse();
    
    fileLogger = new FileLogger(serverConfig->data_dir);
    fileLogger->ErrorLog("Server Startup Initiated");
    fileLogger->ErrorLog("Server Configuration Read");

    DirectoryInit(serverConfig->data_dir);

    fileLogger->ErrorLog("Server Setup Complete");
    

}


void DirectoryInit(std::string rootFSPath){
    
    fileLogger->ErrorLog("Checking Filesystem Paths");
    
    if(!FileSystem::CheckExists(rootFSPath)){
        FileSystem::MkDir(rootFSPath);
    }
    if(!FileSystem::CheckExists(rootFSPath + "/ents")){
        FileSystem::MkDir(rootFSPath + "/ents");
    }
    if(!FileSystem::CheckExists(rootFSPath + "/logs")){
        FileSystem::MkDir(rootFSPath + "/logs");
    }
    if(!FileSystem::CheckExists(rootFSPath + "/maps")){
        FileSystem::MkDir(rootFSPath + "/maps");
    }
    if(!FileSystem::CheckExists(rootFSPath + "/maps/tilemaps")){
        FileSystem::MkDir(rootFSPath + "/maps/tilemaps");
    }
    if(!FileSystem::CheckExists(rootFSPath + "/players")){
        FileSystem::MkDir(rootFSPath + "/players");
    }
    if(!FileSystem::CheckExists(rootFSPath + "/scripts")){
        FileSystem::MkDir(rootFSPath + "/scripts");
    }
    
    fileLogger->ErrorLog("Filesystem Paths Verified");
    
    return;
    
}



void NetInit(int port){
    
    try {
        fileLogger->ErrorLog("Attempting to Start Network Module on Port: " + boost::lexical_cast<std::string>(port));
        boost::asio::io_service io_service;
        boost::asio::signal_set signals(io_service, SIGINT, SIGTERM);
        signals.async_wait(boost::bind(&boost::asio::io_service::stop, &io_service));
        tcp_handler_ptr tcp_server(new TCP_Handler(io_service, port));
        fileLogger->ErrorLog("Network Module Started and Listening on Port: " + boost::lexical_cast<std::string>(port));
        ConsoleLog::Print("Server is now Listening on Port: " + boost::lexical_cast<std::string>(port));
        io_service.run();
    }
    catch(std::exception& e){
        
        std::cerr << "Exception: " << e.what() << std::endl;
        
    }
    
}
