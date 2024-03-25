#ifndef NETWORKHANDLE_H
#define NETWORKHANDLE_H

#include <QList>
#include <QPair>
#include <QString>

//文件编码可能有问题，需要反复验证，
class NetworkHandle
{
public:
    //全局初始化，在程序开始前尽量调用一次
    static bool GlobalInit();

    //全局清理
    static void CleanUp();

    static QString Get(const QString &url, const QList<QPair<QString, QString> > &header = QList<QPair<QString, QString> >(), int timeout_ms = 3000);
    static QString Post(const QString &url, const QString request, const QList<QPair<QString, QString> > &header = QList<QPair<QString, QString> >(), int timeout_ms = 3000);

    //上传文件
    static QString UploadFiles(const QString &url, const QStringList & fileList, const QList<QPair<QString, QString> > &header = QList<QPair<QString, QString> >(),
                               int timeout_ms = 3000);

    //下载文件
    static bool    DownloadFile(const QString &url, const QString & downloadFileName, const QString &postRes = QString(),
                                const QList<QPair<QString, QString> > &header = QList<QPair<QString, QString> >());
private:
    static size_t qbytearray_callback(void *ptr, size_t size, size_t nmemb, void *qs);
    static size_t qfile_callback(void *ptr, size_t size, size_t nmemb, void *qf);

    static QList<QPair<QString, QString> > InitHeader(const QList<QPair<QString, QString> > & header);
    static QList<QPair<QString, QString> > InitHeader_UploadFile(const QList<QPair<QString, QString> > & header);
};

#endif // NETWORKHANDLE_H
