#pragma once
//
//  TCP_Pool.h
//  The ASCII Project
//
//  Created by Jonathan Rumion on 3/1/13.
//  Copyright (c) 2013 TAP. All rights reserved.
//
#include "TCP_Participant.h"
#include "Chat_Message.h"
#include <boost/shared_ptr.hpp>
#include <memory>
#include <set>
#include <deque>

typedef std::deque<chat_message> chat_message_queue;

class TCP_Pool {
    
private:
    
    std::set<tcp_participant_ptr> tcp_participants;
    
    chat_message_queue recent_msgs_;
    enum { max_recent_msgs = 100 };

    
public:
    
    TCP_Pool(){};
    virtual ~TCP_Pool(){};
    
    
    void join(tcp_participant_ptr tcp_participant);
    void leave(tcp_participant_ptr tcp_participant);
    void deliver(const chat_message& msg);
    
    
};

typedef boost::shared_ptr<TCP_Pool> tcp_pool_ptr;
