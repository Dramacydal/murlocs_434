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

#ifndef DO_POSTGRESQL

#include "DatabaseEnv.h"
#include "Errors.h"
#ifdef SQL_STAT_COLLECT 
	#include "Timer.h"
#endif

#ifdef SQL_STAT_COLLECT 
QueryResultMysql::QueryResultMysql(MYSQL_RES *result, MYSQL_FIELD *fields, uint64 rowCount, uint32 fieldCount, void* caller) :
    QueryResult(rowCount, fieldCount, caller), mResult(result)
#else
QueryResultMysql::QueryResultMysql(MYSQL_RES *result, MYSQL_FIELD *fields, uint64 rowCount, uint32 fieldCount) :
    QueryResult(rowCount, fieldCount), mResult(result)
#endif
{

    mCurrentRow = new Field[mFieldCount];
    MANGOS_ASSERT(mCurrentRow);

    for (uint32 i = 0; i < mFieldCount; ++i)
        mCurrentRow[i].SetType(ConvertNativeType(fields[i].type));
}

QueryResultMysql::~QueryResultMysql()
{
    EndQuery();
}

bool QueryResultMysql::NextRow()
{
    MYSQL_ROW row;

    if (!mResult)
        return false;

#ifdef SQL_STAT_COLLECT 
    uint32 stime = getMSTime();
#endif

    row = mysql_fetch_row(mResult);

    if (!row)
    {
        EndQuery();
        return false;
    }

    for (uint32 i = 0; i < mFieldCount; ++i)
        mCurrentRow[i].SetValue(row[i]);

#ifdef SQL_STAT_COLLECT
    fetchTimer += getMSTimeDiff(stime, getMSTime());
    ++fetchCount;
#endif

    return true;
}

void QueryResultMysql::EndQuery()
{
    delete [] mCurrentRow;
    mCurrentRow = 0;

    if (mResult)
    {
        mysql_free_result(mResult);
        mResult = 0;
    }

#ifdef SQL_STAT_COLLECT
    if(srcDb) static_cast<Database*>(srcDb)->DumpQueryEnd(getMSTimeDiff(lifetime, getMSTime()), fetchTimer, fetchCount); 
#endif
}

enum Field::DataTypes QueryResultMysql::ConvertNativeType(enum_field_types mysqlType) const
{
    switch (mysqlType)
    {
        case FIELD_TYPE_TIMESTAMP:
        case FIELD_TYPE_DATE:
        case FIELD_TYPE_TIME:
        case FIELD_TYPE_DATETIME:
        case FIELD_TYPE_YEAR:
        case FIELD_TYPE_STRING:
        case FIELD_TYPE_VAR_STRING:
        case FIELD_TYPE_BLOB:
        case FIELD_TYPE_SET:
        case FIELD_TYPE_NULL:
            return Field::DB_TYPE_STRING;
        case FIELD_TYPE_TINY:
        case FIELD_TYPE_SHORT:
        case FIELD_TYPE_LONG:
        case FIELD_TYPE_INT24:
        case FIELD_TYPE_LONGLONG:
        case FIELD_TYPE_ENUM:
            return Field::DB_TYPE_INTEGER;
        case FIELD_TYPE_DECIMAL:
        case FIELD_TYPE_FLOAT:
        case FIELD_TYPE_DOUBLE:
            return Field::DB_TYPE_FLOAT;
        default:
            return Field::DB_TYPE_UNKNOWN;
    }
}
#endif
