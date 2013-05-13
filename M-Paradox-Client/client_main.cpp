//
//  main.cpp
//  M-Paradox-Client
//
//  Created by Jonathan Rumion on 5/12/13.
//  Copyright (c) 2013 TAP. All rights reserved.
//

#include "SDL/SDL.h"
#include "SDL_image/SDL_image.h"
#include "SDL_ttf/SDL_ttf.h"
#include "SDL_mixer/SDL_mixer.h"


int main( int argc, char* args[] )
{
    //The images
    SDL_Surface* hello = NULL;
    SDL_Surface* screen = NULL;
    
    //Start SDL
    SDL_Init( SDL_INIT_EVERYTHING );
    
    //Set up screen
    screen = SDL_SetVideoMode( 640, 480, 32, SDL_SWSURFACE );
    
    //Load image
    hello = IMG_Load( "./Data/Images/Backgrounds/loading.png" );
    
    //Apply image to screen
    SDL_BlitSurface( hello, NULL, screen, NULL );
    
    //Update Screen
    SDL_Flip( screen );
    
    //Pause
    SDL_Delay( 2000 );
    
    //Quit SDL
    SDL_Quit();
    
    return 0;
}

