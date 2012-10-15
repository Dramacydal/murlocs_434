/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
 * Copyright (C) 2011 - 2012 MangosR2 <http://github.com/mangosR2/>
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
SDName: boss_headless_horseman
SD%Complete: 80
SDComment: 

TODO: 
// only get one chance per summon to win  if u wipe or lose aggro he despawns
// fix phase change when body heals to full 
// his little flight entrace needs to have spawn creature movement in DB or script
// could use a little tweaking

SDCategory: Scarlet Monastery
EndScriptData */

#include "precompiled.h"
#include "scarlet_monastery.h"

enum Texts
{
    SAY_ENTRANCE           = -1189022,
    SAY_REJOINED           = -1189023,
    SAY_BODY_DEFEAT        = -1189024,
    SAY_LOST_HEAD          = -1189025,
    SAY_CONFLAGRATION      = -1189026,
    SAY_SPROUTING_PUMPKINS = -1189027,
    SAY_SLAY               = -1189028,
    SAY_DEATH              = -1189029,

    EMOTE_LAUGH            = -1189030,

    SAY_PLAYER1            = -1189031,
    SAY_PLAYER2            = -1189032,
    SAY_PLAYER3            = -1189033,
    SAY_PLAYER4            = -1189034
};

enum HorsemanSpells
{
    //spells
    SPELL_HORSEMAN_CLEAVE       = 42587,
    SPELL_CONFLAGRATION         = 42380,
    SPELL_SUMMON_PUMPKIN        = 42394,  //Jack o laterned      //summons 4 pumpkins missiles or 52236

    SPELL_WHIRLWIND             = 43116,        //or  ?41194  // After losing its head, the body will AoE whirlwind for ~2k/sec.
    SPELL_CLEAVE_PROC           = 42587,        //procs after "killing" body(at each end of phase or when he really dies?)
    SPELL_IMMUNE                = 42556,
    SPELL_BODY_REGEN            = 42403,        //regen 4% of total hp per 1sec;not used, because this, body regen and whirlwind non stackable with each other
    SPELL_BODY_CONFUSE          = 43105,        //confused movement

    SPELL_SEND_HEAD             = 42399,        //visual flying head
    SPELL_HEAD                  = 42413,        //visual buff, "head"
    SPELL_HEAD_IS_DEAD          = 42428,        //at killing head, Phase 3
    SPELL_HEAD_INVIS            = 44312,        //not used
    SPELL_SUMMON_WISP_HEAD      = 42823,        //summonig whispering head

    SPELL_PUMPKIN_AURA          = 42280,
    SPELL_PUMPKIN_AURA_GREEN    = 42294,
    SPELL_PUMPKIN_DEATH         = 42291,
    SPELL_SQUASH_SOUL           = 42514,        //pumpkin debuff
    SPELL_SPROUTING             = 42281,
    SPELL_SPROUT_BODY           = 42285,

    //Effects
    SPELL_RHYME_BIG             = 42909,        //summoning rhyme
    SPELL_RHYME_SMALL           = 42910,
    SPELL_HEAD_SPEAKS           = 43129,
    SPELL_HEAD_LANDS            = 42400,
    SPELL_BODY_FLAME            = 42074,
    SPELL_ON_KILL_PROC          = 43877,        //procs after killing players?
    SPELL_ENRAGE_VISUAL         = 42438,        // he uses this spell?
    SPELL_WISP                  = 43034,

// laughs
    SPELL_LAUGH0                = 43881,
    SPELL_LAUGH1                = 43885,
    SPELL_LAUGH2                = 43894,
};

enum Phazes
{
    PHASE_ZERO  = 0,   // summon the npc head  ( invis at first til phase 2
    PHASE_ONE   = 1,   // just summoned once aggro starts off with cleave and summons head buff at aggro
    PHASE_TWO   = 2,   // head and body split
    PHASE_THREE = 3,   // head and body rejoin  start conflartion
    PHASE_FOUR  = 4,   // head and body split again
    PHASE_FIVE  = 5,   // head and body rejoin he cast conf and summons pumkin .. if body damaged enough repeat phase 4
};

uint8 m_uiPhase;      // golbal phase counter

/*######
## boss_headless_horseman
######*/
struct MANGOS_DLL_DECL boss_headless_horsemanAI : public ScriptedAI
{
    boss_headless_horsemanAI(Creature* pCreature) : ScriptedAI(pCreature)

    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();
        m_uiPhase = PHASE_ZERO;
        pEntered = false;
    }
    
    ScriptedInstance* m_pInstance;

    bool pEntered;

    uint32 m_uiCleave_Timer;
    uint32 m_uiConflageration_Timer;
    uint32 m_uiPumpkinSprout_Timer;

    ObjectGuid HeadGUID;

    void Reset()
    {
        m_uiCleave_Timer = 5000;
        m_uiConflageration_Timer = 5000;
        m_uiPumpkinSprout_Timer = 5000;
    }

    void Aggro(Unit* pWho)
    {
        if (!pWho)
            return;

        if (pEntered)
            m_creature->SetInCombatWithZone();
    }

    void Laugh()
    {
        switch(urand(0,2))
        {
            case 0: DoCast(m_creature,SPELL_LAUGH0,true); break;
            case 1: DoCast(m_creature,SPELL_LAUGH1,true); break;
            case 2: DoCast(m_creature,SPELL_LAUGH2,true); break;
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(SAY_SLAY, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
    }

    void RemoveRegenAuras()
    {
        if (m_creature->HasAura(SPELL_BODY_REGEN))
            m_creature->RemoveAurasDueToSpell(SPELL_BODY_REGEN);
        if (m_creature->HasAura(SPELL_IMMUNE))
            m_creature->RemoveAurasDueToSpell(SPELL_IMMUNE);
        if (m_creature->HasAura(SPELL_BODY_CONFUSE))
            m_creature->RemoveAurasDueToSpell(SPELL_BODY_CONFUSE);
        if (m_creature->HasAura(SPELL_WHIRLWIND))
            m_creature->RemoveAurasDueToSpell(SPELL_WHIRLWIND);
    }

    void ApplyRegenAuras()
    {
        if (!m_creature->HasAura(SPELL_BODY_REGEN))
            DoCast(m_creature,SPELL_BODY_REGEN,true);
        if (!m_creature->HasAura(SPELL_IMMUNE))
            DoCast(m_creature,SPELL_IMMUNE,true);
        if (!m_creature->HasAura(SPELL_BODY_CONFUSE))
            DoCast(m_creature,SPELL_BODY_CONFUSE,true);
        if (!m_creature->HasAura(SPELL_WHIRLWIND))
            DoCast(m_creature,SPELL_WHIRLWIND,true);
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (!pSpell)
            return;

        if (pSpell->Id == SPELL_SEND_HEAD)
        {
            DoScriptText(SAY_REJOINED,m_creature);
            //m_creature->CastSpell(m_creature,SPELL_HEAD,false);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!pEntered) // should also be in phase zero
        {
            float x,y,z;
            m_creature->GetClosePoint(x, y, z, m_creature->GetObjectBoundingRadius(), 15.0f, 0.0f);
            if (Creature* pTemp = m_creature->SummonCreature(NPC_HEAD, x, y, z, 0, TEMPSUMMON_CORPSE_DESPAWN, 0))
                HeadGUID = pTemp->GetObjectGuid();

            pEntered = true;
            m_uiPhase = PHASE_ONE;

        }

       Creature* pHead = m_pInstance->GetSingleCreatureFromStorage(NPC_HEAD);

        if (!m_pInstance)
            return;

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        
        switch (m_uiPhase)
        {
            case PHASE_ONE:
            {
                // he gets his head
                if (!m_creature->HasAura(SPELL_HEAD))
                    DoCast(m_creature,SPELL_HEAD,false);

                if (((m_creature->GetHealth()*100) / (m_creature->GetMaxHealth())) <= 1)
                     m_uiPhase = PHASE_TWO;

                if (m_uiCleave_Timer <= uiDiff)
                {
                    m_creature->CastSpell(m_creature->getVictim(),SPELL_CLEAVE_PROC,false);
                    m_uiCleave_Timer = 5000;
                }else m_uiCleave_Timer -= uiDiff;
                DoMeleeAttackIfReady();

                break;
            }
            case PHASE_TWO:
            {
                if (m_creature->HasAura(SPELL_HEAD))
                {
                    m_creature->RemoveAurasDueToSpell(SPELL_HEAD);
// visual of head flying off body ( really head flying at target which in this case needs to be the head npc )
                    DoCast(pHead,SPELL_SEND_HEAD,true);
                }

                ApplyRegenAuras();

                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    if (pHead->AI())
                        pHead->AI()->AttackStart(pTarget);

                if ((pHead->GetHealth()*100) / (pHead->GetMaxHealth()) <= 66) /*|| (m_creature->GetHealth()*100) / (m_creature->GetMaxHealth()) <= 100)*/
                {
                    RemoveRegenAuras();
                    m_uiPhase = PHASE_THREE;
                }

                break;
            }
            case PHASE_THREE:
            {
// give him his head back ( the head will target and cast spell send head at the body will look like head flying back on to body
                if (!m_creature->HasAura(SPELL_HEAD))
                    DoCast(m_creature,SPELL_HEAD,false);

                if (((m_creature->GetHealth()*100) / (m_creature->GetMaxHealth())) <= 1)
                    m_uiPhase = PHASE_FOUR;

                if (m_uiConflageration_Timer <= uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        DoScriptText(SAY_CONFLAGRATION,m_creature);
                        pTarget->CastSpell(pTarget,SPELL_CONFLAGRATION,false);
                    }
                    m_uiConflageration_Timer = 15000;
                }else m_uiConflageration_Timer -= uiDiff;

                DoMeleeAttackIfReady();
                break;
            }
            case PHASE_FOUR:
            {
                if (m_creature->HasAura(SPELL_HEAD))
                {
                    m_creature->RemoveAurasDueToSpell(SPELL_HEAD);
// visual of head flying off body ( really head flying at target which in this case needs to be the head npc )
                    DoCast(pHead,SPELL_SEND_HEAD,true);
                }

                ApplyRegenAuras();

                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    if (pHead->AI())
                        pHead->AI()->AttackStart(pTarget);

                if ((pHead->GetHealth()*100) / (pHead->GetMaxHealth()) <= 33) /*|| (m_creature->GetHealth()*100) / (m_creature->GetMaxHealth()) <= 100)*/
                {
                    RemoveRegenAuras();
                    m_uiPhase = PHASE_FIVE;
                }

                break;
            }
            case PHASE_FIVE:
            {

                if (!m_creature->HasAura(SPELL_HEAD))
                    DoCast(m_creature,SPELL_HEAD,false);

                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    if (pHead->AI())
                        pHead->AI()->AttackStart(pTarget);

                if (m_uiPumpkinSprout_Timer <= uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        DoScriptText(SAY_SPROUTING_PUMPKINS,m_creature);
                        m_creature->CastSpell(pTarget,SPELL_SUMMON_PUMPKIN,false);
                    }
                    m_uiPumpkinSprout_Timer = 30000;
                }else m_uiPumpkinSprout_Timer -= uiDiff;

                if (m_uiConflageration_Timer <= uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        DoScriptText(SAY_CONFLAGRATION,m_creature);
                        pTarget->CastSpell(pTarget,SPELL_CONFLAGRATION,false);
                    }
                    m_uiConflageration_Timer = 15000;
                }else m_uiConflageration_Timer -= uiDiff;

                if (((m_creature->GetHealth()*100) / (m_creature->GetMaxHealth())) <= 1)
                {
                    m_uiPhase = PHASE_FOUR;
                    DoCast(pHead,SPELL_SEND_HEAD,true);
                }

                DoMeleeAttackIfReady();
                break;
                //default: break;
            }
        }
    }
};

CreatureAI* GetAI_boss_headless_horseman(Creature* pCreature)
{
    return new boss_headless_horsemanAI(pCreature);
}

/*######
## npc_horsemans_head
######*/

struct MANGOS_DLL_DECL npc_horsemans_headAI : public ScriptedAI
{
    npc_horsemans_headAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();
    }
    ScriptedInstance* m_pInstance;

    void Reset()
    {
        if (!m_creature->HasAura(SPELL_HEAD_INVIS))
            m_creature->CastSpell(m_creature,SPELL_HEAD_INVIS,false);
        if (m_creature->HasAura(SPELL_HEAD))
            m_creature->RemoveAurasDueToSpell(SPELL_HEAD);
    }

    void Aggro(Unit* pWho)
    {
        if (!pWho)
            return;

        DoCast(m_creature,SPELL_HEAD,false);
        //m_creature->SetInCombatWithZone();
    }

    void JustDied(Unit* pKiller)
    {
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (!pSpell)
            return;

        if (pSpell->Id == SPELL_SEND_HEAD)
        {
            m_creature->CastSpell(m_creature,SPELL_HEAD,true);
            DoScriptText(SAY_LOST_HEAD,m_creature);
        }

        if (m_creature->HasAura(SPELL_HEAD_INVIS))
            m_creature->RemoveAurasDueToSpell(SPELL_HEAD_INVIS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance)
            return;

        Creature* pHorseman = m_pInstance->GetSingleCreatureFromStorage(NPC_HEADLESS_HORSEMAN);

        switch (m_uiPhase)
        {
            case 0:
            case 1:
            case 3:
            case 5:
                if (m_creature->HasAura(SPELL_HEAD))
                {
                    DoCast(pHorseman,SPELL_SEND_HEAD,true);
                    m_creature->RemoveAurasDueToSpell(SPELL_HEAD);
                }
                if (!m_creature->HasAura(SPELL_HEAD_INVIS))
                    m_creature->CastSpell(m_creature,SPELL_HEAD_INVIS,true);
                break;
            case 2:
            case 4:
                if (m_creature->HasAura(SPELL_HEAD_INVIS))
                    m_creature->RemoveAurasDueToSpell(SPELL_HEAD_INVIS);

                DoMeleeAttackIfReady();
                break;
        }
    }

};
CreatureAI* GetAI_npc_horsemans_head(Creature* pCreature)
{
    return new npc_horsemans_headAI(pCreature);
}

/*######
## mob_pulsing_pumpkin
######*/

struct MANGOS_DLL_DECL mob_pulsing_pumpkinAI : public ScriptedAI
{
    mob_pulsing_pumpkinAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        // on creature create
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();
        DoCast(m_creature,SPELL_PUMPKIN_AURA_GREEN,true);
        DoCast(m_creature,SPELL_PUMPKIN_AURA,true);
        DoCast(m_creature,SPELL_SPROUTING,false);

        bSquashed = false;
        bTransformed = false;
    }

    ScriptedInstance* m_pInstance;

    bool bSquashed;
    bool bTransformed;

    void Reset() {}

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance)
            return;

        Creature* pHorseman = m_pInstance->GetSingleCreatureFromStorage(NPC_HEADLESS_HORSEMAN);

        // Apply squash after landing
        if (!bSquashed && m_creature->HasAura(SPELL_PUMPKIN_AURA))
        {
            Map::PlayerList const& lPlayers = m_pInstance->instance->GetPlayers();
            if (!lPlayers.isEmpty())
            {
                for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                {
                    if (Player* pPlayer = itr->getSource())
                        if (m_creature->GetDistance(pPlayer) <= INTERACTION_DISTANCE)
                            DoCast(pPlayer,SPELL_SQUASH_SOUL,true);
                }
            }
            bSquashed = true;
        }

        if (!bTransformed && m_creature->HasAura(SPELL_SPROUT_BODY))
        {
            m_creature->UpdateEntry(NPC_PUMPKIN_FIEND);
            if (Unit* pTarget = pHorseman->getVictim())
                AttackStart(pTarget);
            bTransformed = true;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_pulsing_pumpkin(Creature* pCreature)
{
    return new mob_pulsing_pumpkinAI(pCreature);
}

void AddSC_boss_headless_horseman()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_headless_horseman";
    pNewScript->GetAI = GetAI_boss_headless_horseman;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_horsemans_head";
    pNewScript->GetAI = GetAI_npc_horsemans_head;
    pNewScript->RegisterSelf(); 

    pNewScript = new Script;
    pNewScript->Name = "mob_pulsing_pumpkin";
    pNewScript->GetAI = GetAI_mob_pulsing_pumpkin;
    pNewScript->RegisterSelf();
}
