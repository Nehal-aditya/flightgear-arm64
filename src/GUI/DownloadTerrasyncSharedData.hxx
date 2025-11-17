#pragma once

// std
#include <deque>
#include <memory>

// Qt
#include <QThread>
#include <QTimer>
#include <QNetworkReply>
#include <QPointer>

#include <GUI/QtDNSClient.hxx>
#include <Main/options.hxx>

// forward decls
class QNetworkAccessManager;

namespace simgear { class ArchiveExtractor; }

class DownloadTerrasyncSharedData : public QThread
{
    Q_OBJECT
public:
    DownloadTerrasyncSharedData(QObject* pr, QNetworkAccessManager* nam);

    ~DownloadTerrasyncSharedData();

signals:

    void extractionError(QString file, QString msg);

    void installProgress(QString fileName, int percent);

    void downloadProgress(quint64 cur, quint64 total);

    void failed(QString message);

    // emitted when we start downloading a new archive, for user feedback
    void beginArchive(QString name);
protected:
     void run() override;

private slots:
    void onDownloadProgress(quint64 got, quint64 total);
    void onReplyFinished();
    void processBytes();
    void onNetworkError(QNetworkReply::NetworkError code);

private:
    struct DownloadItem
    {
        DownloadItem(QString uri, const SGPath& path, QString n) : 
            relativeURIPath(uri), terrasyncPath(path), archiveName(n)
        {}

        QString relativeURIPath;
        SGPath terrasyncPath;
        QString archiveName;
    };

    void startNextDownload();
    void updateProgress();

    QString m_serverUri;

    QNetworkAccessManager* m_networkManager = nullptr;
    QtDNSClient* m_dns = nullptr;
    std::mutex m_mutex;
    std::condition_variable m_bufferWait;
    QByteArray m_buffer;
    quint64 m_extractedBytes = 0;
    QTimer m_updateTimer;
    bool m_error = false;
    bool m_done = false;
    QPointer<QNetworkReply> m_download;
    std::unique_ptr<simgear::ArchiveExtractor> m_archive;
    bool m_haveFirstMByte = false;
    quint64 m_totalSize = 0;
    uint32_t m_pathPrefixLength = 0;

    // current item is the front one
    std::deque<DownloadItem> m_items; 
};

