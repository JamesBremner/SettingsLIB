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
class sqlite3;

// ---------------------------------------------------------------------------
// ISettingsStorage
// ---------------------------------------------------------------------------
class ISettingsStorage {
public:

    /** Constructor

        @param[in] dbfname name of file to store the database
    */
    ISettingsStorage( const std::string& dbfname );

    virtual ~ISettingsStorage() = default;

    //! Open connection to datasource. It may be file or database.
    //! Subclass constructor should accept required information like db name or file path.
    //! If data store does not exist, open mast create it.
    //! When open fails, it must throw subclass of std::runtime_error
    //! @todo define exception for ISettingsStorage::open
    virtual void open( ISettingsService* settings );

    //! Close connection to already opened data source.
    //! If open failed, this method should not fail, just igonre the call
    virtual void close( void );

    //! Read all settings already stored in datastore, and push them to ISettingsService
    virtual void loadAll( void ) {}

    //! File based datastore should use this to recreate settings file
    virtual void saveAll( void ) {}

    //! Any Database based data store may use this to update values in db. File data stores should ignore this.
    //! std::runtime_error is thrown on error
    virtual void updateBool( const char* groupName, const char* paramName, bool val );
    virtual void updateInt( const char* groupName, const char* paramName, int val );
    virtual void updateFloat( const char* groupName, const char* paramName, float val );
    virtual void updateString( const char* groupName, const char* paramName, const char* val );

    int readInt( const char* groupName, const char* paramName );
    std::string readString( const char* groupName, const char* paramName );

private:
    std::string mydbfname;       ///< Name of database file
    sqlite3 * myDB;             ///< Pointer to database engine
    ISettingsService* mySettings;

    //  enumerator defining the various types that can be stored
    enum class eType
    {
        tBool,
        tInt,
        tFloat,
        tString
    };

    /** Write row to database table

        @param[in] groupName table name
        @param[in] paramName row name
        @param[in] type type of the value to be written
        @param[in] bool_val value, if bool type
        @param[in] int_val value, if integer type
        @param[in] int_float value, if float type
        @param[in] string_val value, if string type

        If a row with the paramName exists, then it is updated

        Exception thrown on error
    */
    void Write( const char* groupName, const char* paramName, eType type,
                              bool bool_val, int int_val, float float_val, const char* string_val  );

    /** Endure table exists in database

        @param[in] groupName table name

        If the table does not exist, and the name is legal ( not a reserver SQL keyword )
        then the table will be created.

        Exception thrown on error
    */

    void IsGroup( const char* groupName );

    void IsType( const char* groupName, const char* paramName, eType type );
};

#endif /* ndef __ISETTINGSSTORAGE_H__ */
/* EOF */
