#ifndef TABLE_H
#define TABLE_H

#include <QObject>
#include <QString>
#include <QVector>

class TagSocket;

class Table : public QObject
{
    Q_OBJECT
public:
    explicit Table(QObject *parent = nullptr);

    void setTableName(const QString &aName);
    void addTagSocketName(const QString &aName);

    QString toString() const; ///< Return the table formated for influxdb.

private slots:
    void onTagCreated();

private:
    QString mTableName;
    QVector<TagSocket*> mTagSockets;
    QVector<QString> mTagNames; ///< tag names to hookup
};

#endif // TABLE_H
