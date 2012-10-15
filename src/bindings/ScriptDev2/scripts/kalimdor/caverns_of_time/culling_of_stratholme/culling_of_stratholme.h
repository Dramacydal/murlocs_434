/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
 * Copyright (C) 2011 - 2012 MangosR2 <http://github.com/mangosR2/>
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
SDName: instance_culling_of_stratholme
SD%Complete: %
SDComment:
EndScriptData */

#ifndef DEF_CULLING_OF_STRATHOLME_H
#define DEF_CULLING_OF_STRATHOLME_H

enum Data
{
    TYPE_QUEST           = 1,
    TYPE_INTRO           = 2,
    TYPE_CRATES_COUNT    = 3,
    TYPE_PHASE           = 4,
    TYPE_ENCOUNTER       = 5,
    TYPE_WAVE_COUNT      = 6,
    TYPE_WING            = 7,
    TYPE_BONUS           = 8,
    TYPE_MALGANIS        = 9,

    DATA_TEMPSUMMON      = 10,

///-> First quest stuff for CoS
    QUEST_DISPELLING_ILLUSIONS  = 13149,

    NPC_CRATE_KC_BUNNY          = 30996,
    NPC_CHROMI01                = 26527,
    NPC_MIKE                    = 30571,
    NPC_MAL_CORICS              = 31017,
    NPC_GRIAN_STONE             = 30561,
    NPC_JAMES                   = 30553,
    NPC_FRAS_FRASIABI           = 30552,
    NPC_FORRESTER               = 30551,
    NPC_ROGER                   = 27903,
    NPC_MORIGAN                 = 27877,
    NPC_PERELLI                 = 27876,
    NPC_JENA                    = 27885,
    NPC_MARTHA                  = 27884,
    NPC_MALCOLM                 = 27891,
    NPC_DOG                     = 27892,
    NPC_BARTLEBY                = 27907,

    SPELL_ARCANE_DISRUPTION     = 49590,
    SPELL_CRATES_KILL_CREDIT    = 58109,

    ITEM_ARCANE_DISRUPTOR       = 37888,
    GO_CRATE_LIGHT              = 190117,

    WORLD_STATE_COS_CRATE_ON    = 3479,
    WORLD_STATE_COS_CRATE_COUNT = 3480,
///-> end

///-> Second Quest stuff
    QUEST_A_ROYAL_ESCORT    = 13151,

    NPC_MARINE_1            = 70000,
    NPC_MARINE_2            = 70001,
    NPC_MARINE_3            = 70002,
    NPC_MARINE_4            = 70003,
    NPC_PRIEST_1            = 70004,
    NPC_PRIEST_2            = 70005,
    NPC_ARTHAS              = 26499,
    NPC_JAINA               = 26497,
    NPC_UTHER               = 26528,
    NPC_KNIGHT              = 28612,
    NPC_CHROMI02            = 27915,
    NPC_CITY                = 28167,
    NPC_CRAZYMAN            = 28169,
    NPC_INVIS_STALKER       = 20562,
    WORLD_STATE_COS_WAVE_COUNT    = 3504,

    MALGANIS_KC_BUNNY      = 31006,

///-> end

    NPC_CHROMI03            = 30997,

    NPC_INFINITE_ADVERSARY  = 27742,
    NPC_INFINITE_HUNTER     = 27743,
    NPC_INFINITE_AGENT      = 27744,
    NPC_TIME_RIFT           = 28409,
    NPC_TIME_RIFT_2         = 28439,
    NPC_ZOMBIE              = 27737,
    NPC_GHOUL               = 28249,
    NPC_NECROMANCER         = 28200,
    NPC_STALKER             = 28199,  ///->??? might be typo for invis_stalker
    NPC_FIEND               = 27734,
    NPC_GOLEM               = 28201,
    NPC_EGHOUL              = 27729,
    NPC_CONSTRUCT           = 27736,
    NPC_ACOLYTE             = 27731,
    NPC_MEATHOOK            = 26529,
    NPC_SALRAMM             = 26530,
    NPC_EPOCH               = 26532,
    NPC_MALGANIS            = 26533,
    NPC_INFINITE_CORRUPTOR  = 32273,

    GO_SHKAF_GATE          = 188686,
    GO_MALGANIS_GATE1      = 187711,
    GO_MALGANIS_GATE2      = 187723,
    GO_MALGANIS_CHEST      = 190663,
    GO_MALGANIS_CHEST_H    = 193597,
    GO_EXIT                = 191788,


    WORLD_STATE_COS_TIME_ON       = 3932,
    WORLD_STATE_COS_TIME_COUNT    = 3931,

    NPC_MAGISTRATE_BARTHILAS        = 30994,
    NPC_STEPHANIE_SINDREE           = 31019,
    NPC_LEEKA_TURNER                = 31027,
    NPC_SOPHIE_AAREN                = 31021,
    NPC_ROBERT_PIERCE               = 31025,
    NPC_GEORGE_GOODMAN              = 31022,

    NPC_EMERY_NEILL                 = 30570,
    NPC_EDWARD_ORRICK               = 31018,
    NPC_OLIVIA_ZENITH               = 31020,

    RIGHT               = 1,
    LEFT                = 2
};

#endif
