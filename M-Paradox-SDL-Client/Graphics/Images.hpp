//
//  Images.h
//  M-Paradox
//
//  Created by Jonathan Rumion on 5/12/13.
//  Copyright (c) 2013 TAP. All rights reserved.
//

#ifndef __M_Paradox__Images__
#define __M_Paradox__Images__

#include <string>
#include "SDL/SDL.h"


SDL_Surface *load_image( std::string filename );
void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL );


#endif /* defined(__M_Paradox__Images__) */