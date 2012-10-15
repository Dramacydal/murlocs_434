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

#ifndef DEF_TOC_H
#define DEF_TOC_H

enum
{
    MAX_ENCOUNTER               = 3,

    TYPE_GRAND_CHAMPIONS        = 0,
    TYPE_ARGENT_CHALLENGE       = 1,
    TYPE_BLACK_KNIGHT           = 2,

    DATA_CHAMPION_1             = 3,
    DATA_CHAMPION_2             = 4,
    DATA_CHAMPION_3             = 5,
    DATA_BLACK_KNIGHT           = 6,
    DATA_BLACK_KNIGHT_MINION    = 7,
    DATA_TOC5_ANNOUNCER         = 8,
    DATA_JAEREN                 = 9,
    DATA_ARELAS                 = 10,
    DATA_CHAMPIONID_1           = 11,
    DATA_CHAMPIONID_2           = 12,
    DATA_CHAMPIONID_3           = 13,
    DATA_MEMORY                 = 14,
    DATA_ARGENT_CHALLENGER      = 15,
    DATA_CHAMPIONS_COUNT        = 16,

    NPC_JACOB                   = 34705,
    NPC_AMBROSE                 = 34702,
    NPC_COLOSOS                 = 34701,
    NPC_JAELYNE                 = 34657,
    NPC_LANA                    = 34703,
    NPC_MOKRA                   = 35572,
    NPC_ERESSEA                 = 35569,
    NPC_RUNOK                   = 35571,
    NPC_ZULTORE                 = 35570,
    NPC_VISCERI                 = 35617,
    NPC_EADRIC                  = 35119,
    NPC_PALETRESS               = 34928,
    NPC_BLACK_KNIGHT            = 35451,
    NPC_RISEN_JAEREN            = 35545,
    NPC_RISEN_ARELAS            = 35564,
    NPC_JAEREN                  = 35004,

    NPC_ARELAS                  = 35005,
    MEMORY_ALGALON              = 35052,
    MEMORY_ARCHIMONDE           = 35041,
    MEMORY_CHROMAGGUS           = 35033,
    MEMORY_CYANIGOSA            = 35046,
    MEMORY_DELRISSA             = 35043,
    MEMORY_ECK                  = 35047,
    MEMORY_ENTROPIUS            = 35044,
    MEMORY_GRUUL                = 35039,
    MEMORY_HAKKAR               = 35034,
    MEMORY_HEIGAN               = 35049,
    MEMORY_HEROD                = 35030,
    MEMORY_HOGGER               = 34942,
    MEMORY_IGNIS                = 35050,
    MEMORY_ILLIDAN              = 35042,
    MEMORY_INGVAR               = 35045,
    MEMORY_KALITHRESH           = 35037,
    MEMORY_LUCIFRON             = 35031,
    MEMORY_MALCHEZAAR           = 35038,
    MEMORY_MUTANUS              = 35029,
    MEMORY_ONYXIA               = 35048,
    MEMORY_THUNDERAAN           = 35032,
    MEMORY_VANCLEEF             = 35028,
    MEMORY_VASHJ                = 35040,
    MEMORY_VEKNILASH            = 35036,
    MEMORY_VEZAX                = 35051,

    GO_CHAMPIONS_LOOT           = 195709,
    GO_CHAMPIONS_LOOT_H         = 195710,
    GO_EADRIC_LOOT              = 195374,
    GO_EADRIC_LOOT_H            = 195375,
    GO_PALETRESS_LOOT           = 195323,
    GO_PALETRESS_LOOT_H         = 195324,

    SPELL_CREDIT_GRAND_CHAMP    = 68572,
    SPELL_CREDIT_PALETRESS      = 68574,
    SPELL_CREDIT_EADRIC         = 68575,
    SPELL_CREDIT_BLACK_KNIGHT   = 68663,

    DESPAWN_TIME                = 300000
};

#endif
