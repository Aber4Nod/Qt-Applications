#include "item.h"

Item::Item()
{

    qDebug()<<QString("create item");
    m_parent=0;
    m_name=QString("unknown");
    m_comment=QString("unknown");
}

Item::Item(QString name, Item *parent)
{
    qDebug()<<QString("create item")<<name;
    m_parent=parent;
    m_name=name;
    m_comment=QString("");
}

Item::~Item()
{
    qDebug()<<QString("delete item")<<m_name;
    setParent(0);
    foreach (Item* child,m_children)
    {
        if (child) delete child;
    }
}

Item *Item::parent() const
{
    return m_parent;
}

void Item::setParent(Item *p)
{
    if (m_parent==p)
        return;
    if (p)
    {
        qDebug()<<QString("set parent")<<p->m_name<<" to item "<<m_name;
        if(m_parent)//если уже есть родитель
        {
            m_parent->takeChild(m_parent->indexOf(p));
        }
        m_parent=p;
        m_parent->insertChild(this);
    }
}

void Item::insertChild(Item *p, int pos)
{
    qDebug()<<QString("inset child item")<<p->m_name<<" to item "<<m_name<<" at pos "<<pos;
    if (!p) return;
    if(m_children.contains(p))
        p->setParent(this);//на всякий случай
    else
    {
        if(pos<0||pos>m_children.size())
            m_children.insert(m_children.size(),p);//если неправильно, вставляем в конец списка
        else
            m_children.insert(pos,p);
        p->setParent(this);
    }

}

Item *Item::takeChild(int pos)
{
    Item* p=m_children.takeAt(pos);
    if (p)
        p->setParent(0);
    qDebug()<<QString("take child at")<<p->m_name<<" to item "<<m_name<<" at pos "<<pos;

    return p;
}

Item *Item::ChildAt(int pos) const
{
    if(pos<0||pos>m_children.size())
        return 0;
    Item* p=m_children.at(pos);
    qDebug()<<QString("child at")<<pos;

    return p;
}

int Item::indexOf(Item *p)
{
    return m_children.indexOf(p);
}

int Item::childCount()
{
    return m_children.size();
}

bool Item::insertChildren(int position, int count)
{

    if (position < 0 || position > m_children.size())
        return false;

    for (int row = 0; row < count; ++row) {
        QString data;
        Item *item = new Item(data,this);
        m_children.insert(position, item);
    }

    return true;
}

bool Item::removeChildren(int position, int count)
{
    if (position < 0 || position + count > m_children.size())
        return false;

    for (int row = 0; row < count; ++row)
        delete m_children.takeAt(position);

    return true;
}



Album::Album(QString name, Item *parent): Item(name)
{
    setParent(parent);;

}

Artist::Artist(QString name, Item *parent): Item(name)
{
    setParent(parent);
}

Song::Song(QString name, Item *parent): Item(name)
{
    setParent(parent);
    m_rating=0;
    m_duration.setHMS(0,0,0);
}

QDataStream &operator<<(QDataStream &stream, const Artist& artist)
{
    // stream<<"Artist";
    stream<<artist.m_name<<artist.m_country<<artist.m_comment<<artist.m_photo;
    int cnt = artist.m_children.size();
    stream<<cnt;
    for(int i=0; i<cnt; ++i)
    {
        Album *album=static_cast<Album*>(artist.ChildAt(i));
        if (album)
        {
            stream<<album->m_name<<album->m_year<<album->m_cover<<album->m_genre<<album->m_comment;
            int cnt_alb = album->m_children.size();
            stream<<cnt_alb;
            for(int j=0; j<cnt_alb; ++j)
            {
                Song *song = static_cast<Song*>(album->ChildAt(j));
                if (song)
                {
                    stream<<song->m_name<<song->m_duration<<song->m_rating<<song->m_comment;
                }
            }
        }
    }
    return stream;
}

QDataStream &operator>>(QDataStream & stream, Artist& artist)
{
    stream>>artist.m_name>>artist.m_country>>artist.m_comment>>artist.m_photo;
    qDebug()<<artist.m_name;

    return stream;
}
QDataStream &operator>>(QDataStream & stream, Album & album)
{
    stream>>album.m_name>>album.m_year>>album.m_cover>>album.m_genre>>album.m_comment;

    return stream;
}
QDataStream &operator>>(QDataStream & stream, Song & song)
{
    stream>>song.m_name>>song.m_duration>>song.m_rating>>song.m_comment;

    return stream;
}
