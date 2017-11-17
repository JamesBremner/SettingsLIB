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
    Update( ISettingsParam( groupName, paramName, val )  );
}
void ISettingsStorage::updateInt( const char* groupName, const char* paramName, int val )
{
    Update( ISettingsParam( groupName, paramName, val )  );
}
void ISettingsStorage::updateFloat( const char* groupName, const char* paramName, float val )
{
    Update( ISettingsParam( groupName, paramName, val )  );
}
void ISettingsStorage::updateDouble( const char* groupName, const char* paramName, double val )
{
    Update( ISettingsParam( groupName, paramName, val )  );
}
void ISettingsStorage::updateString( const char* groupName, const char* paramName, const char* val )
{
    Update( ISettingsParam( groupName, paramName, val )  );
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
        std::string msg = "ISettingsStorage::IsGroup ";
        msg += sqlite3_errmsg(myDB);
        throw std::runtime_error( msg  );
    }

}

void ISettingsStorage::Update( const ISettingsParam& param )
{
    // Ensure group table exists

    IsGroup( param.myGroup.c_str() );

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

    // execute SQL query to update database

    if( sqlite3_exec( myDB, query.str().c_str(), 0, 0, 0) != SQLITE_OK )
    {
        std::string msg = "ISettingsStorage::Update ";
        msg += sqlite3_errmsg(myDB);
        throw std::runtime_error( msg  );
    }
}

void ISettingsStorage::read( ISettingsParam& param )
{
    // read parameter type
    param.type = ISettingsParam::eType::tNone;
    std::stringstream query;
    query  << "SELECT val_type FROM " << param.myGroup
           << " WHERE param " << " = '" << param.myName << "';";
    if( sqlite3_exec(
                myDB,
                query.str().c_str(),
                +[](void* p,int n,char** r,char** names )
{
    *(ISettingsParam::eType*)p = (ISettingsParam::eType)atoi(*r);
        return 0;
    },
    (void*)&param.type,
    0) != SQLITE_OK )
    {
        std::string msg = "ISettingsStorage::read ";
        msg += sqlite3_errmsg(myDB);
        throw std::runtime_error( msg );
    }
    if( param.type == ISettingsParam::eType::tNone )
        throw std::runtime_error("cannot read parameter");

    // build query to read parameter value
    query.str("");
    query << "SELECT ";
    switch( param.type )
    {
    case ISettingsParam::eType::tBool:
        query << "bool_val ";
        break;
    case ISettingsParam::eType::tInt:
        query << "int_val ";
        break;
    case ISettingsParam::eType::tFloat:
        query << "float_val ";
        break;
    case ISettingsParam::eType::tString:
        query << "string_val ";
        break;
    default:
        throw std::runtime_error("ISettingsStorage::read unrecognized type");
    }
    query << "FROM " << param.myGroup
          << " WHERE param = '" << param.myName
          << "';";

    // std::cout <<"\n" << query.str() << std::endl;

    // execute query in database engine

    if( sqlite3_exec(
                myDB,
                query.str().c_str(),
                +[](void* p,int n,char** r,char** names )
{
    switch( ((ISettingsParam*)p)->type )
        {
        case ISettingsParam::eType::tBool:
            ((ISettingsParam*)p)->bVal = atoi( *r );
            break;
        case ISettingsParam::eType::tInt:
            ((ISettingsParam*)p)->iVal = atoi( *r );
            break;
        case ISettingsParam::eType::tFloat:
            ((ISettingsParam*)p)->fVal = atof( *r );
            break;
        case ISettingsParam::eType::tString:
            ((ISettingsParam*)p)->sVal = *r;
            break;
        }
        return 0;
    },
    (void*)&param,
    0) != SQLITE_OK )
    {
        std::string msg = "ISettingsStorage::read ";
        msg += sqlite3_errmsg(myDB);
        throw std::runtime_error( msg );
    }


}

static int callback (void* p,int n,char** r,char** names )
{
    strcpy( (char*)p, *r );
    return 0;
}

void ISettingsStorage::IsType( const char* groupName, const char* paramName, ISettingsParam::eType type )
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

std::vector< std::string >
ISettingsStorage::loadParams( const std::string& groupName )
{
    std::vector< std::string > vParam;
    std::stringstream query;
    query << "SELECT param FROM "
          << groupName
          << ";";
    if( sqlite3_exec(
                myDB,
                query.str().c_str(),
                +[](void* p,int n,char** r,char** names )
{
        ((std::vector< std::string >*)p)->push_back( *r );
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
                //mySettings->setBool( group.c_str(), param.c_str(), vBool );
                break;
            case ISettingsParam::eType::tInt:
                //mySettings->setInt( group.c_str(), param.c_str(), vInt );
                break;
            case ISettingsParam::eType::tFloat:
                //mySettings->setFloat( group.c_str(), param.c_str(), vFloat );
                break;
            case ISettingsParam::eType::tString:
                //mySettings->setString( group.c_str(), param.c_str(), vString.c_str() );
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
