#include "influxdb.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QProcessEnvironment>
#include <QDateTime>
#include <QTextStream>


InfluxDB::InfluxDB(QNetworkAccessManager &networkAccessManager) :
    networkAcessManager_(networkAccessManager),
    mDBAdress("localhost"),
    mDbPort(8086)
{
    readConfigFile();
    openLogFile();
    updateDatabaseList();
}

QString InfluxDB::pressisionToString(Pressision aPressision) const
{
    switch (aPressision)
    {
        case eNanoSecond:
            return "ns";
        case eMicroSecond:
            return "u";
        case eMiliSecond:
            return "ms";
        case eSecond:
            return "s";
        case eMinut:
            return "m";
        case eHour:
            return "h";
    }
    return QString();
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
 * $ curl -XPOST 'http://localhost:8086/query' --data-urlencode 'q=CREATE DATABASE "june"'

{"results":[{"statement_id":0}]}
*/
void InfluxDB::createDb(QString aDbName)
{
    QString url = QString("http://%1:%2/query").arg(mDBAdress).arg(QString::number(mDbPort));

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QByteArray postData;
    postData.append(QString("q=CREATE DATABASE \"%1\"").arg(aDbName).toLatin1());

    QNetworkReply *reply = networkAcessManager_.post(request, postData);
    Q_UNUSED(reply);
}

/*
 * $ curl -i -XPOST "http://localhost:8086/write?db=june&precision=s" --data-binary 'bmv,v=12.3 1463683075'
 * */
void InfluxDB::insert(QString aQuery, Pressision aPressision)
{
    QString url = QString("http://%1:%2/write?db=%3&precision=%4")
            .arg(mDBAdress)
            .arg(QString::number(mDbPort))
            .arg(mDbName)
            .arg(pressisionToString(aPressision));

    QNetworkReply *reply = networkAcessManager_.post(QNetworkRequest(url), aQuery.toLatin1());
    connect(reply, &QNetworkReply::finished, this, &InfluxDB::onReplyFinnished);
}

/**
 * @brief InfluxDB::insert
 * @param aTableName
 * @param aTuppleList a QString with data in format "a0=val0 a1=val1 a2=val2" and so on.
 */
void InfluxDB::insert(QString aTableName, QString aTuppleList)
{
     qint64 timestamp = QDateTime::currentSecsSinceEpoch();

    QString query = QString("%1 %2 %3").arg(aTableName).arg(aTuppleList).arg(timestamp);
    logDbQuery(query);
    insert(query);
}

void InfluxDB::insert(QString aTableName, QString aTuppleList, qint64 aTimestamp, InfluxDB::Pressision aPression)
{
    QString query = QString("%1 %2 %3").arg(aTableName).arg(aTuppleList).arg(aTimestamp);
    logDbQuery(query);
    insert(query, aPression);
}

void InfluxDB::useDb(QString aDbName)
{
    mDbName = aDbName;
}


QStringList InfluxDB::getDatabases()
{
    return mDatabases;
}


// curl -G "http://localhost:8086/query?pretty=true" --data-urlencode "q=show databases"
void InfluxDB::updateDatabaseList()
{
    QString url = QString("http://%1:%2/query?pretty=true").arg(mDBAdress).arg(mDbPort);
    QString query("q=show databases");

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QByteArray postDate;
    postDate.append(query.toLatin1());
    if(mReply)
    {
        mReply->deleteLater();
        mReply = nullptr;
    }

    mReply = networkAcessManager_.post(request, postDate);
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



void InfluxDB::logDbQuery(QString aQuery)
{
    if(!mLogFile.isOpen())
        return;

    QTextStream stream(&mLogFile);
    stream << aQuery << "\n";
}


void InfluxDB::readConfigFile()
{
    QString path = QProcessEnvironment::systemEnvironment().value("JUNE_ROOT");
    path.append("/influxdb/influxdb.conf");
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


void InfluxDB::openLogFile()
{
    int msHour = 1000*3600;

    QTime time = QTime::currentTime();
    int ms = time.msecsSinceStartOfDay();
    while(ms > msHour)
        ms -= msHour;
    ms = msHour - ms;
    qDebug() << "ms: " << ms;
    QTimer::singleShot(ms, this, &InfluxDB::onHourChange); // call next hour change.

    QString path = QProcessEnvironment::systemEnvironment().value("JUNE_ROOT");
    path.append("/log/");
    QString file = QDateTime::currentDateTime().toString("dd.MM.yyyy-hh.mm.ss");
    file.append(".log");
    path.append(file);

    mLogFileName = path;
    mLogFile.setFileName(mLogFileName);
    if(!mLogFile.open(QIODevice::WriteOnly))
    {
        qDebug() << "Error opening file, " << mLogFileName;
        return;
    }
    qDebug() << "open logifile, " << mLogFileName;
}

bool InfluxDB::isServerSideError(QNetworkReply::NetworkError error)
{
    return error == QNetworkReply::InternalServerError
            || error == QNetworkReply::OperationNotImplementedError
            || error == QNetworkReply::ServiceUnavailableError
            || error == QNetworkReply::UnknownServerError;
}


void InfluxDB::onHourChange()
{
    if(mLogFile.isOpen())
        mLogFile.close();
    openLogFile();
}

void InfluxDB::onReplyFinnished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(!reply)
    {
        qDebug() << "Unknown QNetworkReply";
        return;
    }

    if(isServerSideError(reply->error()))
    {
        qDebug() << reply->errorString();
    }
    else if(reply->error() != 0)
    {
        qDebug() << reply->errorString();
        // is there a log file, read it put in queue, delete file.
    }

    reply->deleteLater();
}
