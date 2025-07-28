// VectorMath - class for vector calculations
// SPDX-FileCopyrightText: Keith Paterson, 2022
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <array>

class SGGeod;

class VectorMath {
  public:
  /**Angles of inner tangent between two circles.*/
  static std::array<double, 2> innerTangentsAngle( SGGeod m1, SGGeod m2, double r1, double r2);
  /**Length of inner tangent between two circles.*/
  static double innerTangentsLength( SGGeod m1, SGGeod m2, double r1, double r2);
  /**Angles of outer tangent between two circles normalized to 0-360*/
  static std::array<double, 2> outerTangentsAngle( SGGeod m1, SGGeod m2, double r1, double r2);
  /**Length of outer tangent between two circles.*/
  static double outerTangentsLength( SGGeod m1, SGGeod m2, double r1, double r2);
};
