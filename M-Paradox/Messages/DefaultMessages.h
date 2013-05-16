//
//  DefaultMessages.h
//  M-Paradox
//
//  Created by Jonathan Rumion on 5/16/13.
//  Copyright (c) 2013 TAP. All rights reserved.
//

#ifndef M_Paradox_DefaultMessages_h
#define M_Paradox_DefaultMessages_h

#include <string>

std::string resetColors_ANSI("\x1b[0m");

std::string loginMenu_MSG(resetColors_ANSI+"\n\nCommands Available: \n"
                      "------------------- \n\n"
                      "login \n"
                      "logout \n"
                      "quit \r\n\r\n");

std::string flower_MSG("\x1b[2J\x1b[H\x1b[7h\x1b[0;1;40;37m\x1b[?33h\n"
                       "\x1b[18C\x1b[5m..\x1b[25m\x1b[10C\x1b[0m\x1b[7C\x1b[34m€€€€€€€€€€€\x1b[37m\x1b[19C\x1b[1m   \x1b[5m..\x1b[25m\n"
                       "\x1b[18C\x1b[5m..\x1b[25m\x1b[10C\x1b[0m    \x1b[34m€€€\x1b[37m \x1b[34m.,;;;;;,.\x1b[37m \x1b[34m€€€\n"
                       "\x1b[1;37m\x1b[30C\x1b[0m  \x1b[34m€€\x1b[37m \x1b[34m.,%!&&!%;%%%,.\x1b[37m  \x1b[34m€€\x1b[37m\x1b[14C\x1b[1m\x1b[6C.,,;%\n"
                       "\x1b[29C\x1b[0m \x1b[34m€€.,\x1b[32m%!&\x1b[34m###&!!&@@@&&!;.\x1b[37m \x1b[34m€€\x1b[37m\x1b[10C\x1b[1m...,,;!!%!&!%\n"
                       "\x1b[24C\x1b[0m\x1b[5C\x1b[34m€.\x1b[32m;&#@@@\x1b[34m@\x1b[32m@@@@@\x1b[34m@@@@@@@\x1b[32m@@&;\x1b[37m \x1b[34m€\x1b[37m   \x1b[1m.,,.,;%!!###@@!;%,\n"
                       "\x1b[25C\x1b[0m   \x1b[34m€\x1b[32m.;&@@@@@@@@@@@\x1b[34m@@@@@@\x1b[32m@@@@@\x1b[34m;.€\x1b[37m  \x1b[1m,%%;%!%%!#&%!&;\n"
                       "\x1b[7C\x1b[5m..\x1b[25m\x1b[16C\x1b[0m  \x1b[34m€\x1b[32m,!#@@@@@@@@@@@@\x1b[34m@@@@@@\x1b[32m@@@@\x1b[34m@#;\x1b[37m \x1b[34m€\x1b[37m \x1b[1m.;%;;%%%%%,  \x1b[5m..\x1b[25m\n"
                       "\x1b[7C\x1b[5m..\x1b[25m\x1b[16C\x1b[0m  \x1b[34m€\x1b[32m;#@@@@@@@@@@@@\x1b[34m@@@@@@@@@@@@@%\x1b[37m \x1b[34m€\x1b[37m \x1b[1m,;%%,,;%;;;.\n"
                       "\x1b[17C\x1b[5m..\x1b[25m\x1b[7C\x1b[0m \x1b[34m€\x1b[32m%@@#@\x1b[34m@\x1b[32m@@@@@\x1b[34m@@#@@@@@@@@@@@@#%\x1b[37m \x1b[34m€\x1b[37m \x1b[1m;;;%.    .,.\n"
                       "\x1b[17C\x1b[5m..\x1b[25m\x1b[7C\x1b[0m \x1b[34m€,\x1b[32m%#&&\x1b[34m@@@#######@@@@@\x1b[32m@@@\x1b[34m@@\x1b[32m@#%\x1b[37m \x1b[34m€\x1b[37m \x1b[1m....\n"
                       "\x1b[26C\x1b[0m \x1b[34m€.,%!!##@@@@@#@#@@\x1b[32m@@@@@@@@@#;\x1b[37m \x1b[34m€\n"
                       "\x1b[1;37m\x1b[24C.,,.\x1b[0;34m€\x1b[37m \x1b[34m.;%&&#&#&##@@\x1b[32m@@@@@@@@@@#%\x1b[34m.€\n"
                       "\x1b[1;37m\x1b[21C;!%%%&%\x1b[0m \x1b[34m€\x1b[37m \x1b[34m.,%&@@@###@@\x1b[32m@@@@@@\x1b[34m&\x1b[32m&!!;\x1b[34m.€\x1b[1;37m\x1b[10C\x1b[5m..\x1b[25m\n"
                       "   .;,\x1b[10C.. ,%&#!;,%%.\x1b[0m \x1b[34m€€\x1b[37m  \x1b[34m.,;%!@@@@@\x1b[32m@@\x1b[34m@##%%,.€€\x1b[37m \x1b[1m\x1b[10C\x1b[5m..\x1b[25m\n"
                       "   .;,\x1b[7C,;,%!%!&%,,,,,;%,..\x1b[0;34m€€.,;;;%!&&&&&#&!,.€€\n"
                       "\x1b[1;37m\x1b[6C;%,;%;;!!%%%!&!;,,,,,,;,..\x1b[0m  \x1b[34m€€€.,;;%%%;;;,.€€\n"
                       "\x1b[37m \x1b[1m\x1b[5C;%,;%;;;,,..,;;,,.\x1b[5C\x1b[0m\x1b[8C\x1b[34m€€€€€€€€€€€\n");


#endif
