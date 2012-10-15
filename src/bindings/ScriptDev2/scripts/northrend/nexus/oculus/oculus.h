/* Copyright (C) 2010 - 2011 by /dev/rsa for ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

#ifndef DEF_OCULUS_H
#define DEF_OCULUS_H
#include "BSW_ai.h"
#include "BSW_instance.h"

enum
{
    TYPE_DRAKOS,
    TYPE_VAROS,
    TYPE_UROM,
    TYPE_EREGOS,
    TYPE_ROBOTS,
    TYPE_UROM_PHASE,
    MAX_ENCOUNTERS,

    DATA_DRAKOS,
    DATA_VAROS,
    DATA_UROM,
    DATA_EREGOS,

    NPC_ROBOT              = 27641,
    NPC_BALGAR_IMAGE       = 28012,
    NPC_VERDISA            = 27657,
    NPC_BELGARISTRASZ      = 27658,
    NPC_ETERNOS            = 27659,
    NPC_DRAKOS             = 27654,
    NPC_VAROS              = 27447,
    NPC_UROM               = 27655,
    NPC_EREGOS             = 27656,

    GO_DRAGON_CAGE_DOOR    = 193995,
    GO_EREGOS_CACHE        = 191349,
    GO_EREGOS_CACHE_H      = 193603,
    GO_SPOTLIGHT           = 191351,
    GO_ORB_OF_NEXUS        = 188715,

    BELGAR_TEXT_0          = 13267,
    BELGAR_TEXT_1          = 13268,
    BELGAR_TEXT_2          = 13269,

    // Yells after Drakos dies
    SAY_VAROS_INTRO                 = -1578020,
    SAY_BELGARISTRASZ_GREET         = -1578021,

    // world states to show how many constructs are still alive
    WORLD_STATE_CONSTRUCTS          = 3524,
    WORLD_STATE_CONSTRUCTS_COUNT    = 3486,

    ACHIEV_START_EREGOS_ID          = 18153,            // eregos timed kill achiev

};

#endif
