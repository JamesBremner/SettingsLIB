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

#include <vector>

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

    ~ISettingsStorage() = default;

    //! Open connection to datasource. It may be file or database.
    //! Subclass constructor should accept required information like db name or file path.
    //! If data store does not exist, open mast create it.
    //! When open fails, it must throw subclass of std::runtime_error
    //! @todo define exception for ISettingsStorage::open
     void open( ISettingsService* settings );

    //! Close connection to already opened data source.
    //! If open failed, this method should not fail, just igonre the call
     void close( void );

    //! Read all settings already stored in datastore, and push them to ISettingsService
     void loadAll( void );

    //! File based datastore should use this to recreate settings file
     void saveAll( void ) {}

    //! Any Database based data store may use this to update values in db. File data stores should ignore this.
    //! std::runtime_error is thrown on error
     void updateBool( const char* groupName, const char* paramName, bool val );
     void updateInt( const char* groupName, const char* paramName, int val );
     void updateFloat( const char* groupName, const char* paramName, float val );
     void updateDouble( const char* groupName, const char* paramName, double val );
     void updateString( const char* groupName, const char* paramName, const char* val );

    //! Read bool from database
    bool readBool( const char* groupName, const char* paramName );

    //! Read integer from database
    int readInt( const char* groupName, const char* paramName );

    //! Read float from database
    float readFloat( const char* groupName, const char* paramName );

    double readDouble( const char* groupName, const char* paramName );

    /** Read string stored in db
        @param[in] groupName
        @param[in] paramName
        @return string read from database

        std::runtime_error is thrown on error

        A string is returned so that we do not have to worry about allocating/freeing memory
    */
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
                              bool bool_val, int int_val, double float_val, const char* string_val  );

    /** Ensure table exists in database

        @param[in] groupName table name

        If the table does not exist, and the name is legal ( not a reserver SQL keyword )
        then the table will be created.

        Exception thrown on error
    */

    void IsGroup( const char* groupName );

    /** Check that value stored in database row is of expected type
        @param[in] groupName
        @param[in] paramName
        @param[in] type expected

        If the type is not as expected, an exception is thrown
    */
    void IsType( const char* groupName, const char* paramName, eType type );

    /** Groups in the database
        @return vector of all groups in dartabase
    */
    std::vector< std::string > loadGroups();

    /** Params in group
        @param[in] groupName
        @return vector of pairs, each pair contains name and storage type of param
    */
    std::vector< std::pair< std::string, eType > > loadParams( const std::string& groupName );
};

#endif /* ndef __ISETTINGSSTORAGE_H__ */
/* EOF */
