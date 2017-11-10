//                                    .-""-.
//                                   / _  _ \
//                                   |(_)(_)|
// .------------------------------ooO(_ /\ _)Ooo-----------------------------.
// |                                  |====|                                 |
// |                                  '-..-'                                 |
// | Desc:     Settings Variable                                             |
// | By:       Nuroferatu                                                    |
// '-------------------------------------------------------------------------'
// ----= Change log =---------------------------------------------------------
//   1. 2017.11.06, 18:00    [+] Initial
// ---------------------------------------------------------------------------
#pragma once
#ifndef __SETTINGSVAR_H__
#define __SETTINGSVAR_H__

#include <string>

// ---------------------------------------------------------------------------
// SettingsVar
// ---------------------------------------------------------------------------
struct SettingsVar {
    enum class Type { UNKNOWN, BOOL, INT, FLOAT, STRING };

    Type            varType;
    std::string     name;
    std::string     stringVal;
    union {
        bool    boolVal;
        int     intVal;
        float   floatVal;
    };
};

#endif /* ndef __SETTINGSVAR_H__ */
/* EOF */