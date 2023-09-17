#ifndef CameraController_H
#define CameraController_H

#include <QDir>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStandardPaths>

#define DB_MAPS_CONNECTION "imgconn3"
/**
 * @brief   The CameraController class
 *          Manages the data contained in SQLite DB
 *          Uses a singleton pattern to obtain a single instance
 *          Instances are obtained using the static public method getInstance()
 *          If the pointer needs to be destroyed. Use deleteInstance()
 */
class CameraController : public QObject
{
    Q_OBJECT
public:
    static CameraController *getInstance();
    static void deleteInstance();

    bool open(QString path);
    void close();
    bool format();
    bool compress();
    bool remove();
    void getQuery(QSqlQuery *query);
    bool isEmpty();
    void beginTransaction();
    void endTransaction();
    bool setPragmas();
    bool disablePragmas();

private:
    explicit CameraController(QObject *parent = nullptr);
    ~CameraController();
    bool checkTables();
    bool checkCameraImageTable();
    bool createCameraImagesTable();

    static CameraController *mapsController;                        // Singleton object
    QSqlDatabase sqlMaps;
    QString path;

signals:
    void closed();
};

#endif // CameraController_H
