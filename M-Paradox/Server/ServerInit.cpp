//
//  ServerInit.cpp
//  M-Paradox
//
//  Created by Jonathan Rumion on 5/16/13.
//  Copyright (c) 2013 TAP. All rights reserved.
//

#include <locale>
#include <thread>
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
#include "../Messages/banner.h"
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

FileLogger *fileLogger;
std::string rootFSPath;

void ServerInit(int argc, char *argv[]){
    
    setlocale(LC_ALL,"");
    
    std::shared_ptr<ServerConfigParser> serverConfig(new ServerConfigParser( argc, argv));
    serverConfig->Parse();
    
    rootFSPath = serverConfig->data_dir;
    
    fileLogger = new FileLogger(serverConfig->data_dir);
    fileLogger->ErrorLog("Server Startup Initiated");
    ConsoleLog::Print("Server Startup Initiated");
    fileLogger->ErrorLog("Server Configuration Read");
    ConsoleLog::Print("Server Configuration Read");
    
    if(DirectoryInit(serverConfig->data_dir)){
        fileLogger->ErrorLog("Filesystem Directories Created");
        fileLogger->ErrorLog("However they require propagation before server startup can continue");
        fileLogger->ErrorLog("Server Shutting Down");
        ConsoleLog::Print("Filesystem Directories Created");
        ConsoleLog::Print("However they require propagation before server startup can continue");
        ConsoleLog::Print("Server Shutting Down");
        exit(0);
    }
    
    // Here we're going to put a check to make sure a valid internal config is present
    // We will handle missing config files as the server is running.

    fileLogger->ErrorLog("Server Configuration Setup Complete");
    ConsoleLog::Print("Server Configuration Setup Complete");
    
    std::thread tcp_listener(NetInit, serverConfig->server_port);
    
    fileLogger->ErrorLog("Server is Now Running, Congratulations!");
    ConsoleLog::Print("Server is Now Running");


    tcp_listener.join();

    fileLogger->ErrorLog("Server is Now Shutting Down");
    ConsoleLog::Print("Server is Now Shutting Down");

    exit(0);

}


bool DirectoryInit(std::string rootFSPath){
    
    fileLogger->ErrorLog("Checking Filesystem Paths");
    ConsoleLog::Print("Checking Filesystem Paths");
    
    bool NeedsPropagation = false;
    
    if(!FileSystem::CheckExists(rootFSPath)){
        FileSystem::MkDir(rootFSPath);
        fileLogger->ErrorLog("RootFS Directory Created");
        ConsoleLog::Print("RootFS Directory Created");
    }
    if(!FileSystem::CheckExists(rootFSPath + "/ents")){
        FileSystem::MkDir(rootFSPath + "/ents");
        fileLogger->ErrorLog("Entity (Ents) Directory Created");
        ConsoleLog::Print("Entity (Ents) Directory Created");
    }
    if(!FileSystem::CheckExists(rootFSPath + "/logs")){
        FileSystem::MkDir(rootFSPath + "/logs");
        fileLogger->ErrorLog("Logs Directory Created");
        ConsoleLog::Print("Logs Directory Created");
    }
    if(!FileSystem::CheckExists(rootFSPath + "/maps")){
        FileSystem::MkDir(rootFSPath + "/maps");
        fileLogger->ErrorLog("Maps Directory Created");
        ConsoleLog::Print("Maps Directory Created");
        NeedsPropagation = true;
    }
    if(!FileSystem::CheckExists(rootFSPath + "/players")){
        FileSystem::MkDir(rootFSPath + "/players");
        fileLogger->ErrorLog("Players Directory Created");
        ConsoleLog::Print("Players Directory Created");
    }
    if(!FileSystem::CheckExists(rootFSPath + "/scripts")){
        FileSystem::MkDir(rootFSPath + "/scripts");
        fileLogger->ErrorLog("Scripts Directory Created");
        ConsoleLog::Print("Scripts Directory Created");
        NeedsPropagation = true;
    }
    if(!FileSystem::CheckExists(rootFSPath + "/config")){
        FileSystem::MkDir(rootFSPath + "/config");
        fileLogger->ErrorLog("Config Directory Created");
        ConsoleLog::Print("Config Directory Created");
        NeedsPropagation = true;
    }
    
    if(!NeedsPropagation)
        fileLogger->ErrorLog("Filesystem Paths Verified");
    
    return NeedsPropagation;
    
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
