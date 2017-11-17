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
    /* floats are stored in the database as doubles
    so this is just a wrapper for the updateDouble
    */
    updateDouble( groupName, paramName, (double)val);
}
void ISettingsStorage::updateDouble( const char* groupName, const char* paramName, double val )
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
                              bool bool_val, int int_val, double float_val, const char* string_val  )
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
    query << std::setprecision(10) << float_val << ", '";
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
        /* floats are stored in the database as doubles
    so this is just a wrapper for the readDouble
    */
    return (float) readDouble(groupName,paramName);
}
double ISettingsStorage::readDouble( const char* groupName, const char* paramName )
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
    std::string results;
    query  << "SELECT string_val FROM " << groupName << " WHERE param " << " = '" << paramName << "';";
    if( sqlite3_exec(
                     myDB,
                      query.str().c_str(),
                      +[](void* p,int n,char** r,char** names )
                     {
                         *(std::string*)p = *r;
                         return 0;
                     },
                       (void*)&results,
                        0) != SQLITE_OK )
    {
        std::string msg = "ISettingsStorage::ReadType ";
        msg += sqlite3_errmsg(myDB);
        throw std::runtime_error( msg );
    }

    return results;
}

std::vector< std::string > ISettingsStorage::loadGroups()
{
    std::vector< std::string > vGroupName;
    if( sqlite3_exec(
                myDB,
                "select name from sqlite_master where type='table';",
                +[](void* p,int n,char** r,char** names )
{
    ((std::vector< std::string >*)p)->push_back( *r );
        return 0;
    }
    ,
    (void*)&vGroupName
    , 0
            ) != SQLITE_OK )
    {
        std::string msg = "ISettingsStorage::loadAll ";
        msg += sqlite3_errmsg(myDB);
        throw std::runtime_error( msg );
    }

    return vGroupName;
}

std::vector< std::pair< std::string, ISettingsStorage::eType > >
ISettingsStorage::loadParams( const std::string& groupName )
{
    std::vector< std::pair< std::string, eType > > vParam;
    std::stringstream query;
    query << "SELECT param, val_type FROM "
          << groupName
          << ";";
    if( sqlite3_exec(
                myDB,
                query.str().c_str(),
                +[](void* p,int n,char** r,char** names )
{

    std::pair< std::string, eType > pt( *r, (eType)atoi( *(r+1) ) );
        ((std::vector< std::pair< std::string, eType > >*)p)->push_back( pt );
        return 0;
    },
    (void*)&vParam,
    0
            ) != SQLITE_OK )
    {
        std::string msg = "ISettingsStorage::loadAll ";
        msg += sqlite3_errmsg(myDB);
        throw std::runtime_error( msg );
    }
    return vParam;
}

void ISettingsStorage::loadAll( void )
{
    bool vBool;
    int vInt;
    double vFloat;
    std::string vString;

    // loop over groups in database
    for( auto& group : loadGroups() )
    {
        // loop over params in group
        for( auto& param_type : loadParams( group ) )
        {
            // display group and param
            std::string param = param_type.first;
            eType type        = param_type.second;
            std::cout << group <<" "
                      << param <<" = ";

            // extract value of stored type
            // display and pass to settings service
            switch( type )
            {
            case eType::tBool:
                vBool =    readBool( group.c_str(), param.c_str());
                //mySettings->setBool( group.c_str(), param.c_str(), vBool );
                std::cout << vBool;
                break;
            case eType::tInt:
                vInt =     readInt( group.c_str(), param.c_str());
                //mySettings->setInt( group.c_str(), param.c_str(), vInt );
                std::cout << vInt;
                break;
            case eType::tFloat:
                vFloat    = readDouble( group.c_str(), param.c_str());
                //mySettings->setFloat( group.c_str(), param.c_str(), vFloat );
                std::cout << std::setprecision(10) << vFloat;
                break;
            case eType::tString:
                vString  = readString( group.c_str(), param.c_str());
                //mySettings->setString( group.c_str(), param.c_str(), vString.c_str() );
                std::cout << vString;
                break;
            }
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}

