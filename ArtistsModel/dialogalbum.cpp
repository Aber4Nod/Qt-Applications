#include "dialogalbum.h"
#include "ui_dialogalbum.h"
#include <QFileDialog>
#include <QMessageBox>
dialogAlbum::dialogAlbum(QWidget *parent, Item *item) :
    QDialog(parent),
    ui(new Ui::dialogAlbum)
{
    ui->setupUi(this);
    if (item)
    {
        album = static_cast<Album*>(item);
        ui->artistLine->setText(album->parent()->m_name);
        ui->albumNameLine->setText(album->m_name);
        ui->albumYear->setText(QString::number(album->m_year));
        ui->albumGenreLine->setText(album->m_genre);
        ui->commentEdit->setText(album->m_comment);
        ui->albumCoverLabel->setPixmap(album->m_cover);
    }
}

dialogAlbum::~dialogAlbum()
{
    delete ui;
}

void dialogAlbum::setModel(QAbstractItemModel * model)
{
    m_mapper.setModel(model);
}

void dialogAlbum::setModelIndex(const QModelIndex &index)
{

    album = static_cast<Album*>(index.internalPointer());
    ui->artistLine->setText(album->parent()->m_name);
    ui->albumNameLine->setText(album->m_name);
    ui->albumYear->setText(QString::number(album->m_year));
    ui->albumGenreLine->setText(album->m_genre);
    ui->commentEdit->setText(album->m_comment);
    ui->albumCoverLabel->setPixmap(album->m_cover);
}

void dialogAlbum::on_pushButton_2_clicked()
{
    album->m_comment=ui->commentEdit->toPlainText();
    album->m_name=ui->albumNameLine->text();
    album->m_year=ui->albumYear->text().toInt();
    album->m_genre=ui->albumGenreLine->text();
    album->m_cover = ui->albumCoverLabel->pixmap()->copy(ui->albumCoverLabel->pixmap()->rect());
}

void dialogAlbum::on_pushButton_clicked()
{
    fileName = QFileDialog::getOpenFileName(this,
                                            tr("Open File"), QDir::currentPath());
    if (!fileName.isEmpty()) {
        QImage image(fileName);
        if (image.isNull()) {
            QMessageBox::information(this, tr("Image Viewer"),
                                     tr("Cannot load %1.").arg(fileName));
            return;
        }
        QPixmap im_;
        im_.convertFromImage(image,Qt::AutoColor);
        ui->albumCoverLabel->setPixmap(im_.scaled(ui->albumCoverLabel->rect().size()));
    }
}

