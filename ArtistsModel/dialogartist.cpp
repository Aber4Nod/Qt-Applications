#include "dialogartist.h"
#include "ui_dialogartist.h"
#include <QFileDialog>
#include <QMessageBox>
DialogArtist::DialogArtist(QWidget *parent, Item *item) :
    QDialog(parent),
    ui(new Ui::DialogArtist)
{
    ui->setupUi(this);
    if (item)
    {
        artist = static_cast<Artist*>(item);
        ui->artistNameLine->setText(artist->m_name);
        ui->artistCountryLine->setText(artist->m_country);
        ui->commentEdit->setText(artist->m_comment);
        ui->artistPhotoLabel->setPixmap(artist->m_photo);
    }
}

DialogArtist::~DialogArtist()
{
    delete ui;
}

void DialogArtist::setModel(QAbstractItemModel * model)
{
    m_mapper.setModel(model);
}

void DialogArtist::setModelIndex(const QModelIndex &index)
{
    artist = static_cast<Artist*>(index.internalPointer());
    ui->artistNameLine->setText(artist->m_name);
    ui->artistCountryLine->setText(artist->m_country);
    ui->commentEdit->setText(artist->m_comment);
    ui->artistPhotoLabel->setPixmap(artist->m_photo);
}

void DialogArtist::on_pushButton_clicked()
{
    artist->m_comment=ui->commentEdit->toPlainText();
    artist->m_name=ui->artistNameLine->text();
    artist->m_country=ui->artistCountryLine->text();
    artist->m_photo = ui->artistPhotoLabel->pixmap()->copy(ui->artistPhotoLabel->pixmap()->rect());
}
void DialogArtist::on_pushButton_3_clicked()
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
        ui->artistPhotoLabel->setPixmap(im_.scaled(ui->artistPhotoLabel->rect().size()));
    }
}
