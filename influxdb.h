#ifndef INFLUXDB_H
#define INFLUXDB_H

/*This file is part of June.

June is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Foobar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.  If not, see <https://www.gnu.org/licenses/>.*/

#include <QString>
#include <QStringList>
#include <QNetworkAccessManager>
#include <QFile>
#include <QMap>
#include <QTimer>

class QNetworkReply;

/**
 * @brief The InfluxDB class
 *
 * Log values to an influxDB, and keep the logdata local in logfiles
 * if the db is unavailable. The local logged data is sent when the db
 * is available.
 * Data pressision is set to one second
 *
 *
 * Logfile is changed every hour
 */
class InfluxDB : public QObject
{
    Q_OBJECT
public:
    static InfluxDB& sGetInstance();
    enum Pressision
    {
        eNanoSecond,
        eMicroSecond,
        eMiliSecond,
        eSecond,
        eMinut,
        eHour
    };

    void setAdressAndPort(QString aAdress, int aPort);
    void setAdress(QString aAdress);
    void setPort(int aPort);

    void createDb(QString aDbName);
    void useDb(QString aDbName);
    void insert(QString aQuery, Pressision aPressision=eSecond);
    void insert(QString aTableName, QString aTuppleList);
    void insert(QString aTableName, QString aTuppleList, qint64 aTimestamp, Pressision aPression);

    QStringList getDatabases();

private:
    InfluxDB();
    QString pressisionToString(Pressision aPressision) const;
    void logDbQuery(QString aQuery);
    void readConfigFile();

    void updateDatabaseList(); ///< update the available databases at endpoint.
    void openLogFile();

private slots:
    void updateDataBaseNameListSlot();
    void onReplyFinnished(QNetworkReply *aReply);
    void onHourChange(); ///< called everytime the hour is changed.
private:
    QString mDBAdress;
    int mDbPort;

    QString mDbName; // the db current in use.

    QNetworkAccessManager mNetworkAcessManager;

    QNetworkReply *mReply;
    QStringList mDatabases;
    QString mDbLogPath;

    QFile mLogFile;
    QString mLogFileName;

    QMap<QNetworkReply*, QString> mReplies;
};

#endif // INFLUXDB_H
