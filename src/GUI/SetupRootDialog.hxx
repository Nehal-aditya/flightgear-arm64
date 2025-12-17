// SetupRootDialog.hxx - part of GUI launcher using Qt5
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

#include <QDialog>
#include <QScopedPointer>
#include <QString>

#include <string>

#include <simgear/misc/sg_path.hxx>

#include "QtLauncher.hxx"

namespace Ui
{
    class SetupRootDialog;
}

class QNetworkAccessManager;

class SetupRootDialog : public QDialog
{
    Q_OBJECT
public:

    ~SetupRootDialog();

    static bool runDialog(bool usingDefaultRoot);

    static bool runUpdateDialog(bool usingDefaultRoot);

    static flightgear::SetupRootResult restoreUserSelectedRoot(SGPath& path);

    static void askRootOnNextLaunch();
    
    static QString rootPathKey();
private slots:

    void onBrowse();

    void onDownload();
    void onUpdate();
    void onSelectDownloadDir();
    void onUseDefaultDownloadDir();
    void updatePromptText();
private:
    enum PromptState {
        DefaultPathCheckFailed,
        ExplicitPathCheckFailed,
        VersionCheckFailed,
        NeedToUpdateDownloadedData,
        ManualChoiceRequested,
        ChoseInvalidLocation,
        ChoseInvalidVersion,
        ChoseInvalidArchive,
        DownloadingExtractingArchive,
        UpdatingViaTerrasync,
        DownloadFailed,
        ChoseInvalidDownloadLocation
    };

    SetupRootDialog(PromptState prompt, const SGPath&);

    bool locationIsWritable(QString path);

    static bool runDialog(PromptState prompt, const SGPath& checkedPath);

    static bool validatePath(QString path);
    static bool validateVersion(QString path);

    static bool defaultRootAcceptable();
    static bool downloadedDataAcceptable();
    static bool downloadedDataExistsButStale();

    void startSharedDataDownload();

    PromptState m_promptState;
    QScopedPointer<Ui::SetupRootDialog> m_ui;
    QString m_browsedPath;
    SGPath m_checkedPath;
    QString m_lastErrorMessage;
    QNetworkAccessManager* m_networkManager;
    QString m_archiveName;
};
