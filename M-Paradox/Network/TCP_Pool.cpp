//
//  TCP_Pool.cpp
//  The ASCII Project
//
//  Created by Jonathan Rumion on 3/1/13.
//  Copyright (c) 2013 TAP. All rights reserved.
//

#include "TCP_Pool.h"
#include "TCP_Participant.h"
#include "TCP_Session.h"
#include <boost/bind.hpp>
#include <set>

void TCP_Pool::join(tcp_participant_ptr tcp_participant){
    
    tcp_participants.insert(tcp_participant);
    
}

void TCP_Pool::leave(tcp_participant_ptr tcp_participant){
    
    tcp_participants.erase(tcp_participant);
    
}


void TCP_Pool::deliver(const chat_message& msg)
{
    recent_msgs_.push_back(msg);
    while (recent_msgs_.size() > max_recent_msgs)
        recent_msgs_.pop_front();
    
    //std::for_each(tcp_participants.begin(), tcp_participants.end(), boost::bind(&TCP_Session::deliver, _1, boost::ref(msg)));
}