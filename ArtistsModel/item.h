#ifndef ITEM_H
#define ITEM_H
#include<QList>
#include<QString>
#include<QDebug>
#include<QPixmap>
#include<QTime>
class Item
{
public:
    Item();
    Item(QString name,Item* parent=0);
    ~Item();
    Item* parent() const;
    void setParent(Item* p);//вставить в нужное место дерева
    void insertChild(Item* p, int pos=-1);//вставить потомка в нужное место (трек в альбом)
    Item* takeChild(int pos);//на случай удаления элемента из дерева
    Item* ChildAt(int pos) const;//вернет дочерний элемент по заданному индексу
    int indexOf(Item* pd);
    int childCount();//вернёт количество детей
    bool insertChildren(int position, int count);
     bool removeChildren(int position, int count);

    Item *m_parent;//указатель на родителя
    QList<Item*> m_children;
    QString m_name;
    QString m_comment;


};
class Album: public Item
{
public:
    Album():Item(){};
    Album (QString name, Item *parent=0);
    int m_year=0;
    QPixmap m_cover;
    QString m_genre="";
    QString m_comment="";
};
class Artist: public Item
{
public:
    Artist():Item(){};
    Artist(QString name, Item *parent=0);
    QString m_country="";
    QPixmap m_photo;
    QString m_comment="";
};
class Song: public Item
{
public:
    Song():Item(){};
    Song(QString name, Item *parent=0);
    QTime m_duration;
    int m_rating;
    QString m_comment="";
   /* int m_year;
    QPixmap m_cover;
    QString m_genre*/;
};
QDataStream &operator<<(QDataStream &, const Artist& );
QDataStream &operator>>(QDataStream &, Artist &);
QDataStream &operator>>(QDataStream &, Album &);
QDataStream &operator>>(QDataStream &, Song &);



#endif // ITEM_H
