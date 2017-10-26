#ifndef PUZZLEGAME_H
#define PUZZLEGAME_H

#include <QGraphicsScene>
#include <puzzlepiece.h>
class PuzzleGame : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit PuzzleGame(QObject *parent = 0, int col_num=2, int row_num=2);
    void setup();
    PuzzleGame(int col,int row);
    PuzzlePiece::ConnectorPosition reverse(PuzzlePiece::ConnectorPosition pos);
    QGraphicsScene scene;
    QGraphicsView * view;
 QPixmap image1;

private:
     PuzzlePiece* pp;

    int column_numb,row_numb;
signals:

public slots:

};

#endif // PUZZLEGAME_H
