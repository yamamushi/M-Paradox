//
//  Functions.cpp
//  M-Paradox
//
//  Created by Jonathan Rumion on 5/12/13.
//  Copyright (c) 2013 TAP. All rights reserved.
//

#include "Functions.hpp"
#include "SDL/SDL.h"
#include "Window.hpp"
#include "Images.hpp"
#include "Surfaces.hpp"
#include "Events.hpp"



bool init()
{
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;
    }
    
    //If eveything loads fine
    return true;
}

bool load_files()
{
    //Load the image
    extern SDL_Surface *testing;
    testing = load_image( "Data/Images/Backgrounds/nostromo.jpg" );
    
    //If the image didn't load
    if( testing == NULL )
    {
        return false;
    }
    
    //If eveything loaded fine
    return true;
}


void clean_up()
{
    //Free the image
    extern SDL_Surface *testing;
    SDL_FreeSurface( testing );
    
    //Quit SDL
    SDL_Quit();
}

