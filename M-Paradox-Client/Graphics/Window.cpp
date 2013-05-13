//
//  Window.cpp
//  M-Paradox
//
//  Created by Jonathan Rumion on 5/12/13.
//  Copyright (c) 2013 TAP. All rights reserved.
//

#include "Window.hpp"
#include "Functions.hpp"
#include "SDL/SDL.h"
#include "SDL_image/SDL_image.h"

Window::Window()
{
    //Set up the screen
    extern SDL_Surface *screen;
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE | SDL_RESIZABLE );
    
    //If there's an error
    if( screen == NULL )
    {
        windowOK = false;
        return;
    }
    else
    {
        windowOK = true;
    }
    
    //Set the window caption
    SDL_WM_SetCaption( "Window Event Test", NULL );
    
    //Set window flag
    windowed = true;
}

void Window::handle_events()
{
    //If there's something wrong with the window
    if( windowOK == false )
    {
        return;
    }
    
    //If the window resized
    extern SDL_Event event;
    if( event.type == SDL_VIDEORESIZE )
    {
        //Resize the screen
        extern SDL_Surface *screen;
        screen = SDL_SetVideoMode( event.resize.w, event.resize.h, SCREEN_BPP, SDL_SWSURFACE | SDL_RESIZABLE );
        
        //If there's an error
        if( screen == NULL )
        {
            windowOK = false;
            return;
        }
    }
    //If enter was pressed
    else if( ( event.type == SDL_KEYDOWN ) && ( event.key.keysym.sym == SDLK_RETURN ) )
    {
        //Turn fullscreen on/off
        toggle_fullscreen();
    }
    //If the window focus changed
    else if( event.type == SDL_ACTIVEEVENT )
    {
        //If the window was iconified or restored
        if( event.active.state & SDL_APPACTIVE )
        {
            //If the application is no longer active
            if( event.active.gain == 0 )
            {
                SDL_WM_SetCaption( "Window Event Test: Iconified", NULL );
            }
            else
            {
                SDL_WM_SetCaption( "Window Event Test", NULL );
            }
        }
        //If something happened to the keyboard focus
        else if( event.active.state & SDL_APPINPUTFOCUS )
        {
            //If the application lost keyboard focus
            if( event.active.gain == 0 )
            {
                SDL_WM_SetCaption( "Window Event Test: Keyboard focus lost", NULL );
            }
            else
            {
                SDL_WM_SetCaption( "Window Event Test", NULL );
            }
        }
        //If something happened to the mouse focus
        else if( event.active.state & SDL_APPMOUSEFOCUS )
        {
            //If the application lost mouse focus
            if( event.active.gain == 0 )
            {
                SDL_WM_SetCaption( "Window Event Test: Mouse Focus Lost", NULL );
            }
            else
            {
                SDL_WM_SetCaption( "Window Event Test", NULL );
            }
        }
    }
    //If the window's screen has been altered
    else if( event.type == SDL_VIDEOEXPOSE )
    {
        //Update the screen
        extern SDL_Surface *screen;
        if( SDL_Flip( screen ) == -1 )
        {
            //If there's an error
            windowOK = false;
            return;
        }
    }
}

void Window::toggle_fullscreen()
{
    //If the screen is windowed
    if( windowed == true )
    {
        //Set the screen to fullscreen
        extern SDL_Surface *screen;
        screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE | SDL_RESIZABLE | SDL_FULLSCREEN );
        
        //If there's an error
        if( screen == NULL )
        {
            windowOK = false;
            return;
        }
        
        //Set the window state flag
        windowed = false;
    }
    //If the screen is fullscreen
    else if( windowed == false )
    {
        //Window the screen
        extern SDL_Surface *screen;
        screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE | SDL_RESIZABLE );
        
        //If there's an error
        if( screen == NULL )
        {
            windowOK = false;
            return;
        }
        
        //Set the window state flag
        windowed = true;
    }
}

bool Window::error()
{
    return !windowOK;
}

