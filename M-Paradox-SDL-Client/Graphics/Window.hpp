//
//  Window.h
//  M-Paradox
//
//  Created by Jonathan Rumion on 5/12/13.
//  Copyright (c) 2013 TAP. All rights reserved.
//

#ifndef __M_Paradox__Window__
#define __M_Paradox__Window__

//Screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

//Our window
class Window
{
private:
    //Whether the window is windowed or not
    bool windowed;
    
    //Whether the window is fine
    bool windowOK;
    
public:
    //Constructor
    Window();
    
    //Handle window events
    void handle_events();
    
    //Turn fullscreen on/off
    void toggle_fullscreen();
    
    //Check if anything's wrong with the window
    bool error();
};


#endif /* defined(__M_Paradox__Window__) */
