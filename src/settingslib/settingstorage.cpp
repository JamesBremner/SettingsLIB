//
//  Implementation of ISettingsStorage methods by James Bremner
//
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <cstring>

#include "isettingsservice.h"
#include "isettingsstorage.h"
#include "sqlite3.h"

/** Look after the details of executing an SQL query on the sqlite database engine

  Usage:

  <pre>
    cSQL sql;
    sql.open("test.dat");
    sql << "SELECT * FRON tablename;";
    int first_result = atoi( sql[0].c_str() );
  <pre>
*/

class cSQL
{
public:

    /// CTOR

    cSQL();

    /** Open the database
        @param[in] dbfname the name of the file where the database is stored
    */

    void Open( const std::string& dbfname );

    /// Close the database

    void Close();

    /** Execute a SQL query on the database
        @param[in] query the SQL query
    */

    void operator << ( const std::string& query );

    /** Access a result returned from the database
        @param[in] i zero-based index of result to return
        @return reference to string result at index

        The results are stores in co;lumn major order
        Meaning that all the columns in a row are adjacent
        So, if the rows contain 5 columns
        then the 3rd column of the 5th row
        will be at index 2*5 + 2
        column and row indices are zero-base

        Thows a std::runtime_error exception on error
        e.g. index out of bounds
    */
    const std::string& operator [] ( int i );

    /** Reference the results of the query
        @return a vector of strings containing the results in column-major order

        Throws std::runtime_error exception on error
    */
    const std::vector< std::string >&
    Result()
    {
        return myResult;
    }

private:
    sqlite3* db;
    std::string myQuery;
    std::vector< std::string > myResult;
};

cSQL SQL;


cSQL::cSQL()
    : db( 0 )
{

}

void  cSQL::Open( const std::string& dbfname )
{
    if( db )
        return;
    int ret = sqlite3_open( dbfname.c_str(), &db );
    if( ret || ( ! db ) )
    {
        std::string msg = "Open failed on database file ";
        msg += dbfname;
        throw std::runtime_error( msg );
    }
}

void  cSQL::Close()
{
    if( db )
        sqlite3_close( db );
    db = 0;
}

void cSQL::operator << ( const std::string& query )
{
    //std::cout << myQuery.str() << std::endl;

    if( ! db )
        throw std::runtime_error("cSQL::exec db not open");

    myResult.clear();

    int db_ret = sqlite3_exec(

                     // open db context
                     db,

                     // SQL query
                     query.c_str(),

                     // lambda function callback when row read
                     +[](void* p,int n,char** r,char** names )
    {
        std::vector< std::string >& rv = *(std::vector< std::string >*)p;
        for( int col = 0; col < n; col++ )
            rv.push_back( *r++ );
        // OK
        return 0;

        // end of lambda function callback when row read
    },

    // pointer to result vector
    (void*)&myResult,

    // unused
    0 );

    if( db_ret )
    {
        std::string msg = "cSQL::exec ";
        msg += sqlite3_errmsg(db);
        throw std::runtime_error( msg  );
    }

}

const std::string& cSQL::operator [] (int i )
{
    if( 0 > i || i >= (int) myResult.size() )
        throw std::runtime_error( "cSQL out of range result requested" );
    return myResult[ i ];
}

ISettingsStorage::ISettingsStorage( const std::string& dbfname )
    : mydbfname( dbfname )
{

}
void ISettingsStorage::open( ISettingsService* settings )
{
    SQL.Open( mydbfname );
}

void ISettingsStorage::close( void )
{
    SQL.Close();
}

void ISettingsStorage::updateBool( const char* groupName, const char* paramName, bool val )
{
    update( ISettingsParam( groupName, paramName, val )  );
}
void ISettingsStorage::updateInt( const char* groupName, const char* paramName, int val )
{
    update( ISettingsParam( groupName, paramName, val )  );
}
void ISettingsStorage::updateFloat( const char* groupName, const char* paramName, float val )
{
    update( ISettingsParam( groupName, paramName, val )  );
}
void ISettingsStorage::updateDouble( const char* groupName, const char* paramName, double val )
{
    update( ISettingsParam( groupName, paramName, val )  );
}
void ISettingsStorage::updateString( const char* groupName, const char* paramName, const char* val )
{
    update( ISettingsParam( groupName, paramName, val )  );
}

void ISettingsStorage::update( const ISettingsParam& param )
{
    // Ensure group table exists

    std::string q = "CREATE TABLE IF NOT EXISTS "
                    + param.myGroup
                    + " (  param PRIMARY KEY ON CONFLICT REPLACE, val_type, bool_val, int_val, float_val, string_val );";

    SQL << q;

    // build SQL query, according to type

    std::stringstream query;
    query  << "INSERT INTO ";
    query << param.myGroup;
    query << " VALUES ( ";
    query << "'" << param.myName << "', ";
    query << (int) param.type << ", ";

    switch( param.type )
    {
    case ISettingsParam::eType::tBool:
        query <<  (int) param.bVal << ", ";
        query << ", 0, 0,'""' );";
        break;
    case ISettingsParam::eType::tInt:
        query <<  "0, ";
        query << param.iVal << ", ";
        query <<  "0, '""' );";
        break;
    case ISettingsParam::eType::tFloat:
        query <<  "0, 0, ";
        query << param.fVal << ", ";
        query <<  "'""' );";
        break;
    case ISettingsParam::eType::tString:
        query <<  "0, 0, 0, '";
        query << param.sVal;
        query << "' );";
    }

    //std::cout << query.str() << std::endl;

    SQL << query.str();
}

void ISettingsStorage::read( ISettingsParam& param )
{
    std::string query =
        " SELECT val_type, bool_val, int_val, float_val, string_val FROM " + param.myGroup
        + " WHERE param = '" + param.myName + "';";
    SQL << query;

    param.type = (ISettingsParam::eType)atoi( SQL[0].c_str() );

    // copy value from db into param according to type
    switch( param.type )
    {
    case ISettingsParam::eType::tBool:
        param.bVal = atoi( SQL[1].c_str() );
        break;
    case ISettingsParam::eType::tInt:
        param.iVal = atoi( SQL[2].c_str() );
        break;
    case ISettingsParam::eType::tFloat:
        param.fVal = atof( SQL[3].c_str() );
        break;
    case ISettingsParam::eType::tString:
        param.sVal = SQL[4];
        break;
    }
}

std::vector< std::string > ISettingsStorage::loadGroups()
{
    SQL << "SELECT name FROM sqlite_master WHERE type='table';";
    return SQL.Result();
}

std::vector< std::string >
ISettingsStorage::loadParams( const std::string& groupName )
{
    SQL << "SELECT param FROM " + groupName + ";";
    return SQL.Result();
}

void ISettingsStorage::loadAll( void )
{
    // loop over groups in database
    for( auto& group : loadGroups() )
    {
        // loop over params in group
        for( auto& param : loadParams( group ) )
        {
            // read parameter from db

            ISettingsParam result( group, param );
            read( result );

            // display

            std::cout << result.Text();

            // pass to settings service
            switch( result.type )
            {
            case ISettingsParam::eType::tBool:
                //mySettings->setBool( group.c_str(), param.c_str(), result.bVal );
                break;
            case ISettingsParam::eType::tInt:
                //mySettings->setInt( group.c_str(), param.c_str(), result.iVal );
                break;
            case ISettingsParam::eType::tFloat:
                //mySettings->setFloat( group.c_str(), param.c_str(), result.fVal );
                break;
            case ISettingsParam::eType::tString:
                //mySettings->setString( group.c_str(), param.c_str(), result.sVal.c_str() );
                break;
            }
        }
    }
}

std::string ISettingsParam::Text()
{
    std::stringstream ss;
    ss << myGroup << ", " << myName << " = ";
    switch( type )
    {
    case eType::tBool:
        ss << bVal;
        break;
    case eType::tInt:
        ss << iVal;
        break;
    case eType::tFloat:
        ss << fVal;
        break;
    case eType::tString:
        ss << sVal;
        break;
    }
    ss << "\n";
    return ss.str();
}
