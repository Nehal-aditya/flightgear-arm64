// SPDX-FileCopyrightText: 2007 Durk Talsma
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <set>

#include <simgear/xml/easyxml.hxx>

#include "groundnetwork.hxx"
#include <Airports/parking.hxx>

class FGGroundNetXMLLoader : public XMLVisitor
{
public:
    explicit FGGroundNetXMLLoader(FGGroundNetwork* gn);

    bool hasErrors() const
    {
        return _hasErrors;
    }

protected:
    virtual void startXML();
    virtual void endXML();
    virtual void startElement(const char* name, const XMLAttributes& atts);
    virtual void endElement(const char* name);
    virtual void data(const char* s, int len);
    virtual void pi(const char* target, const char* data);
    virtual void warning(const char* message, int line, int column);
    virtual void error(const char* message, int line, int column);

private:
    void startParking(const XMLAttributes& atts);
    void startNode(const XMLAttributes& atts);
    void startArc(const XMLAttributes& atts);

    FGGroundNetwork* _groundNetwork;

    // we set this flag if the ground-network has any problems
    bool _hasErrors = false;

    std::string value;

    // map from local (groundnet.xml) ids to parking instances
    typedef std::map<int, FGTaxiNodeRef> NodeIndexMap;
    NodeIndexMap _indexMap;

    // data integrity - watch for unreferenced nodes and duplicated edges
    typedef std::pair<int, int> IntPair;
    std::set<IntPair> _arcSet;

    std::set<FGTaxiNodeRef> _unreferencedNodes;

    // map from allocated parking position to its local push-back node
    // used to defer binding the push-back node until we've processed
    // all nodes
    typedef std::map<FGParkingRef, int> ParkingPushbackIndex;
    ParkingPushbackIndex _parkingPushbacks;
};
