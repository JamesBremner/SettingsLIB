//                                    .-""-.
//                                   / _  _ \
//                                   |(_)(_)|
// .------------------------------ooO(_ /\ _)Ooo-----------------------------.
// |                                  |====|                                 |
// |                                  '-..-'                                 |
// | Desc:     ISettingsStorage                                              |
// | By:       Nuroferatu                                                    |
// '-------------------------------------------------------------------------'
// ----= Change log =---------------------------------------------------------
//   1. 2017.11.11, 01:10    [+] Initial
// ---------------------------------------------------------------------------
#pragma once
#ifndef __ISETTINGSSTORAGE_H__
#define __ISETTINGSSTORAGE_H__

class ISettingsService;

// ---------------------------------------------------------------------------
// ISettingsStorage
// ---------------------------------------------------------------------------
class ISettingsStorage {
public:
    virtual ~ISettingsStorage() = default;

    //! Open connection to datasource. It may be file or database.
    //! Subclass constructor should accept required information like db name or file path.
    //! If data store does not exist, open mast create it.
    //! When open fails, it must throw subclass of std::runtime_error
    //! @todo define exception for ISettingsStorage::open
    virtual void open( ISettingsService* settings ) = 0;

    //! Close connection to already opened data source.
    //! If open failed, this method should not fail, just igonre the call
    virtual void close( void ) = 0;

    //! Read all settings already stored in datastore, and push them to ISettingsService
    virtual void loadAll( void ) = 0;

    //! File based datastore should use this to recreate settings file
    virtual void saveAll( void ) = 0;

    //! Any Database based data store may use this to update values in db. File data stores should ignore this.
    virtual void updateBool( const char* groupName, const char* paramName, bool val ) = 0;
    virtual void updateInt( const char* groupName, const char* paramName, int val ) = 0;
    virtual void updateFloat( const char* groupName, const char* paramName, float val ) = 0;
    virtual void updateString( const char* groupName, const char* paramName, const char* val ) = 0;
};

#endif /* ndef __ISETTINGSSTORAGE_H__ */
/* EOF */