#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QTextStream>
#include <QtWidgets>
#include <QEvent>
#include <QFontDialog>
#include <QSettings>
#include <QStatusBar>
#include <QIcon>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowIcon(QPixmap(":/new/appIcon/Icons/page_white.png"));
    setWindowTitle("TextEditor");
    flagRedo=0;
    flagUndo=0;

    pcb = QApplication::clipboard();

    QMenu* pmnuFile = new QMenu("&File");
    QMenu* pmnuHelp = new QMenu("&Help");
    QMenu* pmnuView = new QMenu("&View");
    QMenu* pmnuEdit = new QMenu("&Edit");

    newFile = pmnuFile->addAction("&New",
                        this,
                        SLOT(slotNew()),
                        QKeySequence("CTRL+N")
                        );
    newFile->setIcon(QPixmap(":/File/Icons/newFile.png"));

    pmnuFile->addSeparator();

    openFolder = pmnuFile->addAction("&Open...",
                        this,
                        SLOT(slotLoad()),
                        QKeySequence("CTRL+O")
                        );
    openFolder->setIcon(QPixmap(":/File/Icons/openFolder.png"));

    saveFile = pmnuFile->addAction("&Save",
                        this,
                        SLOT(slotSave()),
                        QKeySequence("CTRL+S")
                        );
    saveFile->setIcon(QPixmap(":/File/Icons/save.png"));

    pmnuFile->addAction("S&ave As...",
                        this,
                        SLOT(slotSaveAs()),
                        QKeySequence("CTRL+Shift+S")
                        );
    pmnuFile->addSeparator();
    closeFile = pmnuFile->addAction("&Close",
                        this,
                        SLOT(close()),
                        QKeySequence("CTRL+W")
                        );
    closeFile->setIcon(QPixmap(":/File/Icons/closeFile.png"));

    quitFile = pmnuFile->addAction("&Quit",
                        qApp,
                        SLOT(closeAllWindows())
                        );
    quitFile->setIcon(QPixmap(":/File/Icons/quitFile.png"));

    about = pmnuHelp->addAction("&About",
                        this,
                        SLOT(slotAbout()),
                        Qt::Key_F1
                        );
    about->setIcon(QPixmap(":/Help/Icons/about.png"));

    aboutQt = pmnuHelp->addAction("Abo&ut Qt",
                        this,
                        SLOT(slotAboutQt()),
                        Qt::Key_F2
                        );
    aboutQt->setIcon(QPixmap(":/Help/Icons/qt.png"));

    font = pmnuView->addAction("Se&lect font",
                        this,
                        SLOT(slotFont())
                        );
    font->setIcon(QPixmap(":/View/Icons/font.png"));

    actionCopy = pmnuEdit->addAction("Co&py",
                        ui->textEdit,
                        SLOT(copy()),
                        QKeySequence(Qt::CTRL+Qt::ALT+Qt::Key_C)
                        );
    actionCopy->setIcon(QPixmap(":/Edit/Icons/copyFile.png"));

    actionCut = pmnuEdit->addAction("&Cut",
                        ui->textEdit,
                        SLOT(cut()),
                        QKeySequence(Qt::CTRL+Qt::ALT+Qt::Key_X)
                        );
    actionCut->setIcon(QPixmap(":/Edit/Icons/cut.png"));

    actionPaste = pmnuEdit->addAction("Pas&te",
                        ui->textEdit,
                        SLOT(paste()),
                        QKeySequence(Qt::CTRL+Qt::ALT+Qt::Key_V)
                        );
    actionPaste->setIcon(QPixmap(":/Edit/Icons/insert.png"));

    pmnuEdit->addSeparator();

    actionUndo = new QAction("&Undo",this);
    actionUndo->setShortcut(QKeySequence(Qt::CTRL+Qt::ALT+Qt::Key_Z));
    pmnuEdit->addAction(actionUndo);

    connect(actionUndo,
            SIGNAL(triggered()),
            ui->textEdit,
            SLOT(undo())
            );
    connect(actionUndo,
            SIGNAL(triggered()),
            this,
            SLOT(slotUndo())
            );

    actionUndo->setIcon(QPixmap(":/Edit/Icons/undo.png"));

    actionRedo = new QAction("&Redo",this);
    actionRedo->setShortcut(QKeySequence(Qt::CTRL+Qt::ALT+Qt::Key_Y));
    pmnuEdit->addAction(actionRedo);

    actionRedo->setIcon(QPixmap(":/Edit/Icons/redo.png"));

    connect(actionRedo,
            SIGNAL(triggered()),
            ui->textEdit,
            SLOT(redo())
            );
    connect(actionRedo,
            SIGNAL(triggered()),
            this,
            SLOT(slotRedo())
            );

    actionUndo->setEnabled(false);
    actionRedo->setEnabled(false);
    actionCut->setEnabled(false);
    actionCopy->setEnabled(false);
    actionPaste->setEnabled(false);

    menuBar()->addMenu(pmnuFile);
    menuBar()->addMenu(pmnuEdit);
    menuBar()->addMenu(pmnuView);
    menuBar()->addMenu(pmnuHelp);



    setCentralWidget(ui->textEdit);

    connect(this,
            SIGNAL(selected()),
            SLOT(selectedText())
            );

    connect(this,
            SIGNAL(ChangeWindowTitle(const QString&)),
            SLOT(slotChangeWindowTitle(const QString&))
            );
    connect(ui->textEdit,
            SIGNAL(textChanged()),
            SLOT(documentWasModified())
            );

    statusBar()->showMessage("Ready",3000);

    createToolBars();

    readSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createToolBars()
{
    QList<QAction*> fileAct;
    fileAct.append(newFile);
    fileAct.append(openFolder);
    fileAct.append(saveFile);
    fileAct.append(closeFile);
    fileAct.append(quitFile);

    ui->mainToolBar->addActions(fileAct);
    ui->mainToolBar->addSeparator();
    fileAct.clear();

    fileAct.append(actionCopy);
    fileAct.append(actionCut);
    fileAct.append(actionPaste);
    fileAct.append(actionUndo);
    fileAct.append(actionRedo);

    ui->mainToolBar->addActions(fileAct);
    ui->mainToolBar->addSeparator();
    fileAct.clear();

    fileAct.append(font);

    ui->mainToolBar->addActions(fileAct);
    ui->mainToolBar->addSeparator();
    fileAct.clear();

    fileAct.append(about);
    fileAct.append(aboutQt);

    ui->mainToolBar->addActions(fileAct);
    ui->mainToolBar->addSeparator();
    fileAct.clear();
}

bool MainWindow::event(QEvent* event)
{
    emit selected();
    return QWidget::event(event);
}

void MainWindow::slotLoad()
{
    QString str=QFileDialog::getOpenFileName();
    if (str.isEmpty()) {
        return;
    }

    QFile file(str);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        ui->textEdit->setPlainText(stream.readAll());
        file.close();

        m_strFileName = str;
        emit ChangeWindowTitle(m_strFileName);
    }
    actionUndo->setEnabled(false);
    openedStr = ui->textEdit->toPlainText();
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
       // return;
    }

    QFile file(m_strFileName);
    if (file.open(QIODevice::WriteOnly)) {
        QString TE_str = ui->textEdit->toPlainText();
        QStringList TE_strList=TE_str.split('\n');
        QTextStream out(&file);

        for (int i=0; i<ui->textEdit->document()->lineCount(); i++) {
            out << TE_strList.takeFirst() << "\r\n";
        }
        file.close();
        ui->textEdit->document()->setModified(false);
        emit ChangeWindowTitle(m_strFileName);
    }
}

void MainWindow::slotNew()
{
    MainWindow* r=new MainWindow();
    QFont font = ui->textEdit->currentFont();
    r->ui->textEdit->setFont(font);
    r->ui->textEdit->append(" ");
    r->ui->textEdit->document()->undo();
    r->show();
}

void MainWindow::slotAbout()
{
    QMessageBox::about(this,"Application","A text editor is a computer program that lets a user enter, change and store text.\n"
                       "Text Editor by Mikhnenko N.K.");
}

void MainWindow::slotAboutQt()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::slotChangeWindowTitle(const QString& str)
{
    setWindowTitle(str);
}

void MainWindow::slotFont()
{
    bool bOk;
    ui->textEdit->setCurrentFont(QFontDialog::getFont(&bOk));
    if (bOk) {
        staticFont=ui->textEdit->currentFont();
    }
    else {
        ui->textEdit->setCurrentFont(staticFont);
        statusBar()->showMessage("Canceled",2000);
    }
}

bool MainWindow::maybeSave()
{
    QMessageBox* ret= new QMessageBox(this);
    QPushButton* save = ret->addButton(tr("Save"),QMessageBox::ActionRole);
    if (ui->textEdit->document()->isModified()) {
        ret->setWindowTitle(tr("Application"));
        ret->setText(tr("The document has been modified.\nDo you want to save your changes?"));
        ret->setIcon(QMessageBox::Question);
        QPushButton* discard = ret->addButton(tr("Discard"),QMessageBox::ActionRole);
        QPushButton* cancel = ret->addButton(tr("Cancel"),QMessageBox::ActionRole);

        ret->setDefaultButton(discard);
        ret->exec();

        if (ret->clickedButton() == save)
            slotSave();
        else if (ret->clickedButton() == cancel)
            return false;
    }
    if (ret->clickedButton() == save)
        return false;
    return true;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    }
    else {
        event->ignore();
    }
}

void MainWindow::documentWasModified()
{
    this->setWindowTitle(tr("NewDocument[*]"));

    actionUndo->setEnabled(true);

    if (ui->textEdit->document()->isEmpty()) {
          actionUndo->setEnabled(false);
    }

    if (openedStr == ui->textEdit->toPlainText()) {
          actionUndo->setEnabled(false);
    }

    setWindowModified(ui->textEdit->document()->isModified());
}

void MainWindow::writeSettings()
{
    QSettings settings("settingsFont.ini", QSettings::IniFormat);

    settings.setValue("text/font", this->ui->textEdit->currentFont());
}

void MainWindow::readSettings()
{
    QSettings settings("settingsFont.ini", QSettings::IniFormat);

    this->ui->textEdit->setCurrentFont(
                  settings.value("text/font", QFont()).value<QFont>());
}


void MainWindow::selectedText()
{

    if (!pcb->text().isEmpty()) {
        actionPaste->setEnabled(true);
    }
    else {
        actionPaste->setEnabled(false);
    }
    if (ui->textEdit->textCursor().selectedText()!="") {
        actionCut->setEnabled(true);
        actionCopy->setEnabled(true);
    }
    else {
        actionCut->setEnabled(false);
        actionCopy->setEnabled(false);
    }
}

void MainWindow::slotUndo()
{
    ++flagUndo;
    actionRedo->setEnabled(true);
}
void MainWindow::slotRedo()
{
    if (flagUndo == ++flagRedo) {
        actionRedo->setEnabled(false);
    }
}


