/* DatabaseEnv.h
 *
 * Copyright (C) 2004 Wow Daemon
 * Copyright (C) 2005 MaNGOS <https://opensvn.csie.org/traccgi/MaNGOS/trac.cgi/>
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

#if !defined(DATABASEENV_H)
#define DATABASEENV_H

//! Other libs we depend on.
#include "Common.h"
#include "Singleton.h"
#include "Log.h"

//! Our own includes.
#include "Database/DBCStores.h"
#include "Database/Field.h"
#include "Database/QueryResult.h"
#include "Database/QueryResultMysql.h"
#include "Database/QueryResultSqlite.h"
#include "Database/Database.h"
#include "Database/DatabaseMysql.h"
#include "Database/DatabaseSqlite.h"
#endif