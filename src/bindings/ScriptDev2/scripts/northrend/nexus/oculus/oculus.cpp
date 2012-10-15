/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: oculus
SD%Complete: ?%
SDComment: by /dev/rsa
SDCategory: Oculus - mobs and special
EndScriptData */

#include "precompiled.h"
#include "oculus.h"
enum Spells
{
    SPELL_GREEN_SEAT                        = 49346,
    SPELL_YELLOW_SEAT                       = 49460,
    SPELL_RED_SEAT                          = 49464,
};

enum NPC
{
    NPC_GREEN_DRAGON                        = 27692,
    NPC_YELLOW_DRAGON                       = 27755,
    NPC_RED_DRAGON                          = 27756,
};

struct MANGOS_DLL_DECL mob_oculus_dragonAI : public ScriptedAI
{
    mob_oculus_dragonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    bool Active;
    ObjectGuid ownerGUID;
    uint32 seatSpell;
    uint32 StartTimer;

    void Reset()
    {
        Active = false;
        ownerGUID = ObjectGuid();
        StartTimer = 2000;
        switch (m_creature->GetEntry())
        {
            case NPC_GREEN_DRAGON:
                seatSpell = SPELL_GREEN_SEAT;
                break;
            case NPC_RED_DRAGON:
                seatSpell = SPELL_RED_SEAT;
                break;
            case NPC_YELLOW_DRAGON:
                seatSpell = SPELL_YELLOW_SEAT;
                break;
            default:
                seatSpell = 0;
                break;
        }
        ownerGUID = m_creature->GetCreatorGuid();

        if (Unit* owner = m_creature->GetMap()->GetUnit(ownerGUID))
            owner->RemoveAurasDueToSpell(53797);
    }

    void AttackStart(Unit *) {}
    void MoveInLineOfSight(Unit*) {}

    void JustSummoned(){}

    void JustDied(Unit* killer)
    {
        if (!m_creature || m_creature->GetTypeId() != TYPEID_UNIT)
            return;

        if (ownerGUID.IsEmpty())
            ownerGUID = m_creature->GetCreatorGuid();

        Unit* owner = m_creature->GetMap()->GetUnit(ownerGUID);

        if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
            return;

        owner->RemoveAurasDueToSpell(seatSpell);
        owner->RemoveAurasDueToSpell(53797);
        m_creature->SetCreatorGuid(ObjectGuid());
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE && uiPointId == 0)
            return;

        if (Unit* owner = m_creature->GetMap()->GetUnit(ownerGUID))
        {
             m_creature->setFaction(owner->getFaction());
             owner->CastSpell(m_creature, seatSpell, true);
             owner->CastSpell(owner, 53797, true);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {

        if (ownerGUID.IsEmpty())
            ownerGUID = m_creature->GetCreatorGuid();

        if (!ownerGUID.IsEmpty())
        {
            if (StartTimer < uiDiff && !Active)
            {
               if (Unit* owner = m_creature->GetMap()->GetUnit(ownerGUID))
               {
                   float x, y, z;
                   owner->GetClosePoint(x, y, z, owner->GetObjectBoundingRadius(), 1.0f, owner->GetAngle(m_creature));
                   m_creature->GetMotionMaster()->MovePoint(0, x, y, z);
                   Active = true;
               }
            }
            else
                StartTimer -= uiDiff;
        }
        else
            if (StartTimer < uiDiff)
                m_creature->ForcedDespawn();
    }
};

CreatureAI* GetAI_mob_oculus_dragon(Creature* pCreature)
{
    return new mob_oculus_dragonAI(pCreature);
}

/*#####
# npc_robot
#####*/

struct MANGOS_DLL_DECL npc_oculus_robotAI : public ScriptedAI
{
    npc_oculus_robotAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset(){}

    void JustDied(Unit* pKiller)
    {
        if(m_pInstance)
            m_pInstance->SetData(TYPE_ROBOTS, 1);
    }

    void UpdateAI(const uint32 diff)
    {
        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_oculus_robot(Creature* pCreature)
{
    return new npc_oculus_robotAI (pCreature);
}

struct MANGOS_DLL_DECL npc_belgar_imageAI : public ScriptedAI
{
    npc_belgar_imageAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset(){}

    void UpdateAI(const uint32 diff)
    {
        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
    }
};

CreatureAI* GetAI_npc_belgar_image(Creature* pCreature)
{
    return new npc_belgar_imageAI (pCreature);
}

void AddSC_oculus()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "mob_oculus_dragon";
    newscript->GetAI = &GetAI_mob_oculus_dragon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_oculus_robot";
    newscript->GetAI = &GetAI_npc_oculus_robot;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_belgar_image";
    newscript->GetAI = &GetAI_npc_belgar_image;
    newscript->RegisterSelf();
}