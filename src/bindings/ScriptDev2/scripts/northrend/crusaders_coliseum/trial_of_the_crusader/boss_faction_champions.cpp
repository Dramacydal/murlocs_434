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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */

/* ScriptData
SDName: faction_champions
SD%Complete: 60%
SDComment: Scripts by Selector, modified by /dev/rsa
SDCategory: Crusader Coliseum
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

#define AI_MELEE    0
#define AI_RANGED   1
#define AI_HEALER   2

#define SPELL_ANTI_AOE     68595
#define SPELL_PVP_TRINKET  65547

enum Spells
{
    /********************************************************************
                                HEALERS
    ********************************************************************/

    //Druid Restoration
    SPELL_LIFEBLOOM             = 66093,
    SPELL_NOURISH               = 66066,
    SPELL_REGROWTH              = 66067,
    SPELL_REJUVENATION          = 66065,
    SPELL_TRANQUILITY           = 66086,
    SPELL_BARKSKIN              = 65860, //1 min cd
    SPELL_THORNS                = 66068,
    SPELL_NATURE_GRASP          = 66071, //1 min cd, self buff
    SPELL_ROOTS                 = 66070,

    //Shaman Restoraion
    SPELL_HEALING_WAVE          = 66055,
    SPELL_RIPTIDE               = 66053,
    SPELL_SPIRIT_CLEANSE        = 66056, //friendly only
    SPELL_HEX                   = 66054,
    SPELL_EARTH_SHIELD          = 66063,
    SPELL_EARTH_SHOCK           = 65973,

    //Paladyn Holy
    SPELL_HAND_OF_FREEDOM       = 68757, //25 sec cd
    SPELL_BUBBLE                = 66010, //5 min cd
    SPELL_CLEANSE               = 66116,
    SPELL_FLASH_OF_LIGHT        = 66113,
    SPELL_HOLY_LIGHT            = 66112,
    SPELL_HOLY_SHOCK            = 66114,
    SPELL_FLASH_HEAL            = 66113,
    SPELL_HAND_OF_PROTECTION    = 66009,
    SPELL_HAMMER_OF_JUSTICE     = 66613,
    SPELL_FORBEARANCE           = 25771,

    //Priest Discipline
    SPELL_RENEW                 = 66177,
    SPELL_SHIELD                = 66099,
    SPELL_FLASH_HEAL_P          = 66104,
    SPELL_DISPEL                = 65546,
    SPELL_PSYCHIC_SCREAM        = 65543,
    SPELL_MANA_BURN             = 66100,
    SPELL_PEANANCE              = 68030,
    SPELL_WEAKEN_SOUL           = 6788,

    /********************************************************************
                                RANGED
    ********************************************************************/

    //Priest Shadow
    SPELL_SILENCE               = 65542,
    SPELL_VAMPIRIC_TOUCH        = 65490,
    SPELL_SW_PAIN               = 65541,
    SPELL_MIND_FLAY             = 65488,
    SPELL_MIND_BLAST            = 65492,
    SPELL_HORROR                = 65545,
    SPELL_DISPERSION            = 65544,
    SPELL_SHADOWFORM            = 16592,

    //Warlock
    SPELL_HELLFIRE              = 65816,
    SPELL_CORRUPTION            = 65810,
    SPELL_CURSE_OF_AGONY        = 65814,
    SPELL_CURSE_OF_EXHAUSTION   = 65815,
    SPELL_FEAR                  = 65809, //8s
    SPELL_SEARING_PAIN          = 65819,
    SPELL_SHADOW_BOLT           = 65821,
    SPELL_UNSTABLE              = 65812,
    SPELL_SUMMON_FEL            = 67514,

    //Mage
    SPELL_BARRAGE               = 65799, //3s
    SPELL_BLAST                 = 65791,
    SPELL_EXPLOSION             = 65800,
    SPELL_BLINK                 = 65793, //15s
    SPELL_CS                    = 65790, //24s
    SPELL_FROST_NOVA            = 65792, //25s
    SPELL_FROSTBOLT             = 65807,
    SPELL_ICE_BLOCK             = 65802, //5min
    SPELL_POLY                  = 65801, //15s

    //Hunter
    SPELL_AIMED_SHOT            = 65883,
    SPELL_DETERENCE             = 65871, //90s
    SPELL_DISENGAGE             = 65869, //30s
    SPELL_EXPLOSIVE_SHOT        = 65866,
    SPELL_FROST_TRAP            = 65880, //30s
    SPELL_SHOOT                 = 65868, //1.7s
    SPELL_STEADY_SHOT           = 65867, //3s
    SPELL_WING_CLIP             = 66207, //6s
    SPELL_WYVERN_STING          = 65877, //60s
    SPELL_CALL_PET              = 67777,

    //Druid Balance
    SPELL_CYCLONE               = 65859, //6s
    SPELL_ROOTS_B               = 65857, //10s
    SPELL_FEARIE                = 65863,
    SPELL_FORCED_OF_NATURE      = 65861, //180s
    SPELL_SWARM                 = 65855,
    SPELL_MOONFIRE              = 65856, //5s
    SPELL_STARFIRE              = 65854,
    SPELL_WRATH                 = 65862,
    //Barskin

    /********************************************************************
                            MELEE
    ********************************************************************/

    //Warrior Arms
    SPELL_BLADESTORM            = 65947,
    SPELL_INTIMIDATING_SHOUT    = 65930,
    SPELL_MORTAL_STRIKE         = 65926,
    SPELL_CHARGE                = 68764,
    SPELL_DISARM                = 65935,
    SPELL_OVERPOWER             = 65924,
    SPELL_SUNDER_ARMOR          = 65936,
    SPELL_SHATTERING_THROW      = 65940,
    SPELL_RETALIATION           = 65932,

    //Death Knight
    SPELL_CHAINS                = 66020, //8sec
    SPELL_DEATH_COLI            = 66019, //5sec
    SPELL_DEATH_GRIP            = 66017, //35sec
    SPELL_FROST_STRIKE          = 66047, //6sec
    SPELL_ICEBOUND              = 66023, //1min
    SPELL_ICE_TOUCH             = 66021, //8sec
    SPELL_STRANGULATE           = 66018, //2min

    //Rogue
    SPELL_FAN_OF_KNIVES         = 65955, //2sec
    SPELL_BLIND                 = 65960, //2min
    SPELL_CLOAK                 = 65961, //90sec
    SPELL_BLADE_FLURRY          = 65956, //2min
    SPELL_SHADOWSTEP            = 66178, //30sec
    SPELL_HEMORRHAGE            = 65954,
    SPELL_EVISCERATE            = 65957,
    SPELL_WOUND_POISON          = 65962,

    //Shaman En
    SPELL_LAVA_LASH             = 65974,
    SPELL_STORMSTRIKE           = 65970,
    SPELL_WINDFURY              = 65976,
    SPELL_MALESTROM             = 65986,
    //Heroism
    //Earth Shock

    SPELL_TOTEM_WIND            = 39586,
    SPELL_TOTEM_GROUND          = 65989,
    SPELL_TOTEM_HEAL            = 71984,
    SPELL_TOTEM_TREMOR          = 65992,
    SPELL_TOTEM_STR             = 65991,
    SPELL_TOTEM_SEARING         = 39588,

    //Paladyn Retri
    SPELL_AVENGING_WRATH        = 66011, //3min cd
    SPELL_STRIKE                = 66003, //6sec cd
    SPELL_STORM                 = 66006, //10sec cd
    SPELL_JUDGEMENT_COMAND      = 66005, //8sec cd
    SPELL_REPENTANCE            = 66008, //60sec cd
    SPELL_SEAL_COMAND           = 66004, //no cd
    //SPELL_Divine_Shield       = 66010, //5min cd
    //SPELL_Hand_of_Protection  = 66009, //5min cd
    //SPELL_HAMMER_OF_JUSTICE   = 66007, //40sec cd

    //Felhunter
    SPELL_DEVOUR_MAGIC          = 67518,
    SPELL_SPELL_LOCK            = 67519,

    //Cat
    SPELL_CLAW                  = 67793,

    //Other
    //SPELL_TRINKET             = 65547, //5min cd
    SPELL_HEROISM               = 65983,
    SPELL_BLOODLUST             = 65980,
    AURA_EXHAUSTION             = 57723,
    AURA_SATED                  = 57724,
    SPELL_DIVINE_SHIELD         = 63148,
    SPELL_BLOCK                 = 45438,

    //NPC
    NPC_FELHUNTER               = 35465,
    NPC_CAT                     = 35610,
};

struct MANGOS_DLL_DECL boss_faction_championsAI : public BSWScriptedAI
{
    boss_faction_championsAI(Creature* pCreature, uint32 aitype) : BSWScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance *) pCreature->GetInstanceData();
        m_uiMapDifficulty = pCreature->GetMap()->GetDifficulty();
        m_bIsHeroicMode = (m_uiMapDifficulty == RAID_DIFFICULTY_10MAN_HEROIC || m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_HEROIC);
        mAIType = aitype;
        Init();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroicMode;

    Difficulty m_uiMapDifficulty;
    uint32 mAIType;
    uint32 UpdateTimer;
    uint32 m_uiChangeTargetTimer;
    uint32 m_uiInsygniaTimer;

    void Init()
    {
        UpdateTimer = 5000;
        m_uiChangeTargetTimer = 6000;
        resetTimers();
        m_uiInsygniaTimer = urand(30*IN_MILLISECONDS, 60*IN_MILLISECONDS);
        m_creature->SetRespawnDelay(DAY);
    }

    void Reset()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CRUSADERS, NOT_STARTED);
    }

    void JustDied(Unit *killer)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CRUSADERS_COUNT, 0);
    }

    void Aggro(Unit *who)
    {
        if (!m_pInstance)
            return;

        m_pInstance->SetData(TYPE_CRUSADERS, IN_PROGRESS);
        m_creature->CastSpell(m_creature, SPELL_ANTI_AOE, true);
        if (who->GetTypeId() != TYPEID_PLAYER)
            if (Unit* player = doSelectRandomPlayerAtRange(80.0f))
                    m_creature->AddThreat(player, 100.0f);
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho)
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);

            if (mAIType == AI_MELEE)
                m_creature->GetMotionMaster()->MoveChase(pWho);
            else
                DoStartMovement(pWho, 20.0f);

            if (mAIType == AI_HEALER)
                m_creature->GetMotionMaster()->MoveChase(pWho);

            if (mAIType == AI_RANGED)
                m_creature->GetMotionMaster()->MoveChase(pWho);

            SetCombatMovement(true);
        }
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CRUSADERS, FAIL);

        m_creature->ForcedDespawn();
    }

    float CalculateThreat(float distance, float armor, uint32 health)
    {
        float dist_mod = (mAIType == AI_MELEE) ? 15.0f/(15.0f + distance) : 1.0f;
        float armor_mod = (mAIType == AI_MELEE) ? armor / 16635.0f : 0.0f;
        float eh = (health+1) * (1.0f + armor_mod);
        return dist_mod * 30000.0f / eh;
    }

    void UpdateThreat()
    {
        ThreatList const& tList = m_creature->getThreatManager().getThreatList();
        ThreatList::const_iterator itr;
        bool empty = true;
        for (itr = tList.begin(); itr!=tList.end(); ++itr)
        {
            Unit* pUnit = m_creature->GetMap()->GetUnit((*itr)->getUnitGuid());
            if (pUnit && m_creature->getThreatManager().getThreat(pUnit))
            {
                if (pUnit->GetTypeId()==TYPEID_PLAYER)
                {
                    float threat = CalculateThreat(m_creature->GetDistance2d(pUnit), (float)pUnit->GetArmor(), pUnit->GetHealth());
                    m_creature->getThreatManager().modifyThreatPercent(pUnit, -100);
                    m_creature->AddThreat(pUnit, 1000000.0f * threat);
                    empty = false;
                }
            }
        }
    }

    void UpdatePower()
    {
        if (m_creature->getPowerType() == POWER_MANA)
            m_creature->ModifyPower(POWER_MANA, m_creature->GetMaxPower(POWER_MANA) / 3);
    }

    void RemoveCC()
    {
        m_creature->RemoveSpellsCausingAura(SPELL_AURA_MOD_STUN);
        m_creature->RemoveSpellsCausingAura(SPELL_AURA_MOD_FEAR);
        m_creature->RemoveSpellsCausingAura(SPELL_AURA_MOD_ROOT);
        m_creature->RemoveSpellsCausingAura(SPELL_AURA_MOD_PACIFY);
        m_creature->RemoveSpellsCausingAura(SPELL_AURA_MOD_CONFUSE);
        //DoCast(m_creature, SPELL_PVP_TRINKET);
    }

    //add
    Unit* SelectEnemyTargetWithinMana()
    {
        ThreatList const& tList = m_creature->getThreatManager().getThreatList();
        ThreatList::const_iterator iter;
        for (iter = tList.begin(); iter!=tList.end(); ++iter)
        {
            Unit *target;
            if (target = m_creature->GetMap()->GetUnit((*iter)->getUnitGuid()))
                if (target->getPowerType() == POWER_MANA)
                    return target;
        }
        return NULL;
    }

    Unit* KillPlayer()
    {
        ThreatList const& tList = m_creature->getThreatManager().getThreatList();
        ThreatList::const_iterator iter;
        for (iter = tList.begin(); iter!=tList.end(); ++iter)
        {
            Unit *target;
            if (target = m_creature->GetMap()->GetUnit((*iter)->getUnitGuid()))
                if (target->GetHealthPercent() < 30.0f)
                    return target;
        }
        return NULL;
    }

    Unit* SelectTargetWithinDist()
    {
        ThreatList const& m_threatlist = m_creature->getThreatManager().getThreatList();

        if (m_threatlist.empty())
            return NULL;

        GUIDList distPositive;
        for (ThreatList::const_iterator itr = m_threatlist.begin(); itr!= m_threatlist.end(); ++itr)
        {
            if (Unit* pTemp = m_creature->GetMap()->GetUnit((*itr)->getUnitGuid()))
            {
                //player within 10 yards
                if ((pTemp->GetTypeId() == TYPEID_PLAYER && !m_creature->IsWithinDist(pTemp, 1.0f) && m_creature->IsWithinDist(pTemp, 10.0f)))
                    distPositive.push_back(pTemp->GetObjectGuid());
            }
        }

        if (!distPositive.empty())
        {
            GUIDList::iterator m_uiPlayerGUID = distPositive.begin();
            advance(m_uiPlayerGUID, (rand()%distPositive.size()));

            if (Player* pTemp = m_creature->GetMap()->GetPlayer(*m_uiPlayerGUID))
                return pTemp;
        }
        return NULL;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_CRUSADERS) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if (m_uiChangeTargetTimer < uiDiff)
        {
            UpdateThreat();
            m_uiChangeTargetTimer = 6000;
        }
        else
            m_uiChangeTargetTimer -= uiDiff;

        if (UpdateTimer < uiDiff)
        {
            UpdatePower();
            UpdateTimer = 2000;
        }
        else
            UpdateTimer -= uiDiff;

        if (m_bIsHeroicMode)
        {
            if (m_uiInsygniaTimer < uiDiff)
            {
                RemoveCC();
                m_uiInsygniaTimer = 5*MINUTE*IN_MILLISECONDS;
            }
            else
                m_uiInsygniaTimer -= uiDiff;
        }

    }
};

/********************************************************************
                            HEALERS
********************************************************************/

/**************
** Druid Resto
***************/
struct MANGOS_DLL_DECL mob_toc_druidAI : public boss_faction_championsAI
{
    mob_toc_druidAI(Creature* pCreature) : boss_faction_championsAI(pCreature, AI_HEALER)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiBarskinTimer;
    uint32 m_uiNatureGraspTimer;
    uint32 m_uiTranquilityTimer;
    uint32 m_uiThornsTimer;
    uint32 m_uiLifebloomTimer;
    uint32 m_uiRejuvenationTimer;
    uint32 m_uiCastHealTimer;

    void Reset()
    {
        m_uiBarskinTimer      = 0;
        m_uiNatureGraspTimer  = urand(20*IN_MILLISECONDS, 80*IN_MILLISECONDS);
        m_uiTranquilityTimer  = 35*IN_MILLISECONDS;
        m_uiThornsTimer       = urand(25*IN_MILLISECONDS, 90*IN_MILLISECONDS);
        m_uiLifebloomTimer    = urand(2*IN_MILLISECONDS, 8*IN_MILLISECONDS);
        m_uiRejuvenationTimer = urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS);
        m_uiCastHealTimer     = urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS);

        SetEquipmentSlots(false, 51799, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiBarskinTimer < uiDiff)
        {
            if (m_creature->GetHealthPercent() < 60.0f)
            {
                m_creature->CastSpell(m_creature, SPELL_BARKSKIN, true);
                m_uiBarskinTimer = 1*MINUTE*IN_MILLISECONDS;
            }
        }
        else
            m_uiBarskinTimer -= uiDiff;

        if (m_uiNatureGraspTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_NATURE_GRASP);
            m_uiNatureGraspTimer = 1*MINUTE*IN_MILLISECONDS;
        }
        else
            m_uiNatureGraspTimer -= uiDiff;

        if (m_uiThornsTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_THORNS);
            m_uiThornsTimer = urand(25*IN_MILLISECONDS, 90*IN_MILLISECONDS);
        }
        else
            m_uiThornsTimer -= uiDiff;

        if (m_uiTranquilityTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_TRANQUILITY) == CAST_OK)
            {
                m_uiTranquilityTimer = 10*MINUTE*IN_MILLISECONDS;
            }
        }
        else
            m_uiTranquilityTimer -= uiDiff;

        if (m_uiLifebloomTimer < uiDiff)
        {
            if (Unit *target = DoSelectLowestHpFriendly(40.0f))
            {
                DoCastSpellIfCan(target, SPELL_LIFEBLOOM);
                m_uiLifebloomTimer = urand(5*IN_MILLISECONDS, 8*IN_MILLISECONDS);
            }
        }
        else
            m_uiLifebloomTimer -= uiDiff;

        if (m_uiRejuvenationTimer < uiDiff)
        {
            if (Unit *target = DoSelectLowestHpFriendly(40.0f))
            {
                DoCastSpellIfCan(target, SPELL_REJUVENATION);
                m_uiRejuvenationTimer = urand(8*IN_MILLISECONDS, 18*IN_MILLISECONDS);
            }
        }
        else
            m_uiRejuvenationTimer -= uiDiff; 

        if (m_uiCastHealTimer < uiDiff)
        {
            if (Unit *target = DoSelectLowestHpFriendly(40.0f))
            {
                switch(urand(0, 1))
                {
                    case 0:
                        DoCastSpellIfCan(target, SPELL_NOURISH);
                        m_uiCastHealTimer = 1500;
                        break;
                    case 1:
                        DoCastSpellIfCan(target, SPELL_REGROWTH);
                        m_uiCastHealTimer = urand(2500, 15000);
                        break; 
                }
            }
            m_uiCastHealTimer = urand(8*IN_MILLISECONDS, 18*IN_MILLISECONDS);
        }
        else
            m_uiCastHealTimer -= uiDiff;

        boss_faction_championsAI::UpdateAI(uiDiff);
    }
};

/****************
** Shaman Resto
*****************/
struct MANGOS_DLL_DECL mob_toc_shamanAI : public boss_faction_championsAI
{
    mob_toc_shamanAI(Creature* pCreature) : boss_faction_championsAI(pCreature, AI_HEALER)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiHeroismOrBloodlustTimer;
    uint32 m_uiHexTimer;
    uint32 m_uiEartShockTimer;
    uint32 m_uiHealingWaveTimer;
    uint32 m_uiRiptideTimer;
    uint32 m_uiEarthShieldTimer;

    void Reset()
    {
        m_uiEarthShieldTimer        = 0;
        m_uiHexTimer                = urand(10*IN_MILLISECONDS, 40*IN_MILLISECONDS);
        m_uiEartShockTimer          = urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS);
        m_uiHealingWaveTimer        = urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS);
        m_uiRiptideTimer            = urand(2*IN_MILLISECONDS, 8*IN_MILLISECONDS);
        m_uiHeroismOrBloodlustTimer = 30*IN_MILLISECONDS;

        SetEquipmentSlots(false, 49992, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiHeroismOrBloodlustTimer <= uiDiff)
        {
            if (m_creature->getFaction()) //Am i alliance?
            {
                if (!m_creature->HasAura(AURA_EXHAUSTION))
                {
                    DoCastSpellIfCan(m_creature, SPELL_HEROISM);

                    if (Creature* pChampion = m_pInstance->GetSingleCreatureFromStorage(NPC_CRUSADER_1_1))
                        pChampion->_AddAura(SPELL_HEROISM, 40000);

                    if (Creature* pChampion = m_pInstance->GetSingleCreatureFromStorage(NPC_CRUSADER_1_3))
                        pChampion->_AddAura(SPELL_HEROISM, 40000);

                    if (Creature* pChampion = m_pInstance->GetSingleCreatureFromStorage(NPC_CRUSADER_1_4))
                        pChampion->_AddAura(SPELL_HEROISM, 40000);

                    if (Creature* pChampion = m_pInstance->GetSingleCreatureFromStorage(NPC_CRUSADER_1_5))
                        pChampion->_AddAura(SPELL_HEROISM, 40000);

                    if (Creature* pChampion = m_pInstance->GetSingleCreatureFromStorage(NPC_CRUSADER_1_6))
                        pChampion->_AddAura(SPELL_HEROISM, 40000);

                    if (Creature* pChampion = m_pInstance->GetSingleCreatureFromStorage(NPC_CRUSADER_1_7))
                        pChampion->_AddAura(SPELL_HEROISM, 40000);

                    if (Creature* pChampion = m_pInstance->GetSingleCreatureFromStorage(NPC_CRUSADER_1_8))
                        pChampion->_AddAura(SPELL_HEROISM, 40000);

                    if (Creature* pChampion = m_pInstance->GetSingleCreatureFromStorage(NPC_CRUSADER_1_9))
                        pChampion->_AddAura(SPELL_HEROISM, 40000);

                    if (Creature* pChampion = m_pInstance->GetSingleCreatureFromStorage(NPC_CRUSADER_1_10))
                        pChampion->_AddAura(SPELL_HEROISM, 40000);
              
                    if (Creature* pChampion = m_pInstance->GetSingleCreatureFromStorage(NPC_CRUSADER_1_11))
                        pChampion->_AddAura(SPELL_HEROISM, 40000);

                    if (Creature* pChampion = m_pInstance->GetSingleCreatureFromStorage(NPC_CRUSADER_1_12))
                        pChampion->_AddAura(SPELL_HEROISM, 40000);

                    if (Creature* pChampion = m_pInstance->GetSingleCreatureFromStorage(NPC_CRUSADER_1_13))
                        pChampion->_AddAura(SPELL_HEROISM, 40000);

                    if (Creature* pChampion = m_pInstance->GetSingleCreatureFromStorage(NPC_CRUSADER_1_14))
                        pChampion->_AddAura(SPELL_HEROISM, 40000);
                }
            }
            else
            {
                if (!m_creature->HasAura(AURA_SATED))
                {
                    DoCastSpellIfCan(m_creature, SPELL_BLOODLUST);

                    if (Creature* pChampion = m_pInstance->GetSingleCreatureFromStorage(NPC_CRUSADER_2_1))
                        pChampion->_AddAura(SPELL_BLOODLUST, 40000);

                    if (Creature* pChampion = m_pInstance->GetSingleCreatureFromStorage(NPC_CRUSADER_2_3))
                        pChampion->_AddAura(SPELL_BLOODLUST, 40000);

                    if (Creature* pChampion = m_pInstance->GetSingleCreatureFromStorage(NPC_CRUSADER_2_4))
                        pChampion->_AddAura(SPELL_BLOODLUST, 40000);

                    if (Creature* pChampion = m_pInstance->GetSingleCreatureFromStorage(NPC_CRUSADER_2_5))
                        pChampion->_AddAura(SPELL_BLOODLUST, 40000);

                    if (Creature* pChampion = m_pInstance->GetSingleCreatureFromStorage(NPC_CRUSADER_2_6))
                        pChampion->_AddAura(SPELL_BLOODLUST, 40000);

                    if (Creature* pChampion = m_pInstance->GetSingleCreatureFromStorage(NPC_CRUSADER_2_7))
                        pChampion->_AddAura(SPELL_BLOODLUST, 40000);

                    if (Creature* pChampion = m_pInstance->GetSingleCreatureFromStorage(NPC_CRUSADER_2_8))
                        pChampion->_AddAura(SPELL_BLOODLUST, 40000);

                    if (Creature* pChampion = m_pInstance->GetSingleCreatureFromStorage(NPC_CRUSADER_2_9))
                        pChampion->_AddAura(SPELL_BLOODLUST, 40000);

                    if (Creature* pChampion = m_pInstance->GetSingleCreatureFromStorage(NPC_CRUSADER_2_10))
                        pChampion->_AddAura(SPELL_BLOODLUST, 40000);
              
                    if (Creature* pChampion = m_pInstance->GetSingleCreatureFromStorage(NPC_CRUSADER_2_11))
                        pChampion->_AddAura(SPELL_BLOODLUST, 40000);

                    if (Creature* pChampion = m_pInstance->GetSingleCreatureFromStorage(NPC_CRUSADER_2_12))
                        pChampion->_AddAura(SPELL_BLOODLUST, 40000);

                    if (Creature* pChampion = m_pInstance->GetSingleCreatureFromStorage(NPC_CRUSADER_2_13))
                        pChampion->_AddAura(SPELL_BLOODLUST, 40000);

                    if (Creature* pChampion = m_pInstance->GetSingleCreatureFromStorage(NPC_CRUSADER_2_14))
                        pChampion->_AddAura(SPELL_BLOODLUST, 40000);
                }
            }
            m_uiHeroismOrBloodlustTimer = 10*MINUTE*IN_MILLISECONDS;
        }
        else
            m_uiHeroismOrBloodlustTimer -= uiDiff;

        if (m_uiHexTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                DoCastSpellIfCan(pTarget, SPELL_HEX);
                m_uiHexTimer = 45*IN_MILLISECONDS;
            }
        }
        else
            m_uiHexTimer -= uiDiff;

        if (m_uiEartShockTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                DoCastSpellIfCan(pTarget, SPELL_EARTH_SHOCK);
                m_uiEartShockTimer = urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS);
            }
        }
        else
            m_uiEartShockTimer -= uiDiff;

        if (m_uiHealingWaveTimer < uiDiff)
        {
            if (Unit *target = DoSelectLowestHpFriendly(40.0f))
            {
                DoCastSpellIfCan(target, SPELL_HEALING_WAVE);
                m_uiHealingWaveTimer = urand(7*IN_MILLISECONDS, 12*IN_MILLISECONDS);
            }
        }
        else
            m_uiHealingWaveTimer -= uiDiff;

        if (m_uiRiptideTimer < uiDiff)
        {
            if (Unit *target = DoSelectLowestHpFriendly(40.0f))
            {
                switch(urand(0, 1))
                {
                    case 0:
                        m_creature->CastSpell(target, SPELL_RIPTIDE, false);
                        break;
                    case 1:
                        m_creature->CastSpell(target, SPELL_SPIRIT_CLEANSE , false);
                        break;
                }
            }
            m_uiRiptideTimer = urand(6*IN_MILLISECONDS, 18*IN_MILLISECONDS);
        }
        else
            m_uiRiptideTimer -= uiDiff;

        if (m_uiEarthShieldTimer < uiDiff)
        {
            if (Unit *target = DoSelectLowestHpFriendly(40.0f))
            {
                DoCastSpellIfCan(target, SPELL_EARTH_SHIELD);
                m_uiEarthShieldTimer = urand(10*IN_MILLISECONDS, 18*IN_MILLISECONDS);
            }
        }
        else
            m_uiEarthShieldTimer -= uiDiff;

        boss_faction_championsAI::UpdateAI(uiDiff);
    }
};

/***************
** Paladin Holy
****************/
struct MANGOS_DLL_DECL mob_toc_paladinAI : public boss_faction_championsAI
{
    mob_toc_paladinAI(Creature* pCreature) : boss_faction_championsAI(pCreature, AI_HEALER)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bAttack;

    uint32 m_uiBubbleTimer;
    uint32 m_uiHandOfProtectionTimer;
    uint32 m_uiHolyShockTimer;
    uint32 m_uiHandOfFreedomTimer;
    uint32 m_uiHammerOfJusticeTimer;
    uint32 m_uiHolyLightTimer;
    uint32 m_uiFlashHealTimer;

    void Reset()
    {
        m_uiBubbleTimer             = 0;
        m_uiHandOfProtectionTimer   = 0;
        m_uiHolyShockTimer          = urand(6*IN_MILLISECONDS, 15*IN_MILLISECONDS);
        m_uiHandOfFreedomTimer      = urand(25*IN_MILLISECONDS, 40*IN_MILLISECONDS);
        m_uiHammerOfJusticeTimer    = urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS);
        m_uiHolyLightTimer          = urand(8*IN_MILLISECONDS, 15*IN_MILLISECONDS);
        m_uiFlashHealTimer          = urand(2*IN_MILLISECONDS, 10*IN_MILLISECONDS);

        SetEquipmentSlots(false, 50771, 47079, EQUIP_NO_CHANGE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiBubbleTimer <= uiDiff)
        {
            if (m_creature->GetHealthPercent() <= 25.0f)
            {
                DoCastSpellIfCan(m_creature, SPELL_BUBBLE);
                m_uiBubbleTimer = 5*MINUTE*IN_MILLISECONDS;
            }
        }
        else
            m_uiBubbleTimer -= uiDiff;

        if (m_uiHammerOfJusticeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_HAMMER_OF_JUSTICE);
            m_uiHammerOfJusticeTimer = 40*IN_MILLISECONDS;
        }
        else
            m_uiHammerOfJusticeTimer -= uiDiff;

        if (m_uiHandOfProtectionTimer <= uiDiff)
        {
            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
            {
                if (target->GetHealthPercent() <= 20.0f)
                {
                    DoCastSpellIfCan(target, SPELL_HAND_OF_PROTECTION);
                    m_uiHandOfProtectionTimer = 5*MINUTE*IN_MILLISECONDS;
                }
            }
        }
        else
            m_uiHandOfProtectionTimer -= uiDiff;

        if (m_uiHandOfFreedomTimer <= uiDiff)
        {
            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
            {
                DoCastSpellIfCan(target, SPELL_HAND_OF_FREEDOM);
                m_uiHandOfFreedomTimer = 25*IN_MILLISECONDS;
            }
        }
        else
            m_uiHandOfFreedomTimer -= uiDiff;

        if (m_uiHolyLightTimer < uiDiff)
        {
            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
            {
                DoCastSpellIfCan(target, SPELL_HOLY_LIGHT);
                m_uiHolyLightTimer = urand(9*IN_MILLISECONDS, 25*IN_MILLISECONDS);
            }
        }
        else
            m_uiHolyLightTimer -= uiDiff;

        if (m_uiFlashHealTimer < uiDiff)
        {
            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
            {
                DoCastSpellIfCan(target, SPELL_FLASH_HEAL);
                m_uiFlashHealTimer = urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS);
            }
        }
        else
            m_uiFlashHealTimer -= uiDiff;

        if (m_uiHolyShockTimer < uiDiff)
        {
            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
            {
                DoCastSpellIfCan(target, SPELL_HOLY_SHOCK);
                m_uiHolyShockTimer = urand(6*IN_MILLISECONDS, 16*IN_MILLISECONDS);
            }
        }
        else
            m_uiHolyShockTimer -= uiDiff;

        boss_faction_championsAI::UpdateAI(uiDiff);
    }
};

/**************
** Priest Disc
***************/
struct MANGOS_DLL_DECL mob_toc_priestAI : public boss_faction_championsAI
{
    mob_toc_priestAI(Creature* pCreature) : boss_faction_championsAI(pCreature, AI_HEALER)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiPeananceTimer;
    uint32 m_uiRenewTimer;
    uint32 m_uiShieldTimer;
    uint32 m_uiFlashHealTimer;
    uint32 m_uiPsychicScreamTimer;
    //uint32 m_uiManaBurnTimer;

    void Reset()
    {
        m_uiPsychicScreamTimer  = urand(8*IN_MILLISECONDS, 15*IN_MILLISECONDS);  
        m_uiPeananceTimer       = urand(3*IN_MILLISECONDS, 12*IN_MILLISECONDS);
        m_uiRenewTimer          = urand(2*IN_MILLISECONDS, 16*IN_MILLISECONDS);
        m_uiShieldTimer         = 0;
        m_uiFlashHealTimer      = urand(6*IN_MILLISECONDS, 10*IN_MILLISECONDS);
        //m_uiManaBurnTimer       = urand(6*IN_MILLISECONDS, 16*IN_MILLISECONDS);

        SetEquipmentSlots(false, 49992, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiPeananceTimer < uiDiff)
        {
            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
            {
                if (DoCastSpellIfCan(target, SPELL_PEANANCE) == CAST_OK)
                {
                    m_uiPeananceTimer = urand(6*IN_MILLISECONDS, 15*IN_MILLISECONDS);  
                }
            }
        }
        else
            m_uiPeananceTimer -= uiDiff;

        if (m_uiRenewTimer < uiDiff)
        {
            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
            {
                DoCastSpellIfCan(target, SPELL_RENEW);
                m_uiRenewTimer = urand(10*IN_MILLISECONDS, 18*IN_MILLISECONDS);  
            }
        }
        else
            m_uiRenewTimer -= uiDiff;

        if (m_uiShieldTimer < uiDiff)
        {
            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
            {
                DoCastSpellIfCan(target, SPELL_SHIELD);
                m_uiShieldTimer = urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS);  
            }
        }
        else
            m_uiShieldTimer -= uiDiff;

        if (m_uiFlashHealTimer < uiDiff)
        {
            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
            {
                DoCastSpellIfCan(target, SPELL_FLASH_HEAL_P);
                m_uiFlashHealTimer = urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS);  
            }
        }
        else
            m_uiFlashHealTimer -= uiDiff;

        if (m_uiPsychicScreamTimer < uiDiff)
        {
            m_creature->CastSpell(m_creature->getVictim(), SPELL_DISPEL, true);
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_PSYCHIC_SCREAM);
            m_uiPsychicScreamTimer = 30*IN_MILLISECONDS;  
        }
        else
            m_uiPsychicScreamTimer -= uiDiff;

        /*if(m_uiManaBurnTimer < uiDiff)
        {
            if (Unit* pTarget = SelectEnemyTargetWithinMana())
            {
                DoCastSpellIfCan(pTarget, SPELL_MANA_BURN);
                m_uiManaBurnTimer = urand(6000, 15000);
            }
        }else m_uiManaBurnTimer -= uiDiff;*/

        boss_faction_championsAI::UpdateAI(uiDiff);
    }
};

/****************************************************
                        Ranged
*****************************************************/

/*********************
** Priest Shadow
**********************/
struct MANGOS_DLL_DECL mob_toc_shadow_priestAI : public boss_faction_championsAI
{
    mob_toc_shadow_priestAI(Creature* pCreature) : boss_faction_championsAI(pCreature, AI_RANGED)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiDispersionTimer;
    uint32 m_uiBlastTimer;
    uint32 m_uiSilenceTimer;
    uint32 m_uiHorrorTimer;
    uint32 m_uiFearTimer;
    uint32 m_uiDispelTimer;
    uint32 m_uiTouchTimer;
    uint32 m_uiMindFlayTimer;
    uint32 m_uiKillPlayerTimer;

    void Reset()
    {
        m_uiDispersionTimer = 0;
        m_uiBlastTimer      = urand(1*IN_MILLISECONDS, 15*IN_MILLISECONDS);  
        m_uiHorrorTimer     = urand(3*IN_MILLISECONDS, 30*IN_MILLISECONDS); 
        m_uiFearTimer       = urand(4*IN_MILLISECONDS, 19*IN_MILLISECONDS); 
        m_uiDispelTimer     = urand(8*IN_MILLISECONDS, 11*IN_MILLISECONDS); 
        m_uiTouchTimer      = urand(6*IN_MILLISECONDS, 18*IN_MILLISECONDS); 
        m_uiMindFlayTimer   = urand(7*IN_MILLISECONDS, 15*IN_MILLISECONDS); 
        m_uiSilenceTimer    = urand(9*IN_MILLISECONDS, 25*IN_MILLISECONDS); 
        m_uiKillPlayerTimer = 0;

        SetEquipmentSlots(false, 50040, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiKillPlayerTimer < uiDiff)
        {
            if (Unit *target = KillPlayer())
            {
                m_creature->CastSpell(target, SPELL_MIND_BLAST, false);
                m_creature->CastSpell(target, SPELL_MIND_FLAY, false);
                m_uiKillPlayerTimer = 10*IN_MILLISECONDS;
            }
        }
        else
            m_uiKillPlayerTimer -= uiDiff;

        if (m_uiDispersionTimer < uiDiff)
        {
            if (m_creature->GetHealthPercent() <= 20.0f)
            {
                m_creature->CastSpell(m_creature, SPELL_DISPERSION, true);
                m_uiDispersionTimer = 90*IN_MILLISECONDS;
            }
        }
        else
            m_uiDispersionTimer -= uiDiff;

        if (m_uiHorrorTimer < uiDiff)
        {
            if (Unit *target = SelectTargetWithinDist())
            {
                DoCastSpellIfCan(target, SPELL_HORROR);
                m_uiHorrorTimer = 2*MINUTE*IN_MILLISECONDS;
            }
        }
        else
            m_uiHorrorTimer -= uiDiff;

        if (m_uiFearTimer < uiDiff)
        {
            if (Unit *target = SelectTargetWithinDist())
            {
                DoCastSpellIfCan(m_creature, SPELL_PSYCHIC_SCREAM);
                m_uiFearTimer = urand(20*IN_MILLISECONDS, 35*IN_MILLISECONDS); 
            }
        }
        else
            m_uiFearTimer -= uiDiff;

        if (m_uiDispelTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_DISPEL);
            m_uiDispelTimer = urand(2*IN_MILLISECONDS, 15*IN_MILLISECONDS); 
        }
        else
            m_uiDispelTimer -= uiDiff;

        if (m_uiTouchTimer < uiDiff)
        {
            m_creature->CastSpell(m_creature->getVictim(), SPELL_SW_PAIN, true);
            m_creature->CastSpell(m_creature->getVictim(), SPELL_VAMPIRIC_TOUCH, false);
            m_uiTouchTimer = urand(5*IN_MILLISECONDS, 25*IN_MILLISECONDS); 
        }
        else
            m_uiTouchTimer -= uiDiff;

        if (m_uiBlastTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MIND_BLAST);
            m_uiBlastTimer = urand(8*IN_MILLISECONDS, 16*IN_MILLISECONDS); 
        }
        else
            m_uiBlastTimer -= uiDiff;

        if (m_uiMindFlayTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MIND_FLAY);
            m_uiMindFlayTimer = urand(9*IN_MILLISECONDS, 18*IN_MILLISECONDS); 
        }
        else
            m_uiMindFlayTimer -= uiDiff;

        if (m_uiSilenceTimer < uiDiff)
        {
            if (Unit *target = SelectEnemyTargetWithinMana())
            {
                DoCastSpellIfCan(target, SPELL_SILENCE);
                m_uiSilenceTimer = 45*IN_MILLISECONDS; 
            }
        }
        else
            m_uiSilenceTimer -= uiDiff;

        boss_faction_championsAI::UpdateAI(uiDiff);
    }
};


/************
** Warlock
*************/
struct MANGOS_DLL_DECL mob_toc_warlockAI : public boss_faction_championsAI
{
    mob_toc_warlockAI(Creature* pCreature) : boss_faction_championsAI(pCreature, AI_RANGED)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiHelfireTimer;
    uint32 m_uiFearTimer;
    uint32 m_uiUnstableTimer;
    uint32 m_uiSearingPainTimer;
    uint32 m_uiCoruptionTimer;
    uint32 m_uiExhaustTimer;
    uint32 m_uiAgonyTimer;
    uint32 m_uiShadowBoltTimer;
    uint32 m_uiSummonTimer;
    uint32 m_uiKillPlayerTimer;

    void Reset()
    {  
        m_uiHelfireTimer        = urand(30*IN_MILLISECONDS, 48*IN_MILLISECONDS); 
        m_uiFearTimer           = urand(15*IN_MILLISECONDS, 22*IN_MILLISECONDS); 
        m_uiUnstableTimer       = urand(9*IN_MILLISECONDS, 18*IN_MILLISECONDS); 
        m_uiSearingPainTimer    = urand(5*IN_MILLISECONDS, 9*IN_MILLISECONDS); 
        m_uiCoruptionTimer      = urand(2*IN_MILLISECONDS, 10*IN_MILLISECONDS); 
        m_uiExhaustTimer        = urand(1*IN_MILLISECONDS, 13*IN_MILLISECONDS); 
        m_uiAgonyTimer          = urand(3*IN_MILLISECONDS, 12*IN_MILLISECONDS); 
        m_uiShadowBoltTimer     = urand(6*IN_MILLISECONDS, 15*IN_MILLISECONDS); 
        m_uiSummonTimer         = 0;
        m_uiKillPlayerTimer     = 0;

        SetEquipmentSlots(false, 49992, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
    }


    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiKillPlayerTimer < uiDiff)
        {
            if (Unit *target = KillPlayer())
            {
                m_creature->CastSpell(target, SPELL_FEAR, false);
                m_creature->CastSpell(target, SPELL_SHADOW_BOLT, false);
                m_uiKillPlayerTimer = 10*IN_MILLISECONDS;
            }
        }
        else
            m_uiKillPlayerTimer -= uiDiff;

        if (m_uiHelfireTimer < uiDiff)
        {
            if (Unit *target =  SelectTargetWithinDist())
            {
                DoCastSpellIfCan(m_creature, SPELL_HELLFIRE);
                m_uiHelfireTimer = 45*IN_MILLISECONDS; 
            }
        }
        else
            m_uiHelfireTimer -= uiDiff;

        if (m_uiCoruptionTimer < uiDiff)
        {
            m_creature->CastSpell(m_creature->getVictim(), SPELL_UNSTABLE, false);
            m_creature->CastSpell(m_creature->getVictim(), SPELL_CURSE_OF_AGONY, true);
            m_creature->CastSpell(m_creature->getVictim(), SPELL_CURSE_OF_EXHAUSTION, true);
            m_creature->CastSpell(m_creature->getVictim(), SPELL_CORRUPTION, true);
            m_uiCoruptionTimer = urand(8*IN_MILLISECONDS, 18*IN_MILLISECONDS); 
        }
        else
            m_uiCoruptionTimer -= uiDiff;

        if (m_uiSearingPainTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SEARING_PAIN);
            m_uiSearingPainTimer = urand(6*IN_MILLISECONDS, 15*IN_MILLISECONDS); 
        }
        else
            m_uiSearingPainTimer -= uiDiff;

        if (m_uiFearTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FEAR);
            m_uiFearTimer = urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS); 
        }
        else
            m_uiFearTimer -= uiDiff;

        if (m_uiShadowBoltTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_BOLT);
            m_uiShadowBoltTimer = urand(4*IN_MILLISECONDS, 10*IN_MILLISECONDS); 
        }
        else
            m_uiShadowBoltTimer -= uiDiff;

        if (m_uiSummonTimer < uiDiff)
        {
            m_creature->SummonCreature(NPC_FELHUNTER, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
            m_uiSummonTimer = 10*MINUTE*IN_MILLISECONDS;
        }
        else
            m_uiSummonTimer -= uiDiff;

        boss_faction_championsAI::UpdateAI(uiDiff);
    }
};

/********
** Mage
*********/
struct MANGOS_DLL_DECL mob_toc_mageAI : public boss_faction_championsAI
{
    mob_toc_mageAI(Creature* pCreature) : boss_faction_championsAI(pCreature, AI_RANGED)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiBarrageTimer;
    uint32 m_uiBlastTimer;
    uint32 m_uiExplosionTimer;
    uint32 m_uiCounterSpellTimer;
    uint32 m_uiNovaTimer;
    uint32 m_uiBlinkTimer;
    uint32 m_uiPolymorphTimer;
    uint32 m_uiIceBlockTimer;
    uint32 m_uiFrostboltTimer;
    uint32 m_uiKillPlayerTimer;

    void Reset()
    {
        m_uiBarrageTimer        = urand(2*IN_MILLISECONDS, 15*IN_MILLISECONDS); 
        m_uiBlastTimer          = urand(8*IN_MILLISECONDS, 19*IN_MILLISECONDS); 
        m_uiExplosionTimer      = urand(10*IN_MILLISECONDS, 18*IN_MILLISECONDS); 
        m_uiCounterSpellTimer   = urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS); 
        m_uiBlinkTimer          = urand(7*IN_MILLISECONDS, 12*IN_MILLISECONDS); 
        m_uiIceBlockTimer       = 0; 
        m_uiPolymorphTimer      = urand(2*IN_MILLISECONDS, 15*IN_MILLISECONDS); 
        m_uiFrostboltTimer      = urand(1*IN_MILLISECONDS, 15*IN_MILLISECONDS); 
        m_uiKillPlayerTimer     = 0;

        SetEquipmentSlots(false, 47524, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiKillPlayerTimer < uiDiff)
        {
            if (Unit *target = KillPlayer())
            {
                m_creature->CastSpell(target, SPELL_CS, false);
                m_creature->CastSpell(target, SPELL_BARRAGE, false);
                m_uiKillPlayerTimer = 10*IN_MILLISECONDS;
            }
        }
        else
            m_uiKillPlayerTimer -= uiDiff;

        if (m_uiBarrageTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_BARRAGE);
            m_uiBarrageTimer = urand(4*IN_MILLISECONDS, 10*IN_MILLISECONDS); 
        }
        else
            m_uiBarrageTimer -= uiDiff;

        if (m_uiBlastTimer < uiDiff)
        {
            for (uint32 i = 1; i < 3; ++i)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    DoCastSpellIfCan(pTarget, SPELL_BLAST);
                    m_uiBlastTimer = urand(4*IN_MILLISECONDS, 15*IN_MILLISECONDS); 
                }  
            }
        }
        else
            m_uiBlastTimer -= uiDiff;

        if (m_uiExplosionTimer < uiDiff)
        {
            if (m_creature->IsWithinDist(m_creature->getVictim(), 10.0f))
            {
                DoCastSpellIfCan(m_creature, SPELL_EXPLOSION);
                m_uiExplosionTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS); 
            }
        }
        else
            m_uiExplosionTimer -= uiDiff;

        if (m_uiBlinkTimer < uiDiff)
        {
            if (m_creature->IsWithinDist(m_creature->getVictim(), 5.0f))
            {
                m_creature->CastSpell(m_creature, SPELL_FROST_NOVA, true);
                m_creature->CastSpell(m_creature, SPELL_BLINK, true);
                m_uiBlinkTimer = urand(25*IN_MILLISECONDS, 35*IN_MILLISECONDS); 
            }
        }
        else
            m_uiBlinkTimer -= uiDiff;

        if (m_uiCounterSpellTimer < uiDiff)
        {
            if (Unit* pTarget = SelectEnemyTargetWithinMana())
            {
                DoCastSpellIfCan(pTarget, SPELL_CS);
                m_uiCounterSpellTimer = 24*IN_MILLISECONDS; 
            }
        }
        else
            m_uiCounterSpellTimer -= uiDiff;

        if (m_uiFrostboltTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FROSTBOLT);
            m_uiFrostboltTimer = urand(3*IN_MILLISECONDS, 8*IN_MILLISECONDS); 
        }
        else
            m_uiFrostboltTimer -= uiDiff;

        if (m_uiIceBlockTimer < uiDiff)
        {
             if (m_creature->GetHealthPercent() <= 20.0f)
             {
                 m_creature->CastSpell(m_creature, SPELL_ICE_BLOCK, true);
                 m_uiIceBlockTimer = 3*MINUTE*IN_MILLISECONDS;
             }
        }
        else
            m_uiIceBlockTimer -= uiDiff;

        if (m_uiPolymorphTimer <= uiDiff)
        {
            if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                DoCastSpellIfCan(pTarget, SPELL_POLY);
                m_uiPolymorphTimer = urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS); 
            }
        }
        else
            m_uiPolymorphTimer -= uiDiff;

        boss_faction_championsAI::UpdateAI(uiDiff);
    }
};

/**********
** Hunter
***********/
struct MANGOS_DLL_DECL mob_toc_hunterAI : public boss_faction_championsAI
{
    mob_toc_hunterAI(Creature* pCreature) : boss_faction_championsAI(pCreature, AI_RANGED)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiAimedShotTimer;
    uint32 m_uiDeterenceTimer;
    uint32 m_uiLeapTimer;
    uint32 m_uiExplosivShotTimer;
    uint32 m_uiFrostTrapTimer;
    uint32 m_uiSteadyShotTimer;
    uint32 m_uiWingClipTimer;
    uint32 m_uiWyvernStingTimer;
    uint32 m_uiSummonTimer;
    uint32 m_uiKillPlayerTimer;

    void Reset()
    {
        m_uiAimedShotTimer    = urand(2*IN_MILLISECONDS, 5*IN_MILLISECONDS); 
        m_uiDeterenceTimer    = 0;
        m_uiLeapTimer         = urand(25*IN_MILLISECONDS, 45*IN_MILLISECONDS); 
        m_uiExplosivShotTimer = urand(4*IN_MILLISECONDS, 6*IN_MILLISECONDS); 
        m_uiFrostTrapTimer    = urand(15*IN_MILLISECONDS, 8*IN_MILLISECONDS); 
        m_uiSteadyShotTimer   = urand(6*IN_MILLISECONDS, 15*IN_MILLISECONDS); 
        m_uiWingClipTimer     = urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS); 
        m_uiWyvernStingTimer  = urand(25*IN_MILLISECONDS, 35*IN_MILLISECONDS); 
        m_uiSummonTimer       = 0;
        m_uiKillPlayerTimer   = 0;

        SetEquipmentSlots(false, 47156, EQUIP_NO_CHANGE, 48711);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiKillPlayerTimer < uiDiff)
        {
            if (Unit *target = KillPlayer())
            {
                m_creature->CastSpell(target, SPELL_EXPLOSIVE_SHOT, false);
                m_creature->CastSpell(target, SPELL_AIMED_SHOT, false);
                m_uiKillPlayerTimer = 10*IN_MILLISECONDS;;
            }
        }
        else
            m_uiKillPlayerTimer -= uiDiff;

        if (m_uiAimedShotTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                DoCastSpellIfCan(pTarget, SPELL_AIMED_SHOT);
                m_uiAimedShotTimer = urand(7*IN_MILLISECONDS, 10*IN_MILLISECONDS); 
            }
        }
        else
            m_uiAimedShotTimer -= uiDiff;

        if (m_uiDeterenceTimer < uiDiff)
        {
            if (m_creature->GetHealthPercent() <= 20.0f)
            {
                m_creature->CastSpell(m_creature, SPELL_DETERENCE, true);
                m_uiDeterenceTimer = 90*IN_MILLISECONDS; 
            }
        }
        else
            m_uiDeterenceTimer -= uiDiff;

        if (m_uiLeapTimer < uiDiff)
        {
            if (m_creature->IsWithinDist(m_creature->getVictim(), 5.0f))
            {
                m_creature->CastSpell(m_creature->getVictim(), SPELL_WING_CLIP, true);
                m_creature->CastSpell(m_creature, SPELL_DISENGAGE, true);
                m_uiLeapTimer = urand(35*IN_MILLISECONDS, 45*IN_MILLISECONDS); 
            }
        }
        else
            m_uiLeapTimer -= uiDiff;

        if (m_uiExplosivShotTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_EXPLOSIVE_SHOT);
            m_uiExplosivShotTimer = urand(4*IN_MILLISECONDS, 8*IN_MILLISECONDS); 
        }
        else
            m_uiExplosivShotTimer -= uiDiff;

        if (m_uiSteadyShotTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_STEADY_SHOT);
            m_uiSteadyShotTimer = urand(3*IN_MILLISECONDS, 12*IN_MILLISECONDS); 
        }
        else
            m_uiSteadyShotTimer -= uiDiff;

        if (m_uiFrostTrapTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_FROST_TRAP);
            m_uiFrostTrapTimer = 1*MINUTE*IN_MILLISECONDS; 
        }
        else
            m_uiFrostTrapTimer -= uiDiff;

        if (m_uiWyvernStingTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_WYVERN_STING);
            m_uiWyvernStingTimer  = 2*MINUTE*IN_MILLISECONDS; 
        }
        else
            m_uiWyvernStingTimer -= uiDiff;

        if (m_uiSummonTimer < uiDiff)
        {
            m_creature->SummonCreature(NPC_CAT, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
            m_uiSummonTimer = 10*MINUTE*IN_MILLISECONDS;
        }
        else
            m_uiSummonTimer -= uiDiff;

        boss_faction_championsAI::UpdateAI(uiDiff);
    }
};    



/*****************
** Druid Balance
******************/
struct MANGOS_DLL_DECL mob_toc_boomkinAI : public boss_faction_championsAI
{
    mob_toc_boomkinAI(Creature* pCreature) : boss_faction_championsAI(pCreature, AI_RANGED)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiCycloneTimer;
    uint32 m_uiFearieFeralTimer;
    uint32 m_uiForcedTimer;
    uint32 m_uiSpellTimer;
    uint32 m_uiMonfireTimer;
    uint32 m_uiStarfireTimer;
    uint32 m_uiWrathTimer;
    uint32 m_uiRootsTimer;
    uint32 m_uiBarkskinTimer;
    uint32 m_uiKillPlayerTimer;

    void Reset()
    {
        m_uiCycloneTimer        = urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS); 
        m_uiFearieFeralTimer    = 0;
        m_uiForcedTimer         = urand(35*IN_MILLISECONDS, 45*IN_MILLISECONDS); 
        m_uiSpellTimer          = urand(6*IN_MILLISECONDS, 15*IN_MILLISECONDS); 
        m_uiStarfireTimer       = urand(8*IN_MILLISECONDS, 19*IN_MILLISECONDS); 
        m_uiWrathTimer          = urand(3*IN_MILLISECONDS, 10*IN_MILLISECONDS); 
        m_uiBarkskinTimer       = 0;
        m_uiRootsTimer          = urand(10*IN_MILLISECONDS, 18*IN_MILLISECONDS); 
        m_uiKillPlayerTimer     = 0;

        SetEquipmentSlots(false, 50966, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiKillPlayerTimer < uiDiff)
        {
            if (Unit *target = KillPlayer())
            {
                m_creature->CastSpell(target, SPELL_WRATH, false);
                m_creature->CastSpell(target, SPELL_MOONFIRE, false);
                m_uiKillPlayerTimer = 10*IN_MILLISECONDS;
            }
        }
        else
            m_uiKillPlayerTimer -= uiDiff;

        if (m_uiBarkskinTimer < uiDiff)
        {
            if (m_creature->GetHealthPercent() <= 50.0f)
            {
                m_creature->CastSpell(m_creature, SPELL_BARKSKIN, true);
                m_uiBarkskinTimer = 1*MINUTE*IN_MILLISECONDS;
            }
        }
        else
            m_uiBarkskinTimer -= uiDiff;

        if (m_uiCycloneTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
            {
                DoCastSpellIfCan(pTarget, SPELL_CYCLONE);
                m_uiCycloneTimer = urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS); 
            }
        }
        else
            m_uiCycloneTimer -= uiDiff;

        if (m_uiFearieFeralTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FEARIE);
            m_uiFearieFeralTimer = urand(25*IN_MILLISECONDS, 60*IN_MILLISECONDS); 
        }
        else
            m_uiFearieFeralTimer -= uiDiff;

        if (m_uiForcedTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FORCED_OF_NATURE);
            m_uiForcedTimer = 90*IN_MILLISECONDS;
        }
        else
            m_uiForcedTimer -= uiDiff;

        if (m_uiSpellTimer < uiDiff)
        {
            m_creature->CastSpell(m_creature->getVictim(), SPELL_MOONFIRE, true);
            m_creature->CastSpell(m_creature->getVictim(), SPELL_SWARM, true);
            m_uiSpellTimer = urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS); 
        }
        else
            m_uiSpellTimer -= uiDiff;

        if (m_uiStarfireTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_STARFIRE);
            m_uiStarfireTimer = urand(8*IN_MILLISECONDS, 25*IN_MILLISECONDS); 
        }
        else
            m_uiStarfireTimer -= uiDiff;

        if (m_uiWrathTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_WRATH);
            m_uiWrathTimer = urand(4*IN_MILLISECONDS, 8*IN_MILLISECONDS); 
        }
        else
            m_uiWrathTimer -= uiDiff;

        if (m_uiRootsTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_ROOTS_B);
            m_uiRootsTimer = urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS); 
        }
        else
            m_uiRootsTimer -= uiDiff;

        boss_faction_championsAI::UpdateAI(uiDiff);
    }
};

/********************************************************************
                            MELEE
********************************************************************/

/***********
** Warrior
************/
struct MANGOS_DLL_DECL mob_toc_warriorAI : public boss_faction_championsAI
{
    mob_toc_warriorAI(Creature* pCreature) : boss_faction_championsAI(pCreature, AI_MELEE)
    {
        m_pInstance = (ScriptedInstance *) pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiBladestormTimer;
    uint32 m_uiIntimidatingShoutTimer;
    uint32 m_uiMortalStrikeTimer;
    uint32 m_uiSunderArmorTimer;
    uint32 m_uiChargeTimer;
    uint32 m_uiRetaliationTimer;
    uint32 m_uiOverpowerTimer;
    uint32 m_uiShatteringThrowTimer;
    uint32 m_uiDisarmTimer;
    uint32 m_uiRemoveBubleTimer;
    uint32 m_uiKillPlayerTimer;

    void Reset()
    {
        m_uiBladestormTimer         = urand(35*IN_MILLISECONDS, 45*IN_MILLISECONDS); 
        m_uiChargeTimer             = urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS); 
        m_uiIntimidatingShoutTimer  = urand(25*IN_MILLISECONDS, 35*IN_MILLISECONDS); 
        m_uiMortalStrikeTimer       = urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS); 
        m_uiSunderArmorTimer        = urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS); 
        m_uiRetaliationTimer        = urand(45*IN_MILLISECONDS, 55*IN_MILLISECONDS); 
        m_uiOverpowerTimer          = urand(8*IN_MILLISECONDS, 8*IN_MILLISECONDS); 
        m_uiShatteringThrowTimer    = urand(50*IN_MILLISECONDS, 55*IN_MILLISECONDS); 
        m_uiDisarmTimer             = urand(15*IN_MILLISECONDS, 35*IN_MILLISECONDS); 
        m_uiRemoveBubleTimer        = 0;
        m_uiKillPlayerTimer         = 0;

        SetEquipmentSlots(false, 47427, 46964, EQUIP_NO_CHANGE);
    }

    // Its not good idea
    Unit* SelectPlayerWithDivineShield()
    {
        ThreatList const& tList = m_creature->getThreatManager().getThreatList();
        ThreatList::const_iterator iter;
        for (iter = tList.begin(); iter!=tList.end(); ++iter)
        {
            Unit *target;
            if (target = m_creature->GetMap()->GetUnit((*iter)->getUnitGuid()))
                if (target->HasAura(SPELL_DIVINE_SHIELD))
                    return target;
        }
        return NULL;
    }

    // Its not good idea
    Unit* SelectPlayerWithIceBlock()
    {
        ThreatList const& tList = m_creature->getThreatManager().getThreatList();
        ThreatList::const_iterator iter;
        for (iter = tList.begin(); iter!=tList.end(); ++iter)
        {
            Unit *target;
            if (target = m_creature->GetMap()->GetUnit((*iter)->getUnitGuid()))
                if (target->HasAura(SPELL_ICE_BLOCK))
                    return target;
        }
        return NULL;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiKillPlayerTimer < uiDiff)
        {
            if (Unit *target = KillPlayer())
            {
                m_creature->CastSpell(target, SPELL_INTIMIDATING_SHOUT, false);
                m_creature->CastSpell(target, SPELL_BLADESTORM, false);
                m_uiKillPlayerTimer = 10*IN_MILLISECONDS;
            }
        }
        else
            m_uiKillPlayerTimer -= uiDiff;

        // Its bad solution but work
        if (m_uiRemoveBubleTimer < uiDiff)
        { 
            if (Unit *target = SelectPlayerWithDivineShield())
            {
                DoCastSpellIfCan(target, SPELL_SHATTERING_THROW);

                if (target->HasAura(SPELL_SHATTERING_THROW))
                {
                    target->RemoveAurasDueToSpell(SPELL_DIVINE_SHIELD);
                }
            }
            if (Unit *target = SelectPlayerWithIceBlock())
            {
                DoCastSpellIfCan(target, SPELL_SHATTERING_THROW);

                if (target->HasAura(SPELL_SHATTERING_THROW))
                {
                    target->RemoveAurasDueToSpell(SPELL_ICE_BLOCK);
                }
            }
            m_uiRemoveBubleTimer = 10*IN_MILLISECONDS;
        }
        else
            m_uiRemoveBubleTimer -= uiDiff;

        if (m_uiBladestormTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_BLADESTORM);
            m_uiBladestormTimer = 90*IN_MILLISECONDS;
        }
        else
            m_uiBladestormTimer -= uiDiff;

        if (m_uiChargeTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                DoCastSpellIfCan(pTarget, SPELL_CHARGE);
                m_uiChargeTimer = urand(15*IN_MILLISECONDS, 20*IN_MILLISECONDS); 
            }
        }
        else
            m_uiChargeTimer -= uiDiff;

        if (m_uiIntimidatingShoutTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_INTIMIDATING_SHOUT);
            m_uiIntimidatingShoutTimer = 2*MINUTE*IN_MILLISECONDS;
        }
        else
            m_uiIntimidatingShoutTimer -= uiDiff;

        if (m_uiMortalStrikeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MORTAL_STRIKE);
            m_uiMortalStrikeTimer = urand(10*IN_MILLISECONDS, 18*IN_MILLISECONDS);
        }
        else
            m_uiMortalStrikeTimer -= uiDiff;

        if (m_uiSunderArmorTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SUNDER_ARMOR);
            m_uiSunderArmorTimer = urand(10*IN_MILLISECONDS, 14*IN_MILLISECONDS);
        }
        else
            m_uiSunderArmorTimer -= uiDiff;

        if (m_uiRetaliationTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_RETALIATION);
            m_uiRetaliationTimer = 5*MINUTE*IN_MILLISECONDS;
        }
        else
            m_uiRetaliationTimer -= uiDiff;

        if (m_uiOverpowerTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_OVERPOWER);
            m_uiOverpowerTimer = urand(8*IN_MILLISECONDS, 12*IN_MILLISECONDS);
        }
        else
            m_uiOverpowerTimer -= uiDiff;

        if (m_uiDisarmTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_DISARM);
            m_uiDisarmTimer = 40*IN_MILLISECONDS;
        }
        else
            m_uiDisarmTimer -= uiDiff;

        if (m_uiShatteringThrowTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                DoCastSpellIfCan(pTarget, SPELL_SHATTERING_THROW);
                m_uiShatteringThrowTimer = 5*MINUTE*IN_MILLISECONDS;
            }
        }
        else
            m_uiShatteringThrowTimer -= uiDiff;

        DoMeleeAttackIfReady();

        boss_faction_championsAI::UpdateAI(uiDiff);
    }
};

/*******
** DK
********/
struct MANGOS_DLL_DECL  mob_toc_dkAI : public boss_faction_championsAI
{
    mob_toc_dkAI(Creature* pCreature) : boss_faction_championsAI(pCreature, AI_MELEE)
    {
        m_pInstance = (ScriptedInstance *) pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiIceboundFortitudeTimer;
    uint32 m_uiChainsOfIceTimer;
    uint32 m_uiDeathCoilTimer;
    uint32 m_uiSilenceTimer;
    uint32 m_uiFrostStrikeTimer;
    uint32 m_uiIcyTouchTimer;
    uint32 m_uiDeathGripTimer;
    uint32 m_uiKillPlayerTimer;

    void Reset()
    {
        m_uiChainsOfIceTimer        = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS); 
        m_uiDeathCoilTimer          = urand(8*IN_MILLISECONDS, 12*IN_MILLISECONDS); 
        m_uiSilenceTimer            = urand(25*IN_MILLISECONDS, 40*IN_MILLISECONDS); 
        m_uiFrostStrikeTimer        = urand(7*IN_MILLISECONDS, 15*IN_MILLISECONDS); 
        m_uiIcyTouchTimer           = urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS); 
        m_uiDeathGripTimer          = urand(15*IN_MILLISECONDS, 20*IN_MILLISECONDS); 
        m_uiIceboundFortitudeTimer  = 0;
        m_uiKillPlayerTimer         = 0;

        SetEquipmentSlots(false, 47518, 51021, EQUIP_NO_CHANGE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiKillPlayerTimer < uiDiff)
        {
            if (Unit *target = KillPlayer())
            {
                m_creature->CastSpell(target, SPELL_CHAINS, false);
                m_creature->CastSpell(target, SPELL_DEATH_COLI, false);
                m_uiKillPlayerTimer = 10*IN_MILLISECONDS;
            }
        }
        else
            m_uiKillPlayerTimer -= uiDiff;

        if (m_uiIceboundFortitudeTimer < uiDiff)
        {
            if (m_creature->GetHealthPercent() < 50.0f)
            {
                m_creature->CastSpell(m_creature, SPELL_ICEBOUND, true);
                m_uiIceboundFortitudeTimer = 2*MINUTE*IN_MILLISECONDS;
            }
        }
        else
            m_uiIceboundFortitudeTimer -= uiDiff;

        if (m_uiChainsOfIceTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                DoCastSpellIfCan(pTarget, SPELL_CHAINS);
                m_uiChainsOfIceTimer = urand(15*IN_MILLISECONDS, 20*IN_MILLISECONDS); 
            }
        }
        else
            m_uiChainsOfIceTimer -= uiDiff;

        if (m_uiDeathCoilTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_DEATH_COLI);
            m_uiDeathCoilTimer = urand(8*IN_MILLISECONDS, 14*IN_MILLISECONDS); 
        }
        else
            m_uiDeathCoilTimer -= uiDiff;

        if (m_uiSilenceTimer < uiDiff)
        {
            if (Unit *target = SelectEnemyTargetWithinMana())
            {
                DoCastSpellIfCan(target, SPELL_STRANGULATE);
                m_uiSilenceTimer = 2*IN_MILLISECONDS;
            }
        }
        else
            m_uiSilenceTimer -= uiDiff;

        if (m_uiFrostStrikeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FROST_STRIKE);
            m_uiFrostStrikeTimer = urand(8*IN_MILLISECONDS, 18*IN_MILLISECONDS); 
        }
        else
            m_uiFrostStrikeTimer -= uiDiff;

        if (m_uiIcyTouchTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_ICE_TOUCH);
            m_uiIcyTouchTimer = urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS); 
        }
        else
            m_uiIcyTouchTimer -= uiDiff;

        if (m_uiDeathGripTimer <= uiDiff)
        {
            if (m_creature->IsInRange(m_creature->getVictim(), 10.0f, 30.0f))
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_DEATH_GRIP);
                m_creature->getVictim()->NearTeleportTo(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0.0f);
                m_uiDeathGripTimer = 35*IN_MILLISECONDS; 
            }
        }
        else
            m_uiDeathGripTimer -= uiDiff;

        DoMeleeAttackIfReady();

        boss_faction_championsAI::UpdateAI(uiDiff);
    }
};

/*********
** Rogue
**********/
struct MANGOS_DLL_DECL  mob_toc_rogueAI : public boss_faction_championsAI
{
    mob_toc_rogueAI(Creature* pCreature) : boss_faction_championsAI(pCreature, AI_MELEE)
    {
        m_pInstance = (ScriptedInstance *) pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiFanOfKnivesTimer;
    uint32 m_uiHemorrhageTimer;
    uint32 m_uiEviscerateTimer;
    uint32 m_uiShadowstepTimer;
    uint32 m_uiBlindTimer;
    uint32 m_uiCloakTimer;
    uint32 m_uiBladeFlurryTimer;
    uint32 m_uiKillPlayerTimer;
    uint32 m_uiWoundPoisonTimer;

    void Reset()
    {
        m_uiFanOfKnivesTimer = urand(8*IN_MILLISECONDS, 10*IN_MILLISECONDS);
        m_uiHemorrhageTimer  = 0;
        m_uiEviscerateTimer  = urand(15*IN_MILLISECONDS, 20*IN_MILLISECONDS);
        m_uiShadowstepTimer  = urand(10*IN_MILLISECONDS, 80*IN_MILLISECONDS);
        m_uiBlindTimer       = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
        m_uiCloakTimer       = 0;
        m_uiBladeFlurryTimer = urand(12*IN_MILLISECONDS, 120*IN_MILLISECONDS);
        m_uiWoundPoisonTimer = urand(2*IN_MILLISECONDS, 10*IN_MILLISECONDS); 
        m_uiKillPlayerTimer  = 0;
        
        SetEquipmentSlots(false, 47422, 49982, EQUIP_NO_CHANGE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiKillPlayerTimer < uiDiff)
        {
            if (Unit *target = KillPlayer())
            {
                m_creature->CastSpell(target, SPELL_SHADOWSTEP, false);
                m_creature->CastSpell(target, SPELL_EVISCERATE, true);
                m_uiKillPlayerTimer = 10*IN_MILLISECONDS;
            }
        }
        else
            m_uiKillPlayerTimer -= uiDiff;

        //Its not blizzlike but work.
        if (m_uiWoundPoisonTimer < uiDiff)
        {
            if (m_creature->IsWithinDist(m_creature->getVictim(), 3.0f))
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_WOUND_POISON);
                m_uiWoundPoisonTimer = urand(10*IN_MILLISECONDS, 16*IN_MILLISECONDS);
            }
        }
        else
            m_uiWoundPoisonTimer -= uiDiff;

        if (m_uiFanOfKnivesTimer <= uiDiff)
        {
            if (Unit *target = SelectTargetWithinDist())
            {
                DoCastSpellIfCan(target, SPELL_FAN_OF_KNIVES);
                m_uiFanOfKnivesTimer = urand(16*IN_MILLISECONDS, 20*IN_MILLISECONDS);
            }
        }
        else
            m_uiFanOfKnivesTimer -= uiDiff;

        if (m_uiHemorrhageTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_HEMORRHAGE);
            m_uiHemorrhageTimer = urand(8*IN_MILLISECONDS, 15*IN_MILLISECONDS);
        }
        else
            m_uiHemorrhageTimer -= uiDiff;

        if (m_uiEviscerateTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_EVISCERATE);
            m_uiEviscerateTimer = urand(20*IN_MILLISECONDS, 35*IN_MILLISECONDS);
        }
        else
            m_uiEviscerateTimer -= uiDiff;

        if (m_uiShadowstepTimer <= uiDiff)
        {
            if (m_creature->IsInRange(m_creature->getVictim(), 10.0f, 40.0f))
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOWSTEP);
                m_uiShadowstepTimer = 40*IN_MILLISECONDS;
            }
        }
        else
            m_uiShadowstepTimer -= uiDiff;

        if (m_uiBlindTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
            {
                if(Unit *target = SelectTargetWithinDist())
                {
                    DoCastSpellIfCan(target, SPELL_BLIND);
                    m_uiBlindTimer = 2*MINUTE*IN_MILLISECONDS;
                }
            }
        }
        else
            m_uiBlindTimer -= uiDiff;

        if (m_uiCloakTimer <= uiDiff)
        {
            if (m_creature->GetHealthPercent() < 50.0f)
            {
                DoCastSpellIfCan(m_creature, SPELL_CLOAK);
                m_uiCloakTimer = 90*IN_MILLISECONDS;
            }
        }
        else
            m_uiCloakTimer -= uiDiff;

        if (m_uiBladeFlurryTimer <= uiDiff)
        {
             DoCastSpellIfCan(m_creature, SPELL_BLADE_FLURRY);
             m_uiBladeFlurryTimer = 2*MINUTE*IN_MILLISECONDS;
        }
        else
            m_uiBladeFlurryTimer -= uiDiff;

        DoMeleeAttackIfReady();

        boss_faction_championsAI::UpdateAI(uiDiff);
    }
};

/*************
** Shaman En
**************/
struct MANGOS_DLL_DECL  mob_toc_enh_shamanAI : public boss_faction_championsAI
{
    mob_toc_enh_shamanAI(Creature* pCreature) : boss_faction_championsAI(pCreature, AI_MELEE)
    {
        m_pInstance = (ScriptedInstance *) pCreature->GetInstanceData();
        Reset();
    }
    ScriptedInstance* m_pInstance;

    uint32 m_uiHeroismOrBloodlustTimer;
    uint32 m_uiEarthShockTimer;
    uint32 m_uiStormstrikeTimer;
    uint32 m_uiLavaLashTimer;
    uint32 m_uiTotemTimer;
    uint32 m_uiSummonTotemTimer;
    uint32 m_uiWindFuryTimer;
    uint32 m_uiKillPlayerTimer;

    void Reset()
    {
        m_uiHeroismOrBloodlustTimer = urand(25*IN_MILLISECONDS, 60*IN_MILLISECONDS);
        m_uiEarthShockTimer         = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
        m_uiStormstrikeTimer        = urand(9*IN_MILLISECONDS, 20*IN_MILLISECONDS);
        m_uiLavaLashTimer           = urand(5*IN_MILLISECONDS, 8*IN_MILLISECONDS);
        m_uiWindFuryTimer           = urand(5000, 15000);
        m_uiKillPlayerTimer         = 0;
        m_uiSummonTotemTimer        = 0;

        SetEquipmentSlots(false, 51803, 48013, EQUIP_NO_CHANGE);
    }

    void Totems()
    {
        switch(urand(0, 1))
        {
            case 0:
                m_creature->CastSpell(m_creature, SPELL_TOTEM_GROUND, false);
                break;
            case 1:
                m_creature->CastSpell(m_creature, SPELL_TOTEM_TREMOR, false);
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiKillPlayerTimer < uiDiff)
        {
            if (Unit *target = KillPlayer())
            {
                m_creature->CastSpell(target, SPELL_STORMSTRIKE, false);
                m_creature->CastSpell(target, SPELL_LAVA_LASH, false);
                m_uiKillPlayerTimer = 10*IN_MILLISECONDS;
            }
        }
        else
            m_uiKillPlayerTimer -= uiDiff;

        if (m_uiHeroismOrBloodlustTimer <= uiDiff)
        {
            if (m_creature->getFaction()) //Am i alliance?
            {
                if (!m_creature->HasAura(AURA_EXHAUSTION))
                    DoCastSpellIfCan(m_creature, SPELL_HEROISM);
            }
            else
            {
                if (!m_creature->HasAura(AURA_SATED))
                    DoCastSpellIfCan(m_creature, SPELL_BLOODLUST);
            }
            m_uiHeroismOrBloodlustTimer = 10*MINUTE*IN_MILLISECONDS;
        }
        else
            m_uiHeroismOrBloodlustTimer -= uiDiff;

        if (m_uiEarthShockTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_EARTH_SHOCK);
            m_uiEarthShockTimer = urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS);
        }
        else
            m_uiEarthShockTimer -= uiDiff;

        if (m_uiStormstrikeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_STORMSTRIKE);
            m_uiStormstrikeTimer = urand(14*IN_MILLISECONDS, 30*IN_MILLISECONDS);
        }
        else
            m_uiStormstrikeTimer -= uiDiff;

        if (m_uiLavaLashTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_LAVA_LASH);
            m_uiLavaLashTimer = urand(12*IN_MILLISECONDS, 16*IN_MILLISECONDS);
        }
        else
            m_uiLavaLashTimer -= uiDiff;

        //Its not blizzlike but work.
        if (m_uiWindFuryTimer < uiDiff)
        {
            if (m_creature->IsWithinDist(m_creature->getVictim(), 3.0f))
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_WINDFURY);
                m_uiWindFuryTimer = urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS);
            }
        }
        else
            m_uiWindFuryTimer -= uiDiff;

        if (m_uiTotemTimer < uiDiff)
        {
            Totems();
            m_uiTotemTimer = urand(1*IN_MILLISECONDS, 3*IN_MILLISECONDS);
        }
        else
            m_uiTotemTimer -= uiDiff;

        if (m_uiSummonTotemTimer < uiDiff)
        {
            m_creature->CastSpell(m_creature, SPELL_TOTEM_SEARING, true);
            m_creature->CastSpell(m_creature, SPELL_TOTEM_HEAL, true);
            m_creature->CastSpell(m_creature, SPELL_TOTEM_STR, true);
            m_creature->CastSpell(m_creature, SPELL_TOTEM_WIND, true);

            m_uiSummonTotemTimer = urand(18*IN_MILLISECONDS, 30*IN_MILLISECONDS);
        }
        else
            m_uiSummonTotemTimer -= uiDiff;

        DoMeleeAttackIfReady();

        boss_faction_championsAI::UpdateAI(uiDiff);
    }
};

/****************
** Paladyn Retri
*****************/
struct MANGOS_DLL_DECL  mob_toc_retro_paladinAI : public boss_faction_championsAI
{
    mob_toc_retro_paladinAI(Creature* pCreature) : boss_faction_championsAI(pCreature, AI_MELEE)
    {
        m_pInstance = (ScriptedInstance *) pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiRepeteanceTimer;
    uint32 m_uiCrusaderStrikeTimer;
    uint32 m_uiAvengingWrathTimer;
    uint32 m_uiDivineShieldTimer;
    uint32 m_uiDivineStormTimer;
    uint32 m_uiJudgementOfCommandTimer;
    uint32 m_uiHammerOfJusticeTimer;
    uint32 m_uiHandOfProtectionTimer;
    uint32 m_uiKillPlayerTimer;

    void Reset()
    {
        m_uiRepeteanceTimer         = 30*IN_MILLISECONDS;
        m_uiCrusaderStrikeTimer     = urand(6*IN_MILLISECONDS, 18*IN_MILLISECONDS);
        m_uiAvengingWrathTimer      = urand(35*IN_MILLISECONDS, 40*IN_MILLISECONDS); 
        m_uiDivineShieldTimer       = urand(0*IN_MILLISECONDS, 360*IN_MILLISECONDS);
        m_uiDivineStormTimer        = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS); 
        m_uiJudgementOfCommandTimer = urand(8*IN_MILLISECONDS, 15*IN_MILLISECONDS);
        m_uiHammerOfJusticeTimer    = urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS);
        m_uiHandOfProtectionTimer   = 0;
        m_uiKillPlayerTimer         = 0;

        SetEquipmentSlots(false, 47519, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
        DoCastSpellIfCan(m_creature, SPELL_SEAL_COMAND);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiKillPlayerTimer < uiDiff)
        {
            if (Unit *target = KillPlayer())
            {
                m_creature->CastSpell(target, SPELL_STRIKE, false);
                m_creature->CastSpell(target, SPELL_STORM, false);
                m_uiKillPlayerTimer = 10*IN_MILLISECONDS;
            }
        }
        else
            m_uiKillPlayerTimer -= uiDiff;

        if (m_uiRepeteanceTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                DoCastSpellIfCan(pTarget, SPELL_REPENTANCE);
                m_uiRepeteanceTimer = 1*MINUTE*IN_MILLISECONDS;
            }
        }
        else
            m_uiRepeteanceTimer -= uiDiff;

        if (m_uiHammerOfJusticeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_HAMMER_OF_JUSTICE);
            m_uiHammerOfJusticeTimer = 35*IN_MILLISECONDS;
        }
        else
            m_uiHammerOfJusticeTimer -= uiDiff;

        if (m_uiHandOfProtectionTimer <= uiDiff)
        {
            if (Unit* target = DoSelectLowestHpFriendly(40.0f))
            {
                if (target->GetHealthPercent() <= 20.0f)
                {
                    DoCastSpellIfCan(target, SPELL_HAND_OF_PROTECTION);
                    m_uiHandOfProtectionTimer = 5*MINUTE*IN_MILLISECONDS;
                }
            }
        }
        else
            m_uiHandOfProtectionTimer -= uiDiff;

        if (m_uiCrusaderStrikeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_STRIKE);
            m_uiCrusaderStrikeTimer = urand(6*IN_MILLISECONDS, 18*IN_MILLISECONDS);
        }
        else
            m_uiCrusaderStrikeTimer -= uiDiff;

        if (m_uiAvengingWrathTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_AVENGING_WRATH);
            m_uiAvengingWrathTimer = 2*MINUTE*IN_MILLISECONDS;
        }
        else
            m_uiAvengingWrathTimer -= uiDiff;

        if (m_uiDivineShieldTimer <= uiDiff)
        {
            if (m_creature->GetHealthPercent() < 50.0f)
            {
                DoCastSpellIfCan(m_creature, SPELL_DIVINE_SHIELD);
                m_uiDivineShieldTimer = 5*MINUTE*IN_MILLISECONDS;
            }
        }
        else
            m_uiDivineShieldTimer -= uiDiff;

        if (m_uiDivineStormTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_STORM);
            m_uiDivineStormTimer = urand(10*IN_MILLISECONDS, 18*IN_MILLISECONDS);
        }
        else
            m_uiDivineStormTimer -= uiDiff;

        if (m_uiJudgementOfCommandTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_JUDGEMENT_COMAND);
            m_uiJudgementOfCommandTimer = urand(8*IN_MILLISECONDS, 15*IN_MILLISECONDS);
        }
        else
            m_uiJudgementOfCommandTimer -= uiDiff;

        DoMeleeAttackIfReady();

        boss_faction_championsAI::UpdateAI(uiDiff);
    }
};

/****************
** Felhunter
*****************/
struct MANGOS_DLL_DECL  mob_toc_pet_warlockAI : public boss_faction_championsAI
{
    mob_toc_pet_warlockAI(Creature* pCreature) : boss_faction_championsAI(pCreature, AI_MELEE)
    {
        m_pInstance = (ScriptedInstance *) pCreature->GetInstanceData();
        Reset();
    }

    uint32 m_uiDevourMagicTimer;
    uint32 m_uiSpellLockTimer;

    void Reset()
    {
        m_uiDevourMagicTimer = urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS);
        m_uiSpellLockTimer   = urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_CRUSADERS) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiDevourMagicTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_DEVOUR_MAGIC);
            m_uiDevourMagicTimer = urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS);
        }
        else
            m_uiDevourMagicTimer -= uiDiff;

        if (m_uiSpellLockTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SPELL_LOCK);
            m_uiSpellLockTimer = urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS);
        }
        else
            m_uiSpellLockTimer -= uiDiff;

        DoMeleeAttackIfReady();

        boss_faction_championsAI::UpdateAI(uiDiff);
    }
};

/********
** Cat
*********/
struct MANGOS_DLL_DECL  mob_toc_pet_hunterAI : public boss_faction_championsAI
{
    mob_toc_pet_hunterAI(Creature* pCreature) : boss_faction_championsAI(pCreature, AI_MELEE)
    {
        m_pInstance = (ScriptedInstance *) pCreature->GetInstanceData();
        Reset();
    }

    uint32 m_uiClawTimer;

    void Reset()
    {
        m_uiClawTimer = urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_CRUSADERS) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiClawTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLAW);
            m_uiClawTimer = urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS);
        }
        else
            m_uiClawTimer -= uiDiff;

        DoMeleeAttackIfReady();

        boss_faction_championsAI::UpdateAI(uiDiff);
    }
};

CreatureAI* GetAI_mob_toc_druid(Creature *pCreature)
{
    return new mob_toc_druidAI (pCreature);
}
CreatureAI* GetAI_mob_toc_shaman(Creature *pCreature)
{
    return new mob_toc_shamanAI (pCreature);
}
CreatureAI* GetAI_mob_toc_paladin(Creature *pCreature)
{
    return new mob_toc_paladinAI (pCreature);
}
CreatureAI* GetAI_mob_toc_priest(Creature *pCreature)
{
    return new mob_toc_priestAI (pCreature);
}
CreatureAI* GetAI_mob_toc_shadow_priest(Creature *pCreature)
{
    return new mob_toc_shadow_priestAI (pCreature);
}
CreatureAI* GetAI_mob_toc_warlock(Creature *pCreature)
{
    return new mob_toc_warlockAI (pCreature);
}
CreatureAI* GetAI_mob_toc_mage(Creature *pCreature)
{
    return new mob_toc_mageAI (pCreature);
}
CreatureAI* GetAI_mob_toc_hunter(Creature *pCreature)
{
    return new mob_toc_hunterAI (pCreature);
}
CreatureAI* GetAI_mob_toc_boomkin(Creature *pCreature)
{
    return new mob_toc_boomkinAI (pCreature);
}
CreatureAI* GetAI_mob_toc_warrior(Creature *pCreature)
{
    return new mob_toc_warriorAI (pCreature);
}
CreatureAI* GetAI_mob_toc_dk(Creature *pCreature)
{
    return new mob_toc_dkAI (pCreature);
}
CreatureAI* GetAI_mob_toc_rogue(Creature *pCreature)
{
    return new mob_toc_rogueAI (pCreature);
}
CreatureAI* GetAI_mob_toc_enh_shaman(Creature *pCreature)
{
    return new mob_toc_enh_shamanAI (pCreature);
}
CreatureAI* GetAI_mob_toc_retro_paladin(Creature *pCreature)
{
    return new mob_toc_retro_paladinAI (pCreature);
}
CreatureAI* GetAI_mob_toc_pet_warlock(Creature *pCreature)
{
    return new mob_toc_pet_warlockAI (pCreature);
}
CreatureAI* GetAI_mob_toc_pet_hunter(Creature *pCreature)
{
    return new mob_toc_pet_hunterAI (pCreature);
}

void AddSC_boss_faction_champions()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "mob_toc_druid";
    newscript->GetAI = &GetAI_mob_toc_druid;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_shaman";
    newscript->GetAI = &GetAI_mob_toc_shaman;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_paladin";
    newscript->GetAI = &GetAI_mob_toc_paladin;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_priest";
    newscript->GetAI = &GetAI_mob_toc_priest;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_shadow_priest";
    newscript->GetAI = &GetAI_mob_toc_shadow_priest;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_mage";
    newscript->GetAI = &GetAI_mob_toc_mage;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_warlock";
    newscript->GetAI = &GetAI_mob_toc_warlock;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_hunter";
    newscript->GetAI = &GetAI_mob_toc_hunter;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_boomkin";
    newscript->GetAI = &GetAI_mob_toc_boomkin;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_warrior";
    newscript->GetAI = &GetAI_mob_toc_warrior;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_dk";
    newscript->GetAI = &GetAI_mob_toc_dk;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_rogue";
    newscript->GetAI = &GetAI_mob_toc_rogue;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_enh_shaman";
    newscript->GetAI = &GetAI_mob_toc_enh_shaman;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_retro_paladin";
    newscript->GetAI = &GetAI_mob_toc_retro_paladin;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_pet_warlock";
    newscript->GetAI = &GetAI_mob_toc_pet_warlock;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_pet_hunter";
    newscript->GetAI = &GetAI_mob_toc_pet_hunter;
    newscript->RegisterSelf();
}
