/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
* This program is free software licensed under GPL version 2
* Please see the included DOCS/LICENSE.TXT for more information */

#ifndef SC_INSTANCE_H
#define SC_INSTANCE_H

#include "InstanceData.h"
#include "Map.h"

enum EncounterState
{
    NOT_STARTED   = 0,
    IN_PROGRESS   = 1,
    FAIL          = 2,
    DONE          = 3,
    SPECIAL       = 4
};

enum EncounterFrameType
{
    ENCOUNTER_FRAME_UNK0                    = 0,
    ENCOUNTER_FRAME_UNK1                    = 1,
    ENCOUNTER_FRAME_ENGAGE                  = 2,
    ENCOUNTER_FRAME_DISENGAGE               = 3,
    ENCOUNTER_FRAME_UPDATE_PRIORITY         = 4,
    ENCOUNTER_FRAME_ADD_TIMER               = 5,
    ENCOUNTER_FRAME_ENABLE_OBJECTIVE        = 6,
    ENCOUNTER_FRAME_UPDATE_OBJECTIVE        = 7,
    ENCOUNTER_FRAME_DISABLE_OBJECTIVE       = 8,
    ENCOUNTER_FRAME_SORT_ENCOUNTER_LIST     = 9,    // Seems to have something to do with sorting the encounter units
    ENCOUNTER_FRAME_UNK7                    = 10,
    ENCOUNTER_UNK10                         = 11
};

struct Position2
{
    float x;
    float y;
    float z;
    float o;

    float GetPositionX() const { return x; }
    float GetPositionY() const { return y; }
    float GetPositionZ() const { return z; }
    float GetOrientation() const { return o; }
};

#define OUT_SAVE_INST_DATA             debug_log("SD2: Saving Instance Data for Instance %s (Map %d, Instance Id %d)", instance->GetMapName(), instance->GetId(), instance->GetInstanceId())
#define OUT_SAVE_INST_DATA_COMPLETE    debug_log("SD2: Saving Instance Data for Instance %s (Map %d, Instance Id %d) completed.", instance->GetMapName(), instance->GetId(), instance->GetInstanceId())
#define OUT_LOAD_INST_DATA(a)          debug_log("SD2: Loading Instance Data for Instance %s (Map %d, Instance Id %d). Input is '%s'", instance->GetMapName(), instance->GetId(), instance->GetInstanceId(), a)
#define OUT_LOAD_INST_DATA_COMPLETE    debug_log("SD2: Instance Data Load for Instance %s (Map %d, Instance Id: %d) is complete.", instance->GetMapName(), instance->GetId(), instance->GetInstanceId())
#define OUT_LOAD_INST_DATA_FAIL        error_log("SD2: Unable to load Instance Data for Instance %s (Map %d, Instance Id: %d).", instance->GetMapName(), instance->GetId(), instance->GetInstanceId())

class MANGOS_DLL_DECL ScriptedInstance : public InstanceData
{
    public:
        ScriptedInstance(Map* pMap) : InstanceData(pMap) {}
        ~ScriptedInstance() {}

        // Default accessor functions
        GameObject* GetSingleGameObjectFromStorage(uint32 uiEntry);
        Creature* GetSingleCreatureFromStorage(uint32 uiEntry, bool bSkipDebugLog = false);

        // Change active state of doors or buttons
        void DoUseDoorOrButton(ObjectGuid guid, uint32 uiWithRestoreTime = 0, bool bUseAlternativeState = false);
        void DoUseDoorOrButton(uint32 uiEntry, uint32 uiWithRestoreTime = 0, bool bUseAlternativeState = false);

        // Respawns a GO having negative spawntimesecs in gameobject-table
        void DoRespawnGameObject(ObjectGuid guid, uint32 uiTimeToDespawn = MINUTE);
        void DoRespawnGameObject(uint32 uiEntry, uint32 uiTimeToDespawn = MINUTE);

        // Sends world state update to all players in instance
        void DoUpdateWorldState(uint32 uiStateId, uint32 uiStateData);

        //sends completed achievments to all players in instance
        void DoCompleteAchievement(uint32 uiAchievmentId);
        void UpdateAchievementCriteria(uint32 type, uint32 miscvalue1 = 0, uint32 miscvalue2 = 0, Unit *unit = NULL, uint32 time = 0);

        //sends achievment criteria update to all players in instance
        void DoUpdateAchievementCriteria(AchievementCriteriaTypes type, uint32 miscvalue1 = 0, uint32 miscvalue2 = 0);

        // Get a Player from map
        Player* GetPlayerInMap(bool bOnlyAlive = false, bool bCanBeGamemaster = true);

        // Get group leader
        Player* GetGroupLeader();

        // destroys an item from all players in this instance (encounters like Vashj, Najentus....) 
        void DestroyItemFromAllPlayers(uint32 uiItemId);

        /// Wrapper for simulating map-wide text in this instance. It is expected that the Creature is stored in m_mNpcEntryGuidStore if loaded.
        void DoOrSimulateScriptTextForThisInstance(int32 iTextEntry, uint32 uiCreatureEntry)
        {
            // Prevent debug output in GetSingleCreatureFromStorage
            DoOrSimulateScriptTextForMap(iTextEntry, uiCreatureEntry, instance, GetSingleCreatureFromStorage(uiCreatureEntry, true));
        }

        void DoCastSpellOnPlayers(uint32 spellId, int32* bp0 = NULL, int32* bp1 = NULL, int32* bp2 = NULL);
        void DoRemoveAurasDueToSpellOnPlayers(uint32 spellId);
        void DoSetAlternativePowerOnPlayers(int32 amt);

        // Starts a timed achievement criteria for all players in instance
        void DoStartTimedAchievement(AchievementCriteriaTypes criteriaType, uint32 uiTimedCriteriaMiscId);

        virtual bool IsEncounterAvailable(uint32 encounter)
        {
            debug_log("ScriptedInstance::IsEncounterAvailable called, but is unsupported for map %u", instance->GetId());
            return true;
        }

        virtual std::string GetEncounterName(uint32 encounter)
        {
            std::stringstream enc;
            enc << "Type " << encounter;
            return enc.str();
        }

        void SendGMEncounterNotification(Unit* who, uint32 encounter);

        virtual bool SetBossState(uint32 id, EncounterState state) { return true; }
        void SendEncounterUnit(uint32 type, Unit* unit = NULL, uint8 param1 = 0, uint8 param2 = 0) { }

    protected:
        // Storage for GO-Guids and NPC-Guids
        EntryGuidMap m_mGoEntryGuidStore;                   ///< Store unique GO-Guids by entry
        EntryGuidMap m_mNpcEntryGuidStore;                  ///< Store unique NPC-Guids by entry

        time_t lastGMEncounterAnnounce;
};

// Class for world maps (May need additional zone-wide functions later on)
class MANGOS_DLL_DECL ScriptedMap : public ScriptedInstance
{
    public:
        ScriptedMap(Map* pMap) : ScriptedInstance(pMap) {}
};

/// A static const array of this structure must be handled to DialogueHelper
struct DialogueEntry
{
    int32 iTextEntry;                                       ///< To be said text entry
    uint32 uiSayerEntry;                                    ///< Entry of the mob who should say
    uint32 uiTimer;                                         ///< Time delay until next text of array is said (0 stops)
};

/// A static const array of this structure must be handled to DialogueHelper
struct DialogueEntryTwoSide
{
    int32 iTextEntry;                                       ///< To be said text entry (first side)
    uint32 uiSayerEntry;                                    ///< Entry of the mob who should say (first side)
    int32 iTextEntryAlt;                                    ///< To be said text entry (second side)
    uint32 uiSayerEntryAlt;                                 ///< Entry of the mob who should say (second side)
    uint32 uiTimer;                                         ///< Time delay until next text of array is said (0 stops)
};

/// Helper class handling a dialogue given as static const array of DialogueEntry or DialogueEntryTwoSide
class DialogueHelper
{
    public:
        // The array MUST be terminated by {0,0,0}
        DialogueHelper(DialogueEntry const* pDialogueArray);
        // The array MUST be terminated by {0,0,0,0,0}
        DialogueHelper(DialogueEntryTwoSide const* aDialogueTwoSide);

        /// Function to initialize the dialogue helper for instances. If not used with instances, GetSpeakerByEntry MUST be overwritten to obtain the speakers
        void InitializeDialogueHelper(ScriptedInstance* pInstance, bool bCanSimulateText = false) { m_pInstance = pInstance; m_bCanSimulate = bCanSimulateText; }
        /// Set if take first entries or second entries
        void SetDialogueSide(bool bIsFirstSide) { m_bIsFirstSide = bIsFirstSide; }

        void StartNextDialogueText(int32 iTextEntry);

        void DialogueUpdate(uint32 uiDiff);

    protected:
        /// Will be called when a dialogue step was done
        virtual void JustDidDialogueStep(int32 iEntry) {}
        /// Will be called to get a speaker, MUST be implemented if not used in instances
        virtual Creature* GetSpeakerByEntry(uint32 uiEntry) { return NULL; }

    private:
        void DoNextDialogueStep();

        ScriptedInstance* m_pInstance;

        DialogueEntry const* m_pDialogueArray;
        DialogueEntry const* m_pCurrentEntry;
        DialogueEntryTwoSide const* m_pDialogueTwoSideArray;
        DialogueEntryTwoSide const* m_pCurrentEntryTwoSide;

        uint32 m_uiTimer;
        bool m_bIsFirstSide;
        bool m_bCanSimulate;
};

#endif
