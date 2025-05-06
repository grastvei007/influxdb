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
    networkRequestFactory_(QUrl("http://localhost:8086"))
{
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
    return {};
}


void InfluxDB::setAdressAndPort(QString adress, int port)
{
    networkRequestFactory_.setBaseUrl(QString("http://%1:%2").arg(adress, QString::number(port)));
}


void InfluxDB::setAdress(QString adress)
{
    dbAdress_ = adress;
    setAdressAndPort(dbAdress_, dbPort_);
}


void InfluxDB::setPort(int port)
{
    dbPort_ = port;
    setAdressAndPort(dbAdress_, dbPort_);
}

void InfluxDB::setApiToken(const QByteArray &token)
{
    networkRequestFactory_.setBearerToken(token);
}

/*
 * $ curl -XPOST 'http://localhost:8086/query' --data-urlencode 'q=CREATE DATABASE "june"'

{"results":[{"statement_id":0}]}
*/
void InfluxDB::createDb(QString aDbName)
{
    QNetworkRequest request = networkRequestFactory_.createRequest("query");
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
    QNetworkRequest request = networkRequestFactory_.createRequest(QString("write?db=%1&precision=%1")
            .arg(mDbName, pressisionToString(aPressision)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply *reply = networkAcessManager_.post(request, aQuery.toLatin1());
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

    QString query = QString("%1 %2 %3").arg(aTableName, aTuppleList, QString::number(timestamp));
    insert(query);
}

void InfluxDB::insert(QString aTableName, QString aTuppleList, qint64 aTimestamp, InfluxDB::Pressision aPression)
{
    QString query = QString("%1 %2 %3").arg(aTableName, aTuppleList, QString::number(aTimestamp));
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

bool InfluxDB::isServerSideError(QNetworkReply::NetworkError error)
{
    return error == QNetworkReply::InternalServerError
            || error == QNetworkReply::OperationNotImplementedError
            || error == QNetworkReply::ServiceUnavailableError
            || error == QNetworkReply::UnknownServerError;
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
