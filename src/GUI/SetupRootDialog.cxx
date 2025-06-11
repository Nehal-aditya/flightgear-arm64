// SetupRootDialog.cxx - part of GUI launcher using Qt5
//
// Written by James Turner, started December 2014.
//
// Copyright (C) 2014 James Turner <zakalawe@mac.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "LaunchConfig.hxx"
#include "config.h"

#include "SetupRootDialog.hxx"

#include <condition_variable>
#include <mutex>

#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPointer>
#include <QSettings>
#include <QThread>
#include <QUrl>

#include "ui_SetupRootDialog.h"

#include <Main/globals.hxx>
#include <Main/fg_init.hxx>
#include <Main/options.hxx>
#include <Viewer/WindowBuilder.hxx>

#include "QtLauncher.hxx"
#include "SettingsWrapper.hxx"
#include "UpdateDownloadedFGData.hxx"
#include <GUI/QtDNSClient.hxx>

#include <simgear/io/iostreams/sgstream.hxx>
#include <simgear/io/untar.hxx>
#include <simgear/misc/sg_dir.hxx>

using namespace std::chrono_literals;

const quint32 static_basePackagePatchLevel = 2;

namespace {
    /**
     * @brief Calculates a progress percentage from the current and total values.
     *
     * Computes (current / total) × 100 using integer math. The result is clamped
     * to the range [0, 100] and rounded down. If @p total is zero, returns 0.
     *
     * @param current The current progress value (e.g. bytes downloaded).
     * @param total   The total value representing 100% completion.
     * @return An integer percentage between 0 and 100.
     */
    int calculateProgressPercentage(quint64 current, quint64 total) {

        constexpr int max_percent = 100;

        if (total == 0) {
            return 0;
        }

        // Compute percentage and clamp to avoid overflow or narrowing
        const quint64 percent = std::min((current * max_percent) / total, static_cast<quint64>(max_percent));

        return static_cast<int>(percent);
    }
}

class InstallFGDataThread : public QThread
{
    Q_OBJECT
public:
    InstallFGDataThread(QObject* pr, QNetworkAccessManager* nam) : QThread(pr),
                                                                   m_networkManager(nam),
                                                                   m_dns(new QtDNSClient(this, "dl_fgdata"))
    {
        const auto rp = flightgear::Options::sharedInstance()->downloadedDataRoot();
        // ensure we remove any existing data, since it failed validation
        if (rp.exists()) {
            simgear::Dir ed(rp);
            ed.remove(true);
        }

        m_downloadPath = rp.dirPath() / ("_download_data_" + std::to_string(FLIGHTGEAR_MAJOR_VERSION) + "_" + std::to_string(FLIGHTGEAR_MINOR_VERSION));
        m_downloadPath.set_cached(false);
        if (m_downloadPath.exists()) {
            simgear::Dir ed(m_downloadPath);
            ed.remove(true);
        }

        // +1 to include the leading /
        m_pathPrefixLength = m_downloadPath.utf8Str().length() + 1;

        connect(m_dns, &QtDNSClient::finished, [this]() {
            m_servers = m_dns->results();
            startRequest();
        });

        connect(m_dns, &QtDNSClient::failed, [this](QString msg) {
            m_error = true;
            emit failed(tr("Download of data files failed due to a DNS error: %1").arg(msg));
        });

        m_dns->makeDNSRequest();
    }

    void startRequest()
    {
        QString templateUrl = m_servers.front() + QStringLiteral("/release-%1/FlightGear-%2.%3-data.txz");
        // deal with SF download syntax
        if (templateUrl.startsWith("https://sourceforge.net/")) {
            templateUrl += QStringLiteral("/download");
        }

        QString majorMinorVersion = QString(FLIGHTGEAR_MAJOR_MINOR_VERSION);
        m_downloadUrl = QUrl(templateUrl.arg(majorMinorVersion).arg(majorMinorVersion).arg(static_basePackagePatchLevel));

        qInfo() << "Download URI:" << m_downloadUrl;

        QNetworkRequest req{m_downloadUrl};
        req.setMaximumRedirectsAllowed(5);
        // important to get correct behaviour form SourceForge
        req.setRawHeader("user-agent", "flighgtear-installer");

        m_download = m_networkManager->get(req);
        m_download->setReadBufferSize(64 * 1024 * 1024);

        connect(m_download, &QNetworkReply::downloadProgress, this, &InstallFGDataThread::onDownloadProgress);

        // lambda slot, but scoped to an object living on this thread.
        // this means the extraction work is done asynchronously with the
        // download
        connect(m_download, &QNetworkReply::readyRead, this, &InstallFGDataThread::processBytes);
        connect(m_download, &QNetworkReply::finished, this, &InstallFGDataThread::onReplyFinished);

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        connect(m_download, &QNetworkReply::errorOccurred, this, &InstallFGDataThread::onNetworkError);
#endif
        {
            std::unique_lock g(m_mutex);
            m_haveFirstMByte = false;
            m_buffer.clear();
        }

        // reset the archive too
        m_archive.reset(new simgear::ArchiveExtractor(m_downloadPath));
        m_archive->setRemoveTopmostDirectory(true);
        m_archive->setCreateDirHashEntries(true);
    }

    ~InstallFGDataThread()
    {
        if (!m_done) {
            m_error = true;
        }

        if (m_download) {
            m_download->deleteLater();
            m_download = nullptr;
        }

        wait();

        if (m_error) {
            simgear::Dir ed(m_downloadPath);
            ed.remove(true);
        }
    }


    void run() override
    {
        while (!m_error & !m_done) {
            QByteArray localBytes;
            {
                std::unique_lock g(m_mutex);
                if (m_buffer.isEmpty()) {
                    m_bufferWait.wait_for(g, 100ms);
                }

                // don't start passing bytes to the archive extractor, until we have 1MB
                // this is necssary to avoid passing redirect/404 page bytes in, and breaking
                // the extractor.
                if (!m_haveFirstMByte && (m_buffer.size() < 0x100000)) {
                    continue;
                } else {
                    m_haveFirstMByte = true;
                }

                // take at most 1MB
                localBytes = m_buffer.left(0x100000);
                m_buffer.remove(0, localBytes.length());
            }

            if (!localBytes.isEmpty()) {
                m_archive->extractBytes((const uint8_t*)localBytes.constData(), localBytes.size());
                m_extractedBytes += localBytes.size();
            }

            const int percent = calculateProgressPercentage(m_extractedBytes, m_totalSize);

            auto fullPathStr = m_archive->mostRecentExtractedPath().utf8Str();
            fullPathStr.erase(0, m_pathPrefixLength);

            emit installProgress(QString::fromStdString(fullPathStr), percent);

            if (m_archive->hasError()) {
                m_error = true;
                qWarning() << "Archive error";
            }

            if (m_archive->isAtEndOfArchive()) {
                // end the thread's event loop
                m_done = true;
            }
        }

        if (!m_error) {
            // create marker file for future updates
            {
                SGPath setupInfoPath = m_downloadPath / ".setup-info";
                sg_ofstream stream(setupInfoPath, std::ios::out | std::ios::binary);
                stream << m_downloadUrl.toString().toStdString();
            }

            const auto finalDataPath = flightgear::Options::sharedInstance()->downloadedDataRoot();
            SG_LOG(SG_IO, SG_INFO, "Renaming downloaded data to: " << finalDataPath);
            bool renamedOk = m_downloadPath.rename(finalDataPath);
            if (!renamedOk) {
                m_error = true;
            }
        }
    }

    void onNetworkError(QNetworkReply::NetworkError code)
    {
        SG_LOG(SG_IO, SG_WARN, "FGdata download failed, will re-try next mirror:" << code << " (" << m_download->errorString().toStdString() << ")");

        // don't need to delete, onReplyFinished will also fire

        m_servers.pop_front();
        if (m_servers.empty()) {
            m_error = true;
            emit failed(m_download->errorString());
        } else {
            startRequest();
            // will try a new request!
        }
    }

    void onDownloadProgress(quint64 got, quint64 total)
    {
        emit downloadProgress(got, total);
        m_totalSize = total;
    }

    void processBytes()
    {
        QByteArray bytes = m_download->readAll();
        {
            std::lock_guard g(m_mutex);
            m_buffer.append(bytes);
            m_bufferWait.notify_one();
        }
    }

    void onReplyFinished()
    {
        // we can't use m_download here because in the case of re-trying,
        // we already replaced m_download with our new request.
        QNetworkReply* r = qobject_cast<QNetworkReply*>(sender());
        r->deleteLater();

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
        if (r->error() != QNetworkReply::NoError) {
            onNetworkError(r->error());
        }
#endif
    }
signals:
    void extractionError(QString file, QString msg);

    void installProgress(QString fileName, int percent);

    void downloadProgress(quint64 cur, quint64 total);

    void failed(QString message);

private:
    QNetworkAccessManager* m_networkManager = nullptr;
    QtDNSClient* m_dns = nullptr;
    QStringList m_servers;
    std::mutex m_mutex;
    std::condition_variable m_bufferWait;
    QByteArray m_buffer;
    quint64 m_totalSize = 0;
    quint64 m_extractedBytes = 0;
    bool m_haveFirstMByte = false;
    QUrl m_downloadUrl;

    bool m_done = false;
    QPointer<QNetworkReply> m_download;
    SGPath m_downloadPath;
    std::unique_ptr<simgear::ArchiveExtractor> m_archive;
    bool m_error = false;
    uint32_t m_pathPrefixLength = 0;
};

/////////////////////////////////////////////////////////////////////////////////////////////

QString SetupRootDialog::rootPathKey()
{
    // return a settings key like fg-root-2018-3-0
    return QString("fg-root-%1-%2").arg(FLIGHTGEAR_MAJOR_VERSION).arg(FLIGHTGEAR_MINOR_VERSION);
}

SetupRootDialog::SetupRootDialog(PromptState prompt) :
    QDialog(),
    m_promptState(prompt)
{
    m_ui.reset(new Ui::SetupRootDialog);
    m_ui->setupUi(this);

    connect(m_ui->browseButton, &QPushButton::clicked,
             this, &SetupRootDialog::onBrowse);
    connect(m_ui->downloadButton, &QPushButton::clicked,
            this, &SetupRootDialog::onDownload);
    connect(m_ui->changeDownloadLocation, &QPushButton::clicked,
            this, &SetupRootDialog::onSelectDownloadDir);
    connect(m_ui->buttonBox, &QDialogButtonBox::rejected,
            this, &QDialog::reject);

    auto options = flightgear::Options::sharedInstance();
    if (options->isOptionSet("download-dir")) {
        // if download dir is set on the command line, don't allow changing it here
        m_ui->changeDownloadLocation->setEnabled(false);
    }

    m_ui->versionLabel->setText(tr("<h1>FlightGear %1</h1>").arg(FLIGHTGEAR_VERSION));
    m_ui->bigIcon->setPixmap(QPixmap(":/app-icon-large"));
    m_ui->contentsPages->setCurrentIndex(0);

    updatePromptText();

    if (prompt == NeedToUpdateDownloadedData) {
        m_ui->downloadButton->setText(tr("Update"));
        m_ui->changeDownloadLocation->setEnabled(false);
    }

    m_networkManager = new QNetworkAccessManager(this);
    m_networkManager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
}

bool SetupRootDialog::runDialog(bool usingDefaultRoot)
{
    SetupRootDialog::PromptState prompt =
        usingDefaultRoot ? DefaultPathCheckFailed : ExplicitPathCheckFailed;
    return runDialog(prompt);
}

bool SetupRootDialog::runUpdateDialog(bool usingDefaultRoot)
{
    SG_UNUSED(usingDefaultRoot);
    return runDialog(SetupRootDialog::PromptState::NeedToUpdateDownloadedData);
}

bool SetupRootDialog::runDialog(PromptState prompt)
{
    // avoid double Apple menu and other weirdness if both Qt and OSG
    // try to initialise various Cocoa structures.
    flightgear::WindowBuilder::setPoseAsStandaloneApp(false);

    SetupRootDialog dlg(prompt);
    dlg.exec();
    if (dlg.result() != QDialog::Accepted) {
        return false;
    }

    return true;
}


flightgear::SetupRootResult SetupRootDialog::restoreUserSelectedRoot(SGPath& sgpath)
{
    auto settings = flightgear::getQSettings();
    QString path = settings.value(rootPathKey()).toString();
    const bool ask = flightgear::checkKeyboardModifiersForSettingFGRoot();

    QString downloadDir = settings.value("download-dir").toString();
    if (!downloadDir.isEmpty()) {
        auto options = flightgear::Options::sharedInstance();
        options->setCustomDownloadDir(SGPath::fromUtf8(downloadDir.toStdString()));
    }

    if (ask || (path == QStringLiteral("!ask"))) {
        bool ok = runDialog(ManualChoiceRequested);
        if (!ok) {
            return flightgear::SetupRootResult::UserExit;
        }

        sgpath = globals->get_fg_root();
        return flightgear::SetupRootResult::UserSelected;
    }

    if (path.isEmpty()) {
        if (downloadedDataExistsButStale()) {
            bool ok = runDialog(NeedToUpdateDownloadedData);
            if (!ok) {
                return flightgear::SetupRootResult::UserExit;
            }

            // assume update worked, fall through
        } 

        return flightgear::SetupRootResult::UseDefault;
    }

    if (validatePath(path) && validateVersion(path)) {
        sgpath = SGPath::fromUtf8(path.toStdString());
        return flightgear::SetupRootResult::RestoredOk;
    }

    // we have an existing path but it's invalid.
    // let's see if the default root is acceptable, in which case we will
    // switch to it. (This gives a more friendly upgrade experience).
    if (defaultRootAcceptable()) {
        return flightgear::SetupRootResult::UseDefault;
    }

    if (downloadedDataAcceptable()) {
        return flightgear::SetupRootResult::UseDefault;
    }

    // okay, we don't have an acceptable FG_DATA anywhere we can find, we
    // have to ask the user what they want to do.
    bool ok = runDialog(VersionCheckFailed);
    if (!ok) {
        return flightgear::SetupRootResult::UserExit;
    }

    // run dialog sets fg_root, so this
    // behaviour is safe and correct.
    sgpath = globals->get_fg_root();
    return flightgear::SetupRootResult::UserSelected;
}

void SetupRootDialog::askRootOnNextLaunch()
{
    auto settings = flightgear::getQSettings();
    // set the option to the magic marker value
    settings.setValue(rootPathKey(), "!ask");
}

bool SetupRootDialog::validatePath(QString path)
{
    // check assorted files exist in the root location, to avoid any chance of
    // selecting an incomplete base package. This is probably overkill but does
    // no harm
    QStringList files = QStringList()
        << "version"
        << "defaults.xml"
        << "Materials/base/materials-base.xml"
        << "gui/menubar.xml"
        << "Timezone/zone.tab";

    QDir d(path);
    if (!d.exists()) {
        return false;
    }

    Q_FOREACH(QString s, files) {
        if (!d.exists(s)) {
            return false;
        }
    }

    return true;
}

/**
 * @brief Ensure the base pakcage at 'path' is the same or more recent than our
 * specified base package minimum version.
 * 
 * @param path : candidate base pakcage folder

 */
bool SetupRootDialog::validateVersion(QString path)
{
    std::string minBasePackageVersion = std::to_string(FLIGHTGEAR_MAJOR_VERSION) + "." + std::to_string(FLIGHTGEAR_MINOR_VERSION) + "." + std::to_string(static_basePackagePatchLevel);

    std::string ver = fgBasePackageVersion(SGPath::fromUtf8(path.toStdString()));

    // ensure major & minor fields match exactly
    if (simgear::strutils::compare_versions(minBasePackageVersion, ver, 2) != 0) {
        return false;
    }

    return simgear::strutils::compare_versions(minBasePackageVersion, ver) >= 0;
}

bool SetupRootDialog::defaultRootAcceptable()
{
    return false;

    SGPath r = flightgear::Options::sharedInstance()->platformDefaultRoot();
    QString defaultRoot = QString::fromStdString(r.utf8Str());
    return validatePath(defaultRoot) && validateVersion(defaultRoot);
}

bool SetupRootDialog::downloadedDataAcceptable()
{
    SGPath r = flightgear::Options::sharedInstance()->downloadedDataRoot();
    QString dlRoot = QString::fromStdString(r.utf8Str());
    return validatePath(dlRoot) && validateVersion(dlRoot);
}

bool SetupRootDialog::downloadedDataExistsButStale()
{
    SGPath r = flightgear::Options::sharedInstance()->downloadedDataRoot();
    QString dlRoot = QString::fromStdString(r.utf8Str());
    if (!validatePath(dlRoot)) {
        return false;
    }

    std::string minBasePackageVersion = std::to_string(FLIGHTGEAR_MAJOR_VERSION) + "." + std::to_string(FLIGHTGEAR_MINOR_VERSION) + "." + std::to_string(static_basePackagePatchLevel);
    std::string ver = fgBasePackageVersion(r);

    // major or minor mismatch, we can't use it
    // this 'should' be impossible given how we comput downloadedDataRoot
    if (simgear::strutils::compare_versions(minBasePackageVersion, ver, 2) != 0) {
        return false;
    }

    // update needed if the on-disk base package version is *lower* than static_basePackagePatchLevel
    return simgear::strutils::compare_versions(ver, minBasePackageVersion) < 0;
}

SetupRootDialog::~SetupRootDialog()
{

}

void SetupRootDialog::onBrowse()
{
    m_browsedPath = QFileDialog::getExistingDirectory(this,
                                                     tr("Choose FlightGear data folder"));
    if (m_browsedPath.isEmpty()) {
        return;
    }

    if (!validatePath(m_browsedPath)) {
        m_promptState = ChoseInvalidLocation;
        updatePromptText();
        return;
    }

    if (!validateVersion(m_browsedPath)) {
        m_promptState = ChoseInvalidVersion;
        updatePromptText();
        return;
    }

    globals->set_fg_root(m_browsedPath.toStdString());

    auto settings = flightgear::getQSettings();
    settings.setValue(rootPathKey(), m_browsedPath);

    accept(); // we're done
}

void SetupRootDialog::onSelectDownloadDir()
{
    auto settings = flightgear::getQSettings();

    auto dd = flightgear::Options::sharedInstance()->actualDownloadDir();
    const auto dlp = QString::fromStdString(dd.utf8Str());

    QString downloadDir = QFileDialog::getExistingDirectory(this,
                                                            tr("Choose location to store downloaded files."), dlp);
    if (downloadDir.isEmpty()) {
        return;
    }

    settings.setValue("download-dir", downloadDir);
    flightgear::Options::sharedInstance()->setOption("download-dir", downloadDir.toStdString());
    updatePromptText();
}

void SetupRootDialog::onDownload()
{
    // clear !ask value or custom root
    auto settings = flightgear::getQSettings();
    settings.remove(rootPathKey());

    if (m_promptState == NeedToUpdateDownloadedData) {
        onUpdate();
        return;
    }

    m_promptState = DownloadingExtractingArchive;
    updatePromptText();

    m_ui->contentsPages->setCurrentIndex(1);

    auto installThread = new InstallFGDataThread(this, m_networkManager);
    connect(installThread, &InstallFGDataThread::downloadProgress, this, [this](quint64 current, quint64 total) {
        m_ui->downloadProgress->setValue(current);
        m_ui->downloadProgress->setMaximum(total);

        const quint64 currentMb = current / (1024 * 1024);
        const quint64 totalMb = total / (1024 * 1024);

        const int percent = calculateProgressPercentage(current, total);

        m_ui->downloadText->setText(tr("Downloaded %1 of %2 MB (%3%)").arg(currentMb).arg(totalMb).arg(percent));
    });

    connect(installThread, &InstallFGDataThread::installProgress, this, [this](QString s, int percent) {
        m_ui->installText->setText(tr("Installation %1% complete.\nExtracting %2").arg(percent).arg(s));
        m_ui->installProgress->setValue(percent);
        // m_ui->installProgress->setMaximum(total);
    });

    connect(installThread, &InstallFGDataThread::failed, this, [this](QString s) {
        m_ui->downloadText->setText(tr("Download failed: %1").arg(s));
    });

    connect(installThread, &InstallFGDataThread::finished, this, [this]() {
        accept();
    });

    installThread->start();
}

void SetupRootDialog::onUpdate()
{
    m_promptState = UpdatingViaTerrasync;
    updatePromptText();

    m_ui->contentsPages->setCurrentIndex(1);

    auto updateThread = new UpdateFGData(this);
    connect(updateThread, &UpdateFGData::downloadProgress, this, [this](quint64 cur, quint64 total) {
        m_ui->downloadProgress->setValue(cur);
        m_ui->downloadProgress->setMaximum(total);

        const int curMb = cur / (1024 * 1024);
        const int totalMb = total / (1024 * 1024);
        const int percent = total > 0 ? ((cur * 100) / total) : 0;
        m_ui->downloadText->setText(tr("Downloaded %1 of %2 MB (%3%)").arg(curMb).arg(totalMb).arg(percent));
    });

    connect(updateThread, &UpdateFGData::installProgress, this, [this](QString s, int percent) {
        m_ui->installText->setText(tr("Update %1% complete.\nExtracting %2").arg(percent).arg(s));
        m_ui->installProgress->setValue(percent);
    });

    connect(updateThread, &UpdateFGData::failed, this, [this](QString s) {
        m_ui->downloadText->setText(tr("Update failed: %1").arg(s));
    });

    connect(updateThread, &UpdateFGData::finished, this, [this]() {
        accept();
    });
}

// void SetupRootDialog::onUseDefaults()
// {
//     SGPath r = flightgear::Options::sharedInstance()->platformDefaultRoot();
//     m_browsedPath = QString::fromStdString(r.utf8Str());
//     globals->set_fg_root(r);
//     auto settings = flightgear::getQSettings();
//     settings.remove(rootPathKey()); // remove any setting
//     accept();
// }

void SetupRootDialog::updatePromptText()
{
    QString t;
    QString curRoot = QString::fromStdString(globals->get_fg_root().utf8Str());
    switch (m_promptState) {
    case DefaultPathCheckFailed:
        t = tr("FlightGear needs to download additional data files. This can be done automatically by pressing 'Download', or you can download them yourself and select their location.");
        break;

    case ExplicitPathCheckFailed:
        t = tr("The requested location '%1' does not appear to be a valid set of data files for FlightGear").arg(curRoot);
        break;

    case VersionCheckFailed:
    {
        QString curVer = QString::fromStdString(fgBasePackageVersion(globals->get_fg_root()));
        t = tr("Detected incompatible version of the data files: version %1 found, but this is FlightGear %2. " \
               "(At location: '%3') " \
               "Please install or select a matching set of data files.").arg(curVer).arg(QString::fromLatin1(FLIGHTGEAR_VERSION)).arg(curRoot);
        break;
    }

    case ManualChoiceRequested:
        t = tr("Please select or download a copy of the FlightGear data files.");
        break;

    case ChoseInvalidLocation:
        t = tr("The chosen location (%1) does not appear to contain FlightGear data files. Please try another location.").arg(m_browsedPath);
        break;

    case ChoseInvalidVersion:
    {
        QString curVer = QString::fromStdString(fgBasePackageVersion(m_browsedPath.toStdString()));
        t = tr("The choosen location (%1) contains files for version %2, but this is FlightGear %3. " \
               "Please update or try another location").arg(m_browsedPath).arg(curVer).arg(QString::fromLatin1(FLIGHTGEAR_VERSION));
        break;
    }

    case ChoseInvalidArchive:
        t = tr("The chosen file (%1) is not a valid compressed archive.").arg(m_browsedPath);
        break;


    case DownloadingExtractingArchive:
        t = tr("Please wait while the data files are downloaded, extracted and verified.");
        break;


    case UpdatingViaTerrasync:
        t = tr("Please wait while the data files are updated and verified.");
        break;

    case NeedToUpdateDownloadedData:
        t = tr("The data files need to be updated to version %1. "
               "Please press 'Update', or if you prefer, manually download the correct data files and then select them.")
                .arg(QString::fromLatin1(FLIGHTGEAR_VERSION));
        break;
    }

    m_ui->promptText->setText(t);

    auto dd = flightgear::Options::sharedInstance()->actualDownloadDir();
    const auto dlp = QString::fromStdString(dd.utf8Str());
    m_ui->downloadLocationLabel->setText(tr("Data files will be downloaded to: %1").arg(dlp));
}

#include "SetupRootDialog.moc"
