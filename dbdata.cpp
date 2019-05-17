#include "dbdata.h"

DbData::DbData()
{

}


void DbData::setDataBaseName(QString aDbName)
{
    mDbName = aDbName;
}


void DbData::setServerIp(QString aIp)
{
    mServerIp = aIp;
}


void DbData::setServerPort(QString aPort)
{
    mServerPort = aPort;
}
