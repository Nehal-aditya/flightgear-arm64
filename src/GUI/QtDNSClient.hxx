/*
 * SPDX-FileName: QtDNSClient.hxx
 * SPDX-FileComment: Qt wrapper around DNS client
 * SPDX-FileCopyrightText: Copyright (C) 2025  James Turner
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <memory>

#include <QObject>
#include <QString>
#include <QTimer>

#include <simgear/io/DNSClient.hxx>

class QtDNSClient : public QObject
{
    Q_OBJECT
public:
    QtDNSClient(QObject* pr, QString service);

    void makeDNSRequest();

    QString result() const;

    QStringList results() const;
signals:
    void finished();
    void failed(QString error);

private slots:
    void onUpdateDNSRequest();

private:
    void selectServer();

    QString m_serviceName;
    QStringList m_results;
    std::unique_ptr<simgear::DNS::Client> m_dnsClient;
    SGSharedPtr<simgear::DNS::NAPTRRequest> m_naptrRequest;
    QTimer m_updateTimer;
};
