#include "networkhandle.h"

#include <curl/curl.h>

#include <QPair>
#include <QDebug>
#include <QFileInfo>
#include <QTextCodec>

bool NetworkHandle::GlobalInit()
{
    return !curl_global_init(CURL_GLOBAL_ALL);
}

void NetworkHandle::CleanUp()
{
    curl_global_cleanup();
}

QString NetworkHandle::Get(const QString &url, const QList<QPair<QString, QString> > &header, int timeout_ms)
{
    qDebug().noquote() << "Get Requset:"
                       << "url:" << url << "data";

    QByteArray urlB = url.toUtf8();
    QByteArray resB;
    CURL * curl = curl_easy_init();
    if (curl) {
        struct curl_slist *slist = NULL;

        curl_easy_setopt(curl, CURLOPT_URL, urlB.constData());

        QList<QPair<QString, QString> > newHeader = InitHeader(header);
        for (QList<QPair<QString, QString> >::const_iterator i = newHeader.begin(); i != newHeader.end(); ++i)
            slist = curl_slist_append(slist, (i->first + ": " + i->second).toUtf8());

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &qbytearray_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&resB);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout_ms);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

        CURLcode curl_code = curl_easy_perform(curl);

        if (curl_code != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(curl_code));

        curl_easy_cleanup(curl);
        curl_slist_free_all(slist);
    }

    qDebug().noquote() << "Response:" << QString::fromUtf8(resB);
    qDebug() << "\n";
    return QString::fromUtf8(resB);
}

QString NetworkHandle::Post(const QString &url, const QString request, const QList<QPair<QString, QString> > &header, int timeout_ms)
{
    qDebug().noquote() << "Post Requset:"
                       << "url:" << url << "data:" << request;

    QByteArray urlB = url.toUtf8();
    QByteArray reqB = request.toUtf8();
    QByteArray resB;
    CURL *curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, urlB.constData());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, reqB.constData());

        struct curl_slist *slist = NULL;

        QList<QPair<QString, QString> > newHeader = InitHeader(header);
        for (QList<QPair<QString, QString> >::const_iterator i = newHeader.begin(); i != newHeader.end(); ++i)
            slist = curl_slist_append(slist, (i->first + ": " + i->second).toUtf8());

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &qbytearray_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&resB);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout_ms);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

        CURLcode curl_code = curl_easy_perform(curl);

        if (curl_code != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(curl_code));

        curl_easy_cleanup(curl);
        curl_slist_free_all(slist);
    }

    qDebug().noquote() << "Response:" << QString::fromUtf8(resB);
    qDebug() << "\n";
    return QString::fromUtf8(resB);
}

QString NetworkHandle::UploadFiles(const QString &url, const QStringList &fileList, const QList<QPair<QString, QString> > &header, int timeout_ms)
{
    CURL *curl;
    QByteArray urlB = url.toUtf8();
    QByteArray resB;
    curl = curl_easy_init();
    if (curl) {

        curl_easy_setopt(curl, CURLOPT_URL, urlB.constData());

        struct curl_slist *slist = NULL;
        QList<QPair<QString, QString> > newHeader = InitHeader_UploadFile(header);
        for (QList<QPair<QString, QString> >::const_iterator i = newHeader.begin(); i != newHeader.end(); ++i)
            slist = curl_slist_append(slist, (i->first + ": " + i->second).toUtf8());

        curl_mime *form = curl_mime_init(curl);
        if(!form)
            return QString("error : create curl_mime failed");

        QByteArray arr;

        for(int i = 0; i < fileList.size(); ++i)
        {
            curl_mimepart * field = curl_mime_addpart(form);
            QFileInfo info(fileList[i]);

            //文件编码可能有问题，需要反复验证，
            curl_mime_name(field, "file");
            QTextCodec *codec = QTextCodec::codecForLocale();
            arr = codec->fromUnicode(fileList[i]);
            curl_mime_filedata(field, arr.constData());
            curl_mime_filename(field, fileList[i].toUtf8().constData());
        }

        curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);

        /* set our custom set of headers */
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &qbytearray_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&resB);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout_ms);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

        CURLcode curl_code = curl_easy_perform(curl);

        /* Check for errors */
        if (curl_code != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(curl_code));

        /* always cleanup */
        curl_easy_cleanup(curl);
        curl_mime_free(form);
        curl_slist_free_all(slist);
    }
    return QString::fromUtf8(resB);
}

bool NetworkHandle::DownloadFile(const QString &url, const QString &downloadFileName,
                                 const QString &postRes, const QList<QPair<QString, QString> > &header)
{
    QFile response(downloadFileName);
    if(response.exists())
        response.remove();
    response.open(QFile::WriteOnly);

    CURLcode res;

    QByteArray urlB = url.toUtf8();
    QByteArray reqB = postRes.toUtf8();
    CURL * curl = curl_easy_init();
    if(curl) {

        struct curl_slist *slist = NULL;
        QList<QPair<QString, QString> > newHeader = InitHeader(header);
        for (QList<QPair<QString, QString> >::const_iterator i = newHeader.begin(); i != newHeader.end(); ++i)
            slist = curl_slist_append(slist, (i->first + ": " + i->second).toUtf8());

        res = curl_easy_setopt(curl, CURLOPT_URL, urlB.constData());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, reqB.constData());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);//设定为不验证证书和HOST
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);

        res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, qfile_callback);
        res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);//

        res = curl_easy_perform(curl);

        if(res != CURLE_OK)
        {
            response.close();
            curl_easy_cleanup(curl);
            return false;
        }

        curl_easy_cleanup(curl);
        response.close();

    }else{
        response.close();
        return false;
    }

    return true;
}

size_t NetworkHandle::qbytearray_callback(void *ptr, size_t size, size_t nmemb, void *qs)
{
    QByteArray *s = (QByteArray *)qs;
    s->append(QByteArray((char *)ptr, size * nmemb));
    return size * nmemb;
}

size_t NetworkHandle::qfile_callback(void *ptr, size_t size, size_t nmemb, void *qf)
{
    QFile *f = (QFile *)qf;
    f->write((char *)ptr, size * nmemb);
    return size * nmemb;
}

QList<QPair<QString, QString> > NetworkHandle::InitHeader(const QList<QPair<QString, QString> > &header)
{
    QList<QPair<QString, QString> > returnList = header;
    QPair<QString, QString> f, s;

    f.first = QString("accept"); f.second = QString("*/*");
    s.first = QString("Content-Type"); s.second = QString("application/json");

    returnList << f << s;

    return returnList;
}

QList<QPair<QString, QString> > NetworkHandle::InitHeader_UploadFile(const QList<QPair<QString, QString> > &header)
{
    QList<QPair<QString, QString> > returnList = header;
    QPair<QString, QString> f, s;

    f.first = QString("accept"); f.second = QString("*/*");
    s.first = QString("Content-Type"); s.second = QString("multipart/form-data");

    returnList << f << s;

    return returnList;
}
