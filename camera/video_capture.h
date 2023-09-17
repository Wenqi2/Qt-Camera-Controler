#ifndef VIDEO_CAPTURE_H
#define VIDEO_CAPTURE_H

#include <QDialog>
#include <QVBoxLayout>
#include <QtMultimedia>
#include <QVideoWidget>
#include <QFileDialog>

namespace Ui {
class video_capture;
}

class video_capture : public QDialog
{
    Q_OBJECT

public:
    explicit video_capture(QWidget *parent = 0);
    ~video_capture();
    void ShowVideo();

private slots:


    void on_fileselector_clicked();

signals:


private:
    Ui::video_capture *ui;
    void load_video(QString path);
    QMediaPlayer *player;
    QVideoWidget *videoWidget;


};

#endif // VIDEO_CAPTURE_H
