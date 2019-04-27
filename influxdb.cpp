#include "influxdb.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QProcessEnvironment>

InfluxDB& InfluxDB::sGetInstance()
{
    static InfluxDB db;
    return db;
}


InfluxDB::InfluxDB() :
    mDBAdress("localhost"),
    mDbPort(8086)
{
    readConfigFile();
    updateDatabaseList();
}


void InfluxDB::setAdressAndPort(QString aAdress, int aPort)
{
    mDBAdress = aAdress;
    mDbPort = aPort;

    updateDatabaseList();
}


void InfluxDB::setAdress(QString aAdress)
{
    mDBAdress = aAdress;
}


void InfluxDB::setPort(int aPort)
{
    mDbPort = aPort;
}

/*
 * $ curl -XPOST 'http://localhost:8086/query' --data-urlencode 'q=CREATE DATABASE "mydb"'

{"results":[{"statement_id":0}]}
*/
void InfluxDB::createDb(QString aDbName)
{
    QString url = QString("http://%1:%2/query").arg(mDBAdress).arg(QString::number(mDbPort));

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QByteArray postData;
    postData.append(QString("q=CREATE DATABASE \"%1\"").arg(aDbName));

    mNetworkAcessManager.post(request, postData);
}

/*
 * $ curl -i -XPOST "http://localhost:8086/write?db=mydb&precision=s" --data-binary 'mymeas,mytag=1 myfield=90 1463683075'
 * */
void InfluxDB::insert(QString aQuery)
{
    QString url = QString("http://%1:%2/write?db=%3&precision=s").arg(mDBAdress).arg(QString::number(mDbPort)).arg(mDbName);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QByteArray postData;
    postData.append(aQuery);

    mNetworkAcessManager.post(request, postData);

}

/**
 * @brief InfluxDB::insert
 * @param aTableName
 * @param aTuppleList a QString with data in format "a0=val0 a1=val1 a2=val2" and so on.
 */
void InfluxDB::insert(QString aTableName, QString aTuppleList)
{
    QString query = QString("%1,%2").arg(aTableName).arg(aTuppleList);
    insert(query);
}

void InfluxDB::useDb(QString aDbName)
{
    mDbName = aDbName;
}


QStringList InfluxDB::getDatabases()
{
    return mDatabases;
}


// curl -G "http://somehost:8086/query?pretty=true" --data-urlencode "q=show databases"
void InfluxDB::updateDatabaseList()
{
    QString url = QString("http://%1:%2/query?pretty=true").arg(mDBAdress).arg(mDbPort);
    QString query("q=show databases");

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QByteArray postDate;
    postDate.append(query);
    if(mReply)
    {
        mReply->deleteLater();
        mReply = nullptr;
    }

    mReply = mNetworkAcessManager.post(request, postDate);
    connect(mReply, &QNetworkReply::readyRead, this, &InfluxDB::updateDataBaseNameListSlot);
    if(mReply->waitForReadyRead(30000))
    {
        QByteArray data = mReply->readAll();
        //TODO: get data..
    }
}

void InfluxDB::updateDataBaseNameListSlot()
{
    QByteArray data = mReply->readAll();

    QJsonDocument json = QJsonDocument::fromJson(data);
    QJsonObject jsonObj = json.object();

    QJsonArray result = jsonObj["results"].toArray();
    QJsonObject seriesObj = result[0].toObject();
    QJsonArray series = seriesObj["series"].toArray();
    QJsonObject val = series[0].toObject();
    QJsonArray databases = val["values"].toArray();

    mDatabases.clear();
    for(int i=0; i<databases.size(); ++i)
    {
        QJsonArray db = databases[i].toArray();
        QString val = db[0].toString();
        mDatabases.push_back(val);
    }
    mReply->deleteLater();
    mReply = nullptr;
}


void InfluxDB::readConfigFile()
{
    QString path = QProcessEnvironment::systemEnvironment().value("JUNE_ROOT");
    path.append("/config/influxdb.conf");
    QFile file(path);
    if(!file.exists())
    {
        qDebug() << __FUNCTION__ << "Config file does not exist, " << path;
        return;
    }
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << __FUNCTION__ << "Error opening config file, " << path;
        return;
    }

    while (!file.atEnd()) {
        QString line(file.readLine());
        if(line.startsWith("#")) // skip comments
            continue;

        // prosess line for config
        if(line.startsWith("dblog"))
        {
            QStringList p = line.split(" ");
            mDbLogPath = p[1];
        }
    }


    file.close();
}
