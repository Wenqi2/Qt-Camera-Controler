#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <gst/gst.h>
#include <gst/video/videooverlay.h>
#include <qdebug.h>
#include <QString>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Init_DB();
    Get_ip_Adress();
    num_camera = IPadress.size();
    Start_Camera();
    Create_Dir();
    ui->camara_selector_video->setMaximum(num_camera);
    ui->camara_selector_img->setMaximum(num_camera);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Init_DB()
{
    QDir currentDir = QDir::current();
    QString FileName = "imgDB.db3";

    // Full path of the directory
    QString DB_Path = currentDir.filePath(FileName);
    CameraController* db = CameraController::getInstance();
    db->open(DB_Path);
}


void MainWindow::Create_Dir(){
    // Current directory
    QDir currentDir = QDir::current();

    // Name of the directory you want to create
    QString newDirectoryName = "img";

    // Full path of the new directory
    QString newDirectoryPath = currentDir.filePath(newDirectoryName);
    qDebug() << newDirectoryPath;
    // Check if the directory already exists
    if (currentDir.exists(newDirectoryName)) {
        qDebug() << "The directory "+newDirectoryName+" already exists.";
    } else {
        // Create the directory
        if (currentDir.mkdir(newDirectoryName)) {
            qDebug() << "Directory "+newDirectoryName+"created successfully.";
        } else {
            qDebug() << "Failed to create the directory"+newDirectoryName+".";
        }
    }

    newDirectoryName = "video";

    // Full path of the new directory
    newDirectoryPath = currentDir.filePath(newDirectoryName);

    // Check if the directory already exists
    if (currentDir.exists(newDirectoryName)) {
        qDebug() << "The directory "+newDirectoryName+" already exists.";
    } else {
        // Create the directory
        if (currentDir.mkdir(newDirectoryName)) {
            qDebug() << "Directory "+newDirectoryName+"created successfully.";
        } else {
            qDebug() << "Failedto create the directory "+newDirectoryName+".";
        }
    }
}

void processIPRange(const QString& subnet, int startIP, int endIP, QVector<QString>& IPadress) {
    for (int i = startIP; i <= endIP; ++i) {
        QString ipAddress = subnet + QString::number(i);
        QTcpSocket socket;
        socket.connectToHost(QHostAddress(ipAddress), 554);

        if (socket.waitForConnected(15)) {
            qDebug() << "Dispositivo activo encontrado en la dirección IP:" << ipAddress;
            IPadress.push_back("rtsp://admin:detnov123@" + ipAddress + ":554/Streaming/Channels/101");
        }
    }
}

//get the subnet of the computer and try to ping all the devices inside the subnet with port 554
//this process is parallelized using QtConcurrent
void MainWindow::Get_ip_Adress() {
    QString subnet;
    int startIP = 2;
    int endIP = 255;

    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost){
            QTcpSocket socket;
            QString ipAddress = address.toString();
            int ultimoPunto = ipAddress.lastIndexOf('.');
                if (ultimoPunto != -1) {
                    subnet = ipAddress.left(ultimoPunto+1);
                    qDebug() << subnet;
            }
        }
    }
    QFuture<void> future = QtConcurrent::run(processIPRange, subnet, startIP, endIP, std::ref(IPadress));
    future.waitForFinished();

    IPadress.push_back("rtsp://98.103.59.189:554/stream1");
    IPadress.push_back("rtsp://zephyr.rtsp.stream/movie?streamKey=886c5aa49fc617eb17f5b8c145f9043c");
    IPadress.push_back("rtsp://131.128.103.88/");
    num_camera = IPadress.size();
}


void MainWindow::Start_Camera() {
    int row = 0;
    int col = 0;
    for(int i = 0; i< num_camera; ++i){
        //Set camera position
        QWidget * cameraWindow = new QWidget();
        cameraWindow->setUpdatesEnabled(false);
        //avoid the widget constantly actualize window when updating, resizing, scrolling  happened
//        cameraWindow->setAttribute(Qt::WA_OpaquePaintEvent);
        WId xwinid = cameraWindow->winId();
        ui->gridLayout_3->addWidget(cameraWindow, row, col);
        col++;
        if (col >= 2) {
            col = 0;
            row++;
        }
        //resolution
        int resolutionWidth = 960;
        int resolutionHeight = 480;
        //Init pipeline of the camera
        gst_init (nullptr, nullptr);
        GstElement* pipeline;
        GstElement *sink;
        GError* error = NULL;
        QString m_strPipeline = "gst-launch-1.0 -v rtspsrc \ "
                                       "location=" +IPadress[i]+" ! rtph264depay ! h264parse ! decodebin ! \
                                        videoconvert ! videoscale ! video/x-raw,width=" + QString::number(resolutionWidth) + " \
                                       ,height=" + QString::number(resolutionHeight) + " ! xvimagesink name=image_sink sync=false";


        const char* cStringUtf8 = m_strPipeline.toUtf8().constData();
        pipeline = gst_parse_launch (cStringUtf8, &error);
        sink = gst_bin_get_by_name(GST_BIN(pipeline), "image_sink");
        gst_video_overlay_set_window_handle (GST_VIDEO_OVERLAY (sink), guintptr(xwinid));

        // run the pipeline
        GstStateChangeReturn sret = gst_element_set_state (pipeline,
        GST_STATE_PLAYING);
        if (sret == GST_STATE_CHANGE_FAILURE) {
            qDebug() << "fail to start pipeline";
            gst_element_set_state (pipeline, GST_STATE_NULL);
            gst_object_unref (pipeline);
        }
    }
}


void MainWindow::Capture_Single_Img(){

    //Get parameters likes number_fotos, time, fps
    QString framerate = ui->framerate_imag->text();
    QString fotos = ui->num_fotos->text();
    int time = ui->time_img->text().toInt();
    int index_camera = ui->camara_selector_img->value() - 1;

    QDateTime currentTime = QDateTime::currentDateTime();
    QString formattedTime = currentTime.toString("yyyy-MM-dd_hh:mm:ss");
    QString directory = "cam"+QString::number(index_camera+1)+"_"+formattedTime;

    // Obtener la ruta completa del directorio "img" un nivel más abajo del directorio actual
    QString rutaImg = QDir::currentPath() + "/img";
    // Crear la ruta completa incluyendo el directorio a crear dentro de "img"
    QString rutaCompleta = rutaImg + "/" + directory;
    if (!QDir(rutaCompleta).exists())
    {
       QDir().mkdir(rutaCompleta);
     } else {
        qWarning() << "Dorectoy already exist";
     }

    QString m_strPipeline = "gst-launch-1.0 -e rtspsrc location=" + IPadress[index_camera] + " \
        ! rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! videorate ! \
        video/x-raw,framerate=" + framerate + "/1 ! jpegenc file-extension=jpg ! multifilesink \
        location=img/" + directory + "/cam" + QString::number(index_camera + 1) + "_%03d.jpg max-files=" + fotos;
 qDebug() << m_strPipeline;
    const char* pipelineDescr = m_strPipeline.toUtf8().constData();

    // Crear el pipeline
    GstElement *pipeline = gst_parse_launch(pipelineDescr, nullptr);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Time have to wait to record video
    QThread::sleep(time);

    // Detener y liberar el pipeline de GStreamer
    gst_element_send_event(pipeline, gst_event_new_eos());

    // Esperar a que el pipeline finalice (hasta que se alcance el estado NULL)
    GstStateChangeReturn ret = gst_element_get_state(pipeline, nullptr, nullptr, GST_CLOCK_TIME_NONE);

    gst_object_unref (pipeline);


    //LOAD IMG FROM DIRECTORY
    //------------------------------------------------------------------------------
    // Create a QDir object with the directory path
    QDir currentDir(rutaCompleta);

    // Filter the file list to contain only .jpg files
    QStringList jpgFiles = currentDir.entryList(QStringList() << "*.jpg", QDir::Files);
    int Num_fotos;
    if(fotos.toInt() == 0){
        Num_fotos = framerate.toInt()*time;
    }
    else Num_fotos= fotos.toInt();
    // Check if there are any .jpg files in the directory
    while (jpgFiles.size() != Num_fotos ) {
       jpgFiles = currentDir.entryList(QStringList() << "*.jpg", QDir::Files);
    }
    qDebug() << "There are JPG files in the directory.";
    //Save img in DB
    Load_Img_from_Dir(rutaCompleta, directory);
}

void MainWindow::Load_Img_from_Dir(QString DirPath, QString name)
{
    QDir dir(DirPath);
    if(!dir.exists()){
        qDebug() << "Path doesn't exists";
    }
    QString prefix = "cam";
    QStringList filters;
    filters << prefix+"*.jpg";
    QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files);
    CameraRepository *rep = CameraRepository::getInstance();
    foreach (const QFileInfo &fileInfo, fileList) {
        QString filePath = fileInfo.filePath();
        CameraImage img(name, filePath, QString("camera"), 0, 0);
        rep->insertCamaraImage(img);
        qDebug() << "Path of file:" << filePath;
    }
}


void MainWindow::Capture_all_Img(){
    //Getter
    QString framerate = ui->framerate_imag->text();
    QString fotos = ui->num_fotos->text();
    int time = ui->time_img->text().toInt();
    //Get actual time
    QDateTime currentTime = QDateTime::currentDateTime();
    QString formattedTime = currentTime.toString("yyyy-MM-dd_hh:mm:ss");

    //Vector of pipeline
    QVector<GstElement*> pipelines;

    for(int i = 0; i< num_camera; ++i){
        QString directory = "cam"+QString::number(i+1)+"_"+formattedTime;

        // Obtener la ruta completa del directorio "img" un nivel más abajo del directorio actual
        QString rutaImg = QDir::currentPath() + "/img";
        qDebug() << directory;
        // Crear la ruta completa incluyendo el directorio a crear dentro de "img"
        QString rutaCompleta = rutaImg + "/" + directory;
        if (!QDir(rutaCompleta).exists())
        {
           QDir().mkdir(rutaCompleta);
           qDebug() << "directory";
         } else {
            qWarning() << "Dorectoy already exist";
         }


        QString m_strPipeline = "gst-launch-1.0 -e rtspsrc location=" +IPadress[i]+" \
            ! rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! videorate ! \
            video/x-raw,framerate="+framerate+"/1 ! jpegenc ! multifilesink \
             location=img/"+directory+"/cam"+QString::number(i+1)+"_%03d.jpg max-files="+fotos;

        const char* pipelineDescr = m_strPipeline.toUtf8().constData();

        // Crear el pipeline
        GstElement *pipeline = gst_parse_launch(pipelineDescr, nullptr);
        pipelines.push_back(pipeline);
        gst_element_set_state(pipeline, GST_STATE_PLAYING);
    }
    // Time have to wait to record video
    QThread::sleep(time);

    // Detener y liberar el pipeline de GStreamer
    for(int i = 0; i< num_camera; ++i){
        gst_element_send_event(pipelines[i], gst_event_new_eos());
        gst_object_unref (pipelines[i]);
    }
}


void MainWindow::Record_Single_Video(){
    // Obtener el tiempo actual
    QDateTime currentTime = QDateTime::currentDateTime();
    // Convertir a QString en el formato deseado
    QString formattedTime = currentTime.toString("yyyy-MM-dd_hh:mm");

    QString framerate = ui->framerate_video->text();
    int frame = framerate.toInt();
    int time = ui->time_video->text().toInt();
    int index_camera = ui->camara_selector_video->value() - 1;

//        gchar *pipelineDescr = g_strdup_printf(
//            "gst-launch-1.0 -e ximagesrc xid=0x%lx ! video/x-raw,framerate=30/1 ! videoconvert ! filesink location=desktop.ogg",
//            xwinid
//        );

    QString m_strPipeline = "gst-launch-1.0 -e rtspsrc location=" +IPadress[index_camera]+" \
                            ! rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! videorate ! \
                            video/x-raw,framerate=" +QString::number(frame)+"/1 \
                            ! x264enc tune=zerolatency speed-preset=superfast ! h264parse ! mp4mux ! \
                            filesink location=video/"+formattedTime+"_cam"+QString::number(index_camera+1)+".mp4";


    const char* pipelineDescr = m_strPipeline.toUtf8().constData();

    // Crear el pipeline
    GstElement *pipeline = gst_parse_launch(pipelineDescr, nullptr);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Time have to wait to record video
    QThread::sleep(time);

    // Detener y liberar el pipeline de GStreamer
    gst_element_send_event(pipeline, gst_event_new_eos());
    gst_object_unref (pipeline);
}


void MainWindow::Record_All_Video(){
    QDateTime currentTime = QDateTime::currentDateTime();
    // Convertir a QString en el formato deseado
    QString formattedTime = currentTime.toString("yyyy-MM-dd_hh:mm");
    int time = ui->time_video->text().toInt();

    QVector<GstElement*> pipelines;
    for(int i = 0; i< num_camera; ++i){
        QString framerate = ui->framerate_video->text();
        int frame = framerate.toInt();
        QString m_strPipeline = "gst-launch-1.0 -e rtspsrc location=" +IPadress[i]+" \ "
                                "! rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! videorate ! video/x-raw,framerate=" +QString::number(frame)+"/1 \ "
                                "! x264enc tune=zerolatency speed-preset=superfast ! h264parse ! \
                                mp4mux ! filesink location=video/"+formattedTime+"_cam"+QString::number(i+1)+".mp4";

        qDebug() << m_strPipeline;
        const char* pipelineDescr = m_strPipeline.toUtf8().constData();

        // Crear el pipeline
        GstElement *pipeline = gst_parse_launch(pipelineDescr, nullptr);
        gst_element_set_state(pipeline, GST_STATE_PLAYING);
        //insert pipeline to vector
        pipelines.push_back(pipeline);
    }
    // Time that have to wait to record video
    QThread::sleep(time);

    // Detener y liberar el pipeline de GStreamer
    for(int i = 0; i< num_camera; ++i){
        gst_element_send_event(pipelines[i], gst_event_new_eos());
        gst_object_unref (pipelines[i]);
    }
}


void MainWindow::on_single_video_capture_clicked()
{
    Record_Single_Video();
    video_capture *video = new video_capture();
    video->show();
}

void MainWindow::on_All_Video_capture_clicked()
{
    Record_All_Video();
    video_capture *video = new video_capture();
    video->show();
}

void MainWindow::on_single_Image_capture_clicked()
{
    Capture_Single_Img();
    image_capture *img = new image_capture();
    img->ShowImage();
}

void MainWindow::on_All_Image_capture_clicked()
{
    Capture_all_Img();
    image_capture *img = new image_capture();
    img->ShowImage();
}

void MainWindow::on_pushButton_clicked()
{
    ImgManager *manager = new ImgManager();
    manager->show();
}
