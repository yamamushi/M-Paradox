//
//  TCP_Session.cpp
//  The ASCII Project
//
//  Created by Jonathan Rumion on 3/1/13.
//  Copyright (c) 2013 TAP. All rights reserved.
//

#include "Packets.h"
#include "TCP_Session.h"
#include "TCP_Participant.h"
#include "Telnet_Protocol.h"
#include "../utils/FileLogger.h"
#include "../serialization/Boost_Serialization.h"
#include "../Messages/DefaultMessages.h"
#include "../Messages/banner.h"
#include "../Messages/ANSI_Escape.h"
#include "../Messages/IAC.h"
#include "../parsers/ServerConfig.h"
#include "../Server/ServerInit.h"
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <memory>
#include <iostream>
#include <vector>
#include <boost/pool/pool.hpp>




extern FileLogger *fileLogger;

void TCP_Session::start(){
    
    fileLogger->ErrorLog("Client " + getClientIP() + ": Connected");
    // Disable Nagles Algorithm - http://en.wikipedia.org/wiki/Nagle's_algorithm
    tcp_socket.set_option(boost::asio::ip::tcp::no_delay(true));
    // Join our tcp client pool
    
    tcp_pool.join(shared_from_this());

    kickStart();
}


/*
 
 Wrap all of our client cleanup into one function.
 
 */
void TCP_Session::end(){
    
    tcp_pool.leave(shared_from_this());
    fileLogger->ErrorLog("Client " + clientIP + ": Disconnected");
    
}



// Executed at TCP_Session::start()
std::string TCP_Session::getClientIP(){
    
    std::string remoteIP = tcp_socket.remote_endpoint().address().to_string();
    
    clientIP = remoteIP;
    return remoteIP;
    
}


// Start the async cycle
void TCP_Session::kickStart(){
        
    const static unsigned char COMMAND_DO_PACKET[] = {COMMAND_IAC, COMMAND_DO, 0};
    unsigned char* data = (unsigned char*)pool.malloc();
    memcpy(data, COMMAND_DO_PACKET, COMMAND_PACKET_LENGTH);
    unsigned char option = {OPTION_WINDOW_SIZE};
    data[COMMAND_PACKET_LENGTH - 1] = option;
    
    boost::asio::streambuf buffer;
    boost::asio::write(this->tcp_socket, boost::asio::buffer(COMMAND_DO_PACKET));
    boost::asio::read_until(this->tcp_socket, buffer, '\0');
    
    
    
    Telnet_Protocol telnet(512);
    
    telnet.ProcessTelnetProtocol(boost::asio::buffer_cast<const char*>( buffer.data()), sizeof(buffer));
    std::istream str(&buffer);
    std::string s;
    std::getline(str, s);
    std::cout << s << std::endl;
    
    
    
    loginMenu();
    
}


// Parse our Input
void TCP_Session::startSession(const boost::system::error_code& error)
{
    if (!error)
    {
        boost::asio::async_read_until(tcp_socket, raw_line_command, "", boost::bind(&TCP_Session::initMode, shared_from_this(),
                                                                                        boost::asio::placeholders::error ) );
    }
    else
    {
        end();
    }
}


void TCP_Session::readMode(const boost::system::error_code& error)
{
    if (!error)
    {
        boost::asio::async_read_until(tcp_socket, raw_line_command, "\n", boost::bind(&TCP_Session::initMode, shared_from_this(),
                                                                                    boost::asio::placeholders::error ) );
    }
    else
    {
        end();
    }
}

void TCP_Session::initMode(const boost::system::error_code& error)
{
    if(!error)
    {
        std::string modeRequest;
        std::istream inputStream(&raw_line_command);
        
        inputStream >> modeRequest;
        
        fileLogger->ErrorLog("Client " + clientIP + ": INIT - " + modeRequest);
        
        if(modeRequest == "logout" || modeRequest == "quit" || modeRequest == "exit" )
        {
            //tcp_socket.shutdown(tcp_socket.shutdown_both);
            //end();
            tcp_socket.cancel();
            tcp_socket.close();
        }
        if(modeRequest == "clear")
        {
            boost::asio::async_write(this->tcp_socket, boost::asio::buffer(clearScreen_ANS+resetCursor_ANS), boost::bind(&TCP_Session::end, shared_from_this()));
        }
        if(modeRequest == "login")
        {
            startRaw();
        }
        else
        {
            boost::asio::async_write(this->tcp_socket, boost::asio::buffer(""), boost::bind(&TCP_Session::readMode, shared_from_this(), boost::asio::placeholders::error ));
        }
        
    }
    else
    {
        end();
    }
}


void TCP_Session::loginMenu(){
    
    extern std::string rootFSPath;
    Banner banner(rootFSPath);
    // We can put flower_MSG here to show off the "animations" (ansi escape codes)
    boost::asio::async_write(this->tcp_socket, boost::asio::buffer(banner.print_banner() + loginMenu_MSG),
                             boost::bind(&TCP_Session::startSession, shared_from_this(), boost::asio::placeholders::error ));
    
}



void TCP_Session::startRaw(){
    
    /*
     
     For now we're just packing some junk data and sending it over the wire, later on
     We'll assemble a real handshake starting here.
     
     */
    std::string rawTest = "This is a Raw Test";
    
    async_data_write(rawTest, boost::bind(&TCP_Session::end, shared_from_this()));
    
}


/**
 Sends sub negotiation command
 */
void TCP_Session::SendSubnegotiation(std::vector<unsigned char>& subn)
{
    if (!subn.empty())
    {
        const static unsigned char COMMAND_SB_PACKET[] = {COMMAND_IAC, COMMAND_SB};
        const static unsigned char COMMAND_SE_PACKET[] = {COMMAND_IAC, COMMAND_SE};
        size_t length = sizeof(COMMAND_SB_PACKET) + sizeof(COMMAND_SE_PACKET) + subn.size();
        // +2 for COMMAND_SB and COMMAND_SE
        unsigned char* byteresp = (unsigned char*)pool.malloc();
        
        unsigned char* index = byteresp;
        memcpy(index, COMMAND_SB_PACKET, COMMAND_PACKET_LENGTH - 1);
        index += COMMAND_PACKET_LENGTH - 1;
        memcpy(index, &subn[0], subn.size());
        index += subn.size();
        memcpy(index, COMMAND_SE_PACKET, COMMAND_PACKET_LENGTH - 1);
        
        boost::asio::write(this->tcp_socket, boost::asio::buffer(index));
    }
}


