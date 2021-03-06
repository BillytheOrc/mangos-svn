/* 
 * Copyright (C) 2005 MaNGOS <http://www.magosproject.org/>
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

#include "AggressorAI.h"
#include "Errors.h"
#include "Creature.h"
#include "Player.h"
#include "Utilities.h"
#include "FactionTemplateResolver.h"
#include "TargetedMovementGenerator.h"
#include "Database/DataStore.h"


#define TIME_INTERVAL_LOOK   5000

using MaNGOS::Utilities::is_in_line_of_sight;

int
AggressorAI::Permissible(const Creature *creature)
{
    FactionTemplateEntry *fact = sFactionTemplateStore.LookupEntry(creature->GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE));
    FactionTemplateResolver fact_source(fact);
    if( fact_source.IsHostileToAll() )
	return PROACTIVE_PERMIT_BASE;

    return NO_PERMIT;
}


AggressorAI::AggressorAI(Creature &c) : i_creature(c), i_pVictim(NULL), i_myFaction(sFactionTemplateStore.LookupEntry(c.GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE))), i_state(STATE_NORMAL), i_tracker(TIME_INTERVAL_LOOK)
{
}

void 
AggressorAI::MoveInLineOfSight(Unit *u) 
{
    if( i_pVictim == NULL )
    {
	FactionTemplateEntry *your_faction = sFactionTemplateStore.LookupEntry(u->GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE));
	if( i_myFaction.IsHostileTo( your_faction ) )
	    _taggedToKill(u);
    }
}

void 
AggressorAI::AttackStart(Unit *u) 
{
    if( i_pVictim == NULL )
	_taggedToKill(u);
}

void 
AggressorAI::AttackStop(Unit *) 
{
    
}

void 
AggressorAI::HealBy(Unit *healer, uint32 amount_healed) 
{
}

void 
AggressorAI::DamageInflict(Unit *healer, uint32 amount_healed) 
{
}

bool
AggressorAI::IsVisible(Unit *pl) const 
{
    return _isVisible(pl);
}

bool
AggressorAI::_needToStop() const
{
    if( !i_pVictim->isAlive() || !i_creature.isAlive() )
	return true;
    return !(_isVisible(i_pVictim));
}

void
AggressorAI::_stopAttack()
{
    assert( i_pVictim != NULL );
    i_creature.ClearState(ATTACKING);
    i_creature.RemoveFlag(UNIT_FIELD_FLAGS, 0x80000 );

    if( !i_creature.isAlive() )
    {
	DEBUG_LOG("Creature stoped attacking cuz his dead [guid=%s]", i_creature.GetGUIDLow());
	i_creature.StopMoving();
	i_creature->Idle();
	i_pVictim = NULL;
    }
    else if( !i_pVictim->isAlive() )
    {
	DEBUG_LOG("Creature stopped attacking cuz his victim is dead [guid=%d]", i_creature.GetGUIDLow());
	i_pVictim = NULL;
	static_cast<TargetedMovementGenerator *>(i_creature->top())->TargetedHome(i_creature); 
    }
    else 
    {
	DEBUG_LOG("Creature stopped attacking due to target %s [guid=%d]", i_pVictim->isAlive() ? "out run him" : "is dead", i_creature.GetGUIDLow());
	i_creature.StopMoving();
	i_creature->Idle();
	i_state = STATE_LOOK_AT_VICTIM;
	i_tracker.Reset(TIME_INTERVAL_LOOK);
    }    
}


void
AggressorAI::UpdateAI(const uint32 diff)
{
    if( i_pVictim != NULL )
    {
	switch( i_state )
	{
	case STATE_LOOK_AT_VICTIM:
	    {
		if( _isVisible(i_pVictim) )
		{
		    DEBUG_LOG("Victim %d re-enters creature's aggro radius fater stop attacking", i_pVictim->GetGUIDLow());
		    i_state = STATE_NORMAL;
		    i_creature->MovementExpired();
		    
		}

		i_tracker.Update(diff);
		if( i_tracker.Passed() )
		{
		    i_creature->MovementExpired();
		    DEBUG_LOG("Creature running back home [guid=%d]", i_creature.GetGUIDLow());
		    static_cast<TargetedMovementGenerator *>(i_creature->top())->TargetedHome(i_creature); 
		    i_state = STATE_NORMAL;
		    i_pVictim = NULL;
		}
		else
		{
		    
		    float dx = i_pVictim->GetPositionX() - i_creature.GetPositionX();
		    float dy = i_pVictim->GetPositionY() - i_creature.GetPositionY();
		    float orientation = (float)atan2((double)dy, (double)dx);
		    i_creature.Relocate(i_creature.GetPositionX(), i_creature.GetPositionY(), i_creature.GetPositionZ(), orientation);
		}

		break;
	    }
	case STATE_NORMAL:
	    {
		if( _needToStop() )
		{
		    DEBUG_LOG("Aggressor AI stoped attacking [guid=%d]", i_creature.GetGUIDLow());
		    _stopAttack();
		}
		else if( i_creature.IsStopped() )
		{
		    if( i_creature.isAttackReady() )
		    {
			i_creature.AttackerStateUpdate(i_pVictim, 0);
			i_creature.setAttackTimer(0); 

			if( !i_creature.isAlive() || !i_pVictim->isAlive() )
			    _stopAttack();
		    }
		}
		break;
	    }
	default:
	    break;
	} 
    } 
}

bool
AggressorAI::_isVisible(Unit *u) const
{
    return is_in_line_of_sight(i_creature.GetPositionX(), i_creature.GetPositionY(), i_creature.GetPositionZ(),
			       u->GetPositionX(), u->GetPositionY(), u->GetPositionZ(), 1.0);
}

void
AggressorAI::_taggedToKill(Unit *u)
{
    assert( i_pVictim == NULL );
    DEBUG_LOG("Creature %s tagged a victim to kill [guid=%d]", i_creature.GetName(), u->GetGUIDLow());
    i_creature.SetState(ATTACKING);
    i_creature.SetFlag(UNIT_FIELD_FLAGS, 0x80000);
    i_creature->Mutate(new TargetedMovementGenerator(*u));
    i_pVictim = u;
}
