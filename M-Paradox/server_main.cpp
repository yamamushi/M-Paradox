//
//  main.cpp
//  M-Paradox
//
//  Created by Jonathan Rumion on 5/6/13.
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
#include "./Network/chat_message.hpp"
#include "./Network/chat_server.hpp"


int main(int argc, char* argv[])
{
    try
    {
       /* if (argc < 2)
        {
            std::cerr << "Usage: chat_server <port> [<port> ...]\n";
            return 1;
        } */
        
        boost::asio::io_service io_service;
        
        chat_server_list servers;
        //for (int i = 1; i < argc; ++i)
       // {
            using namespace std; // For atoi.
            tcp::endpoint endpoint(tcp::v4(), 2063); //atoi(argv[i]));
            chat_server_ptr server(new chat_server(io_service, endpoint));
            servers.push_back(server);
       // }
        
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    
    return 0;
}