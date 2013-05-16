//
//  m_server.cpp
//  M-Paradox
//
//  Created by Jonathan Rumion on 5/16/13.
//  Copyright (c) 2013 TAP. All rights reserved.
//

#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <set>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include "m_packet.h"
#include "m_server.h"
#include "../utils/FileLogger.h"


// Our list of players on the server
// These functions are global player list functions
// Work will need to be done to filter who these messages actually go to.

void player_list::join(player_participant_ptr participant)
{
    participants_.insert(participant);
    std::for_each(recent_msgs_.begin(), recent_msgs_.end(),
                  boost::bind(&player_participant::deliver, participant, _1));
}

void player_list::leave(player_participant_ptr participant)
{
    participants_.erase(participant);
}

void player_list::deliver(const m_packet& msg)
{
    recent_msgs_.push_back(msg);
    while (recent_msgs_.size() > max_recent_msgs)
        recent_msgs_.pop_front();
    
    std::for_each(participants_.begin(), participants_.end(),
                  boost::bind(&player_participant::deliver, _1, boost::ref(msg)));
}




tcp::socket& player_session::socket()
{
    return socket_;
}

void player_session::start()
{
    player_list_.join(shared_from_this());
    extern FileLogger *fileLogger;
    fileLogger->ErrorLog("Client " + getClientIP() + ": Connected");
    boost::asio::async_read(socket_,
                            boost::asio::buffer(read_msg_.data(), m_packet::header_length),
                            boost::bind(&player_session::handle_read_header, shared_from_this(),
                                        boost::asio::placeholders::error));
}


std::string player_session::getClientIP(){
    
    std::string remoteIP = socket_.remote_endpoint().address().to_string();
    
    return remoteIP;
    
}


void player_session::deliver(const m_packet& msg)
{
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress)
    {
        boost::asio::async_write(socket_,
                                 boost::asio::buffer(write_msgs_.front().data(),
                                                     write_msgs_.front().length()),
                                 boost::bind(&player_session::handle_write, shared_from_this(),
                                             boost::asio::placeholders::error));
    }
}

void player_session::handle_read_header(const boost::system::error_code& error)
{
    if (!error && read_msg_.decode_header())
    {
        boost::asio::async_read(socket_,
                                boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
                                boost::bind(&player_session::handle_read_body, shared_from_this(),
                                            boost::asio::placeholders::error));
    }
    else
    {
        player_list_.leave(shared_from_this());
    }
}

void player_session::handle_read_body(const boost::system::error_code& error)
{
    if (!error)
    {
        player_list_.deliver(read_msg_);
        boost::asio::async_read(socket_,
                                boost::asio::buffer(read_msg_.data(), m_packet::header_length),
                                boost::bind(&player_session::handle_read_header, shared_from_this(),
                                            boost::asio::placeholders::error));
    }
    else
    {
        player_list_.leave(shared_from_this());
    }
}

void player_session::handle_write(const boost::system::error_code& error)
{
    if (!error)
    {
        write_msgs_.pop_front();
        if (!write_msgs_.empty())
        {
            boost::asio::async_write(socket_,
                                     boost::asio::buffer(write_msgs_.front().data(),
                                                         write_msgs_.front().length()),
                                     boost::bind(&player_session::handle_write, shared_from_this(),
                                                 boost::asio::placeholders::error));
        }
    }
    else
    {
        player_list_.leave(shared_from_this());
    }
}





void m_server::start_accept()
{
    player_session_ptr new_session(new player_session(io_service_, player_list_));
    acceptor_.async_accept(new_session->socket(),
                           boost::bind(&m_server::handle_accept, this, new_session,
                                       boost::asio::placeholders::error));
}

void m_server::handle_accept(player_session_ptr session,
                                const boost::system::error_code& error)
{
    if (!error)
    {
        session->start();
    }
    
    start_accept();
}



