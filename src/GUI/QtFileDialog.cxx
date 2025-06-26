// QtFileDialog.cxx - Qt5 implementation of FGFileDialog
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: Copyright (C) 2015 Rebecca Palmer <rebecca_palmer@zoho.com>

#include "QtFileDialog.hxx"
#include "QtLauncher.hxx"
#include <simgear/debug/logstream.hxx>

// Qt
#include <QFileDialog>
#include <QDir>
#include <QString>
#include <QStringList>

QtFileDialog::QtFileDialog(FGFileDialog::Usage use) :
    FGFileDialog(use)
{

}

QtFileDialog::~QtFileDialog() {}

void QtFileDialog::exec()
{
    // concatenate filter patterns, as Qt uses a single string
    std::string filter="";
    for( string_list::const_iterator it = _filterPatterns.begin(); it != _filterPatterns.end();++it ) {
        if(!filter.empty()){
            filter=filter+" ";
        }
        filter=filter+*it;
    }
    QFileDialog dlg(0,QString::fromStdString(_title),QString::fromStdString(_initialPath.utf8Str()),QString::fromStdString(filter));
    if (_usage==USE_SAVE_FILE) {
        dlg.setAcceptMode(QFileDialog::AcceptSave);
    }
    if (_usage==USE_CHOOSE_DIR) {
        dlg.setFileMode(QFileDialog::Directory);
    }
    if (_usage==USE_OPEN_FILE) {
        dlg.setFileMode(QFileDialog::ExistingFile);
    }
    dlg.setLabelText(QFileDialog::Accept,QString::fromStdString(_buttonText));
    dlg.selectFile(QString::fromStdString(_placeholder));
    if(_showHidden){
        dlg.setFilter(dlg.filter() | QDir::Hidden);
    }

    if(dlg.exec()){
        QStringList result = dlg.selectedFiles();
        if (!result.isEmpty()) {
            const auto sgp = SGPath::fromUtf8(result[0].toStdString());
            handleSelectedPath(sgp);
        }
    }
}

void QtFileDialog::close(){}

