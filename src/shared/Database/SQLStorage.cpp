/*
 * Copyright (C) 2005-2012 MaNGOS <http://getmangos.com/>
 *
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

#include "SQLStorage.h"

void SQLStorage::EraseEntry(uint32 id)
{
    uint32 offset = 0;
    for (uint32 x = 0; x < oNumFields; ++x)
    {
        switch(dst_format[x])
        {
            case FT_LOGIC:
                offset += sizeof(bool);   break;
            case FT_BYTE:
                offset += sizeof(char);   break;
            case FT_INT:
                offset += sizeof(uint32); break;
            case FT_FLOAT:
                offset += sizeof(float);  break;
            case FT_STRING:
            {
                if(pIndex[id])
                    delete[] *(char**)((char*)(pIndex[id])+offset);

                offset += sizeof(char*);
                break;
            }
            case FT_NA:
                offset += sizeof(uint32); break;
            case FT_NA_BYTE:
                offset += sizeof(char);   break;
            case FT_NA_FLOAT:
                offset += sizeof(float);  break;
            case FT_NA_POINTER:
                offset += sizeof(char*);  break;
            case FT_IND:
            case FT_SORT:
                assert(false && "SQL storage not have sort field types");
                break;
            default:
                assert(false && "unknown format character");
                break;
        }
    }

    pIndex[id] = NULL;
}

void SQLStorage::Free ()
{
    uint32 offset = 0;
    for (uint32 x = 0; x < oNumFields; ++x)
    {
        switch(dst_format[x])
        {
            case FT_LOGIC:
                offset += sizeof(bool);   break;
            case FT_BYTE:
                offset += sizeof(char);   break;
            case FT_INT:
                offset += sizeof(uint32); break;
            case FT_FLOAT:
                offset += sizeof(float);  break;
            case FT_STRING:
            {
                for(uint32 y = 0; y < MaxEntry; ++y)
                    if(pIndex[y])
                        delete [] *(char**)((char*)(pIndex[y])+offset);

                offset += sizeof(char*);
                break;
            }
            case FT_NA:
                offset += sizeof(uint32); break;
            case FT_NA_BYTE:
                offset += sizeof(char);   break;
            case FT_NA_FLOAT:
                offset += sizeof(float);  break;
            case FT_NA_POINTER:
                offset += sizeof(char*);  break;
            case FT_IND:
            case FT_SORT:
                assert(false && "SQL storage not have sort field types");
                break;
            default:
                assert(false && "unknown format character");
                break;
        }
    }

    delete[] pIndex;
    delete[] data;
}

uint32 SQLStorage::GetTotalSize()
{
    uint32 offset=0, line_size = 0;

    // Циклы нужны в принципе лишь для того, чтобы вычленить индекс строки по её смещению в массиве индексов. В отличие от других типов данных, строка сама представляет собой массив.
    // Поэтому в массиве индексов хранится не сама строка (видимо, слишком жирно) а лишь указатель на неё, сама строка отдельно. 
    for(uint32 x=0;x<iNumFields;x++)
        if (dst_format[x]==FT_STRING)
        {
            // Цикл пробегает по всем строкам таблицы. В данном случае я подсчитываю размер строк, находящихся в колонке по нужному смещению от первой
            for(uint32 y=0;y<MaxEntry;y++)
                if(pIndex[y] && *(char**)(pIndex[y]+offset))
                    // Итак, pIndex - массив данных по индексам. Находим нужный указатель на массив символов (на одну из строк), он имеет тип (char**)
                    // И обращаемся не к самому указателю, а к переменной, находящейся по адресу этого указателя (!!!), то есть *(char**), то есть непосредственно к массиву символов!
                    // Дальше всё просто - считаем его размер ;) Вуаля!
                    line_size += strlen( *(char**)(pIndex[y]+offset)) * sizeof(char);

            offset += sizeof(char*); // здесь считаем размер указателей на строки
                                     // Указатель на массив символов, то есть (char**) (размер её, как и у всех указателей - 4 байта в х32 и 8 байт в х64)
        }
        else if (dst_format[x]==FT_LOGIC)
            offset += sizeof(bool);
        else if (dst_format[x]==FT_BYTE)
            offset += sizeof(char);
        else
            offset += 4;

    // считаем размер = число переменных (включая указатели на строки) * число_строк  + сумма размеров строк
    return (offset * RecordCount + line_size);
}

void SQLStorage::Load()
{
    SQLStorageLoader loader;
    loader.Load(*this);
}
