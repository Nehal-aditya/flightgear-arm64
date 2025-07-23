// WakeMesh.hxx -- Mesh for the computation of a wing wake.
// Written by Bertrand Coconnier, started March 2017.
//
// SPDX-FileCopyrightText: (C) 2017  Bertrand Coconnier  - bcoconni@users.sf.net
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef _FG_WAKEMESH_HXX
#define _FG_WAKEMESH_HXX

#include <string>

#include <simgear/math/SGVec3.hxx>

#include "AeroElement.hxx"

namespace FGTestApi {
namespace PrivateAccessor {
namespace FDM {
class Accessor;
}
} // namespace PrivateAccessor
} // namespace FGTestApi


class WakeMesh : public SGReferenced
{
public:
    WakeMesh(double _span, double _chord, const std::string& aircraft_name);
    virtual ~WakeMesh();
    double computeAoA(double vel, double rho, double weight);
    SGVec3d getInducedVelocityAt(const SGVec3d& at) const;

protected:
    friend class FGTestApi::PrivateAccessor::FDM::Accessor;

    int nelm;
    double span, chord;
    std::vector<AeroElement_ptr> elements;
    double **influenceMtx, **Gamma;
};

typedef SGSharedPtr<WakeMesh> WakeMesh_ptr;

#endif
