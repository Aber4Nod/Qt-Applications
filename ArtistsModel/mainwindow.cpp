#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ratingdelegate.h"
#include "dialogalbum.h"
#include "dialogartist.h"
#include "item.h"
#include <QFileDialog>
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MusicModel *model=new MusicModel();


    modelM=model;
    ui->view->setModel(modelM);
    ui->view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->actionProperties,SIGNAL(triggered()),this,SLOT(slotEditRecord()));
    connect(ui->exitAction,SIGNAL(triggered()),this,SLOT(close()));
    connect(ui->insertRowAction, &QAction::triggered, this, &MainWindow::insertRow);
    connect(ui->removeRowAction, &QAction::triggered, this, &MainWindow::removeRow);
    connect(ui->view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomMenuRequested(QPoint)));
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::slotSave);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::slotSaveAs);
    connect(ui->actionLoad, &QAction::triggered, this, &MainWindow::slotLoad);

    connect(ui->view->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::updateActions);
    connect(ui->actionsMenu, &QMenu::aboutToShow, this, &MainWindow::updateActions);
    connect(ui->insertChildAction, &QAction::triggered, this, &MainWindow::insertChild);
    connect(ui->actionInsert_Child2, &QAction::triggered, this, &MainWindow::insertChild);
    connect(ui->view->model(),SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(slotDataChanged()));




    const int RatingColumn = 2;
    delegate = new RatingDelegate(ui->view);
    ui->view->setItemDelegateForColumn(RatingColumn,delegate);

    currentWindowTitle=windowTitle();
}
MainWindow::~MainWindow()
{
    delete ui;
}
QTreeView *MainWindow::view12() const
{
    return ui->view;
}
void MainWindow::slotSaveAs()
{
    QString str = QFileDialog::getSaveFileName(0, m_strFileName);
    if (!str.isEmpty()) {
        m_strFileName = str;
        slotSave();
    }
}

void MainWindow::slotSave()
{
    if (m_strFileName.isEmpty()) {
        slotSaveAs();
    }

    QFile file(m_strFileName);
    if (file.open(QIODevice::WriteOnly)) {
        QAbstractItemModel *model = ui->view->model();
        QDataStream out(&file);

        for(int i=0;i<model->rowCount();++i)
        {
            QModelIndex ind=model->index(i,0);
            qDebug()<<"NAME: "<<static_cast<Artist*>(ind.internalPointer())->m_name;
            Artist *art=static_cast<Artist*>(ind.internalPointer());
            out<<*art;
        }

        file.close();
        setWindowTitle(m_strFileName);
    }
    currentWindowTitle=windowTitle();
}
void MainWindow::slotLoad()
{
    QAbstractItemModel *model = ui->view->model();
    if (model->rowCount()!=0)
    {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Before loading a new data, clean the model"),QMessageBox::Ok);
        return;
    }
    QString str=QFileDialog::getOpenFileName();
    if (str.isEmpty()) {
        return;
    }

    QFile file(str);
    if (file.open(QIODevice::ReadOnly)) {


        QAbstractItemModel *model = ui->view->model();

        QDataStream in(&file);

        if (model->rowCount()==0){
            while(!in.atEnd()){
                Artist *artist=new Artist("",modelM->m_root);
                in>>*artist;
                int cnt_alb=0;
                in>>cnt_alb;
                for(int qr=0;qr<cnt_alb;++qr)
                {
                    Album *album=new Album("",artist);
                    in>>*album;

                    int cnt_sng=0;
                    in>>cnt_sng;
                    for(int qe=0;qe<cnt_sng;++qe)
                    {
                        Song *song=new Song("",album);
                        in>>*song;
                    }
                }

            }
            ui->view->setModel(modelM);
        }

        model->submit();
        setGeometry(this->x(),this->y(),this->width(),this->height()+1);


        file.close();

        m_strFileName = str;
    }
}

void MainWindow::slotDataChanged()
{
    setWindowTitle(currentWindowTitle+"*");
}


void MainWindow::insertRow()
{
    QModelIndex index = ui->view->selectionModel()->currentIndex();
    QAbstractItemModel *model = ui->view->model();

    if (!model->insertRow(0, index.parent().parent()))
        return;
    ui->view->selectionModel()->setCurrentIndex(model->index(0, 0, index.parent()),
                                        QItemSelectionModel::ClearAndSelect);
}
void MainWindow::insertChild()
{

    QModelIndex index = ui->view->selectionModel()->currentIndex();
    QAbstractItemModel *model = ui->view->model();
    if (model->columnCount(index) == 0) {
        if (!model->insertColumn(0, index))
            return;
    }


    if (!model->insertRow(index.row(), index))
        return;


    ui->view->selectionModel()->setCurrentIndex(model->index(0, 0, index),
                                                QItemSelectionModel::ClearAndSelect);


}
void MainWindow::removeRow()
{
    QModelIndex index = ui->view->selectionModel()->currentIndex();
    QAbstractItemModel *model = ui->view->model();
    if (model->removeRow(index.row(), index.parent()))
        return ;
}

void MainWindow::slotCustomMenuRequested(QPoint pos)
{

    /* Создаем объект контекстного меню */
       QMenu * menu = new QMenu(this);
       /* Создаём действия для контекстного меню */
       QAction * addSong = new QAction(trUtf8("Добавить песню"), this);
       QAction * addAlbum = new QAction(trUtf8("Добавить альбом"), this);
       QAction * addArtist = new QAction(trUtf8("Добавить нового исполнителя"), this);
       QAction * editDevice = new QAction(trUtf8("Свойства"), this);
       QAction * deleteDevice = new QAction(trUtf8("Удалить"), this);

       if (!ui->view->selectionModel()->currentIndex().parent().isValid())
       {
           addArtist->setEnabled(true);
           addSong->setEnabled(false);
           addAlbum->setEnabled(true);
           editDevice->setEnabled(true);
           deleteDevice->setEnabled(true);
       }
       if(ui->view->selectionModel()->currentIndex().parent().parent().isValid())
        {
           addSong->setEnabled(false);
           addAlbum->setEnabled(false);
           editDevice->setEnabled(false);
           addArtist->setEnabled(false);
        }
       if((ui->view->selectionModel()->currentIndex().parent().isValid())&&(!ui->view->selectionModel()->currentIndex().parent().parent().isValid()))
        {
           addSong->setEnabled(true);
           addAlbum->setEnabled(false);
           addArtist->setEnabled(false);
        }
       if (ui->view->selectionModel()->selection().isEmpty())
       {
           addSong->setEnabled(false);
           addArtist->setEnabled(true);
           addAlbum->setEnabled(false);
           editDevice->setEnabled(false);
           deleteDevice->setEnabled(false);
       }

       /* Подключаем СЛОТы обработчики для действий контекстного меню */
              connect(addAlbum, &QAction::triggered, this, &MainWindow::insertChild);
                     connect(addSong, &QAction::triggered, this, &MainWindow::insertChild);

       connect(addArtist, &QAction::triggered, this, &MainWindow::insertRow);
       connect(editDevice, SIGNAL(triggered()), this, SLOT(slotEditRecord()));     // Обработчик вызова диалога редактирования
       connect(deleteDevice, SIGNAL(triggered()), this,SLOT(removeRow())); // Обработчик удаления записи
       /* Устанавливаем действия в меню */
      menu->addAction(addArtist);
       menu->addAction(addAlbum);
       menu->addAction(addSong);
       menu->addAction(editDevice);
       menu->addAction(deleteDevice);
       /* Вызываем контекстное меню */
       menu->popup(ui->view->viewport()->mapToGlobal(pos));
}

void MainWindow::slotEditRecord()
{
    if ((ui->view->selectionModel()->currentIndex().parent().isValid())&&(!ui->view->selectionModel()->currentIndex().parent().parent().isValid())){
        dialogAlbum *addDeviceDialog = new dialogAlbum(this);
        addDeviceDialog->setModel(ui->view->model());
        QModelIndex index = ui->view->selectionModel()->currentIndex();
        addDeviceDialog->setModelIndex(index);

        addDeviceDialog->show();
        //addDeviceDialog->setFocusPolicy(Q);
    }
    else
        if (!ui->view->selectionModel()->currentIndex().parent().isValid())
        {
            DialogArtist *addDeviceDialog = new DialogArtist(this);
            addDeviceDialog->setModel(ui->view->model());
            QModelIndex index = ui->view->selectionModel()->currentIndex();
            addDeviceDialog->setModelIndex(index);
            addDeviceDialog->show();
        }

}
void MainWindow::updateActions()
{
    bool hasSelection = !ui->view->selectionModel()->selection().isEmpty();
    ui->removeRowAction->setEnabled(hasSelection);
    ui->removeColumnAction->setEnabled(hasSelection);
    ui->actionAdd_artist->setEnabled(hasSelection);
    ui->insertChildAction->setEnabled(hasSelection);
    ui->actionInsert_Child2->setEnabled(hasSelection);
    ui->insertRowAction->setEnabled(!hasSelection);

    bool hasCurrent = ui->view->selectionModel()->currentIndex().isValid();
    ui->insertRowAction->setEnabled(!hasCurrent);

    if (!hasCurrent)
    {
        ui->actionAdd_artist->setEnabled(false);
        ui->insertChildAction->setEnabled(false);
        ui->actionInsert_Child2->setEnabled(false);
    }

    if (hasCurrent) {
        ui->view->closePersistentEditor(ui->view->selectionModel()->currentIndex());

        int row = ui->view->selectionModel()->currentIndex().row();
        int column = ui->view->selectionModel()->currentIndex().column();
        if (ui->view->selectionModel()->currentIndex().parent().isValid())
            statusBar()->showMessage(tr("Position: (%1,%2)").arg(row).arg(column));
        else
        {
            statusBar()->showMessage(tr("Position: (%1,%2) in top level").arg(row).arg(column));


        }
        if(ui->view->selectionModel()->currentIndex().parent().parent().isValid())
        {
            ui->actionAdd_artist->setEnabled(false);
            ui->insertChildAction->setEnabled(false);
            ui->actionInsert_Child2->setEnabled(false);
        }
        else{
            if(!ui->view->selectionModel()->currentIndex().parent().isValid())
            {
                ui->actionAdd_artist->setEnabled(false);
                ui->insertChildAction->setEnabled(true);
                ui->actionInsert_Child2->setEnabled(false);
                ui->insertRowAction->setEnabled(true);
            }
            else
            {
                ui->actionAdd_artist->setEnabled(false);
                ui->insertChildAction->setEnabled(false);
                ui->actionInsert_Child2->setEnabled(true);
            }
        }
        if (!ui->view->selectionModel()->currentIndex().parent().isValid())
        {

            ui->actionProperties->setEnabled(true);

        }
        if(ui->view->selectionModel()->currentIndex().parent().parent().isValid())
        {

            ui->actionProperties->setEnabled(false);

        }
        if((ui->view->selectionModel()->currentIndex().parent().isValid())&&(!ui->view->selectionModel()->currentIndex().parent().parent().isValid()))
        {
            ui->actionProperties->setEnabled(true);
        }
        if (ui->view->selectionModel()->selection().isEmpty())
        {

            ui->actionProperties->setEnabled(false);

        }
    }

    else
        ui->actionProperties->setEnabled(false);
}
