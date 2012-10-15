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
SDName: Boss_Varos
SD%Complete: 70%
SDComment:
SDAuthor: originally from TC, reworked by MaxXx2021 Aka Mioka, corrected by /dev/rsa
SDCategory: Oculus
EndScriptData */

#include "precompiled.h"
#include "oculus.h"

enum
{
    SAY_AGGRO                                     = -1578022,
    SAY_KILL_1                                    = -1578023,
    SAY_KILL_2                                    = -1578024,
    SAY_DEATH                                     = -1578025,
    SAY_STRIKE_1                                  = -1578026,
    SAY_STRIKE_2                                  = -1578027,
    SAY_STRIKE_3                                  = -1578028,

    SPELL_ARCANE_SHIELD                           = 50053,
    SPELL_ENERGIZE_CORES                          = 50785, //Damage 5938 to 6562, effec2 Triggers 54069, effect3 Triggers 56251
    SPELL_ENERGIZE_CORES_TRIGGER_1                = 54069,
    SPELL_ENERGIZE_CORES_TRIGGER_2                = 56251,
    SPELL_ENERGIZE_CORES_2                        = 59372, //Damage 9025 to 9975, effect2 Triggers 54069, effect 56251
    SPELL_CALL_AZURE_RING_CAPTAIN                 = 51002, //Effect    Send Event (12229)
    SPELL_CALL_AZURE_RING_CAPTAIN_2               = 51006, //Effect    Send Event (10665)
    SPELL_CALL_AZURE_RING_CAPTAIN_3               = 51007, //Effect    Send Event (18454)
    SPELL_CALL_AZURE_RING_CAPTAIN_4               = 51008, //Effect    Send Event (18455)
    SPELL_CALL_AMPLIFY_MAGIC                      = 51054,
    SPELL_CALL_AMPLIFY_MAGIC_2                    = 59371,

    NPC_AZURE_CAPTAIN                             = 28236,
    NPC_BEAM                                      = 28239,
    NPC_VAROS_CORE                                = 28183,

    SPELL_CORE_VISUAL                             = 50798,
    SPELL_CORE_MISSILE                            = 61407, //need core fix max target 4, and spell script on 28183
    SPELL_BEAM                                    = 51024, //need spell script on 28239
    SPELL_BEAM_DMG_AURA                           = 51019,
    SPELL_BEAM_VISUAL_SOUND                       = 51022, //need script target 28239
    SPELL_SUMMON_BEAM                             = 51017
};

struct dLocations
{
    float x1, y1, x2, y2;
    uint32 id;
};

struct dLocations Regions[]=
{
    {0, 0, 0},
    {1323.0f, 1056.0f, 1333.0f, 1066.0f}, //first orb 1
    {1319.3f, 1084.0f, 1329.5f, 1094.0f}, //second orb 2
    {1288.3f, 1108.8f, 1298.3f, 1118.8f}, //third orb 3
    {1260.3f, 1104.2f, 1270.3f, 1114.2f}, //four orb 4
    {1237.7f, 1074.5f, 1247.7f, 1084.5f}, //fifth orb 5
    {1241.8f, 1046.1f, 1251.8f, 1056.1f}, // 6
    {1272.0f, 1022.1f, 1282.0f, 1032.1f}, // 7
    {1300.5f, 1026.2f, 1310.5f, 1036.2f}  // 8
};

struct MANGOS_DLL_DECL boss_varosAI : public ScriptedAI
{
    boss_varosAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint8 MinOrb;
    uint8 MaxOrb;

    uint32 m_uiCoreTimer;
    uint32 m_uiOrbCast;
    uint32 m_uiDragonAttackTimer;
    uint32 m_uiDragonMoveTimer;
    uint32 m_uiCheckTimer;

    ObjectGuid m_uiAzureDrakeGUID;

    bool m_bIsCastChain;

    float angle01;
    float angle02;

    void Reset()
    {
        MinOrb = 1;
        MaxOrb = 4;
        angle01 = 0;
        angle02 = 0;
        m_uiOrbCast = 7000;
        m_uiCheckTimer = 2000;
        m_uiDragonAttackTimer = 10000;
        m_uiCoreTimer = urand(7000, 15000);
        m_uiDragonMoveTimer = 16000;
        m_bIsCastChain = false;
        if(m_pInstance)
        {
           m_pInstance->SetData(TYPE_VAROS, NOT_STARTED);
           if(m_pInstance->GetData(TYPE_ROBOTS) == 0)
           {
              m_creature->RemoveAurasDueToSpell(SPELL_ARCANE_SHIELD);
              m_creature->InterruptNonMeleeSpells(false);
              m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
           }
           else
           {
              m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
              DoCast(m_creature, SPELL_ARCANE_SHIELD);
           }
           if(Creature* Dragon = m_pInstance->instance->GetCreature(m_uiAzureDrakeGUID))
              Dragon->DealDamage(Dragon, Dragon->GetMaxHealth(),NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
        m_uiAzureDrakeGUID.Clear();
    }

    void CheckVehicle()
    {
        Map* map = m_creature->GetMap();
        if(map && map->IsDungeon())
        {
           Map::PlayerList const &PlayerList = map->GetPlayers();

           if(PlayerList.isEmpty())
              return;

           for(Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
           {
              if(i->getSource()->isAlive() && i->getSource()->GetVehicle())
                 EnterEvadeMode();
           }
        }
    }

    void Aggro(Unit* who)
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if(m_pInstance)
           m_pInstance->SetData(TYPE_VAROS, IN_PROGRESS);

        if(Creature* Dragon = m_creature->SummonCreature(NPC_AZURE_CAPTAIN, (m_creature->GetPositionX()-45)+rand()%90, (m_creature->GetPositionY()-45)+rand()%90, m_creature->GetPositionZ() + 30.0f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 100))
        {
           Dragon->SetLevitate(true);
           Dragon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
           m_uiAzureDrakeGUID = Dragon->GetObjectGuid();
        }
    }

    void AttackStart(Unit* pWho)
    {
        if(m_pInstance)
           if(m_pInstance->GetData(TYPE_ROBOTS) != 0)
              return;

        ScriptedAI::AttackStart(pWho);

    }

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_VAROS, DONE);
    }

    void KilledUnit(Unit *victim)
    {
        DoScriptText(urand(0,1) ? SAY_KILL_1 : SAY_KILL_2, m_creature);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if(pSummoned->GetEntry() == NPC_BEAM)
        {
           pSummoned->setFaction(14);
           pSummoned->SetDisplayId(11686);
           pSummoned->CastSpell(pSummoned, SPELL_BEAM_VISUAL_SOUND, false);
           pSummoned->CastSpell(pSummoned, SPELL_BEAM_DMG_AURA, false);
           pSummoned->SetInCombatWithZone();
           if(Unit* pTarget = pSummoned->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
              pSummoned->AI()->AttackStart(pTarget);
           if(m_pInstance)
              if(Creature* Dragon = m_pInstance->instance->GetCreature(m_uiAzureDrakeGUID))
              {
                 Dragon->GetMotionMaster()->MovementExpired(false);
                 Dragon->GetMotionMaster()->Clear(false);
                 Dragon->CastSpell(pSummoned, SPELL_BEAM, true);
              }
        }
    }

    void SelectFourOrb() //huck work perfecly
    {
        MinOrb = MinOrb+2;
        MaxOrb = MaxOrb+2;
        if(MinOrb > 8)
           MinOrb = 1;
        if(MaxOrb > 8)
           MaxOrb = 2;

        std::list<Creature*> m_pSpheres;
        GetCreatureListWithEntryInGrid(m_pSpheres, m_creature, NPC_VAROS_CORE, DEFAULT_VISIBILITY_INSTANCE);

        if(!m_pSpheres.empty())
           for(std::list<Creature*>::iterator iter = m_pSpheres.begin(); iter != m_pSpheres.end(); ++iter)
           {
              for(uint8 i = 1; i < 9; i++)
                 if((i <= MaxOrb && i >= MinOrb) || (MinOrb == 7 && (i <= MaxOrb || i >= MinOrb)))
                    if((*iter)->GetPositionX() > Regions[i].x1 && (*iter)->GetPositionX() < Regions[i].x2)
                       if((*iter)->GetPositionY() > Regions[i].y1 && (*iter)->GetPositionY() < Regions[i].y2)
                          if(SpellEntry* pTempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_CORE_MISSILE))
                          {
                              // LOLWHAT
                             //pTempSpell->EffectImplicitTargetA[0] = TARGET_EFFECT_SELECT;
                             //pTempSpell->EffectImplicitTargetB[0] = 0;
                             //pTempSpell->EffectImplicitTargetA[1] = TARGET_EFFECT_SELECT;
                             //pTempSpell->EffectImplicitTargetB[1] = 0;
                             //pTempSpell->EffectImplicitTargetA[2] = TARGET_EFFECT_SELECT;
                             //pTempSpell->EffectImplicitTargetB[2] = 0;
                             (*iter)->CastSpell(m_creature, pTempSpell, true);
                          }
           }
    }

    void CastEnergy()
    {
        std::list<Creature*> m_pSpheres;
        GetCreatureListWithEntryInGrid(m_pSpheres, m_creature, NPC_VAROS_CORE, DEFAULT_VISIBILITY_INSTANCE);

        if(!m_pSpheres.empty())
           for(std::list<Creature*>::iterator iter = m_pSpheres.begin(); iter != m_pSpheres.end(); ++iter)
           {
              for(uint8 i = 1; i < 9; i++)
                 if((i <= MaxOrb && i >= MinOrb) || (MinOrb == 7 && (i <= MaxOrb || i >= MinOrb)))
                    if((*iter)->GetPositionX() > Regions[i].x1 && (*iter)->GetPositionX() < Regions[i].x2)
                       if((*iter)->GetPositionY() > Regions[i].y1 && (*iter)->GetPositionY() < Regions[i].y2)
                       {
                          if(SpellEntry* pTempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_ENERGIZE_CORES_TRIGGER_1))
                          {
                              // LOLWHAT
                             //pTempSpell->EffectImplicitTargetA[0] = TARGET_EFFECT_SELECT;
                             //pTempSpell->EffectImplicitTargetB[0] = 0;
                             //pTempSpell->EffectImplicitTargetA[1] = TARGET_EFFECT_SELECT;
                             //pTempSpell->EffectImplicitTargetB[1] = 0;
                             //pTempSpell->EffectImplicitTargetA[2] = TARGET_EFFECT_SELECT;
                             //pTempSpell->EffectImplicitTargetB[2] = 0;
                             (*iter)->CastSpell(m_creature, pTempSpell, true);
                          }

                          if(i == MinOrb)
                             angle01 = m_creature->GetAngle((*iter));
                          if(i == MaxOrb)
                             angle02 = m_creature->GetAngle((*iter));

                          Map *map = m_creature->GetMap();
                          if(map->IsDungeon())
                          {
                             Map::PlayerList const &PlayerList = map->GetPlayers();

                             if(PlayerList.isEmpty())
                                return;

                             for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                             {
                                if (i->getSource()->isAlive())
                                {
                                   float pAngle = m_creature->GetAngle(i->getSource());
                                   if(angle01 < angle02)
                                      if(pAngle < angle02 && pAngle > angle01)
                                         DoEnergy(i->getSource());
                                   if(angle01 > angle02)
                                      if(pAngle < angle02 || pAngle > angle01)
                                         DoEnergy(i->getSource());
                                }
                             }
                          }
                       }
           }
    }

    void DoEnergy(Unit* pTarget)
    {
        if(SpellEntry* pTempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(m_bIsRegularMode ? SPELL_ENERGIZE_CORES : SPELL_ENERGIZE_CORES_2))
        {
            // LOLWHAT
            //pTempSpell->EffectImplicitTargetA[0] = TARGET_EFFECT_SELECT;
            //pTempSpell->EffectImplicitTargetB[0] = 0;
            //pTempSpell->EffectImplicitTargetA[1] = TARGET_EFFECT_SELECT;
            //pTempSpell->EffectImplicitTargetB[1] = 0;
            //pTempSpell->EffectImplicitTargetA[2] = TARGET_EFFECT_SELECT;
            //pTempSpell->EffectImplicitTargetB[2] = 0;
            m_creature->CastSpell(pTarget, pTempSpell, true);
        }

    }

    /*void SpellHitTarget(Unit *target, const SpellEntry *spell)
    {
        if(spell->Id == (m_bIsRegularMode ? SPELL_ENERGIZE_CORES : SPELL_ENERGIZE_CORES_2) && target->GetTypeId() == TYPEID_PLAYER)
        {
           int32 uiDmg = m_bIsRegularMode ? urand(5938, 6562) : urand(9025, 9975);
           m_creature->DealDamage(target, uiDmg,NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_ARCANE, NULL, false);
        }
    }*/

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_uiCheckTimer < uiDiff)
        {
            CheckVehicle();
            m_uiCheckTimer = 2000;
        } else m_uiCheckTimer -= uiDiff;

        if(m_uiDragonAttackTimer <= uiDiff)
        {
           DoCast(m_creature, SPELL_SUMMON_BEAM, true);
           m_uiDragonAttackTimer = 25000;
           m_uiDragonMoveTimer = 16000;
           uint8 uiText = urand(0, 2);
           switch (uiText)
           {
              case 0: DoScriptText(SAY_STRIKE_1, m_creature); break;
              case 1: DoScriptText(SAY_STRIKE_2, m_creature); break;
              case 2: DoScriptText(SAY_STRIKE_3, m_creature); break;
           }
        } else m_uiDragonAttackTimer -= uiDiff;

        if(m_uiDragonMoveTimer <= uiDiff)
        {
           if(m_pInstance)
              if(Creature* Dragon = m_pInstance->instance->GetCreature(m_uiAzureDrakeGUID))
              {
                 Dragon->GetMotionMaster()->MovementExpired(false);
                 Dragon->GetMotionMaster()->Clear(false);
                 Dragon->GetMotionMaster()->MovePoint(0, (m_creature->GetPositionX()-45)+rand()%90, (m_creature->GetPositionY()-45)+rand()%90, m_creature->GetPositionZ() + 30.0f);
              }
           m_uiDragonMoveTimer = 25000;
        } else m_uiDragonMoveTimer -= uiDiff;

        if(!m_bIsCastChain)
        {
           if(m_uiOrbCast <= uiDiff)
           {
              m_uiOrbCast = 1000;
              m_bIsCastChain = true;
              SelectFourOrb();
              m_uiCoreTimer = 5000;
           } else m_uiOrbCast -= uiDiff;
        }
        else
        {
           if(m_uiCoreTimer <= uiDiff)
           {
              m_uiCoreTimer = 5000;
              m_bIsCastChain = false;
              CastEnergy();
              m_uiOrbCast = 1000;
           } else m_uiCoreTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_varos_orbAI : public ScriptedAI
{
    npc_varos_orbAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
       Reset();
    }

    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->setFaction(14);
        DoCast(m_creature, SPELL_CORE_VISUAL, true);
    }

    void AttackStart(Unit* pWho)
    {
    }

    void UpdateAI(const uint32 diff)
    {
    }
};

struct MANGOS_DLL_DECL npc_varos_beam_targetAI : public ScriptedAI
{
    npc_varos_beam_targetAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
       Reset();
    }
    uint32 uiDeathTimer;

    void Reset()
    {
        m_creature->SetSpeedRate(MOVE_RUN, 0.5f, true);
        uiDeathTimer = 15000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (uiDeathTimer < diff)
            m_creature->ForcedDespawn();
        else uiDeathTimer -= diff;
    }
};

CreatureAI* GetAI_boss_varos(Creature* pCreature)
{
    return new boss_varosAI (pCreature);
}

CreatureAI* GetAI_npc_varos_orb(Creature* pCreature)
{
    return new npc_varos_orbAI (pCreature);
}

CreatureAI* GetAI_npc_varos_beam_target(Creature* pCreature)
{
    return new npc_varos_beam_targetAI (pCreature);
}

void AddSC_boss_varos()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_varos";
    newscript->GetAI = &GetAI_boss_varos;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_varos_orb";
    newscript->GetAI = &GetAI_npc_varos_orb;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_varos_beam_target";
    newscript->GetAI = &GetAI_npc_varos_beam_target;
    newscript->RegisterSelf();
}
