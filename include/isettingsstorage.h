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

/**
   Settings parameters of various types
*/
class ISettingsParam
{
public:

    std::string myGroup;          // group to which param belongs

    std::string myName;           // name of param

    enum class eType
    {
        tNone,
        tBool,
        tInt,
        tFloat,
        tString
    } type;                     // type of param

    union                       // the value of the param
    {
        bool    bVal;
        int     iVal;
        float   fVal;
    };
    std::string sVal;

    /** CTOR for boolean type */
    ISettingsParam(
        const std::string& group,
        const std::string& name,
        int val )
        : myGroup( group )
        , myName( name )
        , type( eType::tInt )
        , iVal( val )
    {

    }
    /** CTOR for string type */
    ISettingsParam(
        const std::string& group,
        const std::string& name,
        const std::string& val )
        : myGroup( group )
        , myName( name )
        , type( eType::tString )
        , sVal( val )
    {

    }

    /** CTOR for float type */
    ISettingsParam(
        const std::string& group,
        const std::string& name,
        double val )
        : myGroup( group )
        , myName( name )
        , type( eType::tFloat )
        , fVal( val )
    {

    }

    /** CTOR for empty param */
    ISettingsParam(
        const std::string& group,
        const std::string& name )
        : myGroup( group )
        , myName( name )
        {

        }

    ~ISettingsParam() {}

    std::string Text();
};


// ---------------------------------------------------------------------------
// ISettingsStorage
// ---------------------------------------------------------------------------
class ISettingsStorage
{
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

    /** Insert or update a parameter value in the database
        @param[in] param  the parameter to be inserted or updated

        If a parameter of the same group and name already exists, it is replaced

        On error, a std::runtime_error exception is thrown
    */
    void Update( const ISettingsParam& param );

    void read( ISettingsParam& param );

    //! Any Database based data store may use this to update values in db. File data stores should ignore this.
    //! std::runtime_error is thrown on error
    void updateBool( const char* groupName, const char* paramName, bool val );
    void updateInt( const char* groupName, const char* paramName, int val );
    void updateFloat( const char* groupName, const char* paramName, float val );
    void updateDouble( const char* groupName, const char* paramName, double val );
    void updateString( const char* groupName, const char* paramName, const char* val );

private:

    std::string mydbfname;       ///< Name of database file
    sqlite3 * myDB;             ///< Pointer to database engine
    ISettingsService* mySettings;

    //  enumerator defining the various types that can be stored
//    enum class eType
//    {
//        tNone,
//        tBool,
//        tInt,
//        tFloat,
//        tString
//    };

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
    void IsType( const char* groupName, const char* paramName, ISettingsParam::eType type );

    /** Groups in the database
        @return vector of all groups in dartabase
    */
    std::vector< std::string > loadGroups();

    /** Params in group
        @param[in] groupName
        @return vector of names of params in group
    */
    std::vector< std::string > loadParams( const std::string& groupName );
};

#endif /* ndef __ISETTINGSSTORAGE_H__ */
/* EOF */
