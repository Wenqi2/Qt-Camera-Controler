#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QFile>
#include <QTime>

#include "cameracontroller.h"
#include "camerarepository.h"


const int CameraRepository::generalIconSize = 30;
const int CameraRepository::fixedIconSize = 40;

CameraRepository::CameraRepository(QObject *parent) : QObject(parent)
{

//    initSettings();                                                           // Needs to be called explicitly
//    initCache();                                                              // Needs to be called explicitly
}

CameraRepository *CameraRepository::getInstance()
{
    static CameraRepository *instance = nullptr;
    if (!instance) {
        instance = new CameraRepository();
    }
    return instance;
}

CameraRepository::~CameraRepository()
{

}

/**
 * @brief   CameraRepository::findImage
 *          Returns the image id associated to a hash
 * @param   hash
 * @return  image id if matches or -1 if not
 */
int CameraRepository::findImage(QString hash)
{
    int entityId = -1;
    CameraController *dbMaps = CameraController::getInstance();
    QSqlQuery query;
    dbMaps->getQuery(&query);

    query.prepare("SELECT id_image FROM images WHERE hash = ?");
    query.addBindValue(hash);

    if(query.exec()) {
        if(query.first()) {
            entityId = query.value(0).toInt();
        }
    } else {
        qDebug() << "CameraRepository::findImage" << query.lastError().text();
    }
//qDebug() << "CameraRepository::findImage" << hash << entityId;
    return entityId;
}

int CameraRepository::insertCamaraImage(const CameraImage &image)
{
    int freeId = -1;
    if(image.isValid()) {

        }
        freeId = getFreeImageId();
        qDebug() << freeId;
        if(freeId != -1) {
            CameraController *dbMaps = CameraController::getInstance();
            QSqlQuery query;
            dbMaps->getQuery(&query);

            query.prepare("INSERT INTO images (id_image, data, compressed, hash, description, type) "
                          "VALUES (?, ?, ?, ?, ?, ?)");
            query.addBindValue(freeId);
            query.addBindValue(image.getImage(true));                                   // stores the image without uncompressing it
            query.addBindValue(image.getCompressionLevel() != 0);                       // 0 means no compression at all
            query.addBindValue(QString::number(0));
            query.addBindValue(image.getName());
            query.addBindValue(image.getType());
            if(!query.exec()) {
                qDebug() << "CameraRepository::insertCameraImage error:" << query.lastError().text();
                freeId = -1;
            }
        }


    return freeId;
}

bool CameraRepository::updateCameraImage(const int id, const CameraImage &image)
{
    CameraController *dbMaps = CameraController::getInstance();
    QSqlQuery query;
    dbMaps->getQuery(&query);

    query.prepare("UPDATE images SET data = ?, compressed = ? WHERE id_image = ?");
    query.addBindValue(image.getImage(true));
    query.addBindValue(image.getCompressionLevel());
    query.addBindValue(id);

    bool success = query.exec();
    if(!success) {
        qDebug() << "CameraRepository::updateCameraImage error:" << query.lastError().text();
    }

    return success;
}

bool CameraRepository::deleteCameraImage(int id)
{
    CameraController *dbMaps = CameraController::getInstance();
    QSqlQuery query;
    dbMaps->getQuery(&query);

    query.prepare("DELETE FROM images WHERE id_image=:id_image");
    query.bindValue(":id_image", id);
    bool success = query.exec();
    if(!success) {
        qDebug() << "CameraRepository::deleteCameraImage error:" << query.lastError().text();
    }

    return success;
}

/**
 * @brief   CameraRepository::deleteCameraImage
 *          Removes entries from DB that matches with the IDs provided
 * @param   listId: int list of items to be removed
 * @return  true if success
 */
bool CameraRepository::deleteCameraImage(QList<int> listId)
{
    CameraController *dbMaps = CameraController::getInstance();
    QSqlQuery query;
    dbMaps->getQuery(&query);

    // Creates a comma separated list of unused IDs
    QString unusedString;
    for(auto id : listId) {
        unusedString += QString::number(id) + ',';
    }
    unusedString.chop(1);                                                         // Removes the last ','

    // Removes the selected list from DB
    QString sentence = QString("DELETE FROM images WHERE id_image in (%1)").arg(unusedString);
    query.prepare(sentence);
    bool success = query.exec();
    if(!success) {
        qDebug() << "CameraRepository::deleteCameraImage error:" << query.lastError().text();
    }

    return success;
}

bool CameraRepository::getCameraImage(const int id, CameraImage *CameraImage)
{
    CameraController *dbMaps = CameraController::getInstance();
    QSqlQuery query;
    dbMaps->getQuery(&query);

    query.prepare("SELECT id_image, data, compressed, hash, description FROM images WHERE id_image = ?");
    query.addBindValue(id);

    bool success = false;
    if(query.exec()) {
        if(query.next()) {
            (*CameraImage).setImage(query.value("data").toByteArray());
            (*CameraImage).setCompressionLevel(query.value("compressed").toBool() ? -1 : 0);       // If image is stored compressed, set default compression to the image
            (*CameraImage).setName(query.value("description").toString());
            (*CameraImage).setHash(query.value("hash").toString());
            success = true;
        }
    } else {
        qDebug() << "CameraRepository::getCameraImage error:" << query.lastError().text();
    }

    return success;
}

int CameraRepository::countCameraImage(const int id, const bool idIsEntity)
{
    CameraController *dbMaps = CameraController::getInstance();
    QSqlQuery query;
    dbMaps->getQuery(&query);

    QString comparison = QString(idIsEntity ? "IN (SELECT id_image FROM entities WHERE id_entity IS %1 AND id_image IS NOT -1)" : "IS %1").arg(id);
    QString sentence = QString("SELECT count() FROM entities WHERE id_image %1").arg(comparison);
    query.prepare(sentence);

    int count = -1;
    if(query.exec() && query.next()) {
        count = query.value("count()").toInt();
    } else {
        qDebug() << "CameraRepository::countCameraImage error:" << query.lastError().text();
    }

    return count;
}

/**
 * @brief   CameraRepository::getFreeImageId
 *          Obtains the first free id from tabla 'images'
 * @return  Free id or -1 if error
 */
int CameraRepository::getFreeImageId()
{
    int freeId = -1;
    CameraController *dbMaps = CameraController::getInstance();
    QSqlQuery query;
    dbMaps->getQuery(&query);

    query.prepare("SELECT * FROM ("
                   "SELECT id_image +1 AS id FROM images t1 WHERE NOT EXISTS (SELECT * FROM images t2 WHERE t1.id_image +1 = t2.id_image) "
                   "UNION SELECT 0 WHERE NOT EXISTS (SELECT * FROM images t3 WHERE t3.id_image = 0)) "
                   "ORDER BY id LIMIT 1");

    if(query.exec()) {
        if(query.first())
            freeId = query.value(0).toInt();
    }
    else
        qDebug() << "gCameraRepository::getFreeImageId" << query.lastError().text();

    return freeId;
}



