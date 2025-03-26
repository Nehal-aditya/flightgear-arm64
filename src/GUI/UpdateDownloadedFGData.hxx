#pragma once

// Qt
#include <QTimer>

#include <Main/options.hxx>

#include <simgear/io/HTTPClient.hxx>
#include <simgear/io/HTTPRepository.hxx>

using simgear::HTTPRepository;

class UpdateFGData : public QObject
{
    Q_OBJECT
public:
    UpdateFGData(QObject* pr) : QObject(pr)
    {
        m_updateTimer.setInterval(20);
        connect(&m_updateTimer, &QTimer::timeout, this, &UpdateFGData::onPeriodic);

        const auto rp = flightgear::Options::sharedInstance()->downloadedDataRoot();
     
        m_repo.reset(new simgear::HTTPRepository(rp, &m_http));
        m_repo->setBaseUrl("https://us1mirror.flightgear.org/terrasync/fgdata/fgdata_2024_1");

        m_repo->update();
        m_updateTimer.start();
    }

    void onPeriodic()
    {
        m_repo->process();
        m_http.update();
        
        
        if (!m_repo->isDoingSync()) {
            qInfo() << Q_FUNC_INFO << "finished sync";
            m_updateTimer.stop();
            emit finished();
        }

        const auto status = m_repo->failure();
        if (status != HTTPRepository::REPO_NO_ERROR) {
            m_error = true;
            QString errMsg = QString::fromStdString(m_repo->resultCodeAsString(status));
            m_updateTimer.stop();
            emit failed(errMsg);
        }

        const auto dlBytes = m_repo->bytesDownloaded();
        emit downloadProgress(dlBytes, dlBytes + m_repo->bytesToDownload());
    }

    ~UpdateFGData() = default;

signals:
    void finished();

    void extractionError(QString file, QString msg);

    void installProgress(QString fileName, int percent);

    void downloadProgress(quint64 cur, quint64 total);

    void failed(QString message);

private:
    QTimer m_updateTimer;

    std::unique_ptr<simgear::HTTPRepository> m_repo;
    simgear::HTTP::Client m_http;

    quint64 m_totalSize = 0;
    quint64 m_extractedBytes = 0;
    QUrl m_downloadUrl;

    bool m_done = false;
    bool m_error = false;
};

