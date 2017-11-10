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
#include "settingscontainer.h"
#include <iostream>

// ---------------------------------------------------------------------------
// typeToStr
// --------------------------------------------------------------= HELPER =---
static std::string typeToStr( SettingsVar::Type type ) {
    switch (type) {
        case SettingsVar::Type::BOOL:   return std::string( "BOOL" );
        case SettingsVar::Type::INT:    return std::string( "INT" );
        case SettingsVar::Type::FLOAT:  return std::string( "FLOAT" );
        case SettingsVar::Type::STRING: return std::string( "STRING" );
    }
    return std::string( "UNKNOWN" );
}

// ---------------------------------------------------------------------------
// cTor
// ---------------------------------------------------------------------------
SettingsContainer::SettingsContainer() {
}

SettingsContainer::~SettingsContainer() {
    // Release all resources
    for (auto i = settingsList.begin(); i != settingsList.end(); ++i)
        delete (*i);
    settingsList.clear();
}

// ---------------------------------------------------------------------------
// find
// ---------------------------------------------------------------------------
SettingsVar* SettingsContainer::find( std::string& paramName ) {
    for (auto i = settingsList.begin(); i != settingsList.end(); ++i)
        if ((*i)->name == paramName)
            return (*i);
    return nullptr;
}

// ---------------------------------------------------------------------------
// allocNew
// ---------------------------------------------------------------------------
SettingsVar* SettingsContainer::allocNew( std::string& paramName, bool val, std::string valStr ) {
    SettingsVar* newVar = allocNew( paramName, SettingsVar::Type::BOOL );
    newVar->boolVal = val;
    newVar->stringVal = valStr;
    return newVar;
}

SettingsVar* SettingsContainer::allocNew( std::string& paramName, int val, std::string valStr ) {
    SettingsVar* newVar = allocNew( paramName, SettingsVar::Type::INT );
    newVar->intVal = val;
    newVar->stringVal = valStr;
    return newVar;
}

SettingsVar* SettingsContainer::allocNew( std::string& paramName, float val, std::string valStr ) {
    SettingsVar* newVar = allocNew( paramName, SettingsVar::Type::FLOAT );
    newVar->floatVal = val;
    newVar->stringVal = valStr;
    return newVar;
}

SettingsVar* SettingsContainer::allocNew( std::string& paramName, std::string valStr ) {
    SettingsVar* newVar = allocNew( paramName, SettingsVar::Type::STRING );
    newVar->intVal = 0;
    newVar->stringVal = valStr;
    return newVar;
}

SettingsVar* SettingsContainer::allocNew( std::string& paramName, SettingsVar::Type type ) {
    SettingsVar* newVar = allocNew( paramName );
    newVar->varType = type;
    return newVar;
}

SettingsVar* SettingsContainer::allocNew( std::string& paramName ) {
    SettingsVar* newVar = new SettingsVar();
    newVar->name = paramName;
    newVar->intVal = 0;
    settingsList.push_front( newVar );
    return newVar;
}

// ---------------------------------------------------------------------------
// dbgDump
// --------------------------------------------------------------= HELPER =---
void SettingsContainer::dbgDump( void ) const {
    for (auto i = settingsList.begin(); i != settingsList.end(); ++i) {
        SettingsVar* s = *i;
        switch (s->varType) {
            case SettingsVar::Type::BOOL:   std::cout << s->name.c_str() << "( BOOL => " << std::boolalpha << s->boolVal << std::noboolalpha << " | '" << s->stringVal.c_str() << "' )" << std::endl; break;
            case SettingsVar::Type::INT:    std::cout << s->name.c_str() << "( INT => " << s->intVal << " | '" << s->stringVal.c_str() << "' )" << std::endl; break;
            case SettingsVar::Type::FLOAT:  std::cout << s->name.c_str() << "( FLOAT => " << s->floatVal << " | '" << s->stringVal.c_str() << "' )" << std::endl; break;
            case SettingsVar::Type::STRING: std::cout << s->name.c_str() << "( STRING => " << s->stringVal.c_str() << " )" << std::endl; break;
            default:
                std::cout << s->name.c_str() << "( UNKNOWN )" << std::endl; break;
        }
    }
}

/* EOF */