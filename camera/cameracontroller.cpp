#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QFile>
#include <QSqlTableModel>

#include "cameracontroller.h"
#include "camerarepository.h"

CameraController * CameraController::mapsController = nullptr;

/**
 * @brief   CameraController::getInstance
 *          Singleton pattern so this method allows to get the one and only instantiated object.
 *          If none has been created, instantiates one.
 * @return
 */
CameraController *CameraController::getInstance()
{
    if(!mapsController) {
        mapsController = new CameraController;
    }

    return mapsController;
}

/**
 * @brief   CameraController::deleteInstance
 *          Delete the instantiated object -if any-
 */
void CameraController::deleteInstance()
{
    delete mapsController;
    mapsController = nullptr;
}

bool CameraController::open(QString path)
{
    bool ret = true ;
    this->path = path;
    this->sqlMaps = QSqlDatabase::addDatabase("QSQLITE", DB_MAPS_CONNECTION);
    if(!sqlMaps.isOpen()) {
        QFileInfo fileInfo(path);
        if(!QDir(fileInfo.path()).exists()) {
            QDir().mkdir(fileInfo.path());
        }
        this->sqlMaps.setDatabaseName(path);
        ret = this->sqlMaps.open();
        if (!ret)
            qDebug() << "Unable to open database: " << this->sqlMaps.lastError().text();
        else {
            checkTables();
            setPragmas();                             // Mandatory to set FOREIGN KEY if used as is set to OFF by default
//            checkPragmas();
        }
    }

    return ret;
}

void CameraController::close()
{
    this->sqlMaps.removeDatabase(DB_MAPS_CONNECTION);
    this->sqlMaps.close();

//    CameraRepository::getInstance()->clearCache();                                // Free maps cache. Do not do it as this will cause the need to reload the cache creating problems in the model after being uploaded/downloaded. There is no memory leak as cache is reseted when another instalation is loaded

    emit closed();
}

CameraController::CameraController(QObject *parent) : QObject(parent)
{
}

CameraController::~CameraController()
{

}

/**
 * @brief   CameraController::checkTables
 *          Check integrity of tables created.
 *          If wrong tries to create them
 * @return  true if ok
 */
bool CameraController::checkTables()
{
    bool success = true;

    if(success){
        if(!checkCameraImageTable()) {
            success = createCameraImagesTable();
        }
    }

    return success;
}

/**
 * @brief   CameraController::setPragmas
 *          Needed if FOREIGN KEY is used
 * @return  true if success
 */
bool CameraController::setPragmas()
{
    QSqlQuery query;
    getQuery(&query);
    return query.exec("PRAGMA foreign_keys = ON");
}

/**
 * @brief   CameraController::disablePragmas
 *          Disables FOREIGN KEY to insert for example parent to negative values
 * @return  true if success
 */
bool CameraController::disablePragmas()
{
    QSqlQuery query;
    getQuery(&query);
    return query.exec("PRAGMA foreign_keys = OFF");                                                // Disables the foreign key. If not done, the drop stament will throw an error: too many levels of trigger recursion
}

/**
 * @brief   CameraController::checkCameraImageTable
 *          Checks that table exists and all columns aswell
 * @return  true if success
 */
bool CameraController::checkCameraImageTable()
{
    QSqlRecord record = this->sqlMaps.record("images");
    if(!record.isEmpty()) {
        if(!record.field("description").isValid()) {
            QSqlQuery query;
            getQuery(&query);

            if(!query.exec("ALTER TABLE images ADD COLUMN description INTEGER")) {
                qDebug() << "CameraController::checkCameraImageTable: error creating parent column" << query.lastError().text();
            }
        }
        if(!record.field("hash").isValid()) {
            QSqlQuery query;
            getQuery(&query);

            if(!query.exec("ALTER TABLE images ADD COLUMN hash INTEGER")) {
                qDebug() << "CameraController::checkCameraImageTable: error creating hash column" << query.lastError().text();
            }
        }
    }

    record = this->sqlMaps.record("images");
    if(!record.isEmpty()
            && record.field("id_image").isValid()
            && record.field("data").isValid()
            && record.field("compressed").isValid()
            && record.field("hash").isValid()
            && record.field("description").isValid()) {
        return true;
    }

    return false;
}

/**
 * @brief   CameraController::createCameraImagesTable
 *          Creates the images table with all related columns
 * @return  true if succeed
 */
bool CameraController::createCameraImagesTable()
{
    QSqlQuery query;
    getQuery(&query);

    QString queryStr = (QString("DROP TABLE IF EXISTS images"));
    bool ret = query.exec(queryStr);

    if(ret) {
        queryStr = QString("CREATE TABLE images (id_image INTEGER, data BLOB, compressed BOOLEAN, hash TEXT, description TEXT, type CHAR(30), PRIMARY KEY(id_image))");    // TODO: FOREIGN KEY id_entity?
        ret = query.exec(queryStr);
    }

    if(!ret) {
        qDebug() << "CameraController::createCameraImagesTable error:" << query.lastError().text();
    }

//    query->clear();

    return ret;
}


bool CameraController::isEmpty()
{
    if(sqlMaps.isValid() && sqlMaps.isOpen()) {
        QSqlQuery query;
        getQuery(&query);

        QString queryStr = (QString("SELECT id_entity FROM entities"));
        if(query.exec(queryStr) && query.next()) {
            QString queryStr = (QString("SELECT id_image FROM images"));            // At least 1 image is needed
            if(query.exec(queryStr) && query.next()) {
                return false;
            }
        }
    }

    return true;
}

void CameraController::beginTransaction()
{
    this->sqlMaps.transaction();
}

void CameraController::endTransaction()
{
    this->sqlMaps.commit();
}

void CameraController::getQuery(QSqlQuery *query)
{
    *query = QSqlQuery(this->sqlMaps);
}

/**
 * @brief   CameraController::format
 *          Drops and creates again the DB's tables
 *          DB is compresses to free unused space after deleting all the entries
 * @return  true if success
 */
bool CameraController::format()
{
    bool success =  createCameraImagesTable();
    if(success) {
        compress();
//        CameraRepository::getInstance()->initCache();                       // Caller must explicitly call to initCache after DB has been formated
    }
    return success;
}

/**
 * @brief   CameraController::compress
 *          Compress the DB.
 * @return  true if success
 */
bool CameraController::compress()
{
    bool success = false;
    QSqlQuery query;

    getQuery(&query);
    query.prepare("VACUUM");
    success = query.exec();
    if(!success) {
        qDebug() << "CameraController::compress error:" << query.lastError().type() << query.lastError().text();
    }

    return success;
}

/**
 * @brief   CameraController::remove
 *          Deletes DB from disk
 * @return  true if success
 */
bool CameraController::remove()
{
    return QFile::remove(this->path);
}
