#include <QFile>
#include <QDebug>
#include <QBuffer>
#include <QFileInfo>

#ifdef SCD250
    #include <tools.h>
#endif
#include "cameraimage.h"

CameraImage::CameraImage(QObject *parent) : QObject(parent)
{
    this->compressionLevel = DEFAULT_COMPRESSION_LEVEL;
}

/**
 * @brief   CameraImage::CameraImage
 *          Creates a CameraImage object from the path set and compress the data y compressionLevel is differen from 0
 *          If no name is set, then the file name is assigned
 *          When compression level is set to max -9-
 *              - JPG image of 194443 is compressed to 183853 (5% space saving) but this will depend on the image and its compression algorithms
 *              - PNG image of 882 is stored as 897 bytes length
 * @param   name: name of the image
 * @param   path: file to read image, it can be external path like '/home/mahg/sync/db/Floor-Plans.jpg' or internal resources like ":/icons/devices/button.png"
 * @param   compressionLevel: 0 -default- means no compression at all. Max value is 9. -1 to set default qCompress value
 */
CameraImage::CameraImage(const QString name, const QString path, QString type, int parentId, const int compressionLevel, QObject *parent)
    : QObject(parent), compressionLevel(compressionLevel), name(name), parentId(parentId), type(type)
{
    if(name.isEmpty()) {
        QFileInfo fileInfo(path);
        this->name = fileInfo.baseName();
    }
    qDebug() << name;
    loadImage(path, compressionLevel);
}

/**
 * @brief   CameraImage::isValid
 *          Check whether the image is valir or not
 * @return  true if valid
 */
bool CameraImage::isValid() const
{
    return !this->image.isEmpty();
}

QString CameraImage::getName() const
{
    return name;
}

void CameraImage::setName(const QString &value)
{
    name = value;
}

QByteArray CameraImage::getImage(bool raw) const
{
    return (this->compressionLevel && !raw) ? qUncompress(this->image) : this->image;
}

void CameraImage::setImage(const QByteArray &value)
{
    image = value;
}

bool CameraImage::loadImage(const QString &path, int compressionLevel)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        this->image.clear();
        return false;
    }

    this->image = this->compressionLevel ? qCompress(file.readAll(), compressionLevel) : file.readAll();

qDebug() << "CameraImage::loadImage" << path << "original size:" << file.size() << "stored:" << this->image.size();
    return true;
}

QPixmap CameraImage::getPixmap() const
{
    QPixmap pixmap;
    pixmap.loadFromData(getImage());
    return pixmap;
}

int CameraImage::getCompressionLevel() const
{
    return compressionLevel;
}

void CameraImage::setCompressionLevel(int value)
{
    compressionLevel = value;
}

int CameraImage::getParentId() const
{
    return parentId;
}

void CameraImage::setParentId(int value)
{
    parentId = value;
}

QString CameraImage::getType() const
{
    return type;
}

void CameraImage::setType(QString value){
    type = value;
}
CameraImage *CameraImage::clone(CameraImage *parent)
{
    CameraImage *clon = new CameraImage(parent);
    clon->id = this->id;
    clon->name = this->name;
    clon->parentId = this->parentId;
    clon->image = this->image;
    clon->compressionLevel = this->compressionLevel;
    clon->hash = this->hash;
    clon->type = this->type;

    return clon;
}


QString CameraImage::getHash() const
{
    return hash;
}

void CameraImage::setHash(const QString &value)
{
    hash = value;
}

int CameraImage::getId() const
{
    return id;
}

void CameraImage::setId(int value)
{
    id = value;
}
