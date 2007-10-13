/* 
 * Copyright (C) 2005,2006,2007 MaNGOS <http://www.mangosproject.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/** \file
    \ingroup realmd
*/

#include "Common.h"
#include "RealmList.h"
#include "Policies/SingletonImp.h"
#include "Database/DatabaseEnv.h"

INSTANTIATE_SINGLETON_1( RealmList );

#ifdef DO_POSTGRESQL
extern DatabasePostgre dbRealmServer;
#else
extern DatabaseMysql dbRealmServer;
#endif

RealmList::RealmList( ) : m_UpdateInterval(0), m_NextUpdateTime(time(NULL))
{
}

/// Load the realm list from the database
void RealmList::Initialize(uint32 updateInterval)
{
    m_UpdateInterval = updateInterval;

    ///- Get the content of the realmlist table in the database
    UpdateRealms(true);
}

void RealmList::UpdateRealm( uint32 ID, const char *name, const char *address, uint32 port, uint8 icon, uint8 color, uint8 timezone, AccountTypes allowedSecurityLevel)
{
    ///- Create new if not exist or update existed
    Realm& realm = m_realms[name];

    realm.m_ID      = ID;
    realm.name      = name;
    realm.icon      = icon;
    realm.color     = color;
    realm.timezone  = timezone;
    realm.allowedSecurityLevel = allowedSecurityLevel;

    ///- Append port to IP address.
    std::string addr = address ? address : "";
    std::ostringstream ss;
    ss << addr << ":" << port;
    realm.address   = ss.str();
}

void RealmList::UpdateIfNeed()
{
    // maybe disabled or updated recently
    if(!m_UpdateInterval || m_NextUpdateTime > time(NULL))
        return;

    m_NextUpdateTime = time(NULL) + m_UpdateInterval;

    // Clears Realm list
    m_realms.clear();

    // Get the content of the realmlist table in the database
    UpdateRealms(false);
}

void RealmList::UpdateRealms(bool init)
{
    sLog.outDetail("Updating Realm List...");

    QueryResult *result = dbRealmServer.Query( "SELECT `id`, `name`,`address`,`port`,`icon`,`color`,`timezone`,`allowedSecurityLevel` FROM `realmlist` WHERE `color` != '3' ORDER BY `name`" );

    ///- Circle through results and add them to the realm map
    if(result)
    {
        do
        {
            Field *fields = result->Fetch();

            uint8 allowedSecurityLevel = fields[7].GetUInt8();

            UpdateRealm(fields[0].GetUInt32(), fields[1].GetString(),fields[2].GetString(),fields[3].GetUInt32(),fields[4].GetUInt8(), fields[5].GetUInt8(), fields[6].GetUInt8(), (allowedSecurityLevel <= SEC_ADMINISTRATOR ? AccountTypes(allowedSecurityLevel) : SEC_ADMINISTRATOR) );
            if(init)
                sLog.outString("Added realm \"%s\".", fields[1].GetString());
        } while( result->NextRow() );
        delete result;
    }
}