#include "imgmanager.h"
#include "ui_imgmanager.h"

ImgManager::ImgManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImgManager)
{
    ui->setupUi(this);
    Load_SQL();
    connect(ui->tableWidget,SIGNAL(doubleClicked(QModelIndex)),this, SLOT(Load_Img(QModelIndex)));
}

ImgManager::~ImgManager()
{
    delete ui;
}

void ImgManager:: Load_Img(QModelIndex index){
    if (index.isValid()) {
        int row = index.row(); // Get the index of the selected row

        // Get the value of the ID in column 0 (adjust the column index as per your needs)
        int id = ui->tableWidget->item(row, 0)->data(Qt::DisplayRole).toInt();

        qDebug() << "ID value:" << id;
        // Perform necessary operations with the ID value

        //Get pixmap using ID of the img
        CameraRepository *Repository  =  CameraRepository::getInstance();
        CameraImage *img = new CameraImage();
        Repository->getCameraImage(id,img);
        QPixmap pix = img->getPixmap();
        ui->label->setPixmap(pix.scaled(ui->label->size(),
                                                       Qt::KeepAspectRatio,
                                                     Qt::SmoothTransformation));
    }
}
void ImgManager::Load_SQL()
{
    CameraController *rep = CameraController::getInstance();
    QSqlQuery query;
    rep->getQuery(&query);
    query.prepare("SELECT id_image, description FROM images WHERE type = 'camera'");
    if (query.exec()) {
        // Iterar a través de los resultados y agregarlos al QTableWidget
        int row = 0;
        while (query.next()) {
            QString id_image = query.value(0).toString();
            QString description = query.value(1).toString();

            QTableWidgetItem *id_item = new QTableWidgetItem(id_image);
            QTableWidgetItem *desc_item = new QTableWidgetItem(description);

            ui->tableWidget->insertRow(row);
            ui->tableWidget->setItem(row, 0, id_item);
            ui->tableWidget->setItem(row, 1, desc_item);
            QTableWidgetItem *item = ui->tableWidget->item(row, 0);
                    if (item) {
                        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                    }
            item = ui->tableWidget->item(row, 1);
                    if (item) {
                        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                    }
            row++;
        }
    } else {
        qDebug() << "Error en la consulta:" << query.lastError().text();
    }
    ui->tableWidget->resizeColumnsToContents();
}

void ImgManager::on_pushButton_clicked()
{
    QList<QTableWidgetItem*> selectedItems = ui->tableWidget->selectedItems();
    if (selectedItems.isEmpty()) {
        qDebug() << "USER DIDN'T SELECT ANY ROW";
    }
    else{
        int row = ui->tableWidget->currentRow();
        int id = ui->tableWidget->item(row, 0)->data(Qt::DisplayRole).toInt();
        CameraRepository *Repository  =  CameraRepository::getInstance();
        Repository->deleteCameraImage(id);
        ui->tableWidget->removeRow(row);
    }
}
