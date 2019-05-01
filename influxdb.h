#ifndef INFLUXDB_H
#define INFLUXDB_H

#include <QString>
#include <QStringList>
#include <QNetworkAccessManager>

class QNetworkReply;

class InfluxDB : public QObject
{
    Q_OBJECT
public:
    static InfluxDB& sGetInstance();

    void setAdressAndPort(QString aAdress, int aPort);
    void setAdress(QString aAdress);
    void setPort(int aPort);

    void createDb(QString aDbName);
    void useDb(QString aDbName);
    void insert(QString aQuery);
    void insert(QString aTableName, QString aTuppleList);

    QStringList getDatabases();

private:
    InfluxDB();

    void logDbQuery(QString aQuery);
    void readConfigFile();

    void updateDatabaseList(); ///< update the available databases at endpoint.
private slots:
    void updateDataBaseNameListSlot();

private:
    QString mDBAdress;
    int mDbPort;

    QString mDbName; // the db current in use.

    QNetworkAccessManager mNetworkAcessManager;

    QNetworkReply *mReply;
    QStringList mDatabases;
    QString mDbLogPath;
};

#endif // INFLUXDB_H
