/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: violet_hold
SDAuthor: ckegg, modified by rsa
SD%Complete: 60
SDComment: 
SDCategory: The Violet Hold
EndScriptData */

#include "precompiled.h"
#include "violet_hold.h"
#include "escort_ai.h"
enum
{
    SPELL_TELEPORT_INSIDE                 = 62139,
    SPELL_SHIELD_DISRUPTION               = 58291,

    //DRAGONS SPELLS
    //Azure Captain
    SPELL_MORTAL_STRIKE                   = 32736,
    SPELL_WHIRLWIND                       = 41057,

    //Azure Raider
    SPELL_CONCUSSION_BLOW                 = 52719,
    SPELL_MAGIC_REFLECTION                = 60158,

    //Azure Sorceror
    SPELL_ARCANE_STREAM                   = 60181,
    SPELL_ARCANE_STREAM_H                 = 60204,
    SPELL_MANA_DETONATION                 = 60182,
    SPELL_MANA_DETONATION_H               = 60205,

    //Azure stalker
    SPELL_BACKSTAB                        = 58471,
    SPELL_TACTICAL_BLINK                  = 58470,

    AZURE_SAY_AGGRO_1                     = -1608055,
    AZURE_SAY_AGGRO_2                     = -1608056,
    AZURE_SAY_AGGRO_3                     = -1608057,
    AZURE_SAY_AGGRO_4                     = -1608058,
    PORTAL_KEEPER_GUARDIAN_AGGRO_1        = -1608059,
    PORTAL_KEEPER_GUARDIAN_AGGRO_2        = -1608060,
    PORTAL_KEEPER_GUARDIAN_AGGRO_3        = -1608061,
    PORTAL_KEEPER_GUARDIAN_DEATH_1        = -1608062,
    PORTAL_KEEPER_GUARDIAN_DEATH_2        = -1608063,
    PORTAL_KEEPER_GUARDIAN_DEATH_3        = -1608064,
    AZURE_CAPTAIN_AGGRO_1                 = -1608065

};

uint32 m_uiNextPortal_Timer;
/*######
## mob_vh_dragons
## This script is for ALL mobs which are spawned from portals,
## they have to be scripted in SD2 because in EventAI you cant
## check for distance from door seal :/
## (Intro not implented yet)
######*/
struct MANGOS_DLL_DECL mob_vh_dragonsAI : public ScriptedAI
{
    mob_vh_dragonsAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegular = pCreature->GetMap()->IsRegularDifficulty();
        WayPointList.clear();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegular;
    uint32 creatureEntry;
    uint32 motherPortalID;
    uint32 WalkTimer;
    int8 portalLoc;
    bool IsWalking;
    bool isInCombat;
    bool MovementStarted;
    Creature* pDoorSeal;

    std::list<WayPoints> WayPointList;
    std::list<WayPoints>::iterator WayPoint;

    //Azure Captain
    uint32 m_uiMortalStrike_Timer;
    uint32 m_uiWhirlwind_Timer;

    //Azure Raider
    uint32 m_uiConcussionBlow_Timer;
    uint32 m_uiMagicReflection_Timer;

    //Azure Sorceror
    uint32 m_uiArcaneStream_Timer;
    uint32 m_uiManaDetonation_Timer;

    //Azure Stalker
    uint32 m_uiBackstab_Timer;
    uint32 m_uiBlink_Timer;

    void Reset()
    {
        creatureEntry = m_creature->GetEntry();
        motherPortalID = 0;
        WalkTimer = 200;
        portalLoc = -1;
        IsWalking = false;
        isInCombat = false;
        MovementStarted = false;
        pDoorSeal = GetClosestCreatureWithEntry(m_creature, NPC_DOOR_SEAL, 150.0f);

        //Azure Captain
        m_uiMortalStrike_Timer = 3000;
        m_uiWhirlwind_Timer = 5000;

        //Azure Raider
        m_uiConcussionBlow_Timer = 3000;
        m_uiMagicReflection_Timer = 10000;

        //Azure Sorceror
        m_uiArcaneStream_Timer = 5000;
        m_uiManaDetonation_Timer = 3000;

        //Azure Stalker
        m_uiBackstab_Timer = 7100;
        m_uiBlink_Timer = 7000;

        if (creatureEntry == NPC_KEEPER || creatureEntry == NPC_GUARDIAN)
        {
            SetCombatMovement(false);
            m_creature->GetMotionMaster()->MoveRandom();
        }

    }

    void StartMovement()
    {
        if(!WayPointList.empty() || MovementStarted)
            return;

        uint8 start = 0;
        uint8 end = 0;
        switch(portalLoc)
        {
            case -1:
                return;
            //center & ichoron
            case 0:
            case 5:
                start = 0;
                end = 2;
                break;
            //From lavanthor
            case 1:
                start = 3;
                end = 5;
                break;
            // From Zuramat
            case 2:
                start = 6;
                end = 11;
                break;
            //From Moragg
            case 3:
                start = 12;
                end = 16;
                break;
            //From Erekem
            case 4:
                start = 17;
                end = 21;
                break;
            //From highest platform
            case 6:
                start = 22;
                end = 26;
                break;
        }
        uint8 wpId = 0;
        for(uint8 i = start; i <= end; ++i){
            debug_log("AddWP: %u", i);
            AddWaypoint(wpId, DragonsWP[i].x, DragonsWP[i].y, DragonsWP[i].z);
            wpId++;
        }

        WayPoint = WayPointList.begin();
        m_creature->SetWalk(true);
        IsWalking = true;
        MovementStarted = true;
    }
    void JustReachedHome()
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_EVENT, FAIL);
            m_pInstance->SetData(TYPE_RIFT, FAIL);
        }
    }
    void AddWaypoint(uint32 id, float x, float y, float z)
    {
        WayPoints DWP(id, x, y, z);
        WayPointList.push_back(DWP);
    }
    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if(uiType != POINT_MOTION_TYPE || creatureEntry == NPC_GUARDIAN || creatureEntry == NPC_KEEPER)
            return;

        if(WayPoint->id != uiPointId)
            return;

        ++WayPoint;
        WalkTimer = 200;
    }
    void JustDied(Unit* pKiller)
    {
        switch(creatureEntry)
        {
            case NPC_GUARDIAN:
            case NPC_KEEPER:
                switch(urand(0,2))
                    {
                    case 0: DoScriptText(PORTAL_KEEPER_GUARDIAN_DEATH_1, m_creature); break;
                    case 1: DoScriptText(PORTAL_KEEPER_GUARDIAN_DEATH_2, m_creature); break;
                    case 2: DoScriptText(PORTAL_KEEPER_GUARDIAN_DEATH_3, m_creature); break;
                    }
                break;
        }
    }
    void Aggro(Unit* pWho)
    {
        switch(creatureEntry)
        {
            case NPC_AZURE_CAPTAIN:
                DoScriptText(AZURE_CAPTAIN_AGGRO_1, m_creature);
                break;
            case NPC_AZURE_RAIDER:
            case NPC_AZURE_SORCEROR:
            case NPC_AZURE_STALKER:
            case NPC_AZURE_BINDER:
            case NPC_AZURE_INVADER:
            case NPC_AZURE_MAGE_SLAYER:
            case NPC_AZURE_SPELLBREAKER:
                    switch(urand(0,3))
                    {
                    case 0: DoScriptText(AZURE_SAY_AGGRO_1, m_creature); break;
                    case 1: DoScriptText(AZURE_SAY_AGGRO_2, m_creature); break;
                    case 2: DoScriptText(AZURE_SAY_AGGRO_3, m_creature); break;
                    case 3: DoScriptText(AZURE_SAY_AGGRO_4, m_creature); break;
                    }
                break;
            case NPC_GUARDIAN:
            case NPC_KEEPER:
                switch(urand(0,2))
                    {
                    case 0: DoScriptText(PORTAL_KEEPER_GUARDIAN_AGGRO_1, m_creature); break;
                    case 1: DoScriptText(PORTAL_KEEPER_GUARDIAN_AGGRO_2, m_creature); break;
                    case 2: DoScriptText(PORTAL_KEEPER_GUARDIAN_AGGRO_3, m_creature); break;
                    }
                break;
            default:
                debug_log("SD2: The Violet Hold: Unhandled dragon entry %u!", m_creature->GetEntry());
                break;
        }
    }
    void UpdateAI(const uint32 uiDiff){
        if(portalLoc != -1)
            StartMovement();

        if (IsWalking && WalkTimer)
        {
            if (WalkTimer <= uiDiff)
            {
                if (WayPoint != WayPointList.end())
                {
                    m_creature->GetMotionMaster()->MovePoint(WayPoint->id, WayPoint->x, WayPoint->y,WayPoint->z);
                    WalkTimer = 0;
                }
            }else WalkTimer -= uiDiff;
        }

        //Corrupt Seal

        if(Creature *pDoorSeal2 = GetClosestCreatureWithEntry(m_creature, NPC_DOOR_SEAL, 150.0f)){
            if(m_creature->IsWithinDist(pDoorSeal2, 27.0f, false) && !isInCombat)
            {
                IsWalking = false;
                WayPointList.clear();
                m_creature->GetMotionMaster()->Clear(false);
                m_creature->SetWalk(false);
                DoCast(pDoorSeal2, SPELL_CORRUPT);
                m_pInstance->SetData(TYPE_DOOR,SPECIAL);
            }
        }
        if(!IsWalking && !isInCombat) {
                if (Unit* m_uEmbraceTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                m_creature->GetMotionMaster()->MoveChase(m_uEmbraceTarget);
                m_creature->SetInCombatWithZone();
                isInCombat = true;
                }


        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_creature->getVictim())
            if(m_creature->getVictim()->GetEntry() == NPC_DOOR_SEAL)
                return;
        switch(creatureEntry)
        {
            case NPC_AZURE_CAPTAIN:
                AzureCaptain_UpdateAI(uiDiff);
                break;
            case NPC_AZURE_RAIDER:
                AzureRaider_UpdateAI(uiDiff);
                break;
            case NPC_AZURE_SORCEROR:
                AzureSorceror_UpdateAI(uiDiff);
                break;
            case NPC_AZURE_STALKER:
                AzureStalker_UpdateAI(uiDiff);
                break;
            case NPC_GUARDIAN:
            case NPC_KEEPER:
            case NPC_AZURE_BINDER:
            case NPC_AZURE_INVADER:
            case NPC_AZURE_MAGE_SLAYER:
            case NPC_AZURE_SPELLBREAKER:
                break;
            default:
                debug_log("SD2: The Violet Hold: Unhandled dragon entry %u!", m_creature->GetEntry());
                break;
        }
        DoMeleeAttackIfReady();
    }
    //Azure Captain
    void AzureCaptain_UpdateAI(const uint32 uiDiff)
    {
        //Mortal Strike
        if (m_uiMortalStrike_Timer <= uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_MORTAL_STRIKE);
            m_uiMortalStrike_Timer = 6000;
        }else m_uiMortalStrike_Timer -= uiDiff;

        //Whirlwind
        if (m_uiWhirlwind_Timer <= uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_WHIRLWIND);
            m_uiWhirlwind_Timer = 15000;
        }else m_uiWhirlwind_Timer -= uiDiff;
    }
    //Azure Raider
    void AzureRaider_UpdateAI(const uint32 uiDiff)
    {
        //Concusion Blow
        if (m_uiConcussionBlow_Timer <= uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_CONCUSSION_BLOW);
            m_uiMortalStrike_Timer = 7000;
        }else m_uiConcussionBlow_Timer -= uiDiff;

        //Magic reflection
        if (m_uiMagicReflection_Timer <= uiDiff)
        {
            DoCast(m_creature, SPELL_MAGIC_REFLECTION);
            m_uiMagicReflection_Timer = 30000;
        }else m_uiMagicReflection_Timer -= uiDiff;
    }
    //Azure Sorceror
    void AzureSorceror_UpdateAI(const uint32 uiDiff)
    {
        //Arcane Stream
        if (m_uiArcaneStream_Timer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                DoCast(pTarget, m_bIsRegular ? SPELL_ARCANE_STREAM : SPELL_ARCANE_STREAM_H);
            m_uiArcaneStream_Timer = 7000;
        }else m_uiArcaneStream_Timer -= uiDiff;

        //Mana Detonation
        if (m_uiManaDetonation_Timer <= uiDiff)
        {
            DoCast(m_creature, m_bIsRegular ? SPELL_MANA_DETONATION : SPELL_MANA_DETONATION_H);
            m_uiManaDetonation_Timer = 18000;
        }else m_uiManaDetonation_Timer -= uiDiff;
    }
    //Azure Stalker
    void AzureStalker_UpdateAI(const uint32 uiDiff)
    {
        //Backstab
        if (m_uiBackstab_Timer <= uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_BACKSTAB);
            m_uiBackstab_Timer = 15100;
        }else m_uiBackstab_Timer -= uiDiff;

        //Tactical blink
        if (m_uiBlink_Timer <= uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_TACTICAL_BLINK);
            m_uiBlink_Timer = 15000;
        }else m_uiBlink_Timer -= uiDiff;
    }
};

/*######
## npc_violet_portal
######*/
struct MANGOS_DLL_DECL npc_violet_portalAI : public ScriptedAI
{
    npc_violet_portalAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    bool m_uiGroupSpawned;
    uint8 portalType; // 0 = nothing, 1 = Guard & spawns, 2 = Group of elites
    uint32 portalID; // To identify portal...
    int8 portalLoc;

    uint32 TimeRiftWave_Timer;
    uint32 Check_Timer;

    void Reset()
    {
        m_uiGroupSpawned = false;
        portalType = 0;
        portalID = 0;
        portalLoc = -1;
        TimeRiftWave_Timer = 15000;
        Check_Timer = 5000;

        m_creature->SetRespawnDelay(DAY);
    }

    void JustSummoned(Creature* pSummoned)
    {
        switch(pSummoned->GetEntry())
        {
            case NPC_GUARDIAN:
                DoScriptText(EMOTE_GUARDIAN_PORTAL, pSummoned);
                break;
            case NPC_KEEPER:
                DoScriptText(EMOTE_KEEPER_PORTAL, pSummoned);
                break;
            case NPC_AZURE_CAPTAIN:
                DoScriptText(EMOTE_DRAGONFLIGHT_PORTAL, pSummoned);
                break;
            default:
                return;
        }

   }

    uint32 SelectRandSummon()
    {
        uint32 entry = 0;
        if(portalType == 1)
        {
            switch (urand(0, 3))
            {
                case 0: entry = NPC_AZURE_BINDER; break;
                case 1: entry = NPC_AZURE_INVADER; break;
                case 2: entry = NPC_AZURE_MAGE_SLAYER; break;
                case 3: entry = NPC_AZURE_SPELLBREAKER; break;
            }
        }else{
            switch (urand(0, 3))
            {
                case 0: entry = NPC_AZURE_CAPTAIN; break;
                case 1: entry = NPC_AZURE_RAIDER; break;
                case 2: entry = NPC_AZURE_SORCEROR; break;
                case 3: entry = NPC_AZURE_STALKER; break;
            }
        }
        return entry;
    }
    void SpawnGroup()
    {
        if(portalType == 0)
            return;

        uint8 uiSpawnCount = (m_pInstance->GetData(TYPE_RIFT) < 12) ? 3 : 4;
        for(uint8 i = 0; i < uiSpawnCount; i++)
        {
            uint32 uiSpawnEntry = SelectRandSummon();
            if(Creature* pSummoned = m_creature->SummonCreature(uiSpawnEntry, m_creature->GetPositionX()-5+rand()%10, m_creature->GetPositionY()-5+rand()%10, m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 300000))
            {
                debug_log("Spawn NPC %u, motherPortalID %u, portalLoc %u", uiSpawnEntry, portalID, portalLoc);
                ((mob_vh_dragonsAI*)pSummoned->AI())->motherPortalID = portalID;
                ((mob_vh_dragonsAI*)pSummoned->AI())->portalLoc = portalLoc;
            }
        }
    }
    bool IsThereNearElite(float range)
    {
        //Azure captain
        if(Creature* pTemp = GetClosestCreatureWithEntry(m_creature, NPC_AZURE_CAPTAIN, range))
        {
            if(((mob_vh_dragonsAI*)pTemp->AI())->motherPortalID == portalID)
                return true;
        }
        //Azure raider
        else if(Creature* pTemp = GetClosestCreatureWithEntry(m_creature, NPC_AZURE_RAIDER, range))
        {
            if(((mob_vh_dragonsAI*)pTemp->AI())->motherPortalID == portalID)
                return true;
        }
        //Azure sorceror
        else if(Creature* pTemp = GetClosestCreatureWithEntry(m_creature, NPC_AZURE_SORCEROR, range))
        {
            if(((mob_vh_dragonsAI*)pTemp->AI())->motherPortalID == portalID)
                return true;
        }
        //Azure Stalker
        else if(Creature* pTemp = GetClosestCreatureWithEntry(m_creature, NPC_AZURE_STALKER, range))
        {
            if(((mob_vh_dragonsAI*)pTemp->AI())->motherPortalID == portalID)
                return true;
        }
        return false;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_pInstance)
            return;

        if(m_pInstance->GetData(TYPE_EVENT) != IN_PROGRESS)
            return;

        switch(portalType)
        {
            case 0:
                return;
            case 1:
                if (TimeRiftWave_Timer < diff)
                {
                    debug_log("SpawnGroup()");
                    SpawnGroup();
                    TimeRiftWave_Timer = 15000;
                }else TimeRiftWave_Timer -= diff;

                if (!m_creature->IsNonMeleeSpellCasted(false))
                {
                    m_uiNextPortal_Timer = 5000;
                    debug_log("SD2: npc_time_rift: not casting anylonger, i need to die.");
                    m_creature->ForcedDespawn();
                }
                break;
            case 2:
                if(!m_uiGroupSpawned)
                {
                    SpawnGroup();
                    m_uiGroupSpawned = true;
                }
                if (Check_Timer < diff)
                {
                    if(!IsThereNearElite(150.0f))
                    {
                        m_uiNextPortal_Timer = 5000;
                        debug_log("SD2: npc_time_rift: No elite, i need to die.");
                        m_creature->ForcedDespawn();
                    }
                    Check_Timer = 1000;
                }else Check_Timer -= diff;
                break;
        }
    }
};
/*######
## npc_sinclari
######*/
enum
{
    SAY_BEGIN                   = -1608000,
    SAY_LOCK_DOOR               = -1608001,

    GOSSIP_ITEM_INTRO           = -3608000,
    GOSSIP_ITEM_START           = -3608001,

    GOSSIP_TEXT_ID_INTRO        = 13853,
    GOSSIP_TEXT_ID_START        = 13854,
    SAY_END                     = -1608009,
};

struct MANGOS_DLL_DECL npc_sinclariAI : public ScriptedAI
{
    npc_sinclariAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();
    }
    ScriptedInstance *m_pInstance;

    uint8 m_uiRiftPortalCount;
    uint8 m_bIsRegular;
    uint32 m_uiBossCheck_Timer;
    uint32 m_uiPortalCheck_Timer;
    std::list<Creature*> m_lGuardsList;
    uint32 m_uiWaypoint;
    bool start;
    uint32 m_uiWalkTimer;

    void Reset()
    {
        m_uiRiftPortalCount = 0;
        m_uiWalkTimer = 0;
        m_uiNextPortal_Timer = 0;
        m_uiBossCheck_Timer = 0;
        m_uiPortalCheck_Timer = 1000;
        m_bIsRegular = m_creature->GetMap()->IsRegularDifficulty();
        m_uiWaypoint = 0;
        start = false;
     }

    void CallGuards(bool bRespawn)
    {
    GetCreatureListWithEntryInGrid(m_lGuardsList, m_creature, NPC_GUARD, 50.0f);
    for(std::list<Creature*>::iterator i = m_lGuardsList.begin(); i != m_lGuardsList.end(); ++i)
    {
        if (Creature* pGuard = *i)
        {
            if (bRespawn)
            {
                pGuard->Respawn();
            }
            else if (pGuard->isAlive())
            {
                pGuard->AI()->EnterEvadeMode();

                if (Creature* pSinclari = GetClosestCreatureWithEntry(pGuard,NPC_SINCLARI,50.0f))
                    pGuard->GetMotionMaster()->MoveFollow(pSinclari, 0.0f, 0.0f);

                pGuard->ForcedDespawn(20000);
            }
        }
    }
}


    void SetEvent()
    {

        m_uiNextPortal_Timer = 5000;

        if (m_pInstance){
            m_pInstance->SetData(TYPE_EVENT, IN_PROGRESS);
            m_pInstance->DoUseDoorOrButton(GO_DOOR_SEAL);
        }
    }
    void EventEscort()
    {
        start=true;
        m_uiWalkTimer = 1000;


    }

    void DoSpawnPortal()
    {
        int tmp = urand(1, 6);
        if (Creature* pTemp = m_creature->SummonCreature(NPC_PORTAL, PortalLoc[tmp].x, PortalLoc[tmp].y, PortalLoc[tmp].z, 0, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 300000))
        {
            pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            //set portal type
            uint8 portalType = rand()%2+1;
            uint32 portalID = rand()%50000;
            ((npc_violet_portalAI*)pTemp->AI())->portalType = portalType; 
            ((npc_violet_portalAI*)pTemp->AI())->portalID = portalID;
            ((npc_violet_portalAI*)pTemp->AI())->portalLoc = tmp;

            if(portalType == 1)
            {
                uint32 entry = urand(0, 1) ? NPC_GUARDIAN : NPC_KEEPER;
                if (Creature* pSummoned = pTemp->SummonCreature(entry, PortalLoc[tmp].x, PortalLoc[tmp].y, PortalLoc[tmp].z, pTemp->GetOrientation(), TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 300000))
                {
                    pSummoned->AddThreat(pTemp);
                    pTemp->CastSpell(pSummoned, SPELL_PORTAL_CHANNEL,false);
                    ((mob_vh_dragonsAI*)pSummoned->AI())->motherPortalID = portalID;
                }
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(start)
        {
            if (m_uiWalkTimer <= uiDiff)
            {
                switch(m_uiWaypoint)
                {
                case 0: m_creature->GetMotionMaster()->MovePoint(m_uiWaypoint, SinclariWP[m_uiWaypoint].x, SinclariWP[m_uiWaypoint].y, SinclariWP[m_uiWaypoint].z); 
                    m_uiWalkTimer = 1000;
                    m_uiWaypoint++;
                    break;
                case 1: m_creature->GetMotionMaster()->MovePoint(m_uiWaypoint, SinclariWP[m_uiWaypoint].x, SinclariWP[m_uiWaypoint].y, SinclariWP[m_uiWaypoint].z); 
                    DoScriptText(SAY_BEGIN, m_creature);
                    CallGuards(false);
                    m_uiWalkTimer = 2000;
                    m_uiWaypoint++;
                    break;
                case 2:
                    m_creature->GetMotionMaster()->MovePoint(m_uiWaypoint, SinclariWP[m_uiWaypoint].x, SinclariWP[m_uiWaypoint].y, SinclariWP[m_uiWaypoint].z); 
                    m_uiWalkTimer = 3000;
                    m_uiWaypoint++;
                    break;
                case 3: m_creature->GetMotionMaster()->MovePoint(m_uiWaypoint, SinclariWP[m_uiWaypoint].x, SinclariWP[m_uiWaypoint].y, SinclariWP[m_uiWaypoint].z); 
                    DoScriptText(SAY_LOCK_DOOR, m_creature);
                    m_uiWalkTimer = 3000;
                    m_uiWaypoint++;
                    break;
                case 4:
                    m_creature->GetMotionMaster()->MovePoint(4, 1815.571f, 800.112f, 44.364f);
                    SetEvent();
                    m_uiWalkTimer = 3000;
                    m_uiWaypoint++;
                    break;
                case 5:
                    m_creature->GetMotionMaster()->Clear();
                    start = false;
                    m_uiWalkTimer = 0;
                    break;

                }
            }
            else m_uiWalkTimer -= uiDiff;
        }

        if(m_pInstance->GetData(TYPE_EVENT) != IN_PROGRESS || m_pInstance->GetData(TYPE_CYANIGOSA) == IN_PROGRESS)
            return;

        if (m_uiNextPortal_Timer && m_pInstance->GetData(TYPE_RIFT) != DONE  )
        {
            if (m_uiNextPortal_Timer <= uiDiff)
            {
                ++m_uiRiftPortalCount;
                if (m_pInstance && m_uiRiftPortalCount < 19)
                {
                    m_pInstance->DoUpdateWorldState(WORLD_STATE_VH_PORTALS, m_uiRiftPortalCount);
                    m_pInstance->SetData(TYPE_RIFT, SPECIAL);
                }

                if ( m_uiRiftPortalCount != 6
                     && m_uiRiftPortalCount != 12 
                     && m_uiRiftPortalCount != 18
                     && m_uiRiftPortalCount < 18 
                   )
                {
                    DoSpawnPortal();
                    if (m_uiRiftPortalCount < 12)
                        m_uiNextPortal_Timer = 120000;
                    else
                        m_uiNextPortal_Timer = 90000;
                }
                else if ( m_uiRiftPortalCount == 6
                         || m_uiRiftPortalCount == 12)
                {
                    if(m_uiRiftPortalCount == 6 && m_pInstance->GetData(TYPE_PORTAL6) != DONE || m_uiRiftPortalCount == 12 && m_pInstance->GetData(TYPE_PORTAL12) != DONE)
                    {
                        if (Creature* pTemp = m_creature->SummonCreature(NPC_PORTAL, PortalLoc[0].x, PortalLoc[0].y, PortalLoc[0].z, 0, TEMPSUMMON_TIMED_DESPAWN, 15000))
                        {
                            pTemp->SetRespawnDelay(7*DAY*IN_MILLISECONDS);
                            Creature* pSummoned = m_creature->SummonCreature(NPC_AZURE_SABOTEUR, PortalLoc[0].x, PortalLoc[0].y, PortalLoc[0].z, 0, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 30000);
//                          pSummoned->AddThreat(pTemp);
                            pTemp->CastSpell(pSummoned, SPELL_PORTAL_CHANNEL, false);
                            if(m_uiRiftPortalCount == 6) {m_pInstance->SetData(TYPE_PORTAL6, IN_PROGRESS);}
                            else m_pInstance->SetData(TYPE_PORTAL12, IN_PROGRESS);
                        }
                        m_uiNextPortal_Timer =  m_bIsRegular ? 180000 : 120000;
                    }
                    else m_uiNextPortal_Timer = 4000;
                    m_pInstance->SetData(TYPE_RIFT, IN_PROGRESS);
                    m_uiBossCheck_Timer = 1000;
                }
                else if (m_uiRiftPortalCount == 18 && m_pInstance->GetData(TYPE_RIFT) != DONE)
                {
                    Creature* pCyanigossa = GetClosestCreatureWithEntry(m_creature, NPC_CYANIGOSA, 300.0f);

                    if (!pCyanigossa) 
                    {
                        m_creature->SummonCreature(NPC_CYANIGOSA, 1922.420f, 803.240f, 52.40f, 3.022f, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN , 99999999);}
                        m_pInstance->SetData(TYPE_RIFT, DONE);
                        m_pInstance->SetData(TYPE_DISRUPTIONS, 20);
                       m_uiNextPortal_Timer = 1*DAY;
                }
            }
            else
                m_uiNextPortal_Timer -= uiDiff;

            return;
        }

        if (m_uiBossCheck_Timer)
        {
            if (m_uiBossCheck_Timer <= uiDiff)
            {
                if (!m_pInstance->GetData(DATA_BOSSTIME))
                    m_uiNextPortal_Timer = 10000;
                m_uiBossCheck_Timer = 1000;
            }
            else
                m_uiBossCheck_Timer -= uiDiff;

            return;
        }
    }
};

bool GossipHello_npc_sinclari(Player* pPlayer, Creature* pCreature)
{
    if (ScriptedInstance* m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData())
    {
        if (m_pInstance->GetData(TYPE_EVENT) == NOT_STARTED || m_pInstance->GetData(TYPE_EVENT) == FAIL)
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_INTRO, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_INTRO, pCreature->GetObjectGuid());
        }
    }
    return true;
}

bool GossipSelect_npc_sinclari(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        if (ScriptedInstance* m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData())
        {
            if (m_pInstance->GetData(TYPE_EVENT) == NOT_STARTED || m_pInstance->GetData(TYPE_EVENT) == FAIL)
            {
                pPlayer->PlayerTalkClass->ClearMenus();
                pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_START, pCreature->GetObjectGuid());
            }
        }
        else
            pPlayer->CLOSE_GOSSIP_MENU();
    }

    if (uiAction == GOSSIP_ACTION_INFO_DEF+2)
    {
        if (ScriptedInstance* m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData())
        {
            pPlayer->CLOSE_GOSSIP_MENU();

            if (m_pInstance->GetData(TYPE_EVENT) == NOT_STARTED || m_pInstance->GetData(TYPE_EVENT) == FAIL)
            {

                if (Creature* pSinclari = (m_pInstance->GetSingleCreatureFromStorage(NPC_SINCLARI)))

                    ((npc_sinclariAI*)pSinclari->AI())->EventEscort();
            }
        }
        else
            pPlayer->CLOSE_GOSSIP_MENU();
    }

    return true;
}
/*######
## npc_door_seal_vh
######*/
struct MANGOS_DLL_DECL npc_door_sealAI : public ScriptedAI
{
    npc_door_sealAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 CheckTimer;
    uint32 SpellCorrupt_Timer;
    uint8 lastPortal;

    void Reset()
    {
        CheckTimer = 0;
        SpellCorrupt_Timer = 0;
        lastPortal = 0;
}
    void SpellHit(Unit* caster, const SpellEntry* spell)
    {
        if (SpellCorrupt_Timer)
            return;

        if (spell->Id == SPELL_CORRUPT) {
            SpellCorrupt_Timer = 1000;
            }
    }
    void JustDied(Unit* pKiller)
    {
        m_creature->Respawn();
    }

    void UpdateAI(const uint32 diff){
        if (SpellCorrupt_Timer)
        {
            if (SpellCorrupt_Timer <= diff)
            {
                if (m_creature->HasAura(SPELL_CORRUPT,EFFECT_INDEX_0))
                    SpellCorrupt_Timer = 1500;
                else
                    SpellCorrupt_Timer = 0;
            }else SpellCorrupt_Timer -= diff;
        }
    }
};

/*######
## npc_azure_saboteur
######*/
struct MANGOS_DLL_DECL npc_azure_saboteurAI : public ScriptedAI
{
    npc_azure_saboteurAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();
    }
    ScriptedInstance *m_pInstance;

    bool m_bIsActiving;

    uint32 m_uiDisruption_Timer;
    uint32 m_uiDisruptionCounter;
    uint32 m_uiDisruptionsCount;

    uint8 m_uiBossID;
    uint8 m_bIsRegular;
    uint32 m_uiBossType;
    uint32 m_uiDoorGUID;

    void AttackStart(Unit* pWho)
    {
        return;
    }

    void Reset()
    {
        m_bIsActiving = false;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetWalk(false);
        m_uiDisruptionCounter = 0;
        m_uiDisruptionsCount = 0;
        m_uiDisruption_Timer = 1000;
//        m_bIsRegular = m_creature->GetMap()->IsRegularDifficulty();

        if (m_pInstance)
        {
            m_uiBossID = m_pInstance->GetData(TYPE_LASTBOSS);
            m_uiDisruptionsCount = m_pInstance->GetData(TYPE_DISRUPTIONS);
            switch (m_uiBossID)
            {
                case 6: // Lavanthor
                    m_uiBossType = TYPE_LAVANTHOR;
                    m_uiDoorGUID = GO_DOOR_LAVANTHOR;
                    break;
                case 7: // Zuramat
                    m_uiBossType = TYPE_ZURAMAT;
                    m_uiDoorGUID = GO_DOOR_ZURAMAT;
                    break;
                case 3: // Moragg
                    m_uiBossType = TYPE_MORAGG;
                    m_uiDoorGUID = GO_DOOR_MORAGG;
                    break;
                case 2: // Erekem
                    m_uiBossType = TYPE_EREKEM;
                    m_uiDoorGUID = GO_DOOR_EREKEM;
                    break;
                case 4: // Ichoron
                    m_uiBossType = TYPE_ICHORON;
                    m_uiDoorGUID = GO_DOOR_ICHORON;
                    break;
                case 5: // Xevozz
                    m_uiBossType = TYPE_XEVOZZ;
                    m_uiDoorGUID = GO_DOOR_XEVOZZ;
                    break;
                case 0: // No boss
                    m_uiBossType = 0;
                    break;
            }
            m_pInstance->SetData(TYPE_LASTBOSS_ID, m_uiBossType);
            if (m_uiBossType != 0)  m_creature->GetMotionMaster()->MovePoint(0, BossLoc[m_uiBossID].x,  BossLoc[m_uiBossID].y,  BossLoc[m_uiBossID].z);
            else  m_creature->GetMotionMaster()->MovePoint(0, 1827.960f, 804.208f, 44.364f);
        }
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if(uiType != POINT_MOTION_TYPE)
                return;

        switch(uiPointId)
        {
            case 0:
                m_bIsActiving = true;
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bIsActiving)
            if (m_uiDisruption_Timer < uiDiff)
            {
                if (m_uiDisruptionCounter < 3) 
                {
                    DoCast(m_creature, SPELL_SHIELD_DISRUPTION);
                    ++m_uiDisruptionsCount;
                    m_pInstance->SetData(TYPE_DISRUPTIONS, m_uiDisruptionsCount);
                }
                else if (m_uiDisruptionCounter == 3)
                {
                    m_pInstance->DoUseDoorOrButton(m_uiDoorGUID);
                    if (m_uiBossType == TYPE_EREKEM) 
                    {
                        m_pInstance->DoUseDoorOrButton(GO_DOOR_EREKEM_RIGHT);
                        m_pInstance->DoUseDoorOrButton(GO_DOOR_EREKEM_LEFT);
                    }
                }
                else 
                {
                    m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    if (m_pInstance->GetData(TYPE_LASTBOSS_ID) != 0)
                        m_pInstance->SetData(m_pInstance->GetData(TYPE_LASTBOSS_ID), SPECIAL);
                    m_bIsActiving = false;
                }

                ++m_uiDisruptionCounter;
                m_uiDisruption_Timer = 1000;
            }
            else m_uiDisruption_Timer -= uiDiff;
    }
};



CreatureAI* GetAI_npc_azure_saboteur(Creature* pCreature)
{
    return new npc_azure_saboteurAI (pCreature);
}

CreatureAI* GetAI_mob_vh_dragons(Creature* pCreature)
{
    return new mob_vh_dragonsAI(pCreature);
}
CreatureAI* GetAI_npc_sinclari(Creature* pCreature)
{
    return new npc_sinclariAI (pCreature);
}
CreatureAI* GetAI_npc_violet_portal(Creature* pCreature)
{
    return new npc_violet_portalAI (pCreature);
}
CreatureAI* GetAI_npc_door_seal(Creature* pCreature)
{
    return new npc_door_sealAI(pCreature);
}

void AddSC_violet_hold()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_sinclari";
    newscript->GetAI = &GetAI_npc_sinclari;
    newscript->pGossipHello =  &GossipHello_npc_sinclari;
    newscript->pGossipSelect = &GossipSelect_npc_sinclari;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_violet_portal";
    newscript->GetAI = &GetAI_npc_violet_portal;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_door_seal_vh";
    newscript->GetAI = &GetAI_npc_door_seal;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_azure_saboteur";
    newscript->GetAI = &GetAI_npc_azure_saboteur;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_vh_dragons";
    newscript->GetAI = &GetAI_mob_vh_dragons;
    newscript->RegisterSelf();

}
