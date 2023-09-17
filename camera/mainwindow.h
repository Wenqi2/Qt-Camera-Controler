#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <video_capture.h>
#include <image_capture.h>
#include <cameraimage.h>
#include <cameracontroller.h>
#include <camerarepository.h>
#include <imgmanager.h>
#include <QThread>
#include <QCameraInfo>
#include <QDateTime>
#include <QList>
#include <QUdpSocket>
#include <QNetworkInterface>
#include <QXmlStreamReader>
#include <QtNetwork>
#include <QtConcurrent/QtConcurrent>
#include <QGraphicsView>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:

    void on_single_video_capture_clicked();

    void on_All_Video_capture_clicked();

    void on_single_Image_capture_clicked();

    void on_All_Image_capture_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    void Record_Single_Video();
    void Record_All_Video();
    void Capture_Single_Img();
    void Capture_all_Img();
    void Start_Camera();
    void Get_ip_Adress();
    void Create_Dir();
    void Init_DB();
    void Load_Img_from_Dir(QString path, QString name);
    int num_camera;
    QList<QWidget*> windows;
    QVector<QString> IPadress;



};

#endif // MAINWINDOW_H
