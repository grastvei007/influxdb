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
#include <QNetworkReply>
#include <QFile>
#include <QMap>
#include <QTimer>
#include <QVector>
#include <QNetworkRequestFactory>


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
    InfluxDB(QNetworkAccessManager &networkAccessManager);
    enum Pressision
    {
        eNanoSecond,
        eMicroSecond,
        eMiliSecond,
        eSecond,
        eMinut,
        eHour
    };

    void setAdressAndPort(const QString &adress, int port, const QString &base = {});
    void setAdress(QString adress);
    void setPort(int port);
    void setBasePath(const QString &base);
    void setApiToken(const QByteArray &token);
    // sends data to endpoint at first request to post data
    // after the wait time has expired.
    void setBulkUpdateMs(int ms);

    void createDb(QString aDbName);
    void useDb(QString aDbName);
    void insert(QString aTableName, QString aTuppleList);
    void insert(QString aTableName, QString aTuppleList, qint64 aTimestamp, Pressision aPression);

    void getBuckets(const QString &bucket);

    QStringList getDatabases();
    QString baseUrl() const;

signals:
    void bucketsReceived();

private:
    void insert(QString aQuery, Pressision aPressision=eSecond);
    QString pressisionToString(Pressision aPressision) const;

    bool isServerSideError(QNetworkReply::NetworkError error);

private slots:
    void updateDataBaseNameListSlot();
    void onReplyFinnished();
    void onReplyBucketFinnished();

private:
    QNetworkAccessManager &networkAcessManager_;
    QNetworkRequestFactory networkRequestFactory_;
    QString dbAdress_;
    int dbPort_;
    QString basePath_;

    QString mDbName; // the db current in use.

    QNetworkReply *mReply;
    QStringList mDatabases;
    QString mDbLogPath;
    QString bucket_;
    QString bucketWrite_;

    bool hasAcessToken_ = false;
    bool useBulkUpdate_ = false;
    int bulkUpdateTimeMs_ = 0;
    qint64 lastUpdateMs_ = 0;
    QByteArray requestBuffer_ = {};

};

#endif // INFLUXDB_H
