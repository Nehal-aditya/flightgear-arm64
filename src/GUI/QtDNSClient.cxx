
/*
 * SPDX-FileName: QtDNSClient.cxx
 * SPDX-FileComment: Qt wrapper around DNS client
 * SPDX-FileCopyrightText: Copyright (C) 2025  James Turner
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <QtDNSClient.hxx>

#include <algorithm>
#include <cassert>
#include <random>

#include <simgear/debug/logstream.hxx>
#include <simgear/math/sg_random.hxx>

QtDNSClient::QtDNSClient(QObject* pr, QString service) : QObject(pr),
                                                         m_serviceName(service)
{
    // always use Google for this one: it's too early to configure
    // and this should have a higher chance of success
    m_dnsClient.reset(new simgear::DNS::Client{"8.8.8.8"});
}

void QtDNSClient::makeDNSRequest()
{
    m_naptrRequest = new simgear::DNS::NAPTRRequest("terrasync.flightgear.org");
    m_naptrRequest->qservice = m_serviceName.toStdString();
    m_naptrRequest->qflags = "U";

    m_dnsClient->makeRequest(m_naptrRequest);

    connect(&m_updateTimer, &QTimer::timeout, this, &QtDNSClient::onUpdateDNSRequest);
    m_updateTimer.start();
}

void QtDNSClient::onUpdateDNSRequest()
{
    using namespace std::string_literals;

    m_dnsClient->update(0);
    if (m_naptrRequest->isTimeout()) {
        m_updateTimer.stop();
        emit failed(tr("Unable to query update servers from DNS."));
    }

    if (m_naptrRequest->isComplete()) {
        disconnect(&m_updateTimer, &QTimer::timeout, this, &QtDNSClient::onUpdateDNSRequest);

        if (m_naptrRequest->entries.empty()) {
            m_updateTimer.stop();
            emit failed(tr("No update servers are currently available."));
            return;
        }

        selectServer();
        emit finished();
    }
}

QString QtDNSClient::result() const
{
    if (m_results.empty()) {
        return {};
    }

    return m_results.front();
}

QStringList QtDNSClient::results() const
{
    return m_results;
}

void QtDNSClient::selectServer()
{
    assert(!m_naptrRequest->entries.empty());

    const auto order = m_naptrRequest->entries[0]->order;
    simgear::DNS::NAPTRRequest::NAPTR_list availableServers;

    for (const auto& entry : m_naptrRequest->entries) {
        if (entry->order != order)
            continue;

        const auto regex = entry->regexp;
        if (!simgear::strutils::starts_with(regex, "!^.*$!")) {
            SG_LOG(SG_TERRASYNC, SG_DEV_WARN, "ignoring unsupported regexp: " << regex);
            continue;
        }

        if (!simgear::strutils::ends_with(regex, "!")) {
            SG_LOG(SG_TERRASYNC, SG_DEV_WARN, "ignoring unsupported regexp: " << regex);
            continue;
        }

        // always use first entry
        if (availableServers.empty() || (entry->preference == availableServers[0]->preference)) {
            SG_LOG(SG_TERRASYNC, SG_DEBUG, "available server regexp: " << regex);
            availableServers.push_back(entry);
        }
    } // of initial entries iteration

    // permute
    auto rd = std::random_device{};
    auto rng = std::default_random_engine{rd()};
    std::shuffle(std::begin(availableServers), std::end(availableServers), rng);

    for (auto s : availableServers) {
        const auto server = s->regexp;
        std::string ret = server.substr(6, server.length() - 7); // trim off the regexp
        m_results.append(QString::fromStdString(ret));
    }
}
