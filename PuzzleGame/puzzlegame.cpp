#include "puzzlegame.h"
#include <vector>
#include "puzzlepiece.h"
#include "QTime"
#include <QSet>
#include <QGraphicsView>
PuzzleGame::PuzzleGame(QObject *parent,int col_num,int row_num) :
    QGraphicsScene(parent)
{
    column_numb=col_num;
    row_numb=row_num;
}
PuzzlePiece::ConnectorPosition PuzzleGame::reverse(PuzzlePiece::ConnectorPosition pos)
{
    switch(pos)
    {
       case::PuzzlePiece::Non : return PuzzlePiece::Non;
       case::PuzzlePiece::In : return PuzzlePiece::Out;
       case::PuzzlePiece::Out : return PuzzlePiece::In;
    }
    return PuzzlePiece::Non;
}
void PuzzleGame::setup()
{

    QSize size1(column_numb*100,row_numb*100);
    QPixmap buf_pm=image1.scaled(size1);
    qsrand(QTime::currentTime().msec());
    PuzzlePiece::ConnectorPosition storedWest;
    QVector <PuzzlePiece::ConnectorPosition> prev(column_numb);
    for (int i=0;i<row_numb;i++)
    {
        storedWest = PuzzlePiece::Non;
        for(int j=0;j<column_numb;j++)
        {
            int buf_x = qrand() % column_numb*100;
            int buf_y = qrand() % row_numb*100;
            PuzzlePiece::ConnectorPosition curr[4];

                curr[0]=reverse(prev[j]);
                curr[1]=qrand() %2 ? PuzzlePiece::In : PuzzlePiece::Out;
                curr[3]=qrand() %2 ? PuzzlePiece::In : PuzzlePiece::Out;
                curr[2]=reverse(storedWest);
                if (j==column_numb-1)
                    curr[3]=PuzzlePiece::Non;
                if (i==row_numb-1)
                    curr[1]=PuzzlePiece::Non;
                PuzzlePiece *piece = new PuzzlePiece(curr[0],curr[1],curr[2],curr[3],column_numb*row_numb);
                scene.addItem(piece);
                piece->setFlag(QGraphicsItem::ItemIsMovable);
                piece->setFlag(QGraphicsItem::ItemSendsGeometryChanges);
                piece->setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
                piece->setPos(j*100,i*100);
                QRect rect=piece->boundingRect().toRect();
                const int cellSize=100;
                rect.translate(cellSize/2+j*cellSize,cellSize/2+i*cellSize);
                QPixmap px = buf_pm.copy(rect);
                piece->m_image=px;
                QPoint point_buf(j,i);

                piece->setCoordinates(point_buf);

                piece->setPos(buf_x,buf_y);

                storedWest=curr[3];
                prev[j]=curr[1];
         }
      }
}
