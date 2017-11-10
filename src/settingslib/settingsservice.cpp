//                                    .-""-.
//                                   / _  _ \
//                                   |(_)(_)|
// .------------------------------ooO(_ /\ _)Ooo-----------------------------.
// |                                  |====|                                 |
// |                                  '-..-'                                 |
// | Desc:     Settings service - using SQLite DB stores settings            |
// | By:       Nuroferatu                                                    |
// '-------------------------------------------------------------------------'
// ----= Change log =---------------------------------------------------------
//   1. 2017.11.05, 15:15    [+] Initial
// ---------------------------------------------------------------------------
#include "settingsservice.h"
#include <sstream>
#include <iostream>
#include <assert.h>

// ---------------------------------------------------------------------------
// createParamFullName
//
// Helper that will create single name for param. Result is groupName.paramName
// --------------------------------------------------------------= HELPER =---
static std::string createParamFullName( const char* groupName, const char* paramName ) {
    std::stringstream   ss;
    ss << groupName << "." << paramName;
    return ss.str();
}

// ---------------------------------------------------------------------------
// dbgDump
// --------------------------------------------------------------= HELPER =---
void SettingsService::dbgDump( void ) const {
    container.dbgDump();
}

// ---------------------------------------------------------------------------
// createInstance
// ---------------------------------------------------------------------------
ISettingsService::Ref SettingsService::createInstance( void ) {
    ISettingsService::Ref ref = std::make_unique<SettingsService>();
    return ref;
}

// ---------------------------------------------------------------------------
// onInit
// ---------------------------------------------------------------------------
void SettingsService::onInit( void ) {
}

// ---------------------------------------------------------------------------
// onShutdown
// ---------------------------------------------------------------------------
void SettingsService::onShutdown( void ) {
}

// ---------------------------------------------------------------------------
// getBool
// ---------------------------------------------------------------------------
bool SettingsService::getBool( const char* groupName, const char* paramName, bool defVal ) {
    assert( (groupName != nullptr) && (paramName != nullptr) );

    std::string fullName = createParamFullName( groupName, paramName );
    SettingsVar* var = container.find( fullName );
    if (!var)
        var = container.allocNew( fullName, defVal, valToString( defVal ) );

    return var->boolVal;
}

// ---------------------------------------------------------------------------
// getInt
// ---------------------------------------------------------------------------
int SettingsService::getInt( const char* groupName, const char* paramName, int defVal ) {
    assert( (groupName != nullptr) && (paramName != nullptr) );

    std::string fullName = createParamFullName( groupName, paramName );
    SettingsVar* var = container.find( fullName );
    if (!var)
        var = container.allocNew( fullName, defVal, valToString( defVal ) );

    return var->intVal;
}

// ---------------------------------------------------------------------------
// getFloat
// ---------------------------------------------------------------------------
float SettingsService::getFloat( const char* groupName, const char* paramName, float defVal ) {
    assert( (groupName != nullptr) && (paramName != nullptr) );

    std::string fullName = createParamFullName( groupName, paramName );
    SettingsVar* var = container.find( fullName );
    if (!var)
        var = container.allocNew( fullName, defVal, valToString( defVal ) );

    return var->floatVal;
}

// ---------------------------------------------------------------------------
// getString
// ---------------------------------------------------------------------------
const char* SettingsService::getString( const char* groupName, const char* paramName, const char* defVal ) {
    assert( (groupName != nullptr) && (paramName != nullptr) );

    std::string fullName = createParamFullName( groupName, paramName );
    SettingsVar* var = container.find( fullName );
    if (!var)
        var = container.allocNew( fullName, defVal );

    return var->stringVal.c_str();
}

// ---------------------------------------------------------------------------
// setBool
// ---------------------------------------------------------------------------
void SettingsService::setBool( const char* groupName, const char* paramName, bool val ) {
    assert( (groupName != nullptr) && (paramName != nullptr) );

    std::string fullName = createParamFullName( groupName, paramName );
    SettingsVar* var = container.find( fullName );
    if (var == nullptr)
        var = container.allocNew( fullName, SettingsVar::Type::BOOL );

    var->boolVal = val;
    var->stringVal = valToString( val );
}

// ---------------------------------------------------------------------------
// setInt
// ---------------------------------------------------------------------------
void SettingsService::setInt( const char* groupName, const char* paramName, int val ) {
    assert( (groupName != nullptr) && (paramName != nullptr) );

    std::string fullName = createParamFullName( groupName, paramName );
    SettingsVar* var = container.find( fullName );
    if (var == nullptr)
        var = container.allocNew( fullName, SettingsVar::Type::INT );

    var->intVal = val;
    var->stringVal = valToString( val );
}

// ---------------------------------------------------------------------------
// setFloat
// ---------------------------------------------------------------------------
void SettingsService::setFloat( const char* groupName, const char* paramName, float val ) {
    assert( (groupName != nullptr) && (paramName != nullptr) );

    std::string fullName = createParamFullName( groupName, paramName );
    SettingsVar* var = container.find( fullName );
    if (var == nullptr)
        var = container.allocNew( fullName, SettingsVar::Type::FLOAT );

    var->floatVal = val;
    var->stringVal = valToString( val );
}

// ---------------------------------------------------------------------------
// setString
// ---------------------------------------------------------------------------
void SettingsService::setString( const char* groupName, const char* paramName, const char* val ) {
    assert( (groupName != nullptr) && (paramName != nullptr) );

    std::string fullName = createParamFullName( groupName, paramName );
    SettingsVar* var = container.find( fullName );
    if (var == nullptr)
        var = container.allocNew( fullName, SettingsVar::Type::STRING );

    var->intVal = 0;
    var->stringVal = std::string( val );
}

// ---------------------------------------------------------------------------
// valToString
// ---------------------------------------------------= [PRIVATE][HELPER] =---
std::string SettingsService::valToString( bool val ) {
    std::stringstream   ss;
    ss << std::boolalpha << val;
    return ss.str();
}

std::string SettingsService::valToString( int val ) {
    std::stringstream   ss;
    ss << val;
    return ss.str();
}

std::string SettingsService::valToString( float val ) {
    std::stringstream   ss;
    ss << val;
    return ss.str();
}

const char* SettingsService::LOG_TAG = "settingsservice";

/* EOF */