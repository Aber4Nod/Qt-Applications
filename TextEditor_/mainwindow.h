#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFontDialog>
#include <QSettings>
#include <QUndoStack>
#include <QMouseEvent>
#include <QGraphicsScene>

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
    QString m_strFileName;
    QString openedStr;

    QClipboard* pcb;

    bool maybeSave();
    void closeEvent(QCloseEvent* event);
    bool event(QEvent* event);

    QFont staticFont;

    QAction* newFile;
    QAction* openFolder;
    QAction* saveFile;
    QAction* closeFile;
    QAction* quitFile;

    QAction* actionCopy;
    QAction* actionCut;
    QAction* actionPaste;
    QAction* actionUndo;
    QAction* actionRedo;

    QAction* font;

    QAction* about;
    QAction* aboutQt;

    int flagRedo;
    int flagUndo;
    void writeSettings();
    void readSettings();
    void createToolBars();
signals:
 void ChangeWindowTitle(const QString&);
 void selected();
public slots:
 void slotLoad();
 void slotSave();
 void slotSaveAs();
 void slotNew();
 void slotAbout();
 void slotAboutQt();
 void slotChangeWindowTitle(const QString& str);
 void slotFont();
 void documentWasModified();
 void selectedText();
 void slotUndo();
 void slotRedo();
};

#endif // MAINWINDOW_H
