//
//  m_server.h
//  M-Paradox
//
//  Created by Jonathan Rumion on 5/16/13.
//  Copyright (c) 2013 TAP. All rights reserved.
//

#ifndef __M_Paradox__m_server__
#define __M_Paradox__m_server__

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

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

typedef std::deque<m_packet> chat_message_queue;

//----------------------------------------------------------------------

class player_participant
{
public:
    virtual ~player_participant() {}
    virtual void deliver(const m_packet& msg) = 0;
};

typedef boost::shared_ptr<player_participant> player_participant_ptr;

//----------------------------------------------------------------------

class player_list
{
public:
    void join(player_participant_ptr participant);
    
    void leave(player_participant_ptr participant);
    
    void deliver(const m_packet& msg);
    
private:
    std::set<player_participant_ptr> participants_;
    enum { max_recent_msgs = 100 };
    chat_message_queue recent_msgs_;
};

//----------------------------------------------------------------------

class player_session
: public player_participant,
public boost::enable_shared_from_this<player_session>
{
public:
    player_session(boost::asio::io_service& io_service, player_list& player_list)
    : socket_(io_service),
    player_list_(player_list)
    {
    }
    
    tcp::socket& socket();
    
    void start();
    
    std::string getClientIP();
    
    void deliver(const m_packet& msg);
    
    void handle_read_header(const boost::system::error_code& error);
    
    void handle_read_body(const boost::system::error_code& error);
    
    void handle_write(const boost::system::error_code& error);
    
private:
    tcp::socket socket_;
    player_list& player_list_;
    m_packet read_msg_;
    chat_message_queue write_msgs_;
};

typedef boost::shared_ptr<player_session> player_session_ptr;

//----------------------------------------------------------------------

class m_server
{
public:
    m_server(boost::asio::io_service& io_service,
                const tcp::endpoint& endpoint)
    : io_service_(io_service),
    acceptor_(io_service, endpoint)
    {
        start_accept();
    }
    
    void start_accept();
    
    void handle_accept(player_session_ptr session,
                       const boost::system::error_code& error);
    
private:
    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
    player_list player_list_;
};

typedef boost::shared_ptr<m_server> m_server_ptr;
typedef std::list<m_server_ptr> m_server_list;

//----------------------------------------------------------------------


#endif /* defined(__M_Paradox__m_server__) */
