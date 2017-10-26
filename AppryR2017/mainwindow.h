#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qcustomplot.h>
#include <QPair>
#include <cfloat>
#include <interitem.h>
#include <dialog.h>

typedef struct Interval
{
    QCPItemLine* leftLine;
    QCPItemLine* rightLine;
    QCPItemLine* apprLine;
    QPair<double,double> xLineInt;

    double freeM = nan("");
    double koefAppr = nan("");
    double RMember = nan("");
    double convergVal = nan("");

    QString name="";
} Interval;

typedef struct Graphs
{
    QList<double> CoordX;
    QList<double> CoordXStat;

    QList<double> CoordY;
    QPair<QString,QString> AxesNs;
    QPair<double,double> MaxMinY;

    QCPGraph* customGr;
    QList<Interval*> intervals;
    QList<Interval*> intervalsStat;
    int currentInt = 0;

    QPair<double,double>* DiscrRanges;
    QString Headers;
    QCPTextElement* header;
    QCPTextElement* HeaderDiscr;

    bool discrepancyPlotShown = false;

    bool DiscrConstructed = false;
    QVector<double> CoordYDiscr;
    QCustomPlot* mainPlot;
} Graphs;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    int index = 0;
    bool redrawAppr = false;
    int curNumberGraphs=0;
    QPair<double, double> mostLR;
    QPair<double, double> mostLRStat;

    QStringList files;
    QTableWidget *values;
    QTableWidget *valuesStat;
    double logK = 0.5;

    QList<Graphs*> userGraphs;
    QListWidget options;

    int *lineIsMoving;
    QPair<bool,bool> lineIsFixed;

    int *makeInterval;

    bool grMade=0;

    bool onPlot=false;

    QPair<double, double> apprLin(QList<double> nodesX, QList<double> nodesY, QPair<double,double>* interval = 0);
    double getLine(QPair<double, double> coef, double x);

    double convValEv(double k, double b, double length);    //Расчет лог. критерия
    void edgeFix(bool side); //фиксация прямых при выходе за границы интервала (0-left, 1-right)
    void setLineInterval(double xCoord, int currentInterval);    //Установка прямых интервала на заданную координату x
    bool Stat=false;    //(не)стационарное время
private slots:
    void slotLoad();
    void slotConstr();
    void slotMove(QMouseEvent *e);
    void slotPressInterval(QMouseEvent *e);
    void apprLineBuild();
    void intervalBuild();
    void redrawLines(int col, int row);

    void LineDelete(Interval* interval, bool numberOfLine);
    void deleteRowTable();
    void deleteInterval();

    void cellIsSelected(int row, int column);
    void redrawApprLine();
    void slotTabChanged();

    void TableRebuild(int curTab);
    void slotPlotChanged();
    void discrPlotConstr(int);



    void discrPlotShow(int, bool);
    void discrMainShow(int, bool);

    void on_commandLinkButton_clicked();
    void renameInt(int,int);
    void deleteGraph();
    void deleteData();

    void setKoef(); //Задание параметра лог. критерия

    void setSt(); //Задание (не)стационарного времени
};

#endif // MAINWINDOW_H
