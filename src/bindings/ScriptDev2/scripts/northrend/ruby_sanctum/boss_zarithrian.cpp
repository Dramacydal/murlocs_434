/* Copyright (C) 2010 /dev/rsa for ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: boss_zarithrian
SD%Complete: 90%
SDComment: by /dev/rsa && notagain
SDCategory: Ruby Sanctum
EndScriptData */

// Need correct timers

#include "precompiled.h"
#include "ruby_sanctum.h"

enum BossSpells
{
    SPELL_CALL_FLAMECALLER           = 74398,
    SPELL_CLEAVE_ARMOR               = 74367,
    SPELL_IMTIMIDATING_ROAR          = 74384,
    SPELL_LAVA_GOUT                  = 74394,
    SPELL_BLAST_NOVA                 = 74392,

    NPC_FLAMECALLER                  = 39814,
};

enum Equipment
{
    EQUIP_MAIN           = 47156,
    EQUIP_OFFHAND        = 51812,
    EQUIP_RANGED         = EQUIP_NO_CHANGE,
    EQUIP_DONE           = EQUIP_NO_CHANGE,
};

static Locations SpawnLoc[]=
{
    {3008.552734f, 530.471680f, 89.195290f},     // 0 - Zarithrian start point, o = 6,16
    {3014.313477f, 486.453735f, 89.255096f},    // 1 - Mob spawn 1
    {3025.324951f, 580.588501f, 88.593185f},    // 2 - Mob spawn 2
};

enum
{
    SAY_AGGRO           = -1666200,
    SAY_SLAY_1          = -1666201,
    SAY_SLAY_2          = -1666202,
    SAY_DEATH           = -1666203,
    SAY_SUMMON          = -1666204,
};

struct MANGOS_DLL_DECL boss_zarithrianAI : public BSWScriptedAI
{
    boss_zarithrianAI(Creature* pCreature) : BSWScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance *pInstance;

    void Reset()
    {
        if(!pInstance)
            return;

        if (m_creature->isAlive())
        {
            pInstance->SetData(TYPE_ZARITHRIAN, NOT_STARTED);
            resetTimers();
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            setStage(0);
        }
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (getStage())
            ScriptedAI::MoveInLineOfSight(pWho);

        if (!getStage() &&
             pInstance->GetData(TYPE_XERESTRASZA) == DONE &&
             pInstance->GetData(TYPE_BALTHARUS) == DONE &&
             pInstance->GetData(TYPE_RAGEFIRE) == DONE)
             {
                 m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                 setStage(1);
             };
    }

    void KilledUnit(Unit* pVictim)
    {
    switch (urand(0,1)) {
        case 0:
               DoScriptText(SAY_SLAY_1,m_creature,pVictim);
               break;
        case 1:
               DoScriptText(SAY_SLAY_2,m_creature,pVictim);
               break;
        };
    }

    void JustReachedHome()
    {
        if (!pInstance) return;
        pInstance->SetData(TYPE_ZARITHRIAN, FAIL);
    }

    void JustSummoned(Creature* summoned)
    {
        if(!pInstance || !summoned) return;

        summoned->SetInCombatWithZone();
        if (Unit* pTarget = doSelectRandomPlayerAtRange(180.0f))
        {
            summoned->AddThreat(pTarget, 100.0f);
            summoned->GetMotionMaster()->MoveChase(pTarget);
        }

    }

    void Aggro(Unit *who)
    {
        if(!pInstance) return;

        SetEquipmentSlots(false, EQUIP_MAIN, EQUIP_OFFHAND, EQUIP_RANGED);
        pInstance->SetData(TYPE_ZARITHRIAN, IN_PROGRESS);
        DoScriptText(SAY_AGGRO,m_creature);
    }

    void JustDied(Unit *killer)
    {
        if(!pInstance) return;

        pInstance->SetData(TYPE_ZARITHRIAN, DONE);
        DoScriptText(SAY_DEATH,m_creature);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (timedQuery(SPELL_CALL_FLAMECALLER, diff))
        {
            doSummon(NPC_FLAMECALLER, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
            doSummon(NPC_FLAMECALLER, SpawnLoc[2].x, SpawnLoc[2].y, SpawnLoc[2].z);

//            if (currentDifficulty == RAID_DIFFICULTY_25MAN_NORMAL
//                || currentDifficulty == RAID_DIFFICULTY_25MAN_HEROIC)
//                doCast(SPELL_CALL_FLAMECALLER);

            DoScriptText(SAY_SUMMON,m_creature);
        }

        timedCast(SPELL_CLEAVE_ARMOR, diff);
        timedCast(SPELL_IMTIMIDATING_ROAR, diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_zarithrian(Creature* pCreature)
{
    return new boss_zarithrianAI(pCreature);
};

struct MANGOS_DLL_DECL mob_flamecaller_rubyAI : public BSWScriptedAI
{
    mob_flamecaller_rubyAI(Creature *pCreature) : BSWScriptedAI(pCreature)
    {
        pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();
    }

    ScriptedInstance *pInstance;

    void Reset()
    {
        if(!pInstance) return;
        resetTimers();
        m_creature->SetRespawnDelay(7*DAY);
    }

    void UpdateAI(const uint32 diff)
    {

        if (pInstance && pInstance->GetData(TYPE_ZARITHRIAN) != IN_PROGRESS)
            m_creature->ForcedDespawn();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        doCastAll(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_flamecaller_ruby(Creature* pCreature)
{
    return new mob_flamecaller_rubyAI(pCreature);
};

void AddSC_boss_zarithrian()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_zarithian";
    newscript->GetAI = &GetAI_boss_zarithrian;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_flamecaller_ruby";
    newscript->GetAI = &GetAI_mob_flamecaller_ruby;
    newscript->RegisterSelf();
}
