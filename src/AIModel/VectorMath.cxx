// VectorMath - class for vector calculations
// Written by Keith Paterson
// SPDX-FileCopyrightText: Keith Paterson 2022
// SPDX-License-Identifier: GPL-2.0-or-later

#include <simgear/debug/logstream.hxx>
#include <simgear/math/SGGeod.hxx>

#include "VectorMath.hxx"

std::array<double, 2> VectorMath::innerTangentsAngle( SGGeod m1, SGGeod m2, double r1, double r2) {
  std::array<double, 2> ret;
  double hypotenuse = SGGeodesy::distanceM(m1, m2);
  if (hypotenuse <= r1 + r2) {
      SG_LOG(SG_AI, SG_WARN, "innerTangentsAngle turn circles too near");
  }
  double opposite = r1 + r2;
  double angle = asin(opposite / hypotenuse) * SG_RADIANS_TO_DEGREES;
  double crs;
  if (r1>r2) {
     crs = SGGeodesy::courseDeg(m2, m1);
  } else {
     crs = SGGeodesy::courseDeg(m1, m2);
  }
  ret[0] = SGMiscd::normalizePeriodic(0, 360, crs - angle);
  ret[1] = SGMiscd::normalizePeriodic(0, 360, crs + angle);
  return ret;
}

double VectorMath::innerTangentsLength( SGGeod m1, SGGeod m2, double r1, double r2) {
    double hypotenuse = SGGeodesy::distanceM(m1, m2);
    if (hypotenuse <= r1 + r2) {
        SG_LOG(SG_AI, SG_WARN, "innerTangentsLength turn circles too near");
    }

  double opposite = r1 + r2;
  double angle = asin(opposite / hypotenuse) * SG_RADIANS_TO_DEGREES;
  double crs;
  if (r1>r2) {
     crs = SGGeodesy::courseDeg(m2, m1);
  } else {
     crs = SGGeodesy::courseDeg(m1, m2);
  }
  double angle1 = SGMiscd::normalizePeriodic(0, 360, crs - angle + 90);
  double angle2 = SGMiscd::normalizePeriodic(0, 360, crs - angle - 90);
  SGGeod p1 = SGGeodesy::direct(m1, angle1, r1);
  SGGeod p2 = SGGeodesy::direct(m2, angle2, r2);

  return SGGeodesy::distanceM(p1, p2);
}

std::array<double, 2> VectorMath::outerTangentsAngle( SGGeod m1, SGGeod m2, double r1, double r2) {
  std::array<double, 2> ret;
  double hypotenuse = SGGeodesy::distanceM(m1, m2);
  double radiusDiff = abs(r1 - r2);
  double beta = atan2(radiusDiff, hypotenuse) * SG_RADIANS_TO_DEGREES;
  double gamma = SGGeodesy::courseDeg(m1, m2);
  ret[0] = SGMiscd::normalizePeriodic(0, 360, gamma - beta);
  ret[1] = SGMiscd::normalizePeriodic(0, 360, gamma + beta);
  return ret;
}

double VectorMath::outerTangentsLength( SGGeod m1, SGGeod m2, double r1, double r2) {
    double hypotenuse = SGGeodesy::distanceM(m1, m2);
    double radiusDiff = abs(r1 - r2);
    double dist = sqrt(pow(hypotenuse, 2) - pow(radiusDiff, 2));
    return dist;
}
