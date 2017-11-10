//                                    .-""-.
//                                   / _  _ \
//                                   |(_)(_)|
// .------------------------------ooO(_ /\ _)Ooo-----------------------------.
// |                                  |====|                                 |
// |                                  '-..-'                                 |
// | Desc:     Settings service interface                                    |
// | By:       Nuroferatu                                                    |
// '-------------------------------------------------------------------------'
// ----= Change log =---------------------------------------------------------
//   1. 2017.11.05, 15:05    [+] Initial
// ---------------------------------------------------------------------------
#pragma once
#ifndef __ISETTINGSSERVICE_H__
#define __ISETTINGSSERVICE_H__

#include "iservice.h"
#include <list>

// ---------------------------------------------------------------------------
// ISettingsService
//
//! @brief Service interface, must be implemented by all services of system
//! groupName - max 31 chars + NULL
//! paramName = max 63 chars + NULL
//! string value set by setString - max 1023 chars + NULL
//! All limists are assuption and subiect for change
// ---------------------------------------------------------------------------
class ISettingsService : public IService {
public:
    typedef std::unique_ptr<ISettingsService>   Ref;
    typedef ISettingsService*   Ptr;

    static const unsigned int   INTERFACE_VERSION = 0x01;
    virtual ~ISettingsService() = default;

    //! @name Getters
    //! @{
    virtual bool        getBool( const char* groupName, const char* paramName, bool defVal = false ) = 0;
    virtual int         getInt( const char* groupName, const char* paramName, int defVal = 0 ) = 0;
    virtual float       getFloat( const char* groupName, const char* paramName, float defVal = 1.0f ) = 0;
    virtual const char* getString( const char* groupName, const char* paramName, const char* defVal = "" ) = 0;
    //! @}

    //! @name Setters
    //! @{
    virtual void    setBool( const char* groupName, const char* paramName, bool val ) = 0;
    virtual void    setInt( const char* groupName, const char* paramName, int val ) = 0;
    virtual void    setFloat( const char* groupName, const char* paramName, float val ) = 0;
    virtual void    setString( const char* groupName, const char* paramName, const char* val ) = 0;
    //! @}

protected:
    struct Param {
        enum class Type { BOOL, INT, FLOAT, STRING };
        Type        type;
        const char* name;
        union {
            bool    bVal;
            int     iVal;
            float   fVal;
            const char* str;        // char* or wchar_t* ?
        };
    };

    typedef std::list<Param*>       ParamList;
};

#endif /* ndef __ISETTINGSSERVICE_H__ */
/* EOF */