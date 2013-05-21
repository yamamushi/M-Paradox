//
//  TCP_Session.cpp
//  The ASCII Project
//
//  Created by Jonathan Rumion on 3/1/13.
//  Copyright (c) 2013 TAP. All rights reserved.
//

#include "TCP_Session.h"
#include "TCP_Participant.h"
#include "../Accounting/AccountRegistration.h"
#include "../Accounting/UserDB.h"
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
#include <chrono>
#include <thread>




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
    tcp_socket.cancel();
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
    
    extern std::string rootFSPath;
    Banner banner(rootFSPath);
    
    screen = banner.print_banner() + loginMenu_MSG;
    boost::asio::async_write(this->tcp_socket, boost::asio::buffer(screen),
                             boost::bind(&TCP_Session::startSession, shared_from_this(), boost::asio::placeholders::error ));
    
}


// Parse our Input
void TCP_Session::startSession(const boost::system::error_code& error)
{
    if (!error)
    {
        boost::asio::async_read_until(tcp_socket, raw_line_command, "\r\n", boost::bind(&TCP_Session::initMode, shared_from_this(),
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
        
        std::transform(modeRequest.begin(), modeRequest.end(), modeRequest.begin(), ::tolower);
        
        fileLogger->ErrorLog("Client " + clientIP + ": INIT - " + modeRequest);
        
        if(modeRequest == "quit" || modeRequest == "exit" || modeRequest == "q" )
        {
            tcp_socket.shutdown(tcp_socket.shutdown_both);
            //tcp_socket.cancel();
            end();
        }
        // This should pass to a function that will check registration and add the user to the UserDB pool accordingly.
        if(modeRequest == "login" || modeRequest == "l")
        {
            boost::asio::async_write(this->tcp_socket, boost::asio::buffer("Logins are not working at this time.\r\n"), boost::bind(&TCP_Session::startSession, shared_from_this(), boost::asio::placeholders::error ));
        }
        if(modeRequest == "register" || modeRequest == "r" )
        {
            boost::asio::async_write(this->tcp_socket, boost::asio::buffer("Username: "), boost::bind(&TCP_Session::registration, shared_from_this(), boost::asio::placeholders::error ));
            
        }
        else
        {
            extern std::string rootFSPath;
            Banner banner(rootFSPath);
            if(UserDatabase::HasUser(username))
                screen = UserDatabase::ReturnScreen(this);
            boost::asio::async_write(this->tcp_socket, boost::asio::buffer(screen), boost::bind(&TCP_Session::startSession, shared_from_this(), boost::asio::placeholders::error ));
        }
        
    }
    else
    {
        end();
    }
}

void TCP_Session::registration(const boost::system::error_code& error)
{
    if (!error)
    {
        bool pending = true;
        std::string output;
        std::string usernameAttempt;
        AccountRegistration accountRegistration;
        while(pending){
            boost::asio::streambuf buffer;
            boost::asio::read_until(this->tcp_socket, buffer, "\r\n");
            std::istream str(&buffer);
            str >> usernameAttempt;
            output = accountRegistration.Register( this, usernameAttempt);
            if(accountRegistration.checkRegistrationStatus()){
                pending = false;
            }
            else{
                boost::asio::write(this->tcp_socket, boost::asio::buffer(output));
            }
        }
        
        username = usernameAttempt;
        boost::asio::async_write(this->tcp_socket, boost::asio::buffer(output), boost::bind(&TCP_Session::startSession, shared_from_this(), boost::asio::placeholders::error ));
        //std::chrono::milliseconds dura( 2000 );
        //std::this_thread::sleep_for(dura);
    }
    else
    {
        end();
    }
}


void TCP_Session::startRaw(){
    
    /*
     
     For now we're just packing some junk data and sending it over the wire, later on
     We'll assemble a real handshake starting here.
     
     */
    std::string rawTest = "This is a Raw Test";
    
    async_data_write(rawTest, boost::bind(&TCP_Session::end, shared_from_this()));
    
}
