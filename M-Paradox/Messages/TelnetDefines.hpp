// A modified version of toftlib net source
// Original author at https://code.google.com/p/toftlib/
//

#pragma once


enum TELNET_COMMAND
{
    COMMAND_IAC = 255,
    COMMAND_DONT = 254,
    COMMAND_DO = 253,
    COMMAND_WONT = 252,
    COMMAND_WILL = 251,
    COMMAND_SB = 250,
    COMMAND_GA = 249,
    COMMAND_EL = 248,
    COMMAND_EC = 247,
    COMMAND_AYT = 246,
    COMMAND_AO = 245,
    COMMAND_IP = 244,
    COMMAND_BREAK = 243,
    COMMAND_DM =242,
    COMMAND_NOP = 241,
    COMMAND_SE = 240,
    COMMAND_EOR = 239,
    COMMAND_ABORT = 238,
    COMMAND_SUSP = 237,
    COMMAND_EOF = 236
};

enum TELNET_STATUS
{
    STATE_DATA = 0, STATE_IAC = 1, STATE_WILL = 2, STATE_WONT = 3,
    STATE_DO = 4, STATE_DONT = 5, STATE_SB = 6, STATE_SE = 7, STATE_CR = 8,
    STATE_IAC_SB = 9
};

enum TELNET_OPTION
{
    OPTION_BINARY = 0,
    OPTION_ECHO = 1,
    OPTION_PREPARE_TO_RECONNECT = 2,
    OPTION_SUPPRESS_GO_AHEAD = 3,
    OPTION_APPROXIMATE_MESSAGE_SIZE = 4,
    OPTION_STATUS = 5,
    OPTION_TIMING_MARK = 6,
    OPTION_REMOTE_CONTROLLED_TRANSMISSION = 7,
    OPTION_NEGOTIATE_OUTPUT_LINE_WIDTH = 8,
    OPTION_NEGOTIATE_OUTPUT_PAGE_SIZE = 9,
    OPTION_NEGOTIATE_CARRIAGE_RETURN = 10,
    OPTION_NEGOTIATE_HORIZONTAL_TAB_STOP = 11,
    OPTION_NEGOTIATE_HORIZONTAL_TAB = 12,
    OPTION_NEGOTIATE_FORMFEED = 13,
    OPTION_NEGOTIATE_VERTICAL_TAB_STOP = 14,
    OPTION_NEGOTIATE_VERTICAL_TAB = 15,
    OPTION_NEGOTIATE_LINEFEED = 16,
    OPTION_EXTENDED_ASCII = 17,
    OPTION_FORCE_LOGOUT = 18,
    OPTION_BYTE_MACRO = 19,
    OPTION_DATA_ENTRY_TERMINAL = 20,
    OPTION_SUPDUP = 21,
    OPTION_SUPDUP_OUTPUT = 22,
    OPTION_SEND_LOCATION = 23,
    OPTION_TERMINAL_TYPE = 24,
    OPTION_END_OF_RECORD = 25,
    OPTION_TACACS_USER_IDENTIFICATION = 26,
    OPTION_OUTPUT_MARKING = 27,
    OPTION_TERMINAL_LOCATION_NUMBER = 28,
    OPTION_REGIME_3270 = 29,
    OPTION_X3_PAD = 30,
    OPTION_WINDOW_SIZE = 31,
    OPTION_TERMINAL_SPEED = 32,
    OPTION_REMOTE_FLOW_CONTROL = 33,
    OPTION_LINEMODE = 34,
    OPTION_X_DISPLAY_LOCATION = 35,
    OPTION_OLD_ENVIRONMENT_VARIABLES = 36,
    OPTION_AUTHENTICATION = 37,
    OPTION_ENCRYPTION = 38,
    OPTION_NEW_ENVIRONMENT_VARIABLES = 39,
    OPTION_EXTENDED_OPTIONS_LIST = 255
    
};

enum TELNET_SUBOPTION
{
    SUBOPTION_TERMINAL_TYPE_SEND =  1
};

enum MAX_VALUES
{
    MAX_OPTION_VALUE = 255,
    MAX_COMMAND_VALUE = 255
};

static std::string GetCommandString(TELNET_COMMAND command)
{
    const static unsigned char FIRST_COMMAND = COMMAND_IAC;
    const static unsigned char LAST_COMMAND = COMMAND_EOF;
    const static std::string TELNET_COMMAND_STRING[] = {"IAC", "DONT", "DO", "WONT", "WILL", "SB", "GA", "EL", "EC", "AYT",
        "AO", "IP", "BRK", "DMARK", "NOP", "SE", "EOR", "ABORT", "SUSP", "EOF"};
    
    return TELNET_COMMAND_STRING[FIRST_COMMAND - command];
}

static std::string GetOptionString(TELNET_OPTION option)
{
    const static unsigned char FIRST_OPTION = OPTION_BINARY;
    const static unsigned char LAST_OPTION = OPTION_EXTENDED_OPTIONS_LIST;
    const static std::string TELNET_OPTION_STRING[] = {
        "BINARY", "ECHO", "RCP", "SUPPRESS GO AHEAD", "NAME", "STATUS",
        "TIMING MARK", "RCTE", "NAOL", "NAOP", "NAOCRD", "NAOHTS", "NAOHTD",
        "NAOFFD", "NAOVTS", "NAOVTD", "NAOLFD", "EXTEND ASCII", "LOGOUT",
        "BYTE MACRO", "DATA ENTRY TERMINAL", "SUPDUP", "SUPDUP OUTPUT",
        "SEND LOCATION", "TERMINAL TYPE", "END OF RECORD", "TACACS UID",
        "OUTPUT MARKING", "TTYLOC", "3270 REGIME", "X.3 PAD", "NAWS", "TSPEED",
        "LFLOW", "LINEMODE", "XDISPLOC", "OLD-ENVIRON", "AUTHENTICATION",
        "ENCRYPT", "NEW-ENVIRON", "TN3270E", "XAUTH", "CHARSET", "RSP",
        "Com Port Control", "Suppress Local Echo", "Start TLS",
        "KERMIT", "SEND-URL", "FORWARD_X", "", "", "",
        "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "TELOPT PRAGMA LOGON", "TELOPT SSPI LOGON",
        "TELOPT PRAGMA HEARTBEAT", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "",
        "Extended-Options-List"
    };
    
    
    return TELNET_OPTION_STRING[FIRST_OPTION + option];
}

