#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include<QTreeView>
#include <QMainWindow>
#include <ratingdelegate.h>
#include <musicmodel.h>
#include <item.h>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QTreeView* view12()const;
public slots:
        void updateActions();
        void slotSave();
        void slotSaveAs();
private:
    Ui::MainWindow *ui;
    RatingDelegate *delegate;
    QString m_strFileName;
private slots:
    void insertChild();
    void removeRow();
     void slotCustomMenuRequested(QPoint pos);
     void slotEditRecord();
     void insertRow();
     void slotLoad();
     void slotDataChanged();
private:
     Item *root;
     MusicModel *modelM;
     QString currentWindowTitle;
};

#endif // MAINWINDOW_H
