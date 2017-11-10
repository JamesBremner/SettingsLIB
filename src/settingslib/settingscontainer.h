//                                    .-""-.
//                                   / _  _ \
//                                   |(_)(_)|
// .------------------------------ooO(_ /\ _)Ooo-----------------------------.
// |                                  |====|                                 |
// |                                  '-..-'                                 |
// | Desc:     Settings Container                                            |
// | By:       Nuroferatu                                                    |
// '-------------------------------------------------------------------------'
// ----= Change log =---------------------------------------------------------
//   1. 2017.11.06, 18:30    [+] Initial
// ---------------------------------------------------------------------------
#pragma once
#ifndef __SETTINGSCONTAINER_H__
#define __SETTINGSCONTAINER_H__

#include "settingsvar.h"
#include <list>

class SettingsContainer {
public:
    SettingsContainer();
    ~SettingsContainer();

    SettingsVar*    find( std::string& paramName );

    SettingsVar*    allocNew( std::string& paramName, bool val, std::string valStr );
    SettingsVar*    allocNew( std::string& paramName, int val, std::string valStr );
    SettingsVar*    allocNew( std::string& paramName, float val, std::string valStr );
    SettingsVar*    allocNew( std::string& paramName, std::string valStr );
    SettingsVar*    allocNew( std::string& paramName, SettingsVar::Type type );

    // ---= Debug =-----------------------------------------------------------
    void dbgDump( void ) const;

private:
    SettingsVar*    allocNew( std::string& paramName );
    std::list<SettingsVar*>  settingsList;
};

#endif /* ndef __SETTINGSCONTAINER_H__ */
/* EOF */