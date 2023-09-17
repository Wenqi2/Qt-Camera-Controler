#ifndef IMAGE_CAPTURE_H
#define IMAGE_CAPTURE_H

#include <QDialog>
#include <QPixmap>
#include <QDir>
#include <QDebug>
#include <QImage>
#include <QFileDialog>
namespace Ui {
class image_capture;
}

class image_capture : public QDialog
{
    Q_OBJECT

public:
    explicit image_capture(QWidget *parent = 0);
    ~image_capture();
    void ShowImage();

private slots:

    void on_Next_Img_clicked();

    void on_Prev_Img_clicked();

    void on_Load_img_clicked();

private:
    Ui::image_capture *ui;
    void Load_img(QString path);
    void Display_img(int index);
    QVector<QString> ListPath;
    int currentImageIndex;
};

#endif // IMAGE_CAPTURE_H
