#ifndef DBDATA_H
#define DBDATA_H

#include <QObject>

#include <QString>
/**
 * @brief The DbData class holds information about the database, and logging to it.
 *
 */
class DbData
{
public:
    DbData();

    void setDataBaseName(QString aDbName);
    void setServerIp(QString aIp);
    void setServerPort(QString aPort);

private:
    QString mDbName;
    QString mServerIp;
    QString mServerPort;
};

#endif // DBDATA_H
