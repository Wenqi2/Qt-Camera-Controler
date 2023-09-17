#include "video_capture.h"
#include "ui_video_capture.h"

video_capture::video_capture(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::video_capture)
{
    ui->setupUi(this);

    // Crear un reproductor multimedia
    player = new QMediaPlayer;

    // Crear un widget de video
    videoWidget = new QVideoWidget;
    player->setVideoOutput(videoWidget);
    // Crear un diseño para la ventana principal
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(videoWidget);
    ui->widget->setLayout(layout);
}

video_capture::~video_capture()
{
    delete ui;
}

void video_capture::load_video(QString path){

    // Cargar el archivo .mp4 más reciente
    player->setMedia(QUrl::fromLocalFile(path));

    // Reproducir el video
    player->play();
}

void video_capture::ShowVideo(){
    this->show();
}

void video_capture::on_fileselector_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(nullptr, "Select files", "", "Video files (*.mp4)");
    load_video(filePath);
}
