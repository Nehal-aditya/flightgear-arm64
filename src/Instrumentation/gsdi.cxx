// gsdi.cxx - Ground Speed Drift Angle Indicator (known as GSDI or GSDA)
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2006 Melchior Franz

#include <config.h>

#include <simgear/sg_inlines.h>
#include <simgear/constants.h>

#include <Main/fg_props.hxx>
#include "gsdi.hxx"


/*
 * Failures or inaccuracies are currently not modeled due to lack of data.
 * The Doppler based GSDI should output unreliable data with increasing
 * pitch, roll, vertical acceleration and altitude-agl.
 */


GSDI::GSDI(SGPropertyNode *node) :
	_name(node->getStringValue("name", "gsdi")),
	_num(node->getIntValue("number", 0))
{
}


GSDI::~GSDI()
{
}


void GSDI::init()
{
  std::string branch;
	branch = "/instrumentation/" + _name;
	SGPropertyNode *n = fgGetNode(branch, _num, true);
	_serviceableN = n->getNode("serviceable", true);

	// input
	_ubodyN = fgGetNode("/velocities/uBody-fps", true);
	_vbodyN = fgGetNode("/velocities/vBody-fps", true);

	// output
	_drift_uN = n->getNode("drift-u-kt", true);
	_drift_vN = n->getNode("drift-v-kt", true);
	_drift_speedN = n->getNode("drift-speed-kt", true);
	_drift_angleN = n->getNode("drift-angle-deg", true);
}


void GSDI::update(double /*delta_time_sec*/)
{
	if (!_serviceableN->getBoolValue())
		return;

	double u = _ubodyN->getDoubleValue() * SG_FPS_TO_KT;
	double v = _vbodyN->getDoubleValue() * SG_FPS_TO_KT;

	double speed = sqrt(u * u + v * v);
	double angle = atan2(v, u) * SGD_RADIANS_TO_DEGREES;

	_drift_uN->setDoubleValue(u);
	_drift_vN->setDoubleValue(v);
	_drift_speedN->setDoubleValue(speed);
	_drift_angleN->setDoubleValue(angle);
}


// Register the subsystem.
#if 0
SGSubsystemMgr::InstancedRegistrant<GSDI> registrantGSDI(
    SGSubsystemMgr::FDM,
    {{"instrumentation", SGSubsystemMgr::Dependency::HARD}});
#endif

// end of gsdi.cxx
