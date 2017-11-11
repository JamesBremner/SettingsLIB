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
#pragma once
#ifndef __SETTINGSSERVICE_H__
#define __SETTINGSSERVICE_H__

#include "isettingsservice.h"
#include "isettingsstorage.h"
#include "settingscontainer.h"

// ---------------------------------------------------------------------------
// SettingsService
// ---------------------------------------------------------------------------
class SettingsService : public ISettingsService {
public:
    static const unsigned int   INTERFACE_VERSION = ISettingsService::INTERFACE_VERSION;      //!< Interface version implemented by this compilation
    static ISettingsService::Ref createInstance( ISettingsStorage* settingsStorage );

    SettingsService( ISettingsStorage* settingsStorage ) : storage( settingsStorage ) {}
    virtual ~SettingsService() = default;

    virtual const char* getName( void ) const { return "settings"; }
    virtual const char* getVersion( void ) const { return "0.0.1"; }

    virtual void onInit( void ) override;
    virtual void onShutdown( void ) override;

    //! @name Getters
    //! @{
    virtual bool        getBool( const char* groupName, const char* paramName, bool defVal = false ) override;
    virtual int         getInt( const char* groupName, const char* paramName, int defVal = 0 ) override;
    virtual float       getFloat( const char* groupName, const char* paramName, float defVal = 0.f ) override;
    virtual const char* getString( const char* groupName, const char* paramName, const char* defVal = "" ) override;
    //! @}

    //! @name Setters
    //! @{
    virtual void    setBool( const char* groupName, const char* paramName, bool val ) override;
    virtual void    setInt( const char* groupName, const char* paramName, int val ) override;
    virtual void    setFloat( const char* groupName, const char* paramName, float val ) override;
    virtual void    setString( const char* groupName, const char* paramName, const char* val ) override;
    //! @}

    // ---= Debug / Helper =--------------------------------------------------
    void dbgDump( void ) const;

private:
    std::string     valToString( bool val );        //!< Convert bool to std::string
    std::string     valToString( int val );         //!< Convert int to std::string
    std::string     valToString( float val );       //!< Convert float to dtring

    SettingsContainer   container;
    ISettingsStorage*   storage;

    static const char* LOG_TAG;
};

#endif /* ndef __SETTINGSMODULE_H__ */
/* EOF */