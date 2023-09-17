#ifndef CameraImage_H
#define CameraImage_H

#include <QObject>
#include <QPixmap>

class CameraImage : public QObject
{
    Q_OBJECT
    static const int DEFAULT_COMPRESSION_LEVEL = 0;
public:
    CameraImage(QObject *parentId = nullptr);
    CameraImage(const QString name, const QString path, QString type, int parentId, const int compressionLevel = DEFAULT_COMPRESSION_LEVEL, QObject *parent = nullptr);

    bool isValid() const;

    QString getName() const;
    void setName(const QString &value);

    QByteArray getImage(bool raw = false) const;
    void setImage(const QByteArray &value);
    bool loadImage(const QString &path, int compressionLevel);
    QPixmap getPixmap() const;
    int getCompressionLevel() const;
    void setCompressionLevel(int value);
    int getParentId() const;
    void setParentId(int value);
    void setId(int value);
    int getId() const;
    QString getHash() const;
    void setHash(const QString &value);
    QString getType()const;
    void setType(QString value) ;


    CameraImage *clone(CameraImage *parent = nullptr);

private:
    // ATENTION: any new property must be cloned in clone function!!!!
    int id;
    QByteArray image;                                           // Compressed image
    int compressionLevel;                                       // Compression level
    QString name;
    int parentId;
    QString hash;
    QString entity;                                             // Entity associated
    int entityId;
    QString type;
};

#endif // CameraImage_H
