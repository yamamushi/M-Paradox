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
#include "Graphics/Functions.hpp"
#include "Graphics/Window.hpp"
#include "Graphics/Images.hpp"


int main( int argc, char* args[] )
{
    //Quit flag
    bool quit = false;
    
    //Initialize
    if( init() == false )
    {
        return 1;
    }
    
    //Create a window
    Window myWindow;
    
    //If the window failed
    if( myWindow.error() == true )
    {
        return 1;
    }
    
    //Load the files
    if( load_files() == false )
    {
        return 1;
    }
    
    //While the user hasn't quit
    while( quit == false )
    {
        //While there's events to handle
        extern SDL_Event event;
        while( SDL_PollEvent( &event ) )
        {
            //Handle window events
            myWindow.handle_events();
            
            //If escape was pressed
            if( ( event.type == SDL_KEYDOWN ) && ( event.key.keysym.sym == SDLK_ESCAPE ) )
            {
                //Quit the program
                quit = true;
            }
            
            //If the user has Xed out the window
            if( event.type == SDL_QUIT )
            {
                //Quit the program
                quit = true;
            }
        }
        
        //If the window failed
        if( myWindow.error() == true )
        {
            return 1;
        }
        
        //Fill the screen white
        extern SDL_Surface *screen;
        SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0xFF, 0xFF, 0xFF ) );
        
        //Show the image centered on the screen
        extern SDL_Surface *testing;
        apply_surface( ( screen->w - testing->w ) / 2, ( screen->h - testing->h ) / 2, testing, screen );
        
        //Update the screen
        if( SDL_Flip( screen ) == -1 )
        {
            return 1;
        }
    }
    
    //Clean up
    clean_up();
    
    return 0;
}


