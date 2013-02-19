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
SDName: Boss Sartharion
SD%Complete: 95%
SDComment: Used some workarounds.
SDCategory: Obsidian Sanctum
EndScriptData */

#include "precompiled.h"
#include "obsidian_sanctum.h"

enum
{
    //Sartharion Yell
    SAY_SARTHARION_AGGRO                = -1615018,
    SAY_SARTHARION_BERSERK              = -1615019,
    SAY_SARTHARION_BREATH               = -1615020,
    SAY_SARTHARION_CALL_SHADRON         = -1615021,
    SAY_SARTHARION_CALL_TENEBRON        = -1615022,
    SAY_SARTHARION_CALL_VESPERON        = -1615023,
    SAY_SARTHARION_DEATH                = -1615024,
    SAY_SARTHARION_SPECIAL_1            = -1615025,
    SAY_SARTHARION_SPECIAL_2            = -1615026,
    SAY_SARTHARION_SPECIAL_3            = -1615027,
    SAY_SARTHARION_SPECIAL_4            = -1615028,
    SAY_SARTHARION_SLAY_1               = -1615029,
    SAY_SARTHARION_SLAY_2               = -1615030,
    SAY_SARTHARION_SLAY_3               = -1615031,

    WHISPER_LAVA_CHURN                  = -1615032,

    WHISPER_SHADRON_DICIPLE             = -1615008,
    WHISPER_VESPERON_DICIPLE            = -1615041,
    WHISPER_HATCH_EGGS                  = -1615017,
    WHISPER_OPEN_PORTAL                 = -1615042, // whisper, shared by two dragons

    //Sartharion Spells
    SPELL_BERSERK                       = 61632, // Increases the caster's attack speed by 150% and all damage it deals by 500% for 5 min.
    SPELL_CLEAVE                        = 56909, // Inflicts 35% weapon damage to an enemy and its nearest allies, affecting up to 10 targets.
    SPELL_FLAME_BREATH                  = 56908, // Inflicts 8750 to 11250 Fire damage to enemies in a cone in front of the caster.
    SPELL_FLAME_BREATH_H                = 58956, // Inflicts 10938 to 14062 Fire damage to enemies in a cone in front of the caster.
    SPELL_TAIL_LASH                     = 56910, // A sweeping tail strike hits all enemies behind the caster, inflicting 3063 to 3937 damage and stunning them for 2 sec.
    SPELL_TAIL_LASH_H                   = 58957, // A sweeping tail strike hits all enemies behind the caster, inflicting 4375 to 5625 damage and stunning them for 2 sec.
    SPELL_WILL_OF_SARTHARION            = 61254, // Sartharion's presence bolsters the resolve of the Twilight Drakes, increasing their total health by 25%. This effect also increases Sartharion's health by 25%.
    SPELL_LAVA_STRIKE                   = 57571, // (Real spell casted should be 57578) 57571 then trigger visual missile, then summon Lava Blaze on impact(spell 57572)
    SPELL_LAVA_STRIKE_DMG               = 57591,
    SPELL_CYCLONE_AURA                  = 57560, // knock back aura
    SPELL_CYCLONE_AURA_2                = 57598,
    SPELL_TWILIGHT_REVENGE              = 60639,

    SPELL_PYROBUFFET                    = 58907, // currently used for hard enrage after 15 minutes
    SPELL_PYROBUFFET_RANGE              = 56916, // possibly used when player get too far away from dummy creatures (2x creature entry 30494)

    SPELL_TWILIGHT_SHIFT_ENTER          = 57620, // enter phase. Player get this when click GO
    SPELL_TWILIGHT_SHIFT_REMOVAL        = 61187, // leave phase
    SPELL_TWILIGHT_SHIFT_REMOVAL_ALL    = 61190, // leave phase (probably version to make all leave)

    SPELL_TWILIGHT_RESIDUE              = 61885, // makes immune to shadow damage, applied when leave phase

    //Miniboses (Vesperon, Shadron, Tenebron)
    SPELL_SHADOW_BREATH_H               = 59126, // Inflicts 8788 to 10212 Fire damage to enemies in a cone in front of the caster.
    SPELL_SHADOW_BREATH                 = 57570, // Inflicts 6938 to 8062 Fire damage to enemies in a cone in front of the caster.

    SPELL_SHADOW_FISSURE_H              = 59127, // Deals 9488 to 13512 Shadow damage to any enemy within the Shadow fissure after 5 sec.
    SPELL_SHADOW_FISSURE                = 57579, // Deals 6188 to 8812 Shadow damage to any enemy within the Shadow fissure after 5 sec.

    //Vesperon
    //In portal is a disciple, when disciple killed remove Power_of_vesperon, portal open multiple times
    SPELL_POWER_OF_VESPERON             = 61251, // Vesperon's presence decreases the maximum health of all enemies by 25%.
    SPELL_TWILIGHT_TORMENT_VESP_SOLO    = 57948, // (Shadow only) trigger 57935 then 57988
    SPELL_TWILIGHT_TORMENT_VESP_SARTH   = 58853, // (Fire and Shadow) trigger 58835 then 57988
    SPELL_TWILIGHT_TORMENT_AURA         = 57935,
    SPELL_TWILIGHT_TORMENT_DEBUFF       = 58835,

    //Shadron
    //In portal is a disciple, when disciple killed remove Power_of_vesperon, portal open multiple times
    SPELL_POWER_OF_SHADRON              = 58105, // Shadron's presence increases Fire damage taken by all enemies by 100%.
    SPELL_GIFT_OF_TWILIGTH_SHA          = 57835, // TARGET_SCRIPT shadron
    SPELL_GIFT_OF_TWILIGTH_SAR          = 58766, // TARGET_SCRIPT sartharion

    //Tenebron
    //in the portal spawns 6 eggs, if not killed in time (approx. 20s) they will hatch, whelps can cast 60708
    SPELL_POWER_OF_TENEBRON             = 61248, // Tenebron's presence increases Shadow damage taken by all enemies by 100%.

    SPELL_HATCH_EGGS                    = 58542,
    SPELL_HATCH_SARTHARION_EGGS         = 59189,
    SPELL_HATCH_EGGS_EFFECT_H           = 59190,
    SPELL_HATCH_EGGS_EFFECT             = 58685,

    SPELL_SUMMON_TWILIGHT_WHELP         = 58035,
    SPELL_SUMMON_SARTH_TWILIGHT_WHELP   = 58826,

    NPC_TWILIGHT_EGG                    = 30882,
    NPC_SARTHARION_TWILIGHT_EGG         = 31204,
    NPC_TWILIGHT_EGG_CONTROLLER         = 31138,

    //Whelps
    NPC_TWILIGHT_WHELP                  = 30890,
    NPC_SARTHARION_TWILIGHT_WHELP       = 31214,
    SPELL_FADE_ARMOR                    = 60708, // Reduces the armor of an enemy by 1500 for 15s

    //flame tsunami
    SPELL_FLAME_TSUNAMI                 = 57494, // the visual dummy
    SPELL_FLAME_TSUNAMI_LEAP            = 60241, // SPELL_EFFECT_138 some leap effect, causing caster to move in direction
    SPELL_FLAME_TSUNAMI_DMG_AURA        = 57492, // periodic damage, npc has this aura
    SPELL_FLAME_TSUNAMI_DMG             = 57491, // damage players
    SPELL_FLAME_TSUNAMI_BUFF            = 60430, // buff Lava Blazes

    NPC_FLAME_TSUNAMI                   = 30616, // for the flame waves
    NPC_LAVA_BLAZE                      = 30643, // adds spawning from flame strike
    NPC_FIRE_CYCLONE                    = 30648,
    NPC_ORB_TARGET                      = 25640, // used for summoning Lava Blazes

    //using these custom points for dragons start and end
    POINT_ID_INIT                       = 100,
    POINT_ID_LAND                       = 200
};

struct Waypoint
{
    float m_fX, m_fY, m_fZ;
};

//each dragons special points. First where fly to before connect to connon, second where land point is.
Waypoint m_aTene[]=
{
    {3212.854f, 575.597f, 109.856f}, // init
    {3246.425f, 565.367f, 61.249f}   // end
};

Waypoint m_aShad[]=
{
    {3293.238f, 472.223f, 106.968f},
    {3227.268f, 533.238f, 59.995f}
};

Waypoint m_aVesp[]=
{
    {3193.310f, 472.861f, 102.697f},
    {3271.669f, 526.907f, 61.931f}
};

//points around raid "isle", counter clockwise. should probably be adjusted to be more alike
Waypoint m_aDragonCommon[]=
{
    {3214.012f, 468.932f, 90.652f},
    {3244.950f, 468.427f, 90.652f},
    {3283.520f, 496.869f, 90.652f},
    {3287.316f, 555.875f, 90.652f},
    {3250.479f, 585.827f, 90.652f},
    {3209.969f, 566.523f, 90.652f}
};

float m_afTsunamiStartLoc[5][4]=
{
    //left to right
    {3201.0f, 481.82f, 58.6f, 6.23f}, // covers 470.82 - 481.82 - 492.82
    {3201.0f, 524.50f, 58.6f, 6.23f}, // covers 513.50 - 524.50 - 535.50
    {3201.0f, 566.64f, 58.6f, 6.23f}, // covers 555.64 - 566.64 - 577.64
    //right to left
    {3287.5f, 545.50f, 58.6f, 3.19f}, // covers 534.50 - 545.50 - 556.50
    {3287.5f, 503.00f, 58.6f, 3.19f}, // covers 492.00 - 503.00 - 514.00
};




/*######
## Boss Sartharion
######*/

struct MANGOS_DLL_DECL boss_sartharionAI : public ScriptedAI
{
    boss_sartharionAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_obsidian_sanctum*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_bTenebronHelpedInFight = false;
        m_bShadronHelpedInFight = false;
        m_bVesperonHelpedInFight = false;
        Reset();
    }

    instance_obsidian_sanctum* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bIsBerserk;
    bool m_bIsSoftEnraged;

    uint32 m_uiEnrageTimer;
    bool m_bIsHardEnraged;

    uint32 m_uiCurrentCallDragonData;
    uint32 m_uiCallDragonTimer;

    uint32 m_uiFlameTsunamiTimer;
    uint32 m_uiFlameBreathTimer;
    uint32 m_uiTailSweepTimer;
    uint32 m_uiCleaveTimer;
    uint32 m_uiCycloneAuraTimer;

    bool m_bHasCalledTenebron;
    bool m_bHasCalledShadron;
    bool m_bHasCalledVesperon;
    bool m_bTenebronHelpedInFight;
    bool m_bShadronHelpedInFight;
    bool m_bVesperonHelpedInFight;

    bool bCanUseWill;
    bool bFirstWill;

    uint32 m_uiSarthHealth;
    uint32 m_uiTeneHealth;
    uint32 m_uiShadHealth;
    uint32 m_uiVespHealth;

    uint32 m_uiCheckTwilightTimer;

    std::list<Creature*> m_lFireCyclones;

    void Reset()
    {
        m_bIsBerserk = false;
        m_bIsSoftEnraged = false;

        m_uiEnrageTimer = 15*MINUTE*IN_MILLISECONDS;
        m_bIsHardEnraged = false;

        m_uiCallDragonTimer = 30000;
        m_uiCurrentCallDragonData = 0;

        m_uiFlameTsunamiTimer = 30000;
        m_uiFlameBreathTimer = 20000;
        m_uiTailSweepTimer = 5000;
        m_uiCleaveTimer = 7000;
        m_uiCycloneAuraTimer = 10000;

        m_bHasCalledTenebron = false;
        m_bHasCalledShadron = false;
        m_bHasCalledVesperon = false;

        bCanUseWill = false;
        bFirstWill = true;

        m_uiCheckTwilightTimer = 2000;

        if (m_pInstance)
        {
            Creature* pTene = m_pInstance->GetSingleCreatureFromStorage(NPC_TENEBRON);
            Creature* pShad = m_pInstance->GetSingleCreatureFromStorage(NPC_SHADRON);
            Creature* pVesp = m_pInstance->GetSingleCreatureFromStorage(NPC_VESPERON);

            if (m_bTenebronHelpedInFight && pTene)
            {
                if (pTene->isDead())
                    pTene->Respawn();
                else
                    pTene->AI()->EnterEvadeMode();
            }

            if (m_bShadronHelpedInFight && pShad)
            {
                if (pShad->isDead())
                    pShad->Respawn();
                else
                    pShad->AI()->EnterEvadeMode();
            }

            if (m_bVesperonHelpedInFight && pVesp)
            {
                if (pVesp->isDead())
                    pVesp->Respawn();
                else
                    pVesp->AI()->EnterEvadeMode();
            }

            m_pInstance->m_lHitByVolcanoGUIDList.clear();
        }

        m_bTenebronHelpedInFight = false;
        m_bShadronHelpedInFight = false;
        m_bVesperonHelpedInFight = false;
    }

    void JustReachedHome()
    {
        if (!m_pInstance)
            return;

        m_pInstance->SetData(TYPE_SARTHARION_EVENT, FAIL);

        if (m_pInstance->m_lBlazesGUIDList.empty() )
            return;

        for (GUIDList::iterator i = m_pInstance->m_lBlazesGUIDList.begin(); i != m_pInstance->m_lBlazesGUIDList.end(); i++)
            if (Creature *pBlaze = m_pInstance->instance->GetCreature(*i))
                if (pBlaze->isAlive())
                    pBlaze->ForcedDespawn();
        m_pInstance->m_lBlazesGUIDList.clear();
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_SARTHARION_AGGRO,m_creature);
        DoCastSpellIfCan(m_creature, SPELL_PYROBUFFET_RANGE, CAST_TRIGGERED);

        m_creature->SetInCombatWithZone();

        if (m_pInstance)
        {
            // trash mobs in the area should help Sartharion in the fight
            if (!m_pInstance->m_lTrashMobsGUIDlist.empty())
            {
                for (GUIDList::iterator itr = m_pInstance->m_lTrashMobsGUIDlist.begin(); itr != m_pInstance->m_lTrashMobsGUIDlist.end(); itr++)
                {
                    if (Creature *pCreature = m_pInstance->instance->GetCreature(*itr) )
                        pCreature->AI()->AttackStart(pWho);
                }
            }

            m_pInstance->SetData(TYPE_SARTHARION_EVENT, IN_PROGRESS);
            FetchDragons();
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_SARTHARION_DEATH, m_creature);

        if (!m_pInstance)
            return;

        // Hard Mode implementation
        uint8 uiHardMode = 0;
        if (m_bTenebronHelpedInFight)
            ++uiHardMode;
        if (m_bShadronHelpedInFight)
            ++uiHardMode;
        if (m_bVesperonHelpedInFight)
            ++uiHardMode;

        if (uiHardMode)
        {
            for (int8 i = uiHardMode; i > 0; i--)
                m_pInstance->SetData(i + 1, DONE);
        }

        m_pInstance->SetData(TYPE_SARTHARION_EVENT, DONE);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(SAY_SARTHARION_SLAY_1 - urand(0, 2), m_creature);
    }

    void FetchDragons()
    {
        if (!m_pInstance)
            return;

        Creature* pTene = m_pInstance->GetSingleCreatureFromStorage(NPC_TENEBRON);
        Creature* pShad = m_pInstance->GetSingleCreatureFromStorage(NPC_SHADRON);
        Creature* pVesp = m_pInstance->GetSingleCreatureFromStorage(NPC_VESPERON);

        //if at least one of the dragons are alive and are being called
        if (pTene && pTene->isAlive() && !pTene->getVictim())
        {
            m_bTenebronHelpedInFight = true;
            bCanUseWill = true;
            pTene->CastSpell(pTene, SPELL_POWER_OF_TENEBRON, false);
            pTene->SetLevitate(true);
            pTene->SetWalk(false);
            pTene->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pTene->GetMotionMaster()->MovePoint(POINT_ID_INIT, m_aTene[0].m_fX, m_aTene[0].m_fY, m_aTene[0].m_fZ);
        }

        if (pShad && pShad->isAlive() && !pShad->getVictim())
        {
            m_bShadronHelpedInFight = true;
            bCanUseWill = true;
            pShad->CastSpell(pShad, SPELL_POWER_OF_SHADRON, false);
            pShad->SetLevitate(true);
            pShad->SetWalk(false);
            pShad->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pShad->GetMotionMaster()->MovePoint(POINT_ID_INIT, m_aShad[0].m_fX, m_aShad[0].m_fY, m_aShad[0].m_fZ);
        }

        if (pVesp && pVesp->isAlive() && !pVesp->getVictim())
        {
            m_bVesperonHelpedInFight = true;
            bCanUseWill = true;
            pVesp->CastSpell(pVesp, SPELL_POWER_OF_VESPERON, false);
            pVesp->SetLevitate(true);
            pVesp->SetWalk(false);
            pVesp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pVesp->GetMotionMaster()->MovePoint(POINT_ID_INIT, m_aVesp[0].m_fX, m_aVesp[0].m_fY, m_aVesp[0].m_fZ);
        }

        if (bCanUseWill)
            DoCast(m_creature, SPELL_WILL_OF_SARTHARION);
    }

    bool CallDragon(uint32 uiDataId)
    {
        uint32 uiEntry;

        switch (uiDataId)
        {
            case DATA_TENEBRON:
                uiEntry = NPC_TENEBRON;
                break;
            case DATA_SHADRON:
                uiEntry = NPC_SHADRON;
                break;
            case DATA_VESPERON:
                uiEntry = NPC_VESPERON;
                break;
        }

        if (m_pInstance)
        {
            Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(uiEntry);

            if (pTemp && pTemp->isAlive() && !pTemp->getVictim())
            {
                pTemp->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                pTemp->SetWalk(false);

                m_uiSarthHealth = m_creature->GetHealth();
                DoCast(m_creature, SPELL_WILL_OF_SARTHARION);
                bCanUseWill = true;

                int32 iTextId = 0;

                Creature* pAdd = NULL;
                if (pAdd = m_pInstance->GetSingleCreatureFromStorage(NPC_TENEBRON))
                    m_uiTeneHealth = pAdd->GetHealth();
                if (pAdd = m_pInstance->GetSingleCreatureFromStorage(NPC_SHADRON))
                    m_uiShadHealth = pAdd->GetHealth();
                if (pAdd = m_pInstance->GetSingleCreatureFromStorage(NPC_VESPERON))
                    m_uiVespHealth = pAdd->GetHealth();

                switch (pTemp->GetEntry())
                {
                    case NPC_TENEBRON:
                        iTextId = SAY_SARTHARION_CALL_TENEBRON;
                        pTemp->GetMotionMaster()->MovementExpired(true);
                        pTemp->GetMotionMaster()->MovePoint(POINT_ID_LAND, m_aTene[1].m_fX, m_aTene[1].m_fY, m_aTene[1].m_fZ);
                        break;
                    case NPC_SHADRON:
                        iTextId = SAY_SARTHARION_CALL_SHADRON;
                        pTemp->GetMotionMaster()->MovementExpired(true);
                        pTemp->GetMotionMaster()->MovePoint(POINT_ID_LAND, m_aShad[1].m_fX, m_aShad[1].m_fY, m_aShad[1].m_fZ);
                        break;
                    case NPC_VESPERON:
                        iTextId = SAY_SARTHARION_CALL_VESPERON;
                        pTemp->GetMotionMaster()->MovementExpired(true);
                        pTemp->GetMotionMaster()->MovePoint(POINT_ID_LAND, m_aVesp[1].m_fX, m_aVesp[1].m_fY, m_aVesp[1].m_fZ);
                        break;
                }

                DoScriptText(iTextId, m_creature);
                return true;
            }
        }
        return false;
    }

    void SendFlameTsunami()
    {
        Map* pMap = m_creature->GetMap();

        if (pMap && pMap->IsDungeon())
        {
            Map::PlayerList const &PlayerList = pMap->GetPlayers();

            if (!PlayerList.isEmpty())
            {
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                {
                    if (i->getSource()->isAlive())
                        DoScriptText(WHISPER_LAVA_CHURN, m_creature, i->getSource());
                }
            }
        }

        uint8 uiTsunamiDirection = urand(0, 1);
        uint8 uiTsunamiWavesAmount = 3;
        if (uiTsunamiDirection)
            uiTsunamiWavesAmount = 2;
        for (uint8 i = uiTsunamiDirection*3; i < uiTsunamiDirection*3+uiTsunamiWavesAmount; ++i)
        {
            // Tsunami visual - dummy npc
            if (Creature *pTsunami1 = m_creature->SummonCreature(NPC_FLAME_TSUNAMI, m_afTsunamiStartLoc[i][0], m_afTsunamiStartLoc[i][1], m_afTsunamiStartLoc[i][2], m_afTsunamiStartLoc[i][3], TEMPSUMMON_TIMED_DESPAWN, 12000))
                pTsunami1->CastSpell(pTsunami1, SPELL_FLAME_TSUNAMI, true);
            // Tsunami dmg aura - 2 dummy npcs
            if (Creature *pTsunami2 = m_creature->SummonCreature(NPC_FLAME_TSUNAMI, m_afTsunamiStartLoc[i][0], m_afTsunamiStartLoc[i][1] - 5.0f, m_afTsunamiStartLoc[i][2], m_afTsunamiStartLoc[i][3], TEMPSUMMON_TIMED_DESPAWN, 12000))
                pTsunami2->CastSpell(pTsunami2, SPELL_FLAME_TSUNAMI_DMG_AURA, true);
            if (Creature *pTsunami2 = m_creature->SummonCreature(NPC_FLAME_TSUNAMI, m_afTsunamiStartLoc[i][0], m_afTsunamiStartLoc[i][1] + 5.0f, m_afTsunamiStartLoc[i][2], m_afTsunamiStartLoc[i][3], TEMPSUMMON_TIMED_DESPAWN, 12000))
                pTsunami2->CastSpell(pTsunami2, SPELL_FLAME_TSUNAMI_DMG_AURA, true);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // If the current target is in other realm, then Sartharion will lose his threat on the target
        if (!(m_creature->GetPhaseMask() & m_creature->getVictim()->GetPhaseMask()) )
            m_creature->getThreatManager().modifyThreatPercent(m_creature->getVictim(), -100);

        if (bCanUseWill)
        {
            if (bFirstWill)
            {
                m_creature->SetHealth(m_creature->GetHealth()*1.25);
            }
            else
            {
                m_creature->SetHealth(m_uiSarthHealth);
                Creature* pTemp = NULL;
                pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_TENEBRON);
                if (pTemp && pTemp->isAlive())
                    pTemp->SetHealth(m_uiTeneHealth);
                pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_SHADRON);
                if (pTemp && pTemp->isAlive())
                    pTemp->SetHealth(m_uiShadHealth);
                pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_VESPERON);
                if (pTemp && pTemp->isAlive())
                    pTemp->SetHealth(m_uiVespHealth);
            }
            bCanUseWill = false;
            bFirstWill = false;
        }


        // Enrage - cast if any drake is still alive at Sartharion's 25%
        if (!m_bIsBerserk && m_creature->GetHealthPercent() <= 25.0f)
        {
            Creature* pTemp = NULL;
            pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_TENEBRON);
            if (!pTemp || !pTemp->isAlive())
                pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_SHADRON);
            if (!pTemp || !pTemp->isAlive())
                pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_VESPERON);

            if (pTemp && pTemp->isAlive() )
            {
                DoScriptText(SAY_SARTHARION_BERSERK, m_creature);
                DoCast(m_creature, SPELL_BERSERK);
                m_bIsBerserk = true;
            }
        }

        //soft enrage
        if (!m_bIsSoftEnraged && m_creature->GetHealthPercent() <= 10.0f)
        {
            m_bIsSoftEnraged = true;
        }

        // hard enrage
        if (!m_bIsHardEnraged)
        {
            if (m_uiEnrageTimer < uiDiff)
            {
                DoCast(m_creature, SPELL_PYROBUFFET, true);
                m_bIsHardEnraged = true;
            }
            else
                m_uiEnrageTimer -= uiDiff;
        }

        // flame tsunami
        if (m_uiFlameTsunamiTimer < uiDiff)
        {
            SendFlameTsunami();
            DoScriptText(SAY_SARTHARION_SPECIAL_1 - urand(0, 3), m_creature);
            m_uiFlameTsunamiTimer = 30000;
        }
        else
            m_uiFlameTsunamiTimer -= uiDiff;

        // flame breath
        if (m_uiFlameBreathTimer < uiDiff)
        {
            DoScriptText(SAY_SARTHARION_BREATH, m_creature);
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_FLAME_BREATH : SPELL_FLAME_BREATH_H);
            m_uiFlameBreathTimer = urand(25000, 35000);
        }
        else
            m_uiFlameBreathTimer -= uiDiff;

        // Tail Sweep
        if (m_uiTailSweepTimer < uiDiff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_TAIL_LASH : SPELL_TAIL_LASH_H);
            m_uiTailSweepTimer = urand(5000, 7000);
        }
        else
            m_uiTailSweepTimer -= uiDiff;

        // Cleave
        if (m_uiCleaveTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_CLEAVE);
            m_uiCleaveTimer = urand(7000, 10000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        // Call dragon
        if ((m_uiCurrentCallDragonData < 3) && m_uiCallDragonTimer < uiDiff)
        {
            if (CallDragon(DATA_TENEBRON + m_uiCurrentCallDragonData++))
                m_uiCallDragonTimer = 45000 + (m_uiCurrentCallDragonData - 1) * 10000;
        }
        else
            m_uiCallDragonTimer -= uiDiff;

        // Check if any player should leave the Twilight Realm
        if (m_uiCheckTwilightTimer < uiDiff)
        {
            if (m_pInstance)
            {
                bool bNoAliveTwilightRealm = true;

                if (Creature* pAcolyte = m_pInstance->GetSingleCreatureFromStorage(NPC_ACOLYTE_OF_SHADRON))
                {
                    if (pAcolyte->isAlive())
                        bNoAliveTwilightRealm = false;
                }
                if (Creature* pAcolyte = m_pInstance->GetSingleCreatureFromStorage(NPC_ACOLYTE_OF_VESPERON))
                {
                    if (pAcolyte->isAlive())
                    {
                        bNoAliveTwilightRealm = false;

                        // Sartharion event is in progress
                        SpellEntry* pTempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(58835);
                        if (pTempSpell)
                        {
                            // LOLWHAT
                            //pTempSpell->GetStackAmount() = 1;
                            //pTempSpell->procCharges = 1;
                            m_creature->CastSpell(m_creature, pTempSpell, true);
                        }
                    }
                }
                if (!m_pInstance->m_lEggsGUIDList.empty())
                    for (GUIDList::iterator itr = m_pInstance->m_lEggsGUIDList.begin(); itr != m_pInstance->m_lEggsGUIDList.end(); ++itr)
                        if (Creature* pTemp = m_pInstance->instance->GetCreature(*itr))
                            if (pTemp->isAlive())
                                bNoAliveTwilightRealm = false;

                // probably correct way, since removing from twilight realm should only occur,
                // when no creatures are present in it. or maybe there should be some dummy npc for this?
                if (bNoAliveTwilightRealm)
                {
                    Map* pMap = m_creature->GetMap();

                    if (pMap && pMap->IsDungeon())
                    {
                        Map::PlayerList const &PlayerList = pMap->GetPlayers();

                        if (!PlayerList.isEmpty())
                        {
                            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                            {
                                if (i->getSource()->isAlive() && i->getSource()->HasAura(SPELL_TWILIGHT_SHIFT_ENTER))
                                    i->getSource()->CastSpell(i->getSource(), SPELL_TWILIGHT_SHIFT_REMOVAL, true);
                            }
                        }
                    }
                }
            }
            m_uiCheckTwilightTimer = 3000;
        }
        else
            m_uiCheckTwilightTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_sartharion(Creature* pCreature)
{
    return new boss_sartharionAI(pCreature);
}

enum TeneText
{
    SAY_TENEBRON_AGGRO = -1615009,
    SAY_TENEBRON_SLAY_1 = -1615010,
    SAY_TENEBRON_SLAY_2 = -1615011,
    SAY_TENEBRON_DEATH = -1615012,
    SAY_TENEBRON_BREATH = -1615013,
    SAY_TENEBRON_RESPOND = -1615014,
    SAY_TENEBRON_SPECIAL_1 = -1615015,
    SAY_TENEBRON_SPECIAL_2 = -1615016
};

enum ShadText
{
    SAY_SHADRON_AGGRO = -1615000,
    SAY_SHADRON_SLAY_1 = -1615001,
    SAY_SHADRON_SLAY_2 = -1615002,
    SAY_SHADRON_DEATH = -1615003,
    SAY_SHADRON_BREATH = -1615004,
    SAY_SHADRON_RESPOND = -1615005,
    SAY_SHADRON_SPECIAL_1 = -1615006,
    SAY_SHADRON_SPECIAL_2 = -1615007
};

enum VespText
{
    SAY_VESPERON_AGGRO = -1615033,
    SAY_VESPERON_SLAY_1 = -1615034,
    SAY_VESPERON_SLAY_2 = -1615035,
    SAY_VESPERON_DEATH = -1615036,
    SAY_VESPERON_BREATH = -1615037,
    SAY_VESPERON_RESPOND = -1615038,
    SAY_VESPERON_SPECIAL_1 = -1615039,
    SAY_VESPERON_SPECIAL_2 = -1615040
};

// to control each dragon's common abilities
struct MANGOS_DLL_DECL dummy_dragonAI : public ScriptedAI
{
    dummy_dragonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((instance_obsidian_sanctum*)pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_obsidian_sanctum* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiWaypointId;
    uint32 m_uiMoveNextTimer;
    bool m_bCanMoveFree;

    void Reset()
    {
        m_creature->SetLevitate(true);
        m_creature->SetWalk(false);
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_uiWaypointId = 0;
        m_uiMoveNextTimer = 500;
        m_bCanMoveFree = false;
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho || m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() == POINT_MOTION_TYPE)
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);

            if (IsCombatMovement())
                m_creature->GetMotionMaster()->MoveChase(pWho);
        }
    }

    void JustReachedHome()
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (!m_pInstance || uiType != POINT_MOTION_TYPE)
            return;

        debug_log("dummy_dragonAI: %s reached point %u", m_creature->GetName(), uiPointId);

        //if healers messed up the raid and we was already initialized
        if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
        {
            m_creature->GetMotionMaster()->Clear();
            EnterEvadeMode();
            return;
        }

        //this is the end (!)
        if (uiPointId == POINT_ID_LAND)
        {
            m_creature->GetMotionMaster()->Clear();
            m_bCanMoveFree = false;
            m_creature->SetInCombatWithZone();
            return;
        }

        //get amount of common points
        uint32 uiCommonWPCount = sizeof(m_aDragonCommon)/sizeof(Waypoint);

        //increase
        m_uiWaypointId = uiPointId+1;

        //if we have reached a point bigger or equal to count, it mean we must reset to point 0
        if (m_uiWaypointId >= uiCommonWPCount)
        {
            if (!m_bCanMoveFree)
                m_bCanMoveFree = true;

            m_uiWaypointId = 0;
        }

        m_uiMoveNextTimer = 500;
    }

    //used when open portal and spawn mobs in phase
    void DoRaidWhisper(int32 iTextId)
    {
        Map* pMap = m_creature->GetMap();

        if (pMap && pMap->IsDungeon())
        {
            Map::PlayerList const &PlayerList = pMap->GetPlayers();

            if (!PlayerList.isEmpty())
            {
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    DoScriptText(iTextId, m_creature, i->getSource());
            }
        }
    }

    //Removes debuff from players
    void RemoveDebuff(uint32 uiSpellId)
    {
        Map* pMap = m_creature->GetMap();

        if (pMap && pMap->IsDungeon())
        {
            Map::PlayerList const &PlayerList = pMap->GetPlayers();

            if (PlayerList.isEmpty())
                return;

            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            {
                if (uiSpellId == SPELL_TWILIGHT_SHIFT_ENTER)
                {
                    if (i->getSource() && i->getSource()->isAlive() && i->getSource()->HasAura(SPELL_TWILIGHT_SHIFT_ENTER))
                        i->getSource()->CastSpell(i->getSource(), SPELL_TWILIGHT_SHIFT_REMOVAL, true);
                }
                else
                    i->getSource()->RemoveAurasDueToSpell(uiSpellId);
            }
        }
    }

    //"opens" the portal and does the "opening" whisper
    void OpenPortal()
    {
        int32 iTextId = 0;
        int32 iPortalRespawnTime = 0;

        //there are 4 portal spawn locations, each are expected to be spawned with negative spawntimesecs in database

        //using a grid search here seem to be more efficient than caching all four guids
        //in instance script and calculate range to each.
        if (GameObject* pPortal = GetClosestGameObjectWithEntry(m_creature, GO_TWILIGHT_PORTAL, 100.0f))
        {
            Creature* pAcolyte = NULL;
            switch(m_creature->GetEntry())
            {
                case NPC_TENEBRON:
                    iTextId = WHISPER_HATCH_EGGS;
                    m_creature->SummonCreature(NPC_TWILIGHT_EGG_CONTROLLER, pPortal->GetPositionX(), pPortal->GetPositionY(), pPortal->GetPositionZ(), 0, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 30000);
                    iPortalRespawnTime = 30;
                    break;
                case NPC_SHADRON:
                    iTextId = WHISPER_OPEN_PORTAL;
                    if (m_pInstance)
                    {
                        pAcolyte = m_pInstance->GetSingleCreatureFromStorage(NPC_ACOLYTE_OF_SHADRON);
                        if (!pAcolyte || (pAcolyte && pAcolyte->isDead()))
                        {
                            pAcolyte = NULL;

                            uint32 uiShadronAddEntry = NPC_DISCIPLE_OF_SHADRON;
                            if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
                                uiShadronAddEntry = NPC_ACOLYTE_OF_SHADRON;

                            if (pAcolyte = m_creature->SummonCreature(uiShadronAddEntry, pPortal->GetPositionX()-10+urand(0, 20), pPortal->GetPositionY()-10+urand(0, 20), pPortal->GetPositionZ()+1.0f, 0, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 7*DAY))
                            {
                                m_pInstance->SetAcolyteGuid(NPC_ACOLYTE_OF_SHADRON, pAcolyte->GetObjectGuid());
                                pAcolyte->SetPhaseMask(16, true);
                            }
                        }
                        if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
                        {
                            if (Creature* pSarth = m_pInstance->GetSingleCreatureFromStorage(NPC_SARTHARION))
                                pSarth->CastSpell(pSarth, SPELL_GIFT_OF_TWILIGTH_SAR, true);
                        }
                        else
                        {
                            if (Creature* pShad = m_pInstance->GetSingleCreatureFromStorage(NPC_SHADRON))
                                pShad->CastSpell(pShad, SPELL_GIFT_OF_TWILIGTH_SHA, true);
                        }
                    }
                    iPortalRespawnTime = 60;
                    break;
                case NPC_VESPERON:
                    iTextId = WHISPER_OPEN_PORTAL;
                    if (m_pInstance)
                    {
                        uint32 uiTempSpell;
                        if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
                            uiTempSpell = 58835;
                        else
                            uiTempSpell = 57935;

                        SpellEntry* pTempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(uiTempSpell);
                        if (pTempSpell)
                        {
                            // LOLWHAT
                            //pTempSpell->StackAmount = 1;
                            //pTempSpell->procCharges = 1;
                            m_creature->CastSpell(m_creature, pTempSpell, true);
                        }

                        pAcolyte = m_pInstance->GetSingleCreatureFromStorage(NPC_ACOLYTE_OF_VESPERON);
                        if (!pAcolyte || (pAcolyte && pAcolyte->isDead()))
                        {
                            pAcolyte = NULL;

                            uint32 uiVesperonAddEntry = NPC_DISCIPLE_OF_VESPERON;
                            if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
                                uiVesperonAddEntry = NPC_ACOLYTE_OF_VESPERON;

                            if (pAcolyte = m_creature->SummonCreature(uiVesperonAddEntry, pPortal->GetPositionX()-10+urand(0, 20), pPortal->GetPositionY()-10+urand(0, 20), pPortal->GetPositionZ()+1.0f, 0, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 7*DAY*IN_MILLISECONDS))
                            {
                                m_pInstance->SetAcolyteGuid(NPC_ACOLYTE_OF_VESPERON, pAcolyte->GetObjectGuid());
                                pAcolyte->SetPhaseMask(16, true);
                            }
                        }
                    }
                    iPortalRespawnTime = 60;
                    break;
            }

            DoRaidWhisper(iTextId);

            //By using SetRespawnTime() we will actually "spawn" the object with our defined time.
            //Once time is up, portal will disappear again.
            
            pPortal->SetRespawnTime(iPortalRespawnTime);
            pPortal->Refresh();

            //Unclear what are expected to happen if one drake has a portal open already
            //Refresh respawnTime so time again are set to 30secs?
        }
    }

    void CheckTwilightRealm()
    {
        bool bNoAliveTwilightRealm = true;
        if (m_pInstance)
        {
            if (Creature* pAcolyte = m_pInstance->GetSingleCreatureFromStorage(NPC_ACOLYTE_OF_SHADRON))
                if (pAcolyte->isAlive())
                    bNoAliveTwilightRealm = false;

            if (Creature* pAcolyte = m_pInstance->GetSingleCreatureFromStorage(NPC_ACOLYTE_OF_VESPERON))
                if (pAcolyte->isAlive())
                {
                    bNoAliveTwilightRealm = false;

                    // Sartharion event not in progress
                    SpellEntry* pTempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(57935);
                    if (pTempSpell)
                    {
                        // LOLWHAT
                        //pTempSpell->StackAmount = 1;
                        //pTempSpell->procCharges = 1;
                        m_creature->CastSpell(m_creature, pTempSpell, true);
                    }
                }

            if (!m_pInstance->m_lEggsGUIDList.empty())
                for (GUIDList::iterator itr = m_pInstance->m_lEggsGUIDList.begin(); itr != m_pInstance->m_lEggsGUIDList.end(); ++itr)
                    if (Creature* pTemp = m_pInstance->instance->GetCreature(*itr))
                        if (pTemp->isAlive())
                        {
                            bNoAliveTwilightRealm = false;
                            break;
                        }

            if (bNoAliveTwilightRealm)
                RemoveDebuff(SPELL_TWILIGHT_SHIFT_ENTER);
        }
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
            return;

        int32 iTextId = 0;

        switch(m_creature->GetEntry())
        {
            case NPC_TENEBRON:
            {
                m_pInstance->SetData(TYPE_TENEBRON, DONE);
                iTextId = SAY_TENEBRON_DEATH;
                
                if (!m_pInstance->m_lEggsGUIDList.empty())
                    for (GUIDList::iterator itr = m_pInstance->m_lEggsGUIDList.begin(); itr != m_pInstance->m_lEggsGUIDList.end(); ++itr)
                        if (Creature* pEgg = m_pInstance->instance->GetCreature(*itr))
                            pEgg->ForcedDespawn();
                break;
            }
            case NPC_SHADRON:
            {
                m_pInstance->SetData(TYPE_SHADRON, DONE);
                iTextId = SAY_SHADRON_DEATH;
                break;
            }
            case NPC_VESPERON:
            {
                m_pInstance->SetData(TYPE_VESPERON, DONE);
                iTextId = SAY_VESPERON_DEATH;

                // kill Disciple of Vesperon if solo fight or died after killing Sartharion
                if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
                    if (Creature* pAcolyte = m_pInstance->GetSingleCreatureFromStorage(NPC_ACOLYTE_OF_VESPERON))
                        pAcolyte->ForcedDespawn();
                break;
            }
        }

        DoScriptText(iTextId, m_creature);

        // not if solo mini-boss fight
        if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
        {
            // bind to the instance only when solo fighting
            if (Player *pCreditPlayer = pKiller->GetCharmerOrOwnerPlayerOrPlayerItself() )
                ((DungeonMap *)pCreditPlayer->GetMap())->PermBindAllPlayers(pCreditPlayer);
            RemoveDebuff(SPELL_TWILIGHT_SHIFT_ENTER);
        }
        // Twilight Revenge to main boss
        else if (Unit* pSartharion = m_pInstance->GetSingleCreatureFromStorage(NPC_SARTHARION))
        {
            if (pSartharion->isAlive())
                m_creature->CastSpell(pSartharion, SPELL_TWILIGHT_REVENGE, true, 0, 0, pSartharion->GetObjectGuid());
        }
        // emblems should be in Sartharion's body if fighting in any hard mode
        if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS || m_pInstance->GetData(TYPE_SARTHARION_EVENT) == DONE)
            m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
        else
            m_creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bCanMoveFree && m_uiMoveNextTimer)
        {
            if (!m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            {
                m_uiMoveNextTimer = 0;
                return;
            }
            if (m_uiMoveNextTimer <= uiDiff)
            {
                m_creature->GetMotionMaster()->MovePoint(m_uiWaypointId,
                    m_aDragonCommon[m_uiWaypointId].m_fX, m_aDragonCommon[m_uiWaypointId].m_fY, m_aDragonCommon[m_uiWaypointId].m_fZ);

                debug_log("dummy_dragonAI: %s moving to point %u", m_creature->GetName(), m_uiWaypointId);
                m_uiMoveNextTimer = 0;
            }
            else
                m_uiMoveNextTimer -= uiDiff;
        }
    }
};

/*######
## Mob Tenebron
######*/

struct MANGOS_DLL_DECL mob_tenebronAI : public dummy_dragonAI
{
    mob_tenebronAI(Creature* pCreature) : dummy_dragonAI(pCreature)
    {
        m_pInstance = (instance_obsidian_sanctum*)m_creature->GetInstanceData();
        Reset();
    }

    instance_obsidian_sanctum *m_pInstance;

    uint32 m_uiShadowBreathTimer;
    uint32 m_uiShadowFissureTimer;
    uint32 m_uiSpawnEggsTimer;
    uint32 m_uiHatchEggsTimer;
    uint32 m_uiCheckTimer;

    void Reset()
    {
        m_uiShadowBreathTimer = 10000;
        m_uiShadowFissureTimer = 8000;
        m_uiSpawnEggsTimer = 15000;
        m_uiHatchEggsTimer = 15000 + 15000; // cast dummy spell 15 seconds after eggs spawned
        m_uiCheckTimer = 2000;

        if (!m_pInstance)
            return;

        m_pInstance->SetData(TYPE_TENEBRON, NOT_STARTED);

        if (!m_pInstance->m_lEggsGUIDList.empty())
            for (GUIDList::iterator i = m_pInstance->m_lEggsGUIDList.begin(); i != m_pInstance->m_lEggsGUIDList.end(); i++)
                if (Creature *pEgg = m_pInstance->instance->GetCreature(*i))
                    if (pEgg->isAlive())
                        pEgg->ForcedDespawn();
        m_pInstance->m_lEggsGUIDList.clear();

        if (!m_pInstance->m_lWhelpsGUIDList.empty())
            for (GUIDList::iterator i = m_pInstance->m_lWhelpsGUIDList.begin(); i != m_pInstance->m_lWhelpsGUIDList.end(); i++)
                if (Creature *pWhelp = m_pInstance->instance->GetCreature(*i))
                    if (pWhelp->isAlive())
                        pWhelp->ForcedDespawn();
        m_pInstance->m_lWhelpsGUIDList.clear();
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
            DoScriptText(SAY_TENEBRON_RESPOND,m_creature);
        else
        {
            m_pInstance->SetData(TYPE_TENEBRON, IN_PROGRESS);
            DoScriptText(SAY_TENEBRON_AGGRO,m_creature);
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(SAY_TENEBRON_SLAY_1 - urand(0, 1), m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //if no target, update dummy and return
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            dummy_dragonAI::UpdateAI(uiDiff);
            return;
        }

        // shadow fissure
        if (m_uiShadowFissureTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, m_bIsRegularMode ? SPELL_SHADOW_FISSURE : SPELL_SHADOW_FISSURE_H);

            m_uiShadowFissureTimer = urand(8000, 10000);
        }
        else
            m_uiShadowFissureTimer -= uiDiff;

        // shadow breath
        if (m_uiShadowBreathTimer < uiDiff)
        {
            DoScriptText(SAY_TENEBRON_BREATH, m_creature);
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SHADOW_BREATH : SPELL_SHADOW_BREATH_H);
            m_uiShadowBreathTimer = urand(10000, 30000);
        }
        else
            m_uiShadowBreathTimer -= uiDiff;

        // Spawn eggs in the Twilight Realm
        if (m_uiSpawnEggsTimer < uiDiff)
        {
            OpenPortal();
            m_uiSpawnEggsTimer = 45000;
        }
        else
            m_uiSpawnEggsTimer -= uiDiff;

        // Cast Hatch Eggs - Twilight Whelps spawn
        if (m_uiHatchEggsTimer < uiDiff)
        {
            if (m_pInstance)
            {
                if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
                {
                    DoCastSpellIfCan(m_creature, SPELL_HATCH_SARTHARION_EGGS, CAST_INTERRUPT_PREVIOUS);
                    m_uiHatchEggsTimer = m_uiSpawnEggsTimer + 17500;
                }
                else
                {
                    DoCastSpellIfCan(m_creature, SPELL_HATCH_EGGS, CAST_INTERRUPT_PREVIOUS);
                    m_uiHatchEggsTimer = m_uiSpawnEggsTimer + 15000;
                }
            }
        }
        else
            m_uiHatchEggsTimer -= uiDiff;

        if (m_uiCheckTimer < uiDiff && m_pInstance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
        {
            CheckTwilightRealm();
            m_uiCheckTimer = 2000;
        }
        else
            m_uiCheckTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_tenebron(Creature* pCreature)
{
    return new mob_tenebronAI(pCreature);
}

/*######
## Mob Shadron
######*/

struct MANGOS_DLL_DECL mob_shadronAI : public dummy_dragonAI
{
    mob_shadronAI(Creature* pCreature) : dummy_dragonAI(pCreature)
    {
        m_pInstance = (instance_obsidian_sanctum*)m_creature->GetInstanceData();
        Reset();
    }

    instance_obsidian_sanctum *m_pInstance;

    uint32 m_uiShadowBreathTimer;
    uint32 m_uiShadowFissureTimer;
    uint32 m_uiAcolyteShadronTimer;
    uint32 m_uiCheckTimer;

    void Reset()
    {
        m_uiShadowBreathTimer = 10000;
        m_uiShadowFissureTimer = 8000;
        m_uiAcolyteShadronTimer = 15000;
        m_uiCheckTimer = 2000;

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_SHADRON, NOT_STARTED);

            if (Creature* pAcolyte = m_pInstance->GetSingleCreatureFromStorage(NPC_ACOLYTE_OF_SHADRON))
                pAcolyte->DealDamage(pAcolyte, pAcolyte->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
            DoScriptText(SAY_SHADRON_RESPOND,m_creature);
        else
        {
            DoScriptText(SAY_SHADRON_AGGRO,m_creature);
            m_pInstance->SetData(TYPE_SHADRON, IN_PROGRESS);
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(SAY_SHADRON_SLAY_1 - urand(0, 1), m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //if no target, update dummy and return
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            dummy_dragonAI::UpdateAI(uiDiff);
            return;
        }

        // shadow fissure
        if (m_uiShadowFissureTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, m_bIsRegularMode ? SPELL_SHADOW_FISSURE : SPELL_SHADOW_FISSURE_H);

            m_uiShadowFissureTimer = urand(8000, 10000);
        }
        else
            m_uiShadowFissureTimer -= uiDiff;

        // shadow breath
        if (m_uiShadowBreathTimer < uiDiff)
        {
            DoScriptText(SAY_SHADRON_BREATH, m_creature);
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SHADOW_BREATH : SPELL_SHADOW_BREATH_H);
            m_uiShadowBreathTimer = urand(10000, 30000);
        }
        else
            m_uiShadowBreathTimer -= uiDiff;

        if (m_uiAcolyteShadronTimer < uiDiff)
        {
            OpenPortal();
            m_uiAcolyteShadronTimer = 60000;
        }
        else
            m_uiAcolyteShadronTimer -= uiDiff;

        if (m_uiCheckTimer < uiDiff && m_pInstance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
        {
            CheckTwilightRealm();
            m_uiCheckTimer = 2000;
        }
        else
            m_uiCheckTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_shadron(Creature* pCreature)
{
    return new mob_shadronAI(pCreature);
}

/*######
## Mob Vesperon
######*/

struct MANGOS_DLL_DECL mob_vesperonAI : public dummy_dragonAI
{
    mob_vesperonAI(Creature* pCreature) : dummy_dragonAI(pCreature)
    {
        m_pInstance = (instance_obsidian_sanctum*)m_creature->GetInstanceData();
        Reset();
    }

    instance_obsidian_sanctum *m_pInstance;

    uint32 m_uiShadowBreathTimer;
    uint32 m_uiShadowFissureTimer;
    uint32 m_uiAcolyteVesperonTimer;
    uint32 m_uiCheckTimer;
    uint32 m_uiTwilightRevengeTimer;

    void Reset()
    {
        m_uiShadowBreathTimer = 10000;
        m_uiShadowFissureTimer = 8000;
        m_uiAcolyteVesperonTimer = 15000;
        m_uiCheckTimer = 3000;

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_VESPERON, NOT_STARTED);

            if (Creature* pAcolyte = m_pInstance->GetSingleCreatureFromStorage(NPC_ACOLYTE_OF_VESPERON))
                pAcolyte->DealDamage(pAcolyte, pAcolyte->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
            DoScriptText(SAY_VESPERON_RESPOND,m_creature);
        else
        {
            m_pInstance->SetData(TYPE_VESPERON, IN_PROGRESS);
            DoScriptText(SAY_VESPERON_AGGRO,m_creature);
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(SAY_VESPERON_SLAY_1 - urand(0, 1), m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //if no target, update dummy and return
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            dummy_dragonAI::UpdateAI(uiDiff);
            return;
        }

        // shadow fissure
        if (m_uiShadowFissureTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, m_bIsRegularMode ? SPELL_SHADOW_FISSURE : SPELL_SHADOW_FISSURE_H);

            m_uiShadowFissureTimer = urand(8000, 10000);
        }
        else
            m_uiShadowFissureTimer -= uiDiff;

        // shadow breath
        if (m_uiShadowBreathTimer < uiDiff)
        {
            DoScriptText(SAY_VESPERON_BREATH, m_creature);
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SHADOW_BREATH : SPELL_SHADOW_BREATH_H);
            m_uiShadowBreathTimer = urand(10000, 30000);
        }
        else
            m_uiShadowBreathTimer -= uiDiff;

        if (m_uiAcolyteVesperonTimer < uiDiff)
        {
            OpenPortal();
            m_uiAcolyteVesperonTimer = 60000;
        }
        else
            m_uiAcolyteVesperonTimer -= uiDiff;

        if (m_uiCheckTimer < uiDiff && m_pInstance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
        {
            CheckTwilightRealm();
            m_uiCheckTimer = 3000;
        }
        else
            m_uiCheckTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_vesperon(Creature* pCreature)
{
    return new mob_vesperonAI(pCreature);
}

/*######
## Mob Acolyte of Shadron
######*/

struct MANGOS_DLL_DECL mob_acolyte_of_shadronAI : public ScriptedAI
{
    mob_acolyte_of_shadronAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_obsidian_sanctum*)pCreature->GetInstanceData();
        Reset();
    }

    instance_obsidian_sanctum* m_pInstance;

    void Reset(){}
    void MoveInLineOfSight(Unit *pWho){}

    void JustDied(Unit* killer)
    {
        if (m_pInstance)
        {
            Creature* pDebuffTarget = NULL;

            if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
            {
                //not solo fight, so main boss has deduff
                pDebuffTarget = m_pInstance->GetSingleCreatureFromStorage(NPC_SARTHARION);

                if (pDebuffTarget && pDebuffTarget->isAlive() && pDebuffTarget->HasAura(SPELL_GIFT_OF_TWILIGTH_SAR))
                    pDebuffTarget->RemoveAurasDueToSpell(SPELL_GIFT_OF_TWILIGTH_SAR);
            }
            else
            {
                //event not in progress, then solo fight and must remove debuff mini-boss
                pDebuffTarget = m_pInstance->GetSingleCreatureFromStorage(NPC_SHADRON);

                if (pDebuffTarget && pDebuffTarget->isAlive() && pDebuffTarget->HasAura(SPELL_GIFT_OF_TWILIGTH_SHA))
                    pDebuffTarget->RemoveAurasDueToSpell(SPELL_GIFT_OF_TWILIGTH_SHA);

                // remove from twilight realm
                Map::PlayerList const &PlayerList = m_pInstance->instance->GetPlayers();

                if (!PlayerList.isEmpty())
                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        if (i->getSource()->isAlive() && i->getSource()->HasAura(SPELL_TWILIGHT_SHIFT_ENTER))
                            i->getSource()->CastSpell(i->getSource(), SPELL_TWILIGHT_SHIFT_REMOVAL, true);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_acolyte_of_shadron(Creature* pCreature)
{
    return new mob_acolyte_of_shadronAI(pCreature);
}

/*######
## Mob Acolyte of Vesperon
######*/

struct MANGOS_DLL_DECL mob_acolyte_of_vesperonAI : public ScriptedAI
{
    mob_acolyte_of_vesperonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        //m_pInstance = (instance_obsidian_sanctum*)pCreature->GetInstanceData();
        //Reset();
    }

    //instance_obsidian_sanctum* m_pInstance;

    void Reset(){}

    void MoveInLineOfSight(Unit *pWho){}

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_creature->SelectHostileTarget() && m_creature->getVictim())
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_acolyte_of_vesperon(Creature* pCreature)
{
    return new mob_acolyte_of_vesperonAI(pCreature);
}

/*######
## Mob Twilight Eggs
######*/

struct MANGOS_DLL_DECL mob_twilight_eggsAI : public ScriptedAI
{
    mob_twilight_eggsAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_obsidian_sanctum*)m_creature->GetInstanceData();
        Reset();
    }

    instance_obsidian_sanctum *m_pInstance;

    void Reset(){}
    void AttackStart(Unit* pWho){}
    void MoveInLineOfSight(Unit* pWho){}
    void UpdateAI(const uint32 uiDiff){}

    void JustSummoned(Creature *pCreature)
    {
        if (!m_pInstance)
            return;

        m_pInstance->m_lWhelpsGUIDList.push_back(pCreature->GetObjectGuid());
        pCreature->SetPhaseMask(1, true);
        pCreature->SetInCombatWithZone();
        m_creature->ForcedDespawn();
    }
};

CreatureAI* GetAI_mob_twilight_eggs(Creature* pCreature)
{
    return new mob_twilight_eggsAI(pCreature);
}

/*######
## Mob Twilight Egg Controller
######*/

struct MANGOS_DLL_DECL mob_twilight_egg_controllerAI : public ScriptedAI
{
    mob_twilight_egg_controllerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pCreature->SetDisplayId(11686);
        m_pInstance = (instance_obsidian_sanctum*)m_creature->GetInstanceData();
        Reset();
    }

    instance_obsidian_sanctum *m_pInstance;

    void Reset()
    {
        if (!m_pInstance)
            return;

        uint32 uiEggEntry = 0;

        if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
            uiEggEntry = NPC_SARTHARION_TWILIGHT_EGG;
        else
            uiEggEntry = NPC_TWILIGHT_EGG;

        m_pInstance->m_lEggsGUIDList.clear();

        for (uint8 i=0; i<6; ++i)
        {
            if (Creature* pEgg = m_creature->SummonCreature(uiEggEntry, m_creature->GetPositionX()-10+urand(0, 20), m_creature->GetPositionY()-10+urand(0, 20), m_creature->GetPositionZ()+1.0f, 0, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 30000))
            {
                pEgg->SetPhaseMask(16, true);
                m_pInstance->m_lEggsGUIDList.push_back(pEgg->GetObjectGuid());
            }
        }
    }
    void AttackStart(Unit* pWho){}
    void MoveInLineOfSight(Unit* pWho){}
    void UpdateAI(const uint32 uiDiff){}

    void SpellHit(Unit *pCaster, const SpellEntry *spellInfo)
    {
        if (!m_pInstance)
            return;

        uint32 spell = 0;

        if (spellInfo->Id == SPELL_HATCH_EGGS)
            spell = SPELL_SUMMON_TWILIGHT_WHELP;
        else if (spellInfo->Id == SPELL_HATCH_SARTHARION_EGGS)
            spell = SPELL_SUMMON_SARTH_TWILIGHT_WHELP;
        else
            return;

        if (m_pInstance->m_lEggsGUIDList.empty())
            return;

        for (GUIDList::iterator i = m_pInstance->m_lEggsGUIDList.begin(); i != m_pInstance->m_lEggsGUIDList.end(); i++)
            if (Creature *pEgg = m_pInstance->instance->GetCreature(*i))
                if (pEgg->isAlive())
                    pEgg->CastSpell(pEgg, spell, true);

        m_creature->ForcedDespawn();
    }
};

CreatureAI* GetAI_mob_twilight_egg_controller(Creature* pCreature)
{
    return new mob_twilight_egg_controllerAI(pCreature);
}

/*######
## Mob Twilight Whelps
######*/

struct MANGOS_DLL_DECL mob_twilight_whelpAI : public ScriptedAI
{
    mob_twilight_whelpAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_obsidian_sanctum*)m_creature->GetInstanceData();
        Reset();
    }

    instance_obsidian_sanctum *m_pInstance;
    uint32 m_uiFadeArmorTimer;

    void Reset()
    {
        m_uiFadeArmorTimer = 1000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Whelps travel through both realms when chasing their target
        if (!(m_creature->GetPhaseMask() & m_creature->getVictim()->GetPhaseMask()) )
            m_creature->SetPhaseMask(m_creature->GetPhaseMask() | m_creature->getVictim()->GetPhaseMask(), true);

        // Fade Armor
        if (m_uiFadeArmorTimer < uiDiff)
        {
            m_creature->CastSpell(m_creature->getVictim(), SPELL_FADE_ARMOR, false);
            m_uiFadeArmorTimer = urand(5000, 10000);
        }
        else
            m_uiFadeArmorTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_twilight_whelp(Creature* pCreature)
{
    return new mob_twilight_whelpAI(pCreature);
}

/*######
## Mob Fire Cyclone
######*/

struct MANGOS_DLL_DECL mob_fire_cycloneAI : public ScriptedAI
{
    mob_fire_cycloneAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_obsidian_sanctum*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_obsidian_sanctum* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiRefreshTimer;
    uint32 m_uiLavaStrikeTimer;
    uint8 m_uiLavaBlazeCount;
    bool m_bLavaStrikeAllowed;

    void Reset()
    {
        m_bLavaStrikeAllowed = false;
        m_uiRefreshTimer = (urand(1000, 60000));
    }

    void DamageTaken(Unit *pDoneBy, uint32 &uiDamage)
    {
        uiDamage = 0;
    }

    void AttackStart(Unit* pWho){}
    void MoveInLineOfSight(Unit* pWho){}

    void SpellHitTarget(Unit *pVictim, const SpellEntry *spellInfo)
    {
        if (spellInfo->Id == SPELL_LAVA_STRIKE_DMG)
            if (m_pInstance && pVictim)
                m_pInstance->m_lHitByVolcanoGUIDList.push_back(pVictim->GetObjectGuid());
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (!m_pInstance || !pSummoned || pSummoned->GetEntry() != NPC_LAVA_BLAZE)
            return;

        if(Creature* pSartharion = m_pInstance->GetSingleCreatureFromStorage(NPC_SARTHARION))
        {
            if(pSartharion->isAlive() )
            {
                if(((boss_sartharionAI*)pSartharion->AI())->m_bIsSoftEnraged)
                {
                    m_pInstance->m_lBlazesGUIDList.push_back(pSummoned->GetObjectGuid());
                    pSummoned->setFaction(14);
                    pSummoned->SetInCombatWithZone();
                }
                else
                {
                    if (roll_chance_i(m_bIsRegularMode ? 8 : 18))
                    {
                        m_pInstance->m_lBlazesGUIDList.push_back(pSummoned->GetObjectGuid());
                        pSummoned->setFaction(14);
                        pSummoned->SetInCombatWithZone();
                    }
                    else
                        pSummoned->ForcedDespawn();
                }
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(m_pInstance && m_pInstance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
            return;

        // workaround for Lava Strike - missing implementation for EffectImplicitTargetA/B 53/74
        if (m_bLavaStrikeAllowed)
        {
            if (m_uiLavaStrikeTimer < uiDiff)
            {
                if (Creature *pSarth = m_pInstance->GetSingleCreatureFromStorage(NPC_SARTHARION))
                {
                    Unit *pTarget = pSarth->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1);

                    if (Creature* pTemp = m_creature->SummonCreature(NPC_ORB_TARGET, pTarget ? (pTarget->GetPositionX()-20.0f+urand(0, 40)) : urand(3220, 3275), pTarget ? (pTarget->GetPositionY()-20.0f+urand(0, 40)) : urand(486, 575), 58.8f, 0, TEMPSUMMON_TIMED_DESPAWN, 8000))
                    {
                        pTemp->SetDisplayId(11686);
                        pTemp->setFaction(35);
                        pTemp->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        DoCast(pTemp, SPELL_LAVA_STRIKE);
                    }
                }
                m_uiLavaStrikeTimer = urand(1000, 3000);
                m_uiLavaBlazeCount++;
                if (m_uiLavaBlazeCount>=12)
                {
                    m_bLavaStrikeAllowed = false;
                    m_creature->SetFloatValue(OBJECT_FIELD_SCALE_X, 1.0f);
                }
            }
            else
                m_uiLavaStrikeTimer -= uiDiff;
        }

        if (!m_bLavaStrikeAllowed)
        {
            if (m_uiRefreshTimer < uiDiff)
            {
                DoCast(m_creature, SPELL_CYCLONE_AURA_2);
                DoCast(m_creature, SPELL_CYCLONE_AURA, true);
                m_bLavaStrikeAllowed = true;
                m_uiLavaStrikeTimer = 0;
                m_uiLavaBlazeCount = 0;
                m_creature->SetFloatValue(OBJECT_FIELD_SCALE_X, 2.5f);
                m_uiRefreshTimer = urand(15000, 45000);
            }
            else
                m_uiRefreshTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_mob_fire_cyclone(Creature* pCreature)
{
    return new mob_fire_cycloneAI(pCreature);
}

/*######
## Mob Flame Tsunami
######*/

struct MANGOS_DLL_DECL mob_flame_tsunamiAI : public ScriptedAI
{
    mob_flame_tsunamiAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_obsidian_sanctum*)m_creature->GetInstanceData();
        Reset();
    }

    instance_obsidian_sanctum *m_pInstance;
    uint32 m_uiMovementStartTimer;

    void Reset()
    {
        m_creature->SetDisplayId(11686);
        m_creature->SetLevitate(true);
        m_creature->SetSpeedRate(MOVE_WALK, 4.5f, true);
        m_uiMovementStartTimer = 4000;
    }

    void AttackStart(Unit* pWho){}
    void MoveInLineOfSight(Unit* pWho){}

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiMovementStartTimer < uiDiff)
        {
            int8 uiDirection = 1;
            if (m_creature->GetPositionX() > 3240.0f)
                uiDirection = -1;
            m_creature->GetMotionMaster()->MovePoint(0, m_creature->GetPositionX()+uiDirection*86.5f, m_creature->GetPositionY(), m_creature->GetPositionZ());
            m_uiMovementStartTimer = 30000;
        }
        else
            m_uiMovementStartTimer -= uiDiff;
    }
};

CreatureAI* GetAI_mob_flame_tsunami(Creature* pCreature)
{
    return new mob_flame_tsunamiAI(pCreature);
}

// Twilight Portals - don't allow entering, when the dragon/all dragons is/are dead
// if fighting solo, then drakes shouldn'e be moved more far away then 70yd from their portal
// nor during Sartharion event.
bool GOobsidian_sanctum_portals(Player* pPlayer, GameObject* pGo)
{
    instance_obsidian_sanctum *m_pInstance = (instance_obsidian_sanctum*)pPlayer->GetInstanceData();

    if (!m_pInstance)
        return false;

    // if event is in progress, then the portal should always be working
    if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
    {
        // event is not in progress, so we must check if dragon is alive
        Creature *pDragon = GetClosestCreatureWithEntry(pGo, NPC_TENEBRON, 70.0f);

        if (!pDragon || (pDragon && (!((Unit*)pDragon)->isAlive()) || pDragon->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE)))
            pDragon = GetClosestCreatureWithEntry(pGo, NPC_SHADRON, 70.0f);
        if (!pDragon || (pDragon && (!((Unit*)pDragon)->isAlive()) || pDragon->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE)))
            pDragon = GetClosestCreatureWithEntry(pGo, NPC_VESPERON, 70.0f);

        if (!pDragon || (pDragon && (!((Unit*)pDragon)->isAlive()) || pDragon->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE) || !pDragon->isInCombat()))
            return false;
    }

    pPlayer->CastSpell(pPlayer, SPELL_TWILIGHT_SHIFT_ENTER, true);
    return true;
}

void AddSC_boss_sartharion()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_sartharion";
    newscript->GetAI = &GetAI_boss_sartharion;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_vesperon";
    newscript->GetAI = &GetAI_mob_vesperon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_shadron";
    newscript->GetAI = &GetAI_mob_shadron;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_tenebron";
    newscript->GetAI = &GetAI_mob_tenebron;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_acolyte_of_shadron";
    newscript->GetAI = &GetAI_mob_acolyte_of_shadron;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_acolyte_of_vesperon";
    newscript->GetAI = &GetAI_mob_acolyte_of_vesperon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_twilight_eggs";
    newscript->GetAI = &GetAI_mob_twilight_eggs;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_twilight_egg_controller";
    newscript->GetAI = &GetAI_mob_twilight_egg_controller;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_twilight_whelp";
    newscript->GetAI = &GetAI_mob_twilight_whelp;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_fire_cyclone";
    newscript->GetAI = &GetAI_mob_fire_cyclone;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_flame_tsunami";
    newscript->GetAI = &GetAI_mob_flame_tsunami;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "obsidian_sanctum_portals";
    newscript->pGOUse = &GOobsidian_sanctum_portals;
    newscript->RegisterSelf();
}
