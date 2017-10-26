#include "musicmodel.h"
#include <QDebug>
#include <QFile>
#include "dialogalbum.h"
#include "dialogartist.h"
MusicModel::MusicModel()
{
    m_root=new Item("ROOT");
}

MusicModel::~MusicModel()
{
 if(m_root)
     delete m_root;
 m_root=0;
}
///
Qt::ItemFlags MusicModel::flags(const QModelIndex &index) const
{

    if (!index.isValid())
        return 0;

    if (index.parent().parent().isValid())
        return  Qt::ItemIsEditable | QAbstractItemModel::flags(index);
    else if (index.column()==0)
        return  Qt::ItemIsEditable | QAbstractItemModel::flags(index);
    return QAbstractItemModel::flags(index);
}


QModelIndex MusicModel::index(int row, int column, const QModelIndex &parent) const
{
  Item* parentItem=m_root;
  if (parent.isValid())
  {
      parentItem=static_cast<Item*>(parent.internalPointer());
  }
  qDebug()<<row;
  qDebug()<<column;
  if (  parentItem->m_children.value(row))
  return createIndex(row, column, parentItem->ChildAt(row));
  else
      return QModelIndex();

}
///

QModelIndex MusicModel::parent(const QModelIndex &child) const
{
    Item* childItem=0;
    if(child.isValid())
        childItem=static_cast<Item*>(child.internalPointer());
    else
        return QModelIndex();
    if(!childItem)  return QModelIndex();
    Item* parentItem=childItem->parent();
    if (parentItem)
    {
        Item* grandItem=parentItem->parent();
        if(grandItem)
            return createIndex(grandItem->indexOf(parentItem),child.column(),parentItem);
        else
            return createIndex(m_root->indexOf(parentItem),child.column(),m_root);
    }
    else return QModelIndex();
}

int MusicModel::rowCount(const QModelIndex &parent) const
{
    Item* parentItem=m_root;
    if(parent.isValid())
        parentItem=static_cast<Item*>(parent.internalPointer());
    return parentItem->childCount();
}
bool MusicModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;
    bool result =true;
    Item *item = static_cast<Item*>(index.internalPointer());
    if (index.column()==0)
    {
        item->m_name=value.toString();
        emit dataChanged(index, index);
    }
    else if (index.column()==1)
    {
        QString str=value.toString();
        static_cast<Song*>(item)->m_duration=QTime::fromString(str);
        emit dataChanged(index, index);
    }
    else if (index.column()==2)
    {
        static_cast<Song*>(item)->m_rating=value.toInt();
        qDebug()<<value.toInt();
        emit dataChanged(index, index);
    }
    else if (index.column()==3)
    {
        QString comment=value.toString();
        static_cast<Song*>(item)->m_comment=comment;

        qDebug()<<value.toString();
        emit dataChanged(index, index);
    }
    return result;
}

bool MusicModel::insertRows(int position, int rows, const QModelIndex &parent)
{

    bool success=true;
    if (!parent.parent().isValid()&&parent.isValid())
    {
        Artist *parentItem = static_cast<Artist*>(parent.internalPointer());
        beginInsertRows(parent, position, position + rows - 1);
        Album *item = new Album("newAlbum",parentItem);
        parentItem->insertChild(item,position);
        endInsertRows();

        dialogAlbum *addDeviceDialog = new dialogAlbum(0,item);
        addDeviceDialog->show();
        emit dataChanged(createIndex(parentItem->indexOf(item),0),createIndex(parentItem->indexOf(item),0));

    }
    else if (parent.parent().isValid())
    {
        Album *parentItem = static_cast<Album*>(parent.internalPointer());
        beginInsertRows(parent, position, position + rows - 1);
        Song *item = new Song("newSong",parentItem);
        parentItem->insertChild(item,position);
        endInsertRows();
    }
    if(!parent.isValid())
    {

        Item *parentItem = m_root;

        beginInsertRows(parent, position, position + rows-1);
        Artist *item = new Artist("newArtist",m_root);

        parentItem->insertChild(item,position);


        endInsertRows();

        DialogArtist *addDeviceDialog = new DialogArtist(0,item);
        addDeviceDialog->show();

        emit dataChanged(createIndex(parentItem->indexOf(item),0),createIndex(parentItem->indexOf(item),0));
    }

    return success;
}

bool MusicModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    bool success=true;
    if (!parent.isValid())
    {
        Artist *parentItem = static_cast<Artist*>(parent.internalPointer());
        beginRemoveRows(parent, position, position + rows - 1);
        success = parentItem->removeChildren(position, rows);
        endRemoveRows();
    }
    else if (parent.isValid()&&(!parent.parent().isValid()))
    {
        Album *parentItem = static_cast<Album*>(parent.internalPointer());
        beginRemoveRows(parent, position, position + rows - 1);
        success = parentItem->removeChildren(position, rows);
        endRemoveRows();
    }
    else{
        Song *parentItem = static_cast<Song*>(parent.internalPointer());
        beginRemoveRows(parent, position, position + rows - 1);
        success = parentItem->removeChildren(position, rows);
        endRemoveRows();
    }
    return success;
}


int MusicModel::columnCount(const QModelIndex &parent) const
{
    return 4;
}

QVariant MusicModel::data(const QModelIndex &index, int role) const
{
    Item* item=m_root;
    if(index.isValid()){
        item=static_cast<Item*>(index.internalPointer());
    }
    else return QVariant();
    switch (role) {
    case Qt::DisplayRole:
        switch (index.column()){
        case 0:
            return item->m_name;
        case 1:
            if (!index.parent().isValid())
                return "";
            else
                if (!index.parent().parent().isValid())
                    return "";
            return static_cast<Song*>(item)->m_duration;
        case 2:
            if (!index.parent().isValid())
                return "";
            else
                if (!index.parent().parent().isValid())
                    return "";
            return 2;
        case 3:
            if (!index.parent().isValid())
                return "";
            else
                if (!index.parent().parent().isValid())
                    return "";
            return static_cast<Song*>(item)->m_comment;
        default:
            break;
        }
        break;
    default:
        break;
    }
    return QVariant();
}

QVariant MusicModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation==Qt::Horizontal && role==Qt::DisplayRole)
    {

        switch(section){
        case 0: return QObject::tr("Трек");
        case 1: return QObject::tr("Продолжительность");
        case 2: return QObject::tr("Рейтинг");
        case 3: return QObject::tr("Заметки");
        default: break;
        }
    }
    return QVariant();
}
