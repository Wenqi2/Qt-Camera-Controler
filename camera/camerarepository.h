#ifndef CameraRepository_H
#define CameraRepository_H

#include "cameraimage.h"

#include <QObject>

class CameraRepository : public QObject
{
    Q_OBJECT
private:
    explicit CameraRepository(QObject *parent = nullptr);                                             // Declared as private for singleton pattern
    const int MAX_MAP_ENTITIES = 8000;                                                              // Recursive function createTree() can only handle 52298 items

public:
    static CameraRepository *getInstance();
    ~CameraRepository();

    static const int generalIconSize;
    static const int fixedIconSize;


    QPointF mapPointToPainted(QPointF point, QPointF ratio, bool *success = nullptr);
    QPointF mapPointFromPainted(QPointF point, QPointF ratio, bool *success = nullptr);
    bool unassign(int id);

    int insertCamaraImage(const CameraImage &image);
    bool updateCameraImage(const int id, const CameraImage &image);
    bool deleteCameraImage(int id);
    bool deleteCameraImage(QList<int> listId);
    bool getCameraImage(const int id, CameraImage *CameraImage);

    int getRootId();
    int findEntity(QString source);
    int findImage(QString hash);

    bool updateCameraImageId(int id, int imageId, QString description, bool updateEntities = false);
        int countCameraImage(const int id, const bool idIsEntity = false);



private:
    int maxChildren;                                                        // Maximum number of children allowed from a node
    int gridWidth;
    QString gridColor;

    int getFreeMapEntityId();
    int getFreeImageId();


signals:

public slots:

};

#endif // CameraRepository_H
