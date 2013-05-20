//
//  ANSI_Escape.h
//  M-Paradox
//
//  Created by Jonathan Rumion on 5/20/13.
//  Copyright (c) 2013 TAP. All rights reserved.
//
//  Our ANSI Escape Characters, for doing terminal manipulation
//
//  http://en.wikipedia.org/wiki/ANSI_escape_code
//

#ifndef M_Paradox_ANSI_Escape_h
#define M_Paradox_ANSI_Escape_h

#include <string>

const std::string clearScreen_ANS("\x1b[2J");
const std::string resetCursor_ANS("\x1b[;f");


// Colors

enum ANS_COLORS{
    BLACK,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE
    
};

const std::string colorBlack_ANS("\x1b[30m");
const std::string colorRed_ANS("\x1b[31m");
const std::string colorGreen_ANS("\x1b[32m");
const std::string colorYellow_ANS("\x1b[33m");
const std::string colorBlue_ANS("\x1b[34m");
const std::string colorMagenta_ANS("\x1b[35m");
const std::string colorCyan_ANS("\x1b[36m");
const std::string colorWhite_ANS("\x1b[37m");

#endif
