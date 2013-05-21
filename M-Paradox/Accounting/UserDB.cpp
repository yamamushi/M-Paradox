// MUD Programming
// Ron Penton
// (C)2003
// SCUserDB.cpp - A simple chat server's user database.
// 

#include "UserDB.h"
#include <cctype>

using std::string;
using std::map;

// Important to remember this :
// here we declare the static instance
UserDatabase::users UserDatabase::m_users;

// --------------------------------------------------------------------
//  finds an iterator based on its connection
// --------------------------------------------------------------------
UserDatabase::iterator UserDatabase::find( TCP_Participant* p_connection )
{
    // start at the beginning of the database
    iterator itr = m_users.begin();
    
    // perform the loop
    while( itr != m_users.end() )
    {
        // if pointers match, return the iterator
        if( itr->connection == p_connection ) 
            return itr;

        // else go to next
        ++itr;
    }

    // no match found, return iterator.
    return itr;
}


// --------------------------------------------------------------------
//  adds a new user to the database; returns true on success.
// --------------------------------------------------------------------
bool UserDatabase::AddUser( TCP_Participant* p_connection, string p_name )
{
    if( !HasUser( p_name ) && IsValidName( p_name ) )
    {
        m_users.push_back( User( p_connection, p_name ) );
        return true;
    }
    return false;
}

// --------------------------------------------------------------------
//  Deletes a user based on his connection
// --------------------------------------------------------------------
void UserDatabase::DeleteUser( TCP_Participant* p_connection )
{
    iterator itr = find( p_connection );
    if( itr != m_users.end() )
        m_users.erase( itr );
}


// --------------------------------------------------------------------
//  deternimes if the database has a specified username
// --------------------------------------------------------------------
// We'll need to update this later to also check the MySQL database for
// Existing user accounts.

bool UserDatabase::HasUser( std::string& p_name )
{
    iterator itr = m_users.begin();
    
    while( itr != m_users.end() )
    {
        if( itr->name == p_name ) return true;
        ++itr;
    }

    return false;
}


// --------------------------------------------------------------------
//  determines if a username is valid or not.
// --------------------------------------------------------------------
bool UserDatabase::IsValidName( const std::string& p_name )
{
    static std::string inv = " \"'~!@#$%^&*+/\\[]{}<>()=.,?;:";

    // must not contain any invalid characters
    if(  p_name.find_first_of( inv ) != std::string::npos )
    {
        return false;
    }

    // must be less than 16 chars
    if( p_name.size() > 16 || p_name.size() < 3 )
    {
        return false;
    }

    // must start with an alphabetical character
    if( !isalpha( p_name[0] ) )
    {
        return false;
    }

    return true;
}


std::string UserDatabase::ReturnScreen( TCP_Participant* p_connection )
{
    iterator itr = find( p_connection );
    std::string output = itr->screen;
    return output;
}