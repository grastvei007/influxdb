#include "table.h"

#include <tagsystem/taglist.h>
#include <tagsystem/tagsocket.h>


Table::Table(QObject *parent) : QObject(parent)
{
    connect(&TagList::sGetInstance(), &TagList::tagCreated, this, &Table::onTagCreated);
}


void Table::setTableName(const QString &aName)
{
    mTableName = aName;
}


void Table::addTagSocketName(const QString &aName)
{
    Tag *tag = TagList::sGetInstance().findByTagName(aName);
    if(!tag)
    {
        mTagNames.push_back(aName);
        return;
    }
    switch(tag->getType())
    {
        case Tag::eInt:
        {
            TagSocket *socket = TagSocket::createTagSocket("log", tag->getName(), TagSocket::eInt);
            socket->hookupTag(tag);
            mTagSockets.push_back(socket);
            break;
        }
        case Tag::eDouble:
        {
            TagSocket *socket = TagSocket::createTagSocket("log", tag->getName(), TagSocket::eDouble);
            socket->hookupTag(tag);
            mTagSockets.push_back(socket);
            break;
        }
        case Tag::eBool:
        case Tag::eString:
            break;
    }
}


QString Table::toString() const
{

}


void Table::onTagCreated()
{
    if(mTagNames.isEmpty())
        return;

    for(auto iter = mTagNames.begin(); iter != mTagNames.end(); ++iter)
    {

    }
}
