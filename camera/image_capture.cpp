#include "image_capture.h"
#include "ui_image_capture.h"

image_capture::image_capture(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::image_capture)
{
    ui->setupUi(this);
}

image_capture::~image_capture()
{
    delete ui;
}

void image_capture::Load_img(QString filepath){
    //path of aplication
    QDir dir(filepath);
    if(!dir.exists()){
        qDebug() << "Path doesn't exists";
    }
    QString prefix = "cam";
    QStringList filters;
    filters << prefix+"*.jpg";
    QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files);
    foreach (const QFileInfo &fileInfo, fileList){
        ListPath.push_back(fileInfo.filePath());
    }
}

void image_capture::Display_img(int index){
    if (index >= 0 && index < ListPath.size())
        {
        QPixmap img(ListPath[index]);
            ui->label->setPixmap(img.scaled(ui->label->size(),
                                                           Qt::KeepAspectRatio,
                                                         Qt::SmoothTransformation));
        }
    else{
        qDebug()<< "index of image incorrect";
    }
}

void image_capture::ShowImage(){
    this->show();
    currentImageIndex = 0;
}



void image_capture::on_Next_Img_clicked()
{

    if (currentImageIndex < ListPath.size() - 1)
    {
        currentImageIndex++;
        Display_img(currentImageIndex);
    }
    else{
        currentImageIndex = 0;
        Display_img(currentImageIndex);
    }

}

void image_capture::on_Prev_Img_clicked()
{
    if (currentImageIndex > 0)
    {
        currentImageIndex--;
        Display_img(currentImageIndex);
    }
    else {
        currentImageIndex = ListPath.size()-1;
        Display_img(currentImageIndex);
    }

}


void image_capture::on_Load_img_clicked()
{
    QString DirectoyPath = QFileDialog::getExistingDirectory(nullptr, "Select directory", "");
    qDebug() << DirectoyPath;
    Load_img(DirectoyPath);
    Display_img(currentImageIndex);
}
