//
//  Implementation of ISettingsStorage methods by James Bremner
//
#include <iostream>
#include <sstream>
#include "isettingsservice.h"
#include "isettingsstorage.h"
#include "sqlite3.h"

ISettingsStorage::ISettingsStorage( const std::string& dbfname )
    : db( 0 )
    , mydbfname( dbfname )
{

}
void ISettingsStorage::open( ISettingsService* settings )
{
    if( db )
    {
        // the db has already been opened
        // ignore this call
        return;
    }
    mySettings = settings;

    int ret = sqlite3_open( mydbfname.c_str(),&db);
    if( ret )
        throw std::runtime_error("Database open failed");
    if( ! db )
        throw std::runtime_error("Database open failed");
}

void ISettingsStorage::close( void )
{
    if( db )
        sqlite3_close(db);
    db = 0;
}

void ISettingsStorage::loadAll( void )
{

}
void ISettingsStorage::saveAll( void )
{
    // NYI by specification

}

void ISettingsStorage::updateBool( const char* groupName, const char* paramName, bool val )
{
    Write( groupName, paramName, eType::vbool, val, 0, 0, "''" );
}
void ISettingsStorage::updateInt( const char* groupName, const char* paramName, int val )
{
    Write( groupName, paramName, eType::vint, 0, val, 0, "''" );
}
void ISettingsStorage::updateFloat( const char* groupName, const char* paramName, float val )
{
    Write( groupName, paramName, eType::vfloat, 0, 0, val, "''" );
}
void ISettingsStorage::updateString( const char* groupName, const char* paramName, const char* val )
{
    Write( groupName, paramName, eType::vstr, 0, 0, 0, val );
}

void ISettingsStorage::IsGroup( const char* groupName )
{
    if( ! db )
        return;

    std::string query;
    query  = "CREATE TABLE IF NOT EXISTS ";
    query += std::string ( groupName );
    query += " (  param PRIMARY KEY ON CONFLICT REPLACE, ";
    query += "val_type, bool_val, int_val, float_val, string_val );";

    //std::cout << query << std::endl;

    if( sqlite3_exec( db, query.c_str(), 0, 0, 0) != SQLITE_OK )
    {
        std::cout << "ERROR ISettingsStorage::IsGroup" << std::endl;
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
    query << float_val << ", ";
    query << string_val << " );";

    //std::cout << query.str() << std::endl;

    if( sqlite3_exec( db, query.str().c_str(), 0, 0, 0) != SQLITE_OK )
    {
        std::cout << "ERROR ISettingsStorage::Write" << std::endl;
    }
}

