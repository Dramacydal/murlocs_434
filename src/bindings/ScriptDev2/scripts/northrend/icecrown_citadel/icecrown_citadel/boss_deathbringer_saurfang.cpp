/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: boss_deathbringer_saurfang
SD%Complete: 75%
SDComment: by /dev/rsa
SDCategory: Icecrown Citadel
EndScriptData */
#include "precompiled.h"
#include "icecrown_citadel.h"

enum
{
    //common
    SPELL_BERSERK                           = 47008,
    //yells
    //summons
    NPC_BLOOD_BEASTS                        = 38508,
    //Abilities
    SPELL_BLOOD_LINK                        = 72178,
    SPELL_BLOOD_POWER                       = 72371,
    SPELL_MARK                              = 72293,
    SPELL_MARK_SELF                         = 72256,
    SPELL_FRENZY                            = 72737,
    SPELL_BOILING_BLOOD                     = 72385,
    SPELL_BLOOD_NOVA                        = 72380,
    SPELL_RUNE_OF_BLOOD                     = 72408,
    SPELL_CALL_BLOOD_BEAST_1                = 72172,
    SPELL_CALL_BLOOD_BEAST_2                = 72173,
    SPELL_CALL_BLOOD_BEAST_3                = 72356,
    SPELL_CALL_BLOOD_BEAST_4                = 72357,
    SPELL_CALL_BLOOD_BEAST_5                = 72358,

    SPELL_SCENT_OF_BLOOD                    = 72769,
    SPELL_RESISTANT_SKIN                    = 72723,
    SPELL_BLOOD_LINK_BEAST                  = 72176,

    SPELL_ZERO_REGEN                        = 72242,

    SPELL_ACHIEVEMENT_DEATHBRINGER_SAURFANG = 72928,

};

enum Equipment
{
    EQUIP_MAIN           = 50798,
    EQUIP_OFFHAND        = 50798,
    EQUIP_RANGED         = EQUIP_NO_CHANGE,
    EQUIP_DONE           = EQUIP_NO_CHANGE,
};

struct MANGOS_DLL_DECL boss_deathbringer_saurfangAI : public BSWScriptedAI
{
    boss_deathbringer_saurfangAI(Creature* pCreature) : BSWScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance *pInstance;
    uint8 beasts;
    int32 oldPower;

    void Reset()
    {
        if(!pInstance)
            return;
        m_creature->SetRespawnDelay(7*DAY);
        if (m_creature->isAlive())
            pInstance->SetData(TYPE_SAURFANG, NOT_STARTED);
        setStage(0);
        beasts = 0;
        resetTimers();
        m_creature->SetPower(m_creature->getPowerType(), 0);
        doCast(SPELL_ZERO_REGEN);
        oldPower = 0;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!pInstance)
            return;

        if (!pWho || pWho->GetTypeId() != TYPEID_PLAYER)
            return;

        if (!m_creature->isInCombat() && pWho->IsWithinDistInMap(m_creature, 20.0f))
        {
            m_creature->setFaction(21);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
            AttackStart(pWho);
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit *who)
    {
        if(!pInstance)
            return;

        if (!pInstance->IsEncounterAvailable(TYPE_SAURFANG))
        {
            pInstance->SendGMEncounterNotification(who, TYPE_SAURFANG);
            EnterEvadeMode();
            return;
        }

        pInstance->SetData(TYPE_SAURFANG, IN_PROGRESS);
        SetEquipmentSlots(false, EQUIP_MAIN, EQUIP_OFFHAND, EQUIP_RANGED);
        DoScriptText(-1631100,m_creature);
        doCast(SPELL_BLOOD_LINK);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
    {
        if (!pInstance)
            return;

        if (!pInstance->IsEncounterAvailable(TYPE_SAURFANG))
        {
            pInstance->SendGMEncounterNotification(pDoneBy, TYPE_SAURFANG);
            EnterEvadeMode();
            uiDamage = 0;
            return;
        }
    }

    void JustReachedHome()
    {
        if (pInstance)
            pInstance->SetData(TYPE_SAURFANG, FAIL);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch (urand(0,1)) 
        {
            case 0:
               DoScriptText(-1631103,m_creature,pVictim);
               break;
            case 1:
               DoScriptText(-1631104,m_creature,pVictim);
               break;
        }
    }

    void JustSummoned(Creature* summoned)
    {
        if(!pInstance || !summoned)
            return;

        summoned->SetOwnerGuid(m_creature->GetObjectGuid());

        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0) ) 
        {
            summoned->AddThreat(pTarget, 100.0f);
            summoned->GetMotionMaster()->MoveChase(pTarget);
        }
    }

    void JustDied(Unit *killer)
    {
        if(!pInstance)
            return;
        pInstance->SetData(TYPE_SAURFANG, DONE);
        DoScriptText(-1631106,m_creature);
        doRemoveFromAll(SPELL_MARK);

        DoCastSpellIfCan(m_creature, SPELL_ACHIEVEMENT_DEATHBRINGER_SAURFANG, CAST_TRIGGERED | CAST_FORCE_CAST);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_creature->HasAura(SPELL_BLOOD_POWER))
            doCast(SPELL_BLOOD_POWER);

        if (!m_creature->HasAura(SPELL_BLOOD_LINK))
            doCast(SPELL_BLOOD_LINK);

        if (!m_creature->HasAura(SPELL_MARK_SELF))
            doCast(SPELL_MARK_SELF);

        switch(getStage())
        {
            case 0:
                if (m_creature->GetHealthPercent() <= 30.0f) setStage(1);
                break;
            case 1: 
                doCast(SPELL_FRENZY);
                setStage(2);
                DoScriptText(-1631101,m_creature);
                break;
            case 2:
            default:
                break;
        }

        if (timedQuery(SPELL_MARK, diff))
        {
            if (Unit* pTarget = doSelectRandomPlayer(SPELL_MARK,false,120.0f))
                doCast(SPELL_MARK, pTarget);
        }

        timedCast(SPELL_BLOOD_NOVA, diff);

        timedCast(SPELL_BOILING_BLOOD, diff);

        timedCast(SPELL_RUNE_OF_BLOOD, diff);

        if (timedQuery(SPELL_CALL_BLOOD_BEAST_1, diff))
        {
            beasts = getSpellData(SPELL_CALL_BLOOD_BEAST_1);
            DoScriptText(-1631102,m_creature);
        }

        if (beasts > 0)
        {
            CanCastResult res = CAST_FAIL_OTHER;
            switch (beasts)
            {
                case 1: res = doCast(SPELL_CALL_BLOOD_BEAST_1); break;
                case 2: res = doCast(SPELL_CALL_BLOOD_BEAST_2); break;
                case 3: res = doCast(SPELL_CALL_BLOOD_BEAST_3); break;
                case 4: res = doCast(SPELL_CALL_BLOOD_BEAST_4); break;
                case 5: res = doCast(SPELL_CALL_BLOOD_BEAST_5); break;
                default: break;
            }

            if ( res == CAST_OK)
                --beasts;
        }

        if (timedQuery(SPELL_BERSERK, diff))
        {
            doCast(SPELL_BERSERK);
            DoScriptText(-1631108,m_creature);
        };

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_deathbringer_saurfang(Creature* pCreature)
{
    return new boss_deathbringer_saurfangAI(pCreature);
}

struct MANGOS_DLL_DECL  mob_blood_beastAI : public BSWScriptedAI
{
    mob_blood_beastAI(Creature *pCreature) : BSWScriptedAI(pCreature) 
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance *pInstance;
    Creature* pOwner;
    bool scentcasted;

    void Reset()
    {
        pOwner = m_creature->GetMap()->GetCreature(pInstance->GetData64(NPC_DEATHBRINGER_SAURFANG));
        resetTimers();
        scentcasted = false;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!pInstance || pInstance->GetData(TYPE_SAURFANG) != IN_PROGRESS) 
              m_creature->ForcedDespawn();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_creature->HasAura(SPELL_BLOOD_LINK_BEAST))
            doCast(SPELL_BLOOD_LINK_BEAST);

        if (!m_creature->HasAura(SPELL_RESISTANT_SKIN))
            doCast(SPELL_RESISTANT_SKIN);

        if (!scentcasted && (m_creature->GetHealthPercent() <= 20.0f))
        {
            if (urand(0,1))                            //50%
                doCast(SPELL_SCENT_OF_BLOOD);
            scentcasted = true;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_blood_beast(Creature* pCreature)
{
    return new mob_blood_beastAI(pCreature);
}

void AddSC_boss_deathbringer_saurfang()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_deathbringer_saurfang";
    newscript->GetAI = &GetAI_boss_deathbringer_saurfang;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_blood_beast";
    newscript->GetAI = &GetAI_mob_blood_beast;
    newscript->RegisterSelf();
}
