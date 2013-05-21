// MUD Programming
// Ron Penton
// (C)2003
// SCUserDB.h - A simple chat server's user database.
//


#ifndef SCUSERDB_H
#define SCUSERDB_H


#include "../Network/TCP_Session.h"
#include "../Network/TCP_Participant.h"
#include <string>
#include <list>


class User
{
public:
    
    // default constructor
    User() { connection = 0; };
    
    // initialization constructor
    User( TCP_Participant* c, std::string& n )
    : connection( c ),
    name( n ), screen("")
    {
        
    }
    
    std::string name;
    std::string screen; // What the user is seeing, or is about to see.
    // Here we can put links to attribute lists, item lists, etc.
    TCP_Participant* connection;
};



class UserDatabase
{
public:
    
    // The following typedefs make the program easier to read.
    typedef std::list<User> users;
    typedef std::list<User>::iterator iterator;
    
    // We'll add functions later, so that when the server starts up, this
    // list is managed to track logged-in players.
    // We'll link mysql authentication in here as well.
    
    // --------------------------------------------------------------------
    //  returns an iterator pointing to the first connection
    // --------------------------------------------------------------------
    static iterator begin() { return m_users.begin(); }
    
    // --------------------------------------------------------------------
    //  returns the "invalid" iterator, the iterator that points one
    //  past the end of the DB. Used for comparisons.
    // --------------------------------------------------------------------
    static iterator end() { return m_users.end(); }
    
    // --------------------------------------------------------------------
    //  finds an iterator based on its connection
    // --------------------------------------------------------------------
    static iterator find( TCP_Participant* p_connection );
    
    // --------------------------------------------------------------------
    //  adds a new user to the database; returns true on success.
    // --------------------------------------------------------------------
    static bool AddUser( TCP_Participant* p_connection, std::string p_name );
    
    // --------------------------------------------------------------------
    //  Deletes a user based on his connection
    // --------------------------------------------------------------------
    static void DeleteUser( TCP_Participant* p_connection );
    
    // --------------------------------------------------------------------
    //  deternimes if the database has a specified username
    // --------------------------------------------------------------------
    static bool HasUser( std::string& p_name );
    
    // --------------------------------------------------------------------
    //  determines if a username is valid or not.
    // --------------------------------------------------------------------
    static bool IsValidName( const std::string& p_name );
    
    // returns the screen 
    static std::string ReturnScreen( TCP_Participant* p_connection );
    
    
protected:
    
    static users m_users;
};


#endif
