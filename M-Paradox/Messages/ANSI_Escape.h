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

std::string SelectGraphicRendition_ANS(int code);

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

const std::string colorBlack_ANS("");
const std::string colorRed_ANS("");
const std::string colorGreen_ANS("");
const std::string colorYellow_ANS("");
const std::string colorBlue_ANS("");
const std::string colorMagenta_ANS("");
const std::string colorCyan_ANS("");
const std::string colorWhite_ANS("");

#endif
