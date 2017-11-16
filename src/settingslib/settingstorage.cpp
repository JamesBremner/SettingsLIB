//
//  Implementation of ISettingsStorage methods by James Bremner
//
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include "isettingsservice.h"
#include "isettingsstorage.h"
#include "sqlite3.h"

ISettingsStorage::ISettingsStorage( const std::string& dbfname )
    : myDB( 0 )
    , mydbfname( dbfname )
{

}
void ISettingsStorage::open( ISettingsService* settings )
{
    if( myDB )
    {
        // the db has already been opened
        // ignore this call
        return;
    }
    mySettings = settings;

    int ret = sqlite3_open( mydbfname.c_str(), &myDB );
    if( ret )
        throw std::runtime_error("Database open failed");
    if( ! myDB )
        throw std::runtime_error("Database open failed");
}

void ISettingsStorage::close( void )
{
    if( myDB )
        sqlite3_close(myDB);
    myDB = 0;
}

void ISettingsStorage::updateBool( const char* groupName, const char* paramName, bool val )
{
    Write( groupName, paramName, eType::tBool,  val, 0, 0, "''" );
}
void ISettingsStorage::updateInt( const char* groupName, const char* paramName, int val )
{
    Write( groupName, paramName, eType::tInt, 0, val, 0, "''" );
}
void ISettingsStorage::updateFloat( const char* groupName, const char* paramName, float val )
{
    Write( groupName, paramName, eType::tFloat, 0, 0, val, "''" );
}
void ISettingsStorage::updateString( const char* groupName, const char* paramName, const char* val )
{
    Write( groupName, paramName, eType::tString, 0, 0, 0, val );
}

void ISettingsStorage::IsGroup( const char* groupName )
{
    if( ! myDB )
        throw std::runtime_error( "Database not open" );

    std::string query;
    query  = "CREATE TABLE IF NOT EXISTS ";
    query += std::string ( groupName );
    query += " (  param PRIMARY KEY ON CONFLICT REPLACE, ";
    query += "val_type, bool_val, int_val, float_val, string_val );";

    //std::cout << query << std::endl;

    if( sqlite3_exec( myDB, query.c_str(), 0, 0, 0) != SQLITE_OK )
    {
        throw std::runtime_error( "ISettingsStorage::IsGroup" );
    }

}

void ISettingsStorage::Write( const char* groupName, const char* paramName, eType type,
                              bool bool_val, int int_val, float float_val, const char* string_val  )
{
    IsGroup( groupName );

    std::stringstream query;
    query  << "INSERT INTO ";
    query << std::string ( groupName );
    query << " VALUES ( ";
    query << "'" << paramName << "', ";
    query << (int) type << ", ";
    query << bool_val << ", ";
    query << int_val << ", ";
    query << float_val << ", '";
    query << string_val << "' );";

    //std::cout << query.str() << std::endl;

    if( sqlite3_exec( myDB, query.str().c_str(), 0, 0, 0) != SQLITE_OK )
    {
        std::string msg = "ISettingsStorage::Write ";
        msg += sqlite3_errmsg(myDB);
        throw std::runtime_error( msg  );
    }
}

static int callback (void* p,int n,char** r,char** names )
{
    strcpy( (char*)p, *r );
    return 0;
}
static int callbackstring (void* p,int n,char** r,char** names )
{
    // allocate storage for string read from db
    void * q = malloc( strlen( *r ) );
    // copy string returned from db
    strcpy( (char*)q, *r );
    // copy pointer to string
    *(char**)p = (char*)q;

    return 0;
}

void ISettingsStorage::IsType( const char* groupName, const char* paramName, eType type )
{
    std::stringstream query;
    query  << "SELECT val_type FROM " << groupName << " WHERE param " << " = '" << paramName << "';";
    char results[200];
    if( sqlite3_exec( myDB, query.str().c_str(), &callback, &results, 0) != SQLITE_OK )
    {
        std::string msg = "ISettingsStorage::ReadType ";
        msg += sqlite3_errmsg(myDB);
        throw std::runtime_error( msg );
    }

    if( atoi(results) != (int)type )
        throw std::runtime_error("ISettingsStorage::ReadType wrong type stored");
}

bool ISettingsStorage::readBool( const char* groupName, const char* paramName )
{
    IsType( groupName, paramName, eType::tBool );

    std::stringstream query;
    char results[200];
    query  << "SELECT bool_val FROM " << groupName << " WHERE param " << " = '" << paramName << "';";
    if( sqlite3_exec( myDB, query.str().c_str(), &callback, &results, 0) != SQLITE_OK )
    {
        std::string msg = "ISettingsStorage::ReadType ";
        msg += sqlite3_errmsg(myDB);
        throw std::runtime_error( msg );
    }

    return ( atoi(results) == 1 );
}

int ISettingsStorage::readInt( const char* groupName, const char* paramName )
{
    IsType( groupName, paramName, eType::tInt );

    std::stringstream query;
    char results[200];
    query  << "SELECT int_val FROM " << groupName << " WHERE param " << " = '" << paramName << "';";
    if( sqlite3_exec( myDB, query.str().c_str(), &callback, &results, 0) != SQLITE_OK )
    {
        std::string msg = "ISettingsStorage::ReadType ";
        msg += sqlite3_errmsg(myDB);
        throw std::runtime_error( msg );
    }
    return atoi( results );

}
float ISettingsStorage::readFloat( const char* groupName, const char* paramName )
{
    IsType( groupName, paramName, eType::tFloat );

    std::stringstream query;
    char results[200];
    query  << "SELECT float_val FROM " << groupName << " WHERE param " << " = '" << paramName << "';";
    if( sqlite3_exec( myDB, query.str().c_str(), &callback, &results, 0) != SQLITE_OK )
    {
        std::string msg = "ISettingsStorage::ReadType ";
        msg += sqlite3_errmsg(myDB);
        throw std::runtime_error( msg );
    }
    return atof( results );

}

std::string ISettingsStorage::readString( const char* groupName, const char* paramName )
{
    IsType( groupName, paramName, eType::tString );

    std::stringstream query;
    char * results;
    query  << "SELECT string_val FROM " << groupName << " WHERE param " << " = '" << paramName << "';";
    if( sqlite3_exec( myDB, query.str().c_str(), &callbackstring, &results, 0) != SQLITE_OK )
    {
        std::string msg = "ISettingsStorage::ReadType ";
        msg += sqlite3_errmsg(myDB);
        throw std::runtime_error( msg );
    }
    std::string ret( results );
    free( results );
    return ret;
}

