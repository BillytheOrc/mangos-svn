/* ObjectGridLoader.h
 *
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

#ifndef MANGOS_OBJECTGRIDLOADER_H
#define MANGOS_OBJECTGRIDLOADER_H

#include "Player.h"
#include "GameObject.h"
#include "Creature.h"
#include "Utilities/TypeList.h"
#include "Platform/Define.h"
#include "GameSystem/Grid.h"
#include "GameSystem/GridLoader.h"



typedef TYPELIST_2(GameObject, Creature)    AllObjectTypes;

/*
 * @class ObjectGridLoader class implements a visitor patter for the ContainerMapList
 * becuase that's the container used for storing both GameObjects and Creatures
 * what's in the grid.
 */

typedef Grid<Player, AllObjectTypes> GridType;

class MANGOS_DLL_DECL ObjectGridLoader
{
public:
    ObjectGridLoader(GridType &grid) : i_grid(grid) {}
    void Visit(std::map<OBJECT_HANDLE, GameObject *> &m);
    void Visit(std::map<OBJECT_HANDLE, Creature *> &m);

private:
    GridType &i_grid;
};

/*
 * @class ObjectGridUnloader also implements the visitor pattern
 * for unloading the grid.
 */
class MANGOS_DLL_DECL ObjectGridUnloader
{
public:
    ObjectGridUnloader(GridType &grid, Player *pl) : i_grid(grid) {}

    void Visit(std::map<OBJECT_HANDLE, GameObject *> &m);
    void Visit(std::map<OBJECT_HANDLE, Creature *> &m);

private:
    GridType &i_grid;
};

typedef GridLoader<Player, AllObjectTypes, ObjectGridLoader, ObjectGridUnloader> GridLoaderType;

#endif
