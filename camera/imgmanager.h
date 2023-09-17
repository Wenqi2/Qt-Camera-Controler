#ifndef IMGMANAGER_H
#define IMGMANAGER_H

#include <QDialog>
#include <cameraimage.h>
#include <cameracontroller.h>
#include <camerarepository.h>
#include <QDebug>
#include <QtSql>
#include <QPixmap>
#include <QImage>
namespace Ui {
class ImgManager;
}

class ImgManager : public QDialog
{
    Q_OBJECT

public:
    explicit ImgManager(QWidget *parent = 0);
    ~ImgManager();

public slots:
    void Load_Img(QModelIndex index);

private slots:
    void on_pushButton_clicked();

private:
    Ui::ImgManager *ui;
    void Load_SQL();
};

#endif // IMGMANAGER_H
