#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->widgetPanel->removeTab(0);
    values = new QTableWidget();
    valuesStat = new QTableWidget();

    ui->tabWidget->addTab(values,"Нестационарное время");
    ui->tabWidget->addTab(valuesStat,"Стационарное время");

    connect(ui->action,SIGNAL(triggered(bool)),this,SLOT(slotLoad()));
    connect(&options,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(slotListItemClicked(QListWidgetItem*)));
    connect(values,SIGNAL(cellChanged(int,int)),this,SLOT(redrawLines(int,int)));
    connect(values,SIGNAL(cellClicked(int,int)),this,SLOT(cellIsSelected(int,int)));
    connect(values,SIGNAL(cellChanged(int,int)),this,SLOT(renameInt(int,int)));
    connect(valuesStat,SIGNAL(cellChanged(int,int)),this,SLOT(redrawLines(int,int)));
    connect(valuesStat,SIGNAL(cellClicked(int,int)),this,SLOT(cellIsSelected(int,int)));
    connect(valuesStat,SIGNAL(cellChanged(int,int)),this,SLOT(renameInt(int,int)));
    connect(ui->widgetPanel,SIGNAL(currentChanged(int)),this,SLOT(TableRebuild(int)));
    connect(ui->action_3,SIGNAL(triggered(bool)),this,SLOT(intervalBuild()));
    connect(ui->action_4,SIGNAL(triggered(bool)),this,SLOT(deleteRowTable()));
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(slotPlotChanged()));
    connect(ui->pushButton_2,SIGNAL(clicked(bool)),this,SLOT(on_commandLinkButton_clicked()));
    connect(ui->action_5,SIGNAL(triggered(bool)),this,SLOT(deleteGraph()));
    connect(ui->action_6,SIGNAL(triggered(bool)),this,SLOT(deleteData()));
    connect(ui->actionKoef,SIGNAL(triggered(bool)),this,SLOT(setKoef()));
    connect(ui->action_7,SIGNAL(triggered(bool)),this,SLOT(setSt()));

    values->setColumnCount(5);
    valuesStat->setColumnCount(5);

    QStringList l1;
    l1<<"Наименование интервала"<<"Границы по оси абсцисс"<<"Значение коэффициента\nаппроксимации"<<"Значение на правой границе \nинтервала"<<"Значение логарифмического\nкритерия";
    values->setHorizontalHeaderLabels(QStringList(l1));
    valuesStat->setHorizontalHeaderLabels(QStringList(l1));
    QFontMetrics pr_M(QFont("Times",10,QFont::Bold));
    for (int c=0;c<values->columnCount();c++) {
        values->setColumnWidth(c,this->width()/5+30/*pr_M.width(l1.at(c))*/);
        valuesStat->setColumnWidth(c,this->width()/5+30/*pr_M.width(l1.at(c))*/);
    }

    QListWidgetItem* listI = new QListWidgetItem("lines!");
    listI->setFlags(listI->flags() | Qt::ItemIsUserCheckable);
    listI->setCheckState(Qt::Unchecked);
    options.addItem(listI);


    lineIsMoving = new int [1];
    makeInterval = new int [1];

    values->setItemDelegate(new interItem(0,&onPlot, &mostLR));
    valuesStat->setItemDelegate(new interItem(0,&onPlot, &mostLRStat));

    mostLR.first=DBL_MAX;
    mostLR.second=-DBL_MAX;

    mostLRStat.first=DBL_MAX;
    mostLRStat.second=-DBL_MAX;

    this->setWindowTitle("AppryR2017");
}

MainWindow::~MainWindow()
{
    delete ui;
}

QPair<double,double> MainWindow::apprLin(QList<double> nodesX, QList<double> nodesY, QPair<double,double>* interval)
{
    int numb = nodesX.length();
    QList<double> bufLX, bufLY;
    if (interval)
    {
        for (int c=0;c<numb;c++)
        {
            if (nodesX[c]>interval->first&&nodesX[c]<interval->second)
            {
                bufLX.append(nodesX[c]);
                bufLY.append(nodesY[c]);
            }
        }
    }
    numb = bufLX.length();
    double sumx=0.,sumy=0.,sumx2=0.,sumxy=0.;
    for (int c_=0;c_<numb;c_++)
    {
        sumx+=bufLX[c_];
        sumy+=bufLY[c_];
        sumx2+=bufLX[c_]*bufLX[c_];
        sumxy+=bufLX[c_]*bufLY[c_];
    }
    double k1 = (numb*sumxy-sumx*sumy)/(numb*sumx2-sumx*sumx);
    double k2 = (sumy-k1*sumx)/numb;
    return QPair<double,double>(k1,k2);
}

double MainWindow::getLine(QPair<double, double> coef, double x)
{
    return coef.first*x+coef.second;
}

double MainWindow::convValEv(double k, double b, double length)
{
    return length == 1 ? logK*fabs(log(fabs(k))-log(fabs(b))) : logK*fabs(log(fabs(k))-log(fabs(b)))/fabs(log(length));
}

void MainWindow::edgeFix(bool side)
{
    int currentInt = userGraphs[ui->widgetPanel->currentIndex()]->currentInt;
    if (!side) {
        setLineInterval(mostLR.first,currentInt);
    } else {
        setLineInterval(mostLR.second,currentInt);
    }
}

void MainWindow::setLineInterval(double xCoord, int currentInterval)
{
    QString str;
    int i=0, currentInt=currentInterval;
    double valC = xCoord;
    switch (lineIsMoving[0]) {

    case 0: //К этому моменту не установлено ни одной прямой
        for (;i<userGraphs.size();++i) {
            if (!Stat) {
                userGraphs[i]->intervals[currentInt]->xLineInt.first = valC;
                userGraphs[i]->intervals[currentInt]->leftLine->start->setCoords(valC,DBL_MIN);
                userGraphs[i]->intervals[currentInt]->leftLine->end->setCoords(valC,DBL_MAX);
            } else {
                userGraphs[i]->intervalsStat[currentInt]->xLineInt.first = valC;
                userGraphs[i]->intervalsStat[currentInt]->leftLine->start->setCoords(valC,DBL_MIN);
                userGraphs[i]->intervalsStat[currentInt]->leftLine->end->setCoords(valC,DBL_MAX);
            }

        }
        if (!Stat) {
            str = userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->rightLine->start->key() != DBL_MIN ? QString("%1;%2").arg(userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->leftLine->start->key()).arg(userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->rightLine->start->key())
                : QString("%1;").arg(userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->leftLine->start->key());
            values->model()->setData(values->model()->index(currentInt,1),str);
        } else {
            str = userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->rightLine->start->key() != DBL_MIN ? QString("%1;%2").arg(userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->leftLine->start->key()).arg(userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->rightLine->start->key())
                : QString("%1;").arg(userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->leftLine->start->key());
            valuesStat->model()->setData(valuesStat->model()->index(currentInt,1),str);
        }
        qobject_cast<QCustomPlot*>(ui->widgetPanel->currentWidget())->replot();
        break;
    case 1:
        if (lineIsFixed.first!=1)
        {
            for (;i<userGraphs.size();++i) {
                if (!Stat) {
                    userGraphs[i]->intervals[currentInt]->xLineInt.first = valC;
                    userGraphs[i]->intervals[currentInt]->leftLine->start->setCoords(valC,DBL_MIN);
                    userGraphs[i]->intervals[currentInt]->leftLine->end->setCoords(valC,DBL_MAX);
                } else {
                    userGraphs[i]->intervalsStat[currentInt]->xLineInt.first = valC;
                    userGraphs[i]->intervalsStat[currentInt]->leftLine->start->setCoords(valC,DBL_MIN);
                    userGraphs[i]->intervalsStat[currentInt]->leftLine->end->setCoords(valC,DBL_MAX);
                }
            }
            qobject_cast<QCustomPlot*>(ui->widgetPanel->currentWidget())->replot();
            if (!Stat) {
                str = QString("%1;%2").arg(userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->leftLine->start->key()).arg(userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->rightLine->start->key());
                values->model()->setData(values->model()->index(currentInt,1),str);
            } else {
                str = QString("%1;%2").arg(userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->leftLine->start->key()).arg(userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->rightLine->start->key());
                valuesStat->model()->setData(valuesStat->model()->index(currentInt,1),str);
            }
        }
        if (lineIsFixed.second!=1)
        {
            if (Stat) {
                if (valC > userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->xLineInt.first)
                    for (;i<userGraphs.size();++i) {
                        userGraphs[i]->intervals[currentInt]->xLineInt.second = valC;
                    }
                else {
                    for (;i<userGraphs.size();++i) {
                        userGraphs[i]->intervals[currentInt]->xLineInt.first = userGraphs[i]->intervals[currentInt]->xLineInt.second;
                        userGraphs[i]->intervals[currentInt]->xLineInt.first = valC;
                    }
                }
                for (;i<userGraphs.size();++i) {
                    userGraphs[i]->intervals[currentInt]->rightLine->start->setCoords(valC,DBL_MIN);
                    userGraphs[i]->intervals[currentInt]->rightLine->end->setCoords(valC,DBL_MAX);
                }
                str = userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->rightLine->start->key() != DBL_MIN ? QString("%1;%2").arg(userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->leftLine->start->key()).arg(userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->rightLine->start->key())
                    : QString("%1;").arg(userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->leftLine->start->key());
                values->model()->setData(values->model()->index(currentInt,1),str);
            } else {
                if (valC > userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->xLineInt.first)
                    for (;i<userGraphs.size();++i) {
                        userGraphs[i]->intervalsStat[currentInt]->xLineInt.second = valC;
                    }
                else {
                    for (;i<userGraphs.size();++i) {
                        userGraphs[i]->intervalsStat[currentInt]->xLineInt.first = userGraphs[i]->intervalsStat[currentInt]->xLineInt.second;
                        userGraphs[i]->intervalsStat[currentInt]->xLineInt.first = valC;
                    }
                }
                for (;i<userGraphs.size();++i) {
                    userGraphs[i]->intervalsStat[currentInt]->rightLine->start->setCoords(valC,DBL_MIN);
                    userGraphs[i]->intervalsStat[currentInt]->rightLine->end->setCoords(valC,DBL_MAX);
                }
                str = userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->rightLine->start->key() != DBL_MIN ? QString("%1;%2").arg(userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->leftLine->start->key()).arg(userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->rightLine->start->key())
                    : QString("%1;").arg(userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->leftLine->start->key());
                valuesStat->model()->setData(valuesStat->model()->index(currentInt,1),str);
            }
            qobject_cast<QCustomPlot*>(ui->widgetPanel->currentWidget())->replot();
        }
        break;
    default:
        break;

    }
}

void MainWindow::slotLoad()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
                                                          tr("Open File"), QDir::currentPath());
    if (!fileNames.isEmpty()) {
        files.append(fileNames);
        foreach (QString fileName, fileNames) {
            Graphs* newGraph = new Graphs();
            userGraphs.append(newGraph);

            QFile file(fileName);
            file.open(QIODevice::ReadOnly);
            QTextStream in(&file);

            QRegExp rx(QString("%1(.*)%1").arg(QChar(0x022)));
            QString curL = in.readLine();
            int pos = rx.indexIn(curL);
            newGraph->Headers.append(rx.cap(1));

            curL = in.readLine(); //axesN_s r_ing
            rx.setPattern(QString("%1(.*)%1 %1(.*)%1").arg(QChar(0x022)));
            pos = rx.indexIn(curL);
            if (pos > -1)
                newGraph->AxesNs = QPair<QString,QString>(rx.cap(1),rx.cap((2)));
            else
                qDebug()<<"error in line: "<<curL<<" occured > axe_s";

            QList<double> xC, yC;
            double maxY = DBL_MIN, minY = DBL_MAX;
            int counter=1;
            while (!in.atEnd())
            {
                curL = in.readLine();
                rx.setPattern("(\\S+)[ ]+(.+)");
                pos = rx.indexIn(curL);
                if (pos > -1) {
                    xC.append(rx.cap(1).toDouble());
                    newGraph->CoordXStat.append(counter++);
                    yC.append(rx.cap(2).toDouble());
                    if (yC.last()>maxY)
                        maxY = yC.last();
                    if (yC.last()<minY)
                        minY = yC.last();
                }
                else
                    qDebug()<<"error in line: "<<curL<<" occured > coord_s";
            }
            newGraph->MaxMinY=QPair<double,double>(maxY,minY);

            if (xC.first()<mostLR.first)
                mostLR.first=xC.first();
            if (xC.last()>mostLR.second)
                mostLR.second=xC.last();

            if (newGraph->CoordXStat.first()<mostLRStat.first)
                mostLRStat.first=newGraph->CoordXStat.first();
            if (newGraph->CoordXStat.last()>mostLRStat.second)
                mostLRStat.second=newGraph->CoordXStat.last();

            newGraph->CoordX=xC;
            newGraph->CoordY=yC;
            file.close();
        }
        slotConstr();
    }
    else
        return ;
}

void MainWindow::slotConstr()
{
    for(int counter=curNumberGraphs;counter<userGraphs.count();++counter)
    {
        userGraphs[counter]->mainPlot = new QCustomPlot();
        ui->widgetPanel->addTab(userGraphs[counter]->mainPlot,QString("%1").arg(files.at(counter)));
        userGraphs[counter]->customGr = new QCPGraph(userGraphs[counter]->mainPlot->axisRect()->axis(QCPAxis::atBottom),
                                                     userGraphs[counter]->mainPlot->axisRect()->axis(QCPAxis::atLeft));
        userGraphs[counter]->customGr->setData(userGraphs[counter]->CoordX.toVector(), userGraphs[counter]->CoordY.toVector());

        userGraphs[counter]->mainPlot->xAxis->setLabel(userGraphs[counter]->AxesNs.first);
        userGraphs[counter]->mainPlot->yAxis->setLabel(userGraphs[counter]->AxesNs.second);

        userGraphs[counter]->mainPlot->xAxis->setRange(userGraphs[counter]->CoordX.first()-(userGraphs[counter]->CoordX.last()-userGraphs[counter]->CoordX.first())/1000,
                                                       userGraphs[counter]->CoordX.last()+(userGraphs[counter]->CoordX.last()-userGraphs[counter]->CoordX.first())/1000);
        userGraphs[counter]->mainPlot->yAxis->setRange(userGraphs[counter]->MaxMinY.second, userGraphs[counter]->MaxMinY.first);

        userGraphs[counter]->mainPlot->plotLayout()->insertRow(0);
        userGraphs[counter]->header = new QCPTextElement(userGraphs[counter]->mainPlot, userGraphs[counter]->Headers, QFont("Times New Roman", 12, QFont::Bold));

        userGraphs[counter]->mainPlot->plotLayout()->addElement(0, 0, userGraphs[counter]->header);
        userGraphs[counter]->mainPlot->replot();

        grMade=1;
        curNumberGraphs = userGraphs.count();
        connect(userGraphs[counter]->mainPlot,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(slotMove(QMouseEvent*)));
        connect(userGraphs[counter]->mainPlot,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(testSlot(QMouseEvent*)));
        connect(userGraphs[counter]->mainPlot,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(slotPressInterval(QMouseEvent*)));
    }
}

void MainWindow::slotMove(QMouseEvent* e)
{
    if (makeInterval[0]==1) {
        QString str;
        onPlot=1;
        int currentInt = userGraphs[ui->widgetPanel->currentIndex()]->currentInt;
        if (!Stat) {
            if (qobject_cast<QCustomPlot*>(ui->widgetPanel->currentWidget())->xAxis->pixelToCoord(e->pos().x())>mostLR.second) {
                edgeFix(1); //фиксируем правую границу
                return;
            } else if (qobject_cast<QCustomPlot*>(ui->widgetPanel->currentWidget())->xAxis->pixelToCoord(e->pos().x())<mostLR.first) {
                edgeFix(0); //фиксируем правую границу
                return;
            }
        } else {
            if (qobject_cast<QCustomPlot*>(ui->widgetPanel->currentWidget())->xAxis->pixelToCoord(e->pos().x())>mostLRStat.second) {
                edgeFix(1); //фиксируем правую границу
                return;
            } else if (qobject_cast<QCustomPlot*>(ui->widgetPanel->currentWidget())->xAxis->pixelToCoord(e->pos().x())<mostLRStat.first) {
                edgeFix(0); //фиксируем правую границу
                return;
            }
        }
        switch (lineIsMoving[0]) {
        case 0:
            if (!Stat) {
                for (int i=0;i<userGraphs.size();i++) {
                    userGraphs[i]->intervals[currentInt]->leftLine->start->setCoords(qobject_cast<QCustomPlot*>(ui->widgetPanel->currentWidget())->xAxis->pixelToCoord(e->pos().x()),DBL_MIN);
                    userGraphs[i]->intervals[currentInt]->leftLine->end->setCoords(qobject_cast<QCustomPlot*>(ui->widgetPanel->currentWidget())->xAxis->pixelToCoord(e->pos().x()),DBL_MAX);
                }
                str = userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->rightLine->start->key() != DBL_MIN ? QString("%1;%2").arg(userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->leftLine->start->key()).arg(userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->rightLine->start->key())
                    : QString("%1;").arg(userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->leftLine->start->key());
                values->model()->setData(values->model()->index(currentInt,1),str);
            } else {
                for (int i=0;i<userGraphs.size();i++) {
                    userGraphs[i]->intervalsStat[currentInt]->leftLine->start->setCoords(qobject_cast<QCustomPlot*>(ui->widgetPanel->currentWidget())->xAxis->pixelToCoord(e->pos().x()),DBL_MIN);
                    userGraphs[i]->intervalsStat[currentInt]->leftLine->end->setCoords(qobject_cast<QCustomPlot*>(ui->widgetPanel->currentWidget())->xAxis->pixelToCoord(e->pos().x()),DBL_MAX);
                }
                str = userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->rightLine->start->key() != DBL_MIN ? QString("%1;%2").arg(userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->leftLine->start->key()).arg(userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->rightLine->start->key())
                    : QString("%1;").arg(userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->leftLine->start->key());
                valuesStat->model()->setData(valuesStat->model()->index(currentInt,1),str);

            }
            qobject_cast<QCustomPlot*>(ui->widgetPanel->currentWidget())->replot();
            break;
        case 1:
            QCPItemTracer *tracer;
            if (lineIsFixed.second==0) {
                for (int i=0;i<userGraphs.size();i++) {
                    if (!Stat) {
                        userGraphs[i]->intervals[currentInt]->rightLine->start->setCoords(qobject_cast<QCustomPlot*>(ui->widgetPanel->currentWidget())->xAxis->pixelToCoord(e->pos().x()),DBL_MIN);
                        userGraphs[i]->intervals[currentInt]->rightLine->end->setCoords(qobject_cast<QCustomPlot*>(ui->widgetPanel->currentWidget())->xAxis->pixelToCoord(e->pos().x()),DBL_MAX);
                    } else {
                        userGraphs[i]->intervalsStat[currentInt]->rightLine->start->setCoords(qobject_cast<QCustomPlot*>(ui->widgetPanel->currentWidget())->xAxis->pixelToCoord(e->pos().x()),DBL_MIN);
                        userGraphs[i]->intervalsStat[currentInt]->rightLine->end->setCoords(qobject_cast<QCustomPlot*>(ui->widgetPanel->currentWidget())->xAxis->pixelToCoord(e->pos().x()),DBL_MAX);
                    }

                    tracer = new QCPItemTracer(userGraphs[i]->mainPlot);
                    tracer->setGraph(userGraphs[i]->mainPlot->graph(0));

                    if (!Stat) {
                        tracer->setGraphKey(userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->rightLine->end->key());
                    } else {
                        tracer->setGraphKey(userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->rightLine->end->key());
                    }
                    tracer->updatePosition();

                    if (!Stat) {
                        userGraphs[i]->intervals[currentInt]->RMember=tracer->position->value();
                    } else {
                        userGraphs[i]->intervalsStat[currentInt]->RMember=tracer->position->value();
                    }
                    userGraphs[i]->mainPlot->removeItem(tracer);
                }
                if (!Stat) {
                    str = QString("%1;%2").arg(userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->leftLine->start->key()).arg(userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->rightLine->start->key());
                    values->model()->setData(values->model()->index(currentInt,1),str);

                    values->model()->setData(values->model()->index(currentInt,3),QString("%1").arg(userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->RMember));
                } else {
                    str = QString("%1;%2").arg(userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->leftLine->start->key()).arg(userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->rightLine->start->key());
                    valuesStat->model()->setData(valuesStat->model()->index(currentInt,1),str);

                    valuesStat->model()->setData(valuesStat->model()->index(currentInt,3),QString("%1").arg(userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->RMember));
                }
            } else {
                if (!Stat) {
                    for (int i=0;i<userGraphs.size();i++) {
                        userGraphs[i]->intervals[currentInt]->leftLine->start->setCoords(qobject_cast<QCustomPlot*>(ui->widgetPanel->currentWidget())->xAxis->pixelToCoord(e->pos().x()),DBL_MIN);
                        userGraphs[i]->intervals[currentInt]->leftLine->end->setCoords(qobject_cast<QCustomPlot*>(ui->widgetPanel->currentWidget())->xAxis->pixelToCoord(e->pos().x()),DBL_MAX);

                    }
                    str = userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->rightLine->start->key() != DBL_MIN ? QString("%1;%2").arg(userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->leftLine->start->key()).arg(userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->rightLine->start->key())
                        : QString("%1;").arg(userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->leftLine->start->key());
                    values->model()->setData(values->model()->index(currentInt,1),str);
                } else {
                    for (int i=0;i<userGraphs.size();i++) {
                        userGraphs[i]->intervalsStat[currentInt]->leftLine->start->setCoords(qobject_cast<QCustomPlot*>(ui->widgetPanel->currentWidget())->xAxis->pixelToCoord(e->pos().x()),DBL_MIN);
                        userGraphs[i]->intervalsStat[currentInt]->leftLine->end->setCoords(qobject_cast<QCustomPlot*>(ui->widgetPanel->currentWidget())->xAxis->pixelToCoord(e->pos().x()),DBL_MAX);

                    }
                    str = userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->rightLine->start->key() != DBL_MIN ? QString("%1;%2").arg(userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->leftLine->start->key()).arg(userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->rightLine->start->key())
                        : QString("%1;").arg(userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->leftLine->start->key());
                    valuesStat->model()->setData(valuesStat->model()->index(currentInt,1),str);
                }
            }
            qobject_cast<QCustomPlot*>(ui->widgetPanel->currentWidget())->replot();
            break;
        default:
            break;
        }
    }
}

void MainWindow::slotPressInterval(QMouseEvent *e)
{
    onPlot=0;
    double valC =  qobject_cast<QCustomPlot*>(ui->widgetPanel->currentWidget())->xAxis->pixelToCoord(e->pos().x());
    int currentInt = userGraphs[ui->widgetPanel->currentIndex()]->currentInt;
    int i=0;
    switch (lineIsMoving[0]) {

    case 0: //К этому моменту не установлено ни одной прямой
        lineIsMoving[0]++;
        if (!Stat) {
            for (;i<userGraphs.size();++i) {
                userGraphs[i]->intervals[currentInt]->xLineInt.first = valC;
                userGraphs[i]->intervals[currentInt]->leftLine->start->setCoords(valC,DBL_MIN);
                userGraphs[i]->intervals[currentInt]->leftLine->end->setCoords(valC,DBL_MAX);
            }
        } else {
            for (;i<userGraphs.size();++i) {
                userGraphs[i]->intervalsStat[currentInt]->xLineInt.first = valC;
                userGraphs[i]->intervalsStat[currentInt]->leftLine->start->setCoords(valC,DBL_MIN);
                userGraphs[i]->intervalsStat[currentInt]->leftLine->end->setCoords(valC,DBL_MAX);
            }
        }
        lineIsFixed.first=1;
        break;
    case 1:
        if (lineIsFixed.first!=1)
        {
            lineIsMoving[0]++;
            if (!Stat) {
                for (;i<userGraphs.size();++i) {
                    userGraphs[i]->intervals[currentInt]->xLineInt.first = valC;
                    userGraphs[i]->intervals[currentInt]->leftLine->start->setCoords(valC,DBL_MIN);
                    userGraphs[i]->intervals[currentInt]->leftLine->end->setCoords(valC,DBL_MAX);
                }
            } else {
                for (;i<userGraphs.size();++i) {
                    userGraphs[i]->intervalsStat[currentInt]->xLineInt.first = valC;
                    userGraphs[i]->intervalsStat[currentInt]->leftLine->start->setCoords(valC,DBL_MIN);
                    userGraphs[i]->intervalsStat[currentInt]->leftLine->end->setCoords(valC,DBL_MAX);
                }
            }
            lineIsFixed.first=1;
        }
        if (lineIsFixed.second!=1)
        {
            lineIsMoving[0]++;
            if (!Stat) {
                if (valC > userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->xLineInt.first)
                    for (;i<userGraphs.size();++i) {
                        userGraphs[i]->intervals[currentInt]->xLineInt.second = valC;
                    }
                else {
                    for (;i<userGraphs.size();++i) {
                        userGraphs[i]->intervals[currentInt]->xLineInt.first = userGraphs[i]->intervals[currentInt]->xLineInt.second;
                        userGraphs[i]->intervals[currentInt]->xLineInt.first = valC;
                    }
                }
                for (;i<userGraphs.size();++i) {
                    userGraphs[i]->intervals[currentInt]->rightLine->start->setCoords(valC,DBL_MIN);
                    userGraphs[i]->intervals[currentInt]->rightLine->end->setCoords(valC,DBL_MAX);
                }
            } else {
                if (valC > userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->xLineInt.first)
                    for (;i<userGraphs.size();++i) {
                        userGraphs[i]->intervalsStat[currentInt]->xLineInt.second = valC;
                    }
                else {
                    for (;i<userGraphs.size();++i) {
                        userGraphs[i]->intervalsStat[currentInt]->xLineInt.first = userGraphs[i]->intervalsStat[currentInt]->xLineInt.second;
                        userGraphs[i]->intervalsStat[currentInt]->xLineInt.first = valC;
                    }
                }
                for (;i<userGraphs.size();++i) {
                    userGraphs[i]->intervalsStat[currentInt]->rightLine->start->setCoords(valC,DBL_MIN);
                    userGraphs[i]->intervalsStat[currentInt]->rightLine->end->setCoords(valC,DBL_MAX);
                }
            }
            lineIsFixed.second=1;

            makeInterval[0] = 0;
            lineIsMoving[0] = 2;

            apprLineBuild();
        }
        break;
    default:
        break;
    }
}

void MainWindow::apprLineBuild()
{
    int currentInt = userGraphs[ui->widgetPanel->currentIndex()]->currentInt;

    if (!Stat) {
        if (!redrawAppr && userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->apprLine)
            return;
    } else {
        if (!redrawAppr && userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->apprLine)
            return;
    }

    double yCoord1, yCoord2;
    QPair<double,double> lineC;
    bool exist;

    for (int i=0;i<userGraphs.size();i++) {
        exist=true;

        if (redrawAppr) {
            if (!Stat) {
                lineC = apprLin(userGraphs[i]->CoordX,
                                userGraphs[i]->CoordY,&userGraphs[i]->intervals[currentInt]->xLineInt);
                yCoord1 = getLine(lineC,userGraphs[i]->intervals[currentInt]->xLineInt.first);
                yCoord2 = getLine(lineC,userGraphs[i]->intervals[currentInt]->xLineInt.second);

                userGraphs[i]->intervals[currentInt]->apprLine->start->setCoords(userGraphs[i]->intervals[currentInt]->xLineInt.first,yCoord1);
                userGraphs[i]->intervals[currentInt]->apprLine->end->setCoords(userGraphs[i]->intervals[currentInt]->xLineInt.second,yCoord2);
            } else {
                lineC = apprLin(userGraphs[i]->CoordX,
                                userGraphs[i]->CoordY,&userGraphs[i]->intervalsStat[currentInt]->xLineInt);
                yCoord1 = getLine(lineC,userGraphs[i]->intervalsStat[currentInt]->xLineInt.first);
                yCoord2 = getLine(lineC,userGraphs[i]->intervalsStat[currentInt]->xLineInt.second);

                userGraphs[i]->intervalsStat[currentInt]->apprLine->start->setCoords(userGraphs[i]->intervalsStat[currentInt]->xLineInt.first,yCoord1);
                userGraphs[i]->intervalsStat[currentInt]->apprLine->end->setCoords(userGraphs[i]->intervalsStat[currentInt]->xLineInt.second,yCoord2);
            }
            qobject_cast<QCustomPlot*>(ui->widgetPanel->widget(i))->setInteraction(QCP::iSelectPlottables, true);
        } else {
            if (!Stat) {
                lineC = apprLin(userGraphs[i]->CoordX,
                                userGraphs[i]->CoordY,&userGraphs[i]->intervals[currentInt]->xLineInt);

                QCPItemLine* line = new QCPItemLine(qobject_cast<QCustomPlot*>(ui->widgetPanel->widget(i)));
                line->setSelectable(false);
                userGraphs[i]->intervals[currentInt]->apprLine = line;

                yCoord1 = getLine(lineC,userGraphs[i]->intervals[currentInt]->xLineInt.first);
                yCoord2 = getLine(lineC,userGraphs[i]->intervals[currentInt]->xLineInt.second);

                userGraphs[i]->intervals[currentInt]->apprLine->start->setCoords(userGraphs[i]->intervals[currentInt]->xLineInt.first,yCoord1);
                userGraphs[i]->intervals[currentInt]->apprLine->end->setCoords(userGraphs[i]->intervals[currentInt]->xLineInt.second,yCoord2);
            } else {
                lineC = apprLin(userGraphs[i]->CoordXStat,
                                userGraphs[i]->CoordY,&userGraphs[i]->intervalsStat[currentInt]->xLineInt);

                QCPItemLine* line = new QCPItemLine(qobject_cast<QCustomPlot*>(ui->widgetPanel->widget(i)));
                line->setSelectable(false);
                userGraphs[i]->intervalsStat[currentInt]->apprLine = line;

                yCoord1 = getLine(lineC,userGraphs[i]->intervalsStat[currentInt]->xLineInt.first);
                yCoord2 = getLine(lineC,userGraphs[i]->intervalsStat[currentInt]->xLineInt.second);

                userGraphs[i]->intervalsStat[currentInt]->apprLine->start->setCoords(userGraphs[i]->intervalsStat[currentInt]->xLineInt.first,yCoord1);
                userGraphs[i]->intervalsStat[currentInt]->apprLine->end->setCoords(userGraphs[i]->intervalsStat[currentInt]->xLineInt.second,yCoord2);
            }
            qobject_cast<QCustomPlot*>(ui->widgetPanel->widget(i))->setInteraction(QCP::iSelectPlottables, true);
            exist=false;
        }


        if (!Stat) {
            userGraphs[i]->intervals[currentInt]->koefAppr = lineC.first;
            userGraphs[i]->intervals[currentInt]->freeM = lineC.second;
            userGraphs[i]->intervals[currentInt]->convergVal= convValEv(lineC.first,lineC.second,
                                                                        userGraphs[i]->intervals[currentInt]->xLineInt.second - userGraphs[i]->intervals[currentInt]->xLineInt.first);
        } else {
            userGraphs[i]->intervalsStat[currentInt]->koefAppr = lineC.first;
            userGraphs[i]->intervalsStat[currentInt]->freeM = lineC.second;
            userGraphs[i]->intervalsStat[currentInt]->convergVal= convValEv(lineC.first,lineC.second,
                                                                        userGraphs[i]->intervalsStat[currentInt]->xLineInt.second - userGraphs[i]->intervalsStat[currentInt]->xLineInt.first);
        }
    }
    qobject_cast<QCustomPlot*>(ui->widgetPanel->currentWidget())->replot();
    if (!exist) {
        if (!Stat) {
            values->model()->setData(values->model()->index(currentInt,2),lineC.first);
            values->model()->setData(values->model()->index(currentInt,4),QString("%1 (a=%2)").arg(userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->convergVal)
                                                                                              .arg(logK));
        } else {
            valuesStat->model()->setData(valuesStat->model()->index(currentInt,2),lineC.first);
            valuesStat->model()->setData(valuesStat->model()->index(currentInt,4),QString("%1 (a=%2)").arg(userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->convergVal)
                                                                                                      .arg(logK));
        }

    } else {
        if (!Stat) {
            values->model()->setData(values->model()->index(currentInt,2),userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->koefAppr);
            values->model()->setData(values->model()->index(currentInt,4),QString("%1 (a=%2)").arg(userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->convergVal)
                                                                                              .arg(logK));
        } else {
            valuesStat->model()->setData(valuesStat->model()->index(currentInt,2),userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->koefAppr);
            valuesStat->model()->setData(valuesStat->model()->index(currentInt,4),QString("%1 (a=%2)").arg(userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->convergVal)
                                                                                                      .arg(logK));
        }
    }

    if (redrawAppr) {
        redrawAppr = false;
    }
}

void MainWindow::intervalBuild()
{
    for (int i=0;i<userGraphs.size();++i) {
        Interval* newInterval = new Interval();
        if (!Stat) {
            userGraphs[i]->intervals.append(newInterval);
            userGraphs[i]->currentInt = values->rowCount();
        }
        else {
            userGraphs[i]->intervalsStat.append(newInterval);
            userGraphs[i]->currentInt = valuesStat->rowCount();
        }
    }
    int currentInt = userGraphs[ui->widgetPanel->currentIndex()]->currentInt;
    for (int i=0;i<userGraphs.size();++i) {
        QCPItemLine *line1 = new QCPItemLine(qobject_cast<QCustomPlot*>(ui->widgetPanel->widget(i)));
        QCPItemLine *line2 = new QCPItemLine(qobject_cast<QCustomPlot*>(ui->widgetPanel->widget(i)));

        if (!Stat) {
            userGraphs[i]->intervals[currentInt]->leftLine = line1;
            userGraphs[i]->intervals[currentInt]->rightLine = line2;
        } else {
            userGraphs[i]->intervalsStat[currentInt]->leftLine = line1;
            userGraphs[i]->intervalsStat[currentInt]->rightLine = line2;
        }

        line1->start->setCoords(DBL_MIN,DBL_MIN);
        line2->start->setCoords(DBL_MIN,DBL_MIN);

        line1->end->setCoords(DBL_MIN,DBL_MIN);
        line2->end->setCoords(DBL_MIN,DBL_MIN);

        makeInterval[0] = 1;
        lineIsMoving[0] = 0;

        lineIsFixed.first = 0;
        lineIsFixed.second = 0;

        if (!Stat) {
            userGraphs[i]->intervals[currentInt]->xLineInt.first=DBL_MIN;
            userGraphs[i]->intervals[currentInt]->xLineInt.second=DBL_MIN;
            userGraphs[i]->intervals[currentInt]->name = QString("%1 интервал").arg(currentInt+1);
        } else {
            userGraphs[i]->intervalsStat[currentInt]->xLineInt.first=DBL_MIN;
            userGraphs[i]->intervalsStat[currentInt]->xLineInt.second=DBL_MIN;
            userGraphs[i]->intervalsStat[currentInt]->name = QString("%1 интервал").arg(currentInt+1);
        }
    }

    if (!Stat) {
        values->insertRow(currentInt);

        values->model()->setData(values->model()->index(currentInt,0),userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->name);
        values->model()->setData(values->model()->index(currentInt,2),"nan");
        values->model()->setData(values->model()->index(currentInt,3),"nan");
        values->model()->setData(values->model()->index(currentInt,4),"nan");
    } else {
        valuesStat->insertRow(currentInt);
        valuesStat->model()->setData(valuesStat->model()->index(currentInt,0),userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->name);
        valuesStat->model()->setData(valuesStat->model()->index(currentInt,2),"nan");
        valuesStat->model()->setData(valuesStat->model()->index(currentInt,3),"nan");
        valuesStat->model()->setData(valuesStat->model()->index(currentInt,4),"nan");
    }
}

void MainWindow::redrawLines(int col,int row)
{
    if (!onPlot){
        QRegExp rx(QString("([-]?[0-9]*[.]?[0-9]*);([-]?[0-9]*[.]?[0-9]*)"));
        QString curL;
        if (!Stat)
            curL = values->model()->index(col,row).data().toString();
        else
            curL = valuesStat->model()->index(col,row).data().toString();
        int pos = rx.indexIn(curL);
        int currentInt = userGraphs[ui->widgetPanel->currentIndex()]->currentInt;
        if (pos > -1)
        {
            if (rx.cap(1)!="")
            {
                if (rx.cap(1)==".") {
                    if (!Stat)
                        for (int i=0;i<userGraphs.size();++i) {
                            userGraphs[i]->intervals[currentInt]->xLineInt.first=0;
                        }
                    else
                        for (int i=0;i<userGraphs.size();++i) {
                            userGraphs[i]->intervalsStat[currentInt]->xLineInt.first=0;
                        }
                }
                else {
                    if (!Stat)
                        for (int i=0;i<userGraphs.size();++i) {
                            userGraphs[i]->intervals[currentInt]->xLineInt.first=rx.cap(1).toDouble();
                        }
                    else
                        for (int i=0;i<userGraphs.size();++i) {
                            userGraphs[i]->intervalsStat[currentInt]->xLineInt.first=rx.cap(1).toDouble();
                        }
                }
                if (lineIsFixed.first==1)
                {
                    if (!Stat)
                        for (int i=0;i<userGraphs.size();++i) {
                            userGraphs[i]->intervals[currentInt]->leftLine->start->setCoords(userGraphs[i]->intervals[currentInt]->xLineInt.first,DBL_MIN);
                            userGraphs[i]->intervals[currentInt]->leftLine->end->setCoords(userGraphs[i]->intervals[currentInt]->xLineInt.first,DBL_MAX);
                            if (userGraphs[i/*ui->widgetPanel->currentIndex()*/]->intervals[currentInt]->apprLine) {
                                index = i;
                                redrawAppr=true;
                                apprLineBuild();
                            }
                        }
                    else
                        for (int i=0;i<userGraphs.size();++i) {
                            userGraphs[i]->intervalsStat[currentInt]->leftLine->start->setCoords(userGraphs[i]->intervalsStat[currentInt]->xLineInt.first,DBL_MIN);
                            userGraphs[i]->intervalsStat[currentInt]->leftLine->end->setCoords(userGraphs[i]->intervalsStat[currentInt]->xLineInt.first,DBL_MAX);
                            if (userGraphs[i]->intervalsStat[currentInt]->apprLine) {
                                index = i;
                                redrawAppr=true;
                                apprLineBuild();
                            }
                        }
                }
                else
                {
                    if (!Stat)
                        for (int i=0;i<userGraphs.size();++i) {
                            userGraphs[i]->intervals[currentInt]->leftLine->start->setCoords(userGraphs[i]->intervals[currentInt]->xLineInt.first,DBL_MIN);
                            userGraphs[i]->intervals[currentInt]->leftLine->end->setCoords(userGraphs[i]->intervals[currentInt]->xLineInt.first,DBL_MAX);
                        }
                    else
                        for (int i=0;i<userGraphs.size();++i) {
                            userGraphs[i]->intervalsStat[currentInt]->leftLine->start->setCoords(userGraphs[i]->intervalsStat[currentInt]->xLineInt.first,DBL_MIN);
                            userGraphs[i]->intervalsStat[currentInt]->leftLine->end->setCoords(userGraphs[i]->intervalsStat[currentInt]->xLineInt.first,DBL_MAX);
                        }

                    lineIsFixed.first=1;
                    lineIsMoving[0]++;

                    if (!Stat)
                        for (int i=0;i<userGraphs.size();++i) {
                            if (userGraphs[i]->intervals[currentInt]->apprLine) {
                                index = i;
                                redrawAppr=true;
                                apprLineBuild();
                            }
                        }
                    else
                        for (int i=0;i<userGraphs.size();++i) {
                            if (userGraphs[i]->intervalsStat[currentInt]->apprLine) {
                                index = i;
                                redrawAppr=true;
                                apprLineBuild();
                            }
                        }
                }
                qobject_cast<QCustomPlot*>(ui->widgetPanel->currentWidget())->replot();
            }
            else
            {
                if (lineIsFixed.first) {
                    if (!Stat) {
                        LineDelete(userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt],0);
                        for (int i=0;i<userGraphs.size();++i) {
                            if(userGraphs[i]->intervals[currentInt]->apprLine) {
                                qobject_cast<QCustomPlot*>(ui->widgetPanel->widget(i))->removeItem(userGraphs[i]->intervals[currentInt]->apprLine); //
                                userGraphs[i]->intervals[currentInt]->apprLine=0;
                                userGraphs[i]->intervals[currentInt]->xLineInt.first=DBL_MIN;
                                userGraphs[i]->intervals[currentInt]->koefAppr=nan("");
                                userGraphs[i]->intervals[currentInt]->convergVal=nan("");
                                qobject_cast<QCustomPlot*>(ui->widgetPanel->widget(i))->replot();
                            }
                        }
                    } else {
                        LineDelete(userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt],0);
                        for (int i=0;i<userGraphs.size();++i) {
                            if(userGraphs[i]->intervalsStat[currentInt]->apprLine) {
                                qobject_cast<QCustomPlot*>(ui->widgetPanel->widget(i))->removeItem(userGraphs[i]->intervalsStat[currentInt]->apprLine); //
                                userGraphs[i]->intervalsStat[currentInt]->apprLine=0;
                                userGraphs[i]->intervalsStat[currentInt]->xLineInt.first=DBL_MIN;
                                userGraphs[i]->intervalsStat[currentInt]->koefAppr=nan("");
                                userGraphs[i]->intervalsStat[currentInt]->convergVal=nan("");
                                qobject_cast<QCustomPlot*>(ui->widgetPanel->widget(i))->replot();
                            }
                        }
                    }
                }
            }
            if (rx.cap(2)!="")
            {
                QCPItemTracer *tracer;
                for (int i=0;i<userGraphs.size();++i) {
                    if (rx.cap(2)==".") {
                        if (!Stat)
                            userGraphs[i]->intervals[currentInt]->xLineInt.second=0;
                        else
                            userGraphs[i]->intervalsStat[currentInt]->xLineInt.second=0;
                    }
                    else {
                        if (!Stat)
                            userGraphs[i]->intervals[currentInt]->xLineInt.second=rx.cap(2).toDouble();
                        else
                            userGraphs[i]->intervalsStat[currentInt]->xLineInt.second=rx.cap(2).toDouble();
                    }
                    if (lineIsFixed.second==1)
                    {
                        if (!Stat) {
                            userGraphs[i]->intervals[currentInt]->rightLine->start->setCoords(userGraphs[i]->intervals[currentInt]->xLineInt.second,DBL_MIN);
                            userGraphs[i]->intervals[currentInt]->rightLine->end->setCoords(userGraphs[i]->intervals[currentInt]->xLineInt.second,DBL_MAX);
                            //просто поменяли координаты второй прямой
                            if (userGraphs[i]->intervals[currentInt]->apprLine) {
                                index = i;
                                redrawAppr=true;
                                apprLineBuild();
                            }
                        } else {
                            userGraphs[i]->intervalsStat[currentInt]->rightLine->start->setCoords(userGraphs[i]->intervalsStat[currentInt]->xLineInt.second,DBL_MIN);
                            userGraphs[i]->intervalsStat[currentInt]->rightLine->end->setCoords(userGraphs[i]->intervalsStat[currentInt]->xLineInt.second,DBL_MAX);
                            //просто поменяли координаты второй прямой
                            if (userGraphs[i]->intervalsStat[currentInt]->apprLine) {
                                index = i;
                                redrawAppr=true;
                                apprLineBuild();
                            }
                        }
                    }
                    else
                    {
                        if (!Stat) {
                            userGraphs[i]->intervals[currentInt]->rightLine->start->setCoords(userGraphs[i]->intervals[currentInt]->xLineInt.second,DBL_MIN);
                            userGraphs[i]->intervals[currentInt]->rightLine->end->setCoords(userGraphs[i]->intervals[currentInt]->xLineInt.second,DBL_MAX);

                            lineIsFixed.second=1;
                            lineIsMoving[0]++;
                            if (userGraphs[i]->intervals[currentInt]->apprLine) {
                                index = i;
                                redrawAppr=true;
                                apprLineBuild();
                            }
                        } else {
                            userGraphs[i]->intervalsStat[currentInt]->rightLine->start->setCoords(userGraphs[i]->intervalsStat[currentInt]->xLineInt.second,DBL_MIN);
                            userGraphs[i]->intervalsStat[currentInt]->rightLine->end->setCoords(userGraphs[i]->intervalsStat[currentInt]->xLineInt.second,DBL_MAX);

                            lineIsFixed.second=1;
                            lineIsMoving[0]++;
                            if (userGraphs[i]->intervalsStat[currentInt]->apprLine) {
                                index = i;
                                redrawAppr=true;
                                apprLineBuild();
                            }
                        }
                    }
                    tracer = new QCPItemTracer(userGraphs[i]->mainPlot);
                    tracer->setGraph(userGraphs[i]->mainPlot->graph(0));

                    if (!Stat)
                        tracer->setGraphKey(userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->rightLine->end->key());
                    else
                        tracer->setGraphKey(userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->rightLine->end->key());

                    tracer->updatePosition();

                    if (!Stat)
                        userGraphs[i]->intervals[currentInt]->RMember=tracer->position->value();
                    else
                        userGraphs[i]->intervalsStat[currentInt]->RMember=tracer->position->value();
                    userGraphs[i]->mainPlot->removeItem(tracer);
                }
                if (!Stat)
                    values->model()->setData(values->model()->index(currentInt,3),userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->RMember);
                else
                    valuesStat->model()->setData(valuesStat->model()->index(currentInt,3),userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->RMember);
                qobject_cast<QCustomPlot*>(ui->widgetPanel->currentWidget())->replot();
            }
            else
            {
                if (lineIsFixed.second) {
                    if (!Stat) {
                        LineDelete(userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt],1);
                        for (int i=0;i<userGraphs.size();++i) {
                            if(userGraphs[i]->intervals[currentInt]->apprLine) {
                                qobject_cast<QCustomPlot*>(ui->widgetPanel->widget(i))->removeItem(userGraphs[i]->intervals[currentInt]->apprLine); //
                                userGraphs[i]->intervals[currentInt]->apprLine=0;
                                userGraphs[i]->intervals[currentInt]->xLineInt.second=DBL_MIN;
                                userGraphs[i]->intervals[currentInt]->koefAppr=nan("");
                                userGraphs[i]->intervals[currentInt]->convergVal=nan("");
                                qobject_cast<QCustomPlot*>(ui->widgetPanel->widget(i))->replot();
                            }
                            userGraphs[i]->intervals[currentInt]->RMember=nan("");
                        }
                        values->model()->setData(values->model()->index(currentInt,2),userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->koefAppr);
                        values->model()->setData(values->model()->index(currentInt,3),userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->RMember);
                        values->model()->setData(values->model()->index(currentInt,4),QString("%1 (a=%2)").arg(userGraphs[ui->widgetPanel->currentIndex()]->intervals[currentInt]->convergVal)
                                                                                                          .arg(logK));
                    } else {
                        LineDelete(userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt],1);
                        for (int i=0;i<userGraphs.size();++i) {
                            if(userGraphs[i]->intervalsStat[currentInt]->apprLine) {
                                qobject_cast<QCustomPlot*>(ui->widgetPanel->widget(i))->removeItem(userGraphs[i]->intervalsStat[currentInt]->apprLine); //
                                userGraphs[i]->intervalsStat[currentInt]->apprLine=0;
                                userGraphs[i]->intervalsStat[currentInt]->xLineInt.second=DBL_MIN;
                                userGraphs[i]->intervalsStat[currentInt]->koefAppr=nan("");
                                userGraphs[i]->intervalsStat[currentInt]->convergVal=nan("");
                                qobject_cast<QCustomPlot*>(ui->widgetPanel->widget(i))->replot();
                            }
                            userGraphs[i]->intervalsStat[currentInt]->RMember=nan("");
                        }
                        valuesStat->model()->setData(valuesStat->model()->index(currentInt,2),userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->koefAppr);
                        valuesStat->model()->setData(valuesStat->model()->index(currentInt,3),userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->RMember);
                        valuesStat->model()->setData(valuesStat->model()->index(currentInt,4),QString("%1 (a=%2)").arg(userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[currentInt]->convergVal)
                                                                                                                  .arg(logK));
                    }
                }
            }
        }
    }
}

void MainWindow::LineDelete(Interval *interval, bool numberOfLine)
{
    if (interval==0)
        return;
    int currentInt = userGraphs[ui->widgetPanel->currentIndex()]->currentInt;
    if (!numberOfLine) {
        interval->leftLine->start->setCoords(DBL_MIN,DBL_MIN);
        interval->leftLine->end->setCoords(DBL_MIN,DBL_MIN);
        lineIsFixed.first = 0;

    } else {
        interval->rightLine->start->setCoords(DBL_MIN,DBL_MIN);
        interval->rightLine->end->setCoords(DBL_MIN,DBL_MIN);
        lineIsFixed.second = 0;

    }
    makeInterval[0]=1; //
    lineIsMoving[0]--;
}

void MainWindow::deleteRowTable()
{
    if (!Stat) {
        foreach(QTableWidgetItem* item,values->selectedItems()) {
            int sInd=item->row();
            for (int i=0;i<userGraphs.size();i++) {
                qobject_cast<QCustomPlot*>(ui->widgetPanel->widget(i))->removeItem(userGraphs[i]->intervals.at(sInd)->apprLine); //
                qobject_cast<QCustomPlot*>(ui->widgetPanel->widget(i))->removeItem(userGraphs[i]->intervals.at(sInd)->leftLine);
                qobject_cast<QCustomPlot*>(ui->widgetPanel->widget(i))->removeItem(userGraphs[i]->intervals.at(sInd)->rightLine);

                userGraphs[i]->intervals.at(sInd)->name="";
                userGraphs[i]->intervals.at(sInd)->koefAppr=nan("");
                userGraphs[i]->intervals.at(sInd)->RMember=nan("");
                userGraphs[i]->intervals.at(sInd)->convergVal=nan("");
                delete userGraphs[i]->intervals.at(sInd);

                userGraphs[i]->intervals.removeAt(sInd);
            }
            values->removeRow(sInd);
        }
    } else {
        foreach(QTableWidgetItem* item,valuesStat->selectedItems()) {
            int sInd=item->row();
            for (int i=0;i<userGraphs.size();i++) {
                qobject_cast<QCustomPlot*>(ui->widgetPanel->widget(i))->removeItem(userGraphs[i]->intervalsStat.at(sInd)->apprLine); //
                qobject_cast<QCustomPlot*>(ui->widgetPanel->widget(i))->removeItem(userGraphs[i]->intervalsStat.at(sInd)->leftLine);
                qobject_cast<QCustomPlot*>(ui->widgetPanel->widget(i))->removeItem(userGraphs[i]->intervalsStat.at(sInd)->rightLine);

                userGraphs[i]->intervalsStat.at(sInd)->name="";
                userGraphs[i]->intervalsStat.at(sInd)->koefAppr=nan("");
                userGraphs[i]->intervalsStat.at(sInd)->RMember=nan("");
                userGraphs[i]->intervalsStat.at(sInd)->convergVal=nan("");
                delete userGraphs[i]->intervalsStat.at(sInd);

                userGraphs[i]->intervalsStat.removeAt(sInd);
            }
            valuesStat->removeRow(sInd);
        }
    }
    qobject_cast<QCustomPlot*>(ui->widgetPanel->currentWidget())->replot();
}

void MainWindow::deleteInterval()
{

}

void MainWindow::cellIsSelected(int row, int column)
{
     userGraphs[ui->widgetPanel->currentIndex()]->currentInt = row;
}

void MainWindow::redrawApprLine()
{

}

void MainWindow::slotTabChanged()
{
    int asd=342;
}

void MainWindow::TableRebuild(int curTab)
{
    if (ui->widgetPanel->count()) {
        size_t c = 0;
        qobject_cast<QCustomPlot*>(ui->widgetPanel->widget(curTab))->replot();

        if (!Stat) {
            foreach (Interval* cInterval, userGraphs[curTab]->intervals) {
                values->model()->setData(values->model()->index(c,0),cInterval->name);
                values->model()->setData(values->model()->index(c,2),cInterval->koefAppr);
                values->model()->setData(values->model()->index(c,3),cInterval->RMember);
                values->model()->setData(values->model()->index(c++,4),QString("%1 (a=%2)").arg(cInterval->convergVal).arg(logK));
            }
        } else {
            foreach (Interval* cInterval, userGraphs[curTab]->intervalsStat) {
                valuesStat->model()->setData(valuesStat->model()->index(c,0),cInterval->name);
                valuesStat->model()->setData(valuesStat->model()->index(c,2),cInterval->koefAppr);
                valuesStat->model()->setData(valuesStat->model()->index(c,3),cInterval->RMember);
                valuesStat->model()->setData(values->model()->index(c++,4),QString("%1 (a=%2)").arg(cInterval->convergVal).arg(logK));
            }
        }
        if (userGraphs[curTab]->discrepancyPlotShown)
            ui->pushButton->setText("Зависимости");
        else
            ui->pushButton->setText("Отклонения");
    }

}

void MainWindow::slotPlotChanged()
{
    if (userGraphs.count()!=0) {
        int index = ui->widgetPanel->currentIndex();
        if (!userGraphs[index]->discrepancyPlotShown) {
            if (userGraphs[index]->mainPlot->graphCount()!=2)
                discrPlotConstr(ui->widgetPanel->currentIndex());
            discrPlotShow(index,true);
            discrMainShow(index,false);
        } else {
            discrPlotShow(index,false);
            discrMainShow(index,true);
        }
    }
}

void MainWindow::discrPlotConstr(int index)
{
    userGraphs[index]->DiscrConstructed=true;
    userGraphs[index]->mainPlot->addGraph();
    userGraphs[index]->mainPlot->graph(1)->setPen(QPen(Qt::red));

    QVector<double>* vectX = new QVector<double>(userGraphs[index]->CoordX.size()-1);
    QVector<double>* vectY = new QVector<double>(userGraphs[index]->CoordX.size()-1);

    for(size_t c = 0;c<vectX->size();++c)
    {
        *(vectX->begin()+c)=userGraphs[index]->CoordX[c];
        *(vectY->begin()+c)=userGraphs[index]->CoordY[c+1]-userGraphs[index]->CoordY[c];
    }

    userGraphs[index]->CoordYDiscr = *vectY;
    if (Stat)
        userGraphs[index]->mainPlot->graph(1)->setData(userGraphs[index]->CoordXStat.toVector(),*vectY);
    else
        userGraphs[index]->mainPlot->graph(1)->setData(*vectX,*vectY);
    userGraphs[index]->mainPlot->graph(1)->setVisible(false);

    userGraphs[index]->DiscrRanges = new QPair<double,double>(*std::min_element(vectY->begin(),vectY->end()),
                                                              *std::max_element(vectY->begin(),vectY->end()));

    userGraphs[index]->mainPlot->replot();
}

void MainWindow::discrPlotShow(int index, bool var)
{
    if (var) {
        userGraphs[index]->mainPlot->graph(1)->setVisible(true);
        userGraphs[index]->mainPlot->yAxis->setRange(userGraphs[index]->DiscrRanges->first,userGraphs[index]->DiscrRanges->second);
        userGraphs[index]->mainPlot->plotLayout()->removeAt(0);
        userGraphs[index]->HeaderDiscr = new QCPTextElement(userGraphs[index]->mainPlot, QString("%1 (Discrepancy)").arg(userGraphs[index]->Headers),
                                                            QFont("Times New Roman", 12, QFont::Bold));
        userGraphs[index]->mainPlot->plotLayout()->addElement(0, 0, userGraphs[index]->HeaderDiscr);
        userGraphs[index]->discrepancyPlotShown=true;
    } else {
        userGraphs[index]->mainPlot->graph(1)->setVisible(false);
        userGraphs[index]->discrepancyPlotShown=false;
    }
}

void MainWindow::discrMainShow(int index, bool var)
{
    if (var) {
        userGraphs[index]->mainPlot->graph(0)->setVisible(true);
        if (!Stat) {
            foreach (Interval* curInt, userGraphs[index]->intervals) {
                if (curInt->apprLine) {
                    curInt->apprLine->setVisible(true);
                }
                curInt->leftLine->setVisible(true);
                curInt->rightLine->setVisible(true);
            }
        } else {
            foreach (Interval* curInt, userGraphs[index]->intervalsStat) {
                if (curInt->apprLine) {
                    curInt->apprLine->setVisible(true);
                }
                curInt->leftLine->setVisible(true);
                curInt->rightLine->setVisible(true);
            }
        }
        userGraphs[index]->mainPlot->yAxis->setRange(userGraphs[index]->MaxMinY.second, userGraphs[index]->MaxMinY.first);
        userGraphs[index]->mainPlot->plotLayout()->removeAt(0);
        userGraphs[index]->header = new QCPTextElement(userGraphs[index]->mainPlot, userGraphs[index]->Headers, QFont("Times New Roman", 12, QFont::Bold));
        userGraphs[index]->mainPlot->plotLayout()->addElement(0, 0, userGraphs[index]->header);

        ui->pushButton->setText("Отклонения");
    } else {
        userGraphs[index]->mainPlot->graph(0)->setVisible(false);
        if (!Stat) {
            foreach (Interval* curInt, userGraphs[index]->intervals) {
                if (curInt->apprLine) {
                    curInt->apprLine->setVisible(false);
                }
                curInt->leftLine->setVisible(false);
                curInt->rightLine->setVisible(false);
            }
        } else {
            foreach (Interval* curInt, userGraphs[index]->intervalsStat) {
                if (curInt->apprLine) {
                    curInt->apprLine->setVisible(false);
                }
                curInt->leftLine->setVisible(false);
                curInt->rightLine->setVisible(false);
            }
        }
        ui->pushButton->setText("Зависимости");
    }
    userGraphs[index]->mainPlot->replot();
}

void MainWindow::on_commandLinkButton_clicked()
{
    if (ui->tabWidget->isVisible())
        ui->tabWidget->hide();
    else
        ui->tabWidget->show();
}

void MainWindow::renameInt(int row, int col)
{
    if (col==0) {
        if (!Stat)
            userGraphs[ui->widgetPanel->currentIndex()]->intervals[row]->name=values->model()->index(row,col).data().toString();
        else
            userGraphs[ui->widgetPanel->currentIndex()]->intervalsStat[row]->name=valuesStat->model()->index(row,col).data().toString();
    }
}

void MainWindow::deleteGraph()
{
    int index = ui->widgetPanel->currentIndex();
    delete userGraphs[index]->mainPlot;
    userGraphs.removeAt(index);
    curNumberGraphs--;
    if (ui->widgetPanel->count()!=0)
        TableRebuild(ui->widgetPanel->currentIndex());
    else {
        if (!Stat) {
            values->clearContents();
            values->setRowCount(0);
        } else {
            valuesStat->clearContents();
            valuesStat->setRowCount(0);
        }
        index = 0;
        redrawAppr = false;
        grMade=0;
        onPlot=false;
    }
}

void MainWindow::deleteData()
{
    for(int c=0;c<userGraphs.count();++c)
        delete userGraphs[c]->mainPlot;

    userGraphs.clear();

    if (!Stat) {
        values->clearContents();
        values->setRowCount(0);
    } else {
        valuesStat->clearContents();
        valuesStat->setRowCount(0);
    }
    curNumberGraphs=0;
    index = 0;
    grMade=0;
    redrawAppr = false;
    onPlot=false;
}

void MainWindow::setKoef()
{
    Dialog* dial = new Dialog(this, &logK);
    dial->show();
}

void MainWindow::setSt()
{
    if (!Stat) {
        for (int i=0;i<userGraphs.size();i++)
        {
            userGraphs[i]->customGr->setData(userGraphs[i]->CoordXStat.toVector(), userGraphs[i]->CoordY.toVector());
            userGraphs[i]->mainPlot->xAxis->setLabel(QString("%1 (Stationary)").arg(userGraphs[i]->AxesNs.first));
            userGraphs[i]->mainPlot->xAxis->setRange(userGraphs[i]->CoordXStat.first(), userGraphs[i]->CoordXStat.last());

            if (userGraphs[i]->DiscrConstructed)
                userGraphs[i]->mainPlot->graph(1)->setData(userGraphs[i]->CoordXStat.toVector(),userGraphs[i]->CoordYDiscr);
        }
        userGraphs[ui->widgetPanel->currentIndex()]->mainPlot->replot();
        Stat=true;
        ui->action_7->setText("Задать нестационарное время");
    } else {
        for (int i=0;i<userGraphs.size();i++)
        {
            userGraphs[i]->customGr->setData(userGraphs[i]->CoordX.toVector(), userGraphs[i]->CoordY.toVector());
            userGraphs[i]->mainPlot->xAxis->setLabel(QString("%1").arg(userGraphs[i]->AxesNs.first));
            userGraphs[i]->mainPlot->xAxis->setRange(userGraphs[i]->CoordX.first()-(userGraphs[i]->CoordX.last()-userGraphs[i]->CoordX.first())/1000,
                                                     userGraphs[i]->CoordX.last()+(userGraphs[i]->CoordX.last()-userGraphs[i]->CoordX.first())/1000);

            if (userGraphs[i]->DiscrConstructed)
                userGraphs[i]->mainPlot->graph(1)->setData(userGraphs[i]->CoordX.toVector(),userGraphs[i]->CoordYDiscr);
        }
        userGraphs[ui->widgetPanel->currentIndex()]->mainPlot->replot();
        ui->action_7->setText("Задать стационарное время");
        Stat=false;
    }
}
