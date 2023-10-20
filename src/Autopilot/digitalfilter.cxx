// digitalfilter.cxx - a selection of digital filters
//
// Written by Torsten Dreyer
// Based heavily on work created by Curtis Olson, started January 2004.
//
// Copyright (C) 2004  Curtis L. Olson  - http://www.flightgear.org/~curt
// Copyright (C) 2010  Torsten Dreyer - Torsten (at) t3r (dot) de
//
// Washout/high-pass filter, lead-lag filter and integrator added.
// low-pass and lag aliases added to Exponential filter,
// rate-limit added.   A J Teeder 2013
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
//

#include "digitalfilter.hxx"
#include <GUI/Highlight.hxx>
#include <Main/globals.hxx>

#include <deque>
#include <algorithm>

#include <simgear/misc/strutils.hxx>
#include <simgear/scene/model/SGIKVariable.hxx>

namespace FGXMLAutopilot
{

/**
 *
 *
 */
class DigitalFilterImplementation:
  public SGReferenced
{
  public:
    virtual ~DigitalFilterImplementation() {}
    DigitalFilterImplementation();
    virtual void   initialize( double initvalue ) {}
    virtual double compute( double dt, double input ) = 0;
    // value/deriv initially set to that of input - reference
    virtual void   evalDerivative(double& value, double deriv[2],
                                  const SGPropertyNode* wrt,
                                  const simgear::expression::Binding* b) const
    {
        std::cout << "DigitalFilterImplementation::evalDerivative " << (wrt ? wrt->getPath() : "-") << std::endl;
    }
    // wrt input - reference
    // breaks are in output space
    virtual SGItBreaks evalIterativeBreaks(double input,
                                           const simgear::expression::Binding* b,
                                           const SGItBreaks& breaks) const
    {
        std::cout << "DigitalFilterImplementation::evalIterativeBreaks" << std::endl;
        return breaks;
    }
    // Does the output change over time with constant input?
    virtual bool isKinematic() const
    {
        return false;
    }
    virtual double reverse( double output )
    {
        // FIXME unused
        return output;
    }
    virtual bool configure( SGPropertyNode& cfg_node,
                            const std::string& cfg_name,
                            SGPropertyNode& prop_root ) = 0;

    void setDigitalFilter( DigitalFilter * digitalFilter ) { _digitalFilter = digitalFilter; }
    virtual void collectDependentProperties(std::set<const SGPropertyNode*>& props) const = 0;
  protected:
    DigitalFilter * _digitalFilter = nullptr;
};

/* --------------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------------- */
class GainFilterImplementation : public DigitalFilterImplementation {
protected:
    simgear::ValueList _gainInput;
    bool configure(SGPropertyNode& cfg_node,
                   const std::string& cfg_name,
                   SGPropertyNode& prop_root);
public:
  GainFilterImplementation() : _gainInput(1.0) {}
  double compute(  double dt, double input );
  void evalDerivative(double& value, double deriv[2],
                      const SGPropertyNode* wrt,
                      const simgear::expression::Binding* b) const override;
  SGItBreaks evalIterativeBreaks(double input,
                                 const simgear::expression::Binding* b,
                                 const SGItBreaks& breaks) const override;
  virtual void collectDependentProperties(std::set<const SGPropertyNode*>& props) const
  {
    _gainInput.collectDependentProperties(props);
  }
};

class ReciprocalFilterImplementation : public GainFilterImplementation {
public:
  double compute(  double dt, double input );
  void evalDerivative(double& value, double deriv[2],
                      const SGPropertyNode* wrt,
                      const simgear::expression::Binding* b) const override;
};

class DerivativeFilterImplementation : public GainFilterImplementation {
  simgear::ValueList _TfInput;
  double _input_1;
  bool configure( SGPropertyNode& cfg_node,
                  const std::string& cfg_name,
                  SGPropertyNode& prop_root );
public:
  DerivativeFilterImplementation();
  double compute(  double dt, double input );
  void evalDerivative(double& value, double deriv[2],
                      const SGPropertyNode* wrt,
                      const simgear::expression::Binding* b) const override;
  virtual bool isKinematic() const override
  {
      return true;
  }
  virtual void initialize( double initvalue );
  virtual void collectDependentProperties(std::set<const SGPropertyNode*>& props) const
  {
    GainFilterImplementation::collectDependentProperties(props);
    _TfInput.collectDependentProperties(props);
  }
};

class ExponentialFilterImplementation : public GainFilterImplementation {
protected:
    simgear::ValueList _TfInput;
    bool configure(SGPropertyNode& cfg_node,
                   const std::string& cfg_name,
                   SGPropertyNode& prop_root);
    bool _isSecondOrder;
    double _output_1, _output_2;
public:
  ExponentialFilterImplementation();
  double compute(  double dt, double input );
  void evalDerivative(double& value, double deriv[2],
                      const SGPropertyNode* wrt,
                      const simgear::expression::Binding* b) const override;
  virtual bool isKinematic() const override
  {
      return true;
  }
  virtual void initialize( double initvalue );
  virtual void collectDependentProperties(std::set<const SGPropertyNode*>& props) const
  {
    GainFilterImplementation::collectDependentProperties(props);
    _TfInput.collectDependentProperties(props);
  }
};

class MovingAverageFilterImplementation : public DigitalFilterImplementation {
protected:
    simgear::ValueList _samplesInput;
    double _output_1;
    std::deque<double> _inputQueue;
    bool configure(SGPropertyNode& cfg_node,
                   const std::string& cfg_name,
                   SGPropertyNode& prop_root);
public:
  MovingAverageFilterImplementation();
  double compute(  double dt, double input );
  void evalDerivative(double& value, double deriv[2],
                      const SGPropertyNode* wrt,
                      const simgear::expression::Binding* b) const override;
  virtual void initialize( double initvalue );
  virtual void collectDependentProperties(std::set<const SGPropertyNode*>& props) const
  {
    _samplesInput.collectDependentProperties(props);
  }
};

class NoiseSpikeFilterImplementation : public DigitalFilterImplementation {
protected:
  double _output_1;
  simgear::ValueList _rateOfChangeInput;
  bool configure( SGPropertyNode& cfg_node,
                  const std::string& cfg_name,
                  SGPropertyNode& prop_root );
public:
  NoiseSpikeFilterImplementation();
  double compute(  double dt, double input );
  void evalDerivative(double& value, double deriv[2],
                      const SGPropertyNode* wrt,
                      const simgear::expression::Binding* b) const override;
  virtual bool isKinematic() const override
  {
      return true;
  }
  virtual void initialize( double initvalue );
  virtual void collectDependentProperties(std::set<const SGPropertyNode*>& props) const
  {
    _rateOfChangeInput.collectDependentProperties(props);
  }
};

class RateLimitFilterImplementation : public DigitalFilterImplementation {
protected:
  double _output_1;
  simgear::ValueList _rateOfChangeMax;
  simgear::ValueList _rateOfChangeMin;
  bool configure( SGPropertyNode& cfg_node,
                  const std::string& cfg_name,
                  SGPropertyNode& prop_root );
public:
  RateLimitFilterImplementation();
  double compute(  double dt, double input );
  void evalDerivative(double& value, double deriv[2],
                      const SGPropertyNode* wrt,
                      const simgear::expression::Binding* b) const override;
  virtual bool isKinematic() const override
  {
      return true;
  }
  virtual void initialize( double initvalue );
  virtual void collectDependentProperties(std::set<const SGPropertyNode*>& props) const
  {
    _rateOfChangeMax.collectDependentProperties(props);
    _rateOfChangeMin.collectDependentProperties(props);
  }
};

class IntegratorFilterImplementation : public GainFilterImplementation {
protected:
    simgear::ValueList _TfInput;
    simgear::ValueList _minInput;
    simgear::ValueList _maxInput;
    double _input_1;
    double _output_1;
    bool configure(SGPropertyNode& cfg_node,
                   const std::string& cfg_name,
                   SGPropertyNode& prop_root);
public:
  IntegratorFilterImplementation();
  double compute(  double dt, double input );
  void evalDerivative(double& value, double deriv[2],
                      const SGPropertyNode* wrt,
                      const simgear::expression::Binding* b) const override;
  virtual bool isKinematic() const override
  {
      return true;
  }
  virtual void initialize( double initvalue );
  virtual void collectDependentProperties(std::set<const SGPropertyNode*>& props) const
  {
    GainFilterImplementation::collectDependentProperties(props);
    _TfInput.collectDependentProperties(props);
    _minInput.collectDependentProperties(props);
    _maxInput.collectDependentProperties(props);
  }
};

// integrates x" + ax' + bx + c = 0
class DampedOscillationFilterImplementation : public GainFilterImplementation {
protected:
    simgear::ValueList _aInput;
    simgear::ValueList _bInput;
    simgear::ValueList _cInput;
    double _x2;
    double _x1;
    double _x0;
    bool configure(SGPropertyNode& cfg_node,
                   const std::string& cfg_name,
                   SGPropertyNode& prop_root);
public:
  DampedOscillationFilterImplementation();
  double compute(  double dt, double input );
  void evalDerivative(double& value, double deriv[2],
                      const SGPropertyNode* wrt,
                      const simgear::expression::Binding* b) const override;
  virtual bool isKinematic() const override
  {
      return true;
  }
  virtual void initialize( double initvalue );
  virtual void collectDependentProperties(std::set<const SGPropertyNode*>& props) const
  {
    GainFilterImplementation::collectDependentProperties(props);
    _aInput.collectDependentProperties(props);
    _bInput.collectDependentProperties(props);
    _cInput.collectDependentProperties(props);
  }
};

class HighPassFilterImplementation : public GainFilterImplementation {
protected:
    simgear::ValueList _TfInput;
    double _input_1;
    double _output_1;
    bool configure(SGPropertyNode& cfg_node,
                   const std::string& cfg_name,
                   SGPropertyNode& prop_root);
public:
  HighPassFilterImplementation();
  double compute(  double dt, double input );
  void evalDerivative(double& value, double deriv[2],
                      const SGPropertyNode* wrt,
                      const simgear::expression::Binding* b) const override;
  virtual bool isKinematic() const override
  {
      return true;
  }
  virtual void initialize( double initvalue );
  virtual void collectDependentProperties(std::set<const SGPropertyNode*>& props) const
  {
    GainFilterImplementation::collectDependentProperties(props);
    _TfInput.collectDependentProperties(props);
  }
};
class LeadLagFilterImplementation : public GainFilterImplementation {
protected:
    simgear::ValueList _TfaInput;
    simgear::ValueList _TfbInput;
    double _input_1;
    double _output_1;
    bool configure(SGPropertyNode& cfg_node,
                   const std::string& cfg_name,
                   SGPropertyNode& prop_root);
public:
  LeadLagFilterImplementation();
  double compute(  double dt, double input );
  void evalDerivative(double& value, double deriv[2],
                      const SGPropertyNode* wrt,
                      const simgear::expression::Binding* b) const override;
  virtual bool isKinematic() const override
  {
      return true;
  }
  virtual void initialize( double initvalue );
  virtual void collectDependentProperties(std::set<const SGPropertyNode*>& props) const
  {
    GainFilterImplementation::collectDependentProperties(props);
    _TfaInput.collectDependentProperties(props);
    _TfbInput.collectDependentProperties(props);
  }
};

class CoherentNoiseFilterImplementation : public DigitalFilterImplementation
{
protected:
    simgear::ValueList _amplitude;

    std::vector<double> _discreteValues;
    bool _absoluteVal = false;
    size_t _numDiscreteValues = 1024;

    bool configure(SGPropertyNode& cfg_node,
                   const std::string& cfg_name,
                   SGPropertyNode& prop_root) override;

public:
    CoherentNoiseFilterImplementation();
    double compute(double dt, double input) override;
    void evalDerivative(double& value, double deriv[2],
                        const SGPropertyNode* wrt,
                        const simgear::expression::Binding* b) const override;
    void initialize(double initvalue) override;
  virtual void collectDependentProperties(std::set<const SGPropertyNode*>& props) const
  {
    _amplitude.collectDependentProperties(props);
  }
};

/* --------------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------------- */

} // namespace FGXMLAutopilot

using namespace FGXMLAutopilot;

//------------------------------------------------------------------------------
DigitalFilterImplementation::DigitalFilterImplementation() :
  _digitalFilter(NULL)
{

}

//------------------------------------------------------------------------------
double GainFilterImplementation::compute(  double dt, double input )
{
  return _gainInput.get_value() * input;
}

void GainFilterImplementation::evalDerivative(double& value, double deriv[2],
                                              const SGPropertyNode* wrt,
                                              const simgear::expression::Binding* b) const
{
    double gainVal, gainDeriv[2];
    _gainInput.evalDerivative(gainVal, gainDeriv, wrt, b);
    if (wrt) {
        // Product rule: d(u*v)/dx = v*du/dx + u*dv/dx
        deriv[0] = deriv[0]*gainVal + gainDeriv[0]*value;
        deriv[1] = deriv[1]*gainVal + gainDeriv[1]*value;
    }
    value *= gainVal;
}

SGItBreaks GainFilterImplementation::evalIterativeBreaks(double input,
                                                         const simgear::expression::Binding* b,
                                                         const SGItBreaks& breaks) const
{
    double gain = _gainInput.get_value();
    return SGItBreaks(breaks.first / gain, breaks.second / gain);
}

bool GainFilterImplementation::configure( SGPropertyNode& cfg_node,
                                          const std::string& cfg_name,
                                          SGPropertyNode& prop_root )
{
  if (cfg_name == "gain" ) {
    _gainInput.push_back(new simgear::Value(prop_root, cfg_node, 1));
    return true;
  }

  return false;
}

/* --------------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------------- */

double ReciprocalFilterImplementation::compute(  double dt, double input )
{
  if( input >= -SGLimitsd::min() && input <= SGLimitsd::min() )
    return SGLimitsd::max();

  return _gainInput.get_value() / input;

}

void ReciprocalFilterImplementation::evalDerivative(double& value, double deriv[2],
                                                    const SGPropertyNode* wrt,
                                                    const simgear::expression::Binding* b) const
{
    if( value >= -SGLimitsd::min() && value <= SGLimitsd::min() ) {
        value = SGLimitsd::max();
        if (wrt) {
            deriv[0] = SGLimitsd::max();
            deriv[1] = SGLimitsd::max();
        }
    }

    double numVal, numDeriv[2];
    _gainInput.evalDerivative(numVal, numDeriv, wrt, b);
    // Product rule: d(u*v)/dx = v*du/dx + u*dv/dx
    // d(u/v)/dx = d(u/v)/dx = du/dx/v + u*d(1/v)/dx
    //                       = du/dx/v - u/(v^2)*dv/dx
    if (wrt) {
        double denomSqr = value * value;
        deriv[0] = numDeriv[0] / value - numVal * deriv[0] / denomSqr;
        deriv[1] = numDeriv[1] / value - numVal * deriv[1] / denomSqr;
    }
    value = numVal / value;
}

/* --------------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------------- */

DerivativeFilterImplementation::DerivativeFilterImplementation() :
  _input_1(0.0)
{
}

void DerivativeFilterImplementation::initialize( double initvalue )
{
  _input_1 = initvalue;
}

//------------------------------------------------------------------------------
bool DerivativeFilterImplementation::configure( SGPropertyNode& cfg_node,
                                                const std::string& cfg_name,
                                                SGPropertyNode& prop_root )
{
  if( GainFilterImplementation::configure(cfg_node, cfg_name, prop_root) )
    return true;

  if (cfg_name == "filter-time" ) {
    _TfInput.push_back(new simgear::Value(prop_root, cfg_node, 1));
    return true;
  }

  return false;
}

double DerivativeFilterImplementation::compute(  double dt, double input )
{
  double output = (input - _input_1) * _TfInput.get_value() * _gainInput.get_value() / dt;
  _input_1 = input;
  return output;

}

void DerivativeFilterImplementation::evalDerivative(double& value, double deriv[2],
                                                    const SGPropertyNode* wrt,
                                                    const simgear::expression::Binding* b) const
{
    // FIXME implement
    std::cout << "DerivativeFilterImplementation::evalDerivative " << (wrt ? wrt->getPath() : "-") << std::endl;
}

/* --------------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------------- */

MovingAverageFilterImplementation::MovingAverageFilterImplementation() :
  _output_1(0.0)
{
}

void MovingAverageFilterImplementation::initialize( double initvalue )
{
  _output_1 = initvalue;
}

double MovingAverageFilterImplementation::compute(  double dt, double input )
{
  typedef std::deque<double>::size_type size_type;
  size_type samples = _samplesInput.get_value();

  if (_inputQueue.size() != samples) {
    // For constant size filters, this code executed once.
    bool shrunk = _inputQueue.size() > samples;
    _inputQueue.resize(samples, _output_1);
    if (shrunk) {
      _output_1 = 0.0;
      for (size_type ii = 0; ii < samples; ii++)
      _output_1 += _inputQueue[ii];
      _output_1 /= samples;
    }
  }

  double output_0 = _output_1 + (input - _inputQueue.back()) / samples;

  _output_1 = output_0;
  _inputQueue.pop_back();
  _inputQueue.push_front(input);
  return output_0;
}

void MovingAverageFilterImplementation::evalDerivative(double& value, double deriv[2],
                                                       const SGPropertyNode* wrt,
                                                       const simgear::expression::Binding* b) const
{
    // FIXME implement
    std::cout << "MovingAverageFilterImplementation::evalDerivative " << (wrt ? wrt->getPath() : "-") << std::endl;
}

bool MovingAverageFilterImplementation::configure( SGPropertyNode& cfg_node,
                                                   const std::string& cfg_name,
                                                   SGPropertyNode& prop_root )
{
  if (cfg_name == "samples" ) {
    _samplesInput.push_back(new simgear::Value(prop_root, cfg_node, 1));
    return true;
  }

  return false;
}

/* --------------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------------- */

NoiseSpikeFilterImplementation::NoiseSpikeFilterImplementation() :
  _output_1(0.0)
{
}

void NoiseSpikeFilterImplementation::initialize( double initvalue )
{
  _output_1 = initvalue;
}

double NoiseSpikeFilterImplementation::compute(  double dt, double input )
{
  double delta = input - _output_1;
  if( fabs(delta) <= SGLimitsd::min() ) return input; // trivial

  double maxChange = _rateOfChangeInput.get_value() * dt;
  const auto periodical = _digitalFilter->getPeriodicalValue();
  if( periodical ) delta = periodical->normalizeSymmetric( delta );

  if( fabs(delta) <= maxChange )
    return (_output_1 = input);
  else
    return (_output_1 = _output_1 + copysign( maxChange, delta ));
}

void NoiseSpikeFilterImplementation::evalDerivative(double& value, double deriv[2],
                                                    const SGPropertyNode* wrt,
                                                    const simgear::expression::Binding* b) const
{
    // FIXME implement
    std::cout << "NoiseSpikeFilterImplementation::evalDerivative " << (wrt?wrt->getPath():"-") << std::endl;
}

//------------------------------------------------------------------------------
bool NoiseSpikeFilterImplementation::configure( SGPropertyNode& cfg_node,
                                                const std::string& cfg_name,
                                                SGPropertyNode& prop_root )
{
  if (cfg_name == "max-rate-of-change" ) {
    _rateOfChangeInput.push_back(new simgear::Value(prop_root, cfg_node, 1));
    return true;
  }

  return false;
}

/* --------------------------------------------------------------------------------- */

RateLimitFilterImplementation::RateLimitFilterImplementation() :
  _output_1(0.0)
{
}

void RateLimitFilterImplementation::initialize( double initvalue )
{
  _output_1 = initvalue;
}

double RateLimitFilterImplementation::compute(  double dt, double input )
{
  double delta = input - _output_1;
  double output;

  if( fabs(delta) <= SGLimitsd::min() ) return input; // trivial

  double maxChange = _rateOfChangeMax.get_value() * dt;
  double minChange = _rateOfChangeMin.get_value() * dt;
//  const PeriodicalValue * periodical = _digitalFilter->getPeriodicalValue();
//  if( periodical ) delta = periodical->normalizeSymmetric( delta );

  output = input;
  if(delta >= maxChange ) output = _output_1 + maxChange;
  if(delta <= minChange ) output = _output_1 + minChange;
  _output_1 = output;

  return (output);
}

void RateLimitFilterImplementation::evalDerivative(double& value, double deriv[2],
                                                   const SGPropertyNode* wrt,
                                                   const simgear::expression::Binding* b) const
{
    // FIXME implement
    std::cout << "RateLimitFilterImplementation::evalDerivative " << (wrt?wrt->getPath():"-") << std::endl;
}

bool RateLimitFilterImplementation::configure( SGPropertyNode& cfg_node,
                                               const std::string& cfg_name,
                                               SGPropertyNode& prop_root )
{
//  std::cout << "RateLimitFilterImplementation " << cfg_name << std::endl;
  if (cfg_name == "max-rate-of-change" ) {
    _rateOfChangeMax.push_back(new simgear::Value(prop_root, cfg_node, 1));
    return true;
  }
  if (cfg_name == "min-rate-of-change" ) {
    _rateOfChangeMin.push_back(new simgear::Value(prop_root, cfg_node, 1));
    return true;
  }

  return false;
}

/* --------------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------------- */

ExponentialFilterImplementation::ExponentialFilterImplementation()
  : _isSecondOrder(false),
    _output_1(0.0),
    _output_2(0.0)
{
}

void ExponentialFilterImplementation::initialize( double initvalue )
{
  _output_1 = _output_2 = initvalue;
}

double ExponentialFilterImplementation::compute(  double dt, double input )
{
  input = GainFilterImplementation::compute( dt, input );
  double tf = _TfInput.get_value();

  double output_0;

  // avoid negative filter times
  // and div by zero if -tf == dt

  double alpha = tf > 0.0 ? 1 / ((tf/dt) + 1) : 1.0;

  if(_isSecondOrder) {
    output_0 = alpha * alpha * input +
               2 * (1 - alpha) * _output_1 -
              (1 - alpha) * (1 - alpha) * _output_2;
  } else {
    output_0 = alpha * input + (1 - alpha) * _output_1;
  }
  _output_2 = _output_1;
  return (_output_1 = output_0);
}

void ExponentialFilterImplementation::evalDerivative(double& value, double deriv[2],
                                                    const SGPropertyNode* wrt,
                                                    const simgear::expression::Binding* b) const
{
    // FIXME implement
    std::cout << "ExponentialFilterImplementation::evalDerivative " << (wrt?wrt->getPath():"-") << std::endl;
}

//------------------------------------------------------------------------------
bool ExponentialFilterImplementation::configure( SGPropertyNode& cfg_node,
                                                 const std::string& cfg_name,
                                                 SGPropertyNode& prop_root )
{
  if( GainFilterImplementation::configure(cfg_node, cfg_name, prop_root) )
    return true;

  if (cfg_name == "filter-time" ) {
    _TfInput.push_back(new simgear::Value(prop_root, cfg_node, 1));
    return true;
  }

  if (cfg_name == "type" ) {
      std::string type = simgear::strutils::strip(cfg_node.getStringValue());
    _isSecondOrder = type == "double-exponential";
  }

  return false;
}

/* --------------------------------------------------------------------------------- */

IntegratorFilterImplementation::IntegratorFilterImplementation() :
  _input_1(0.0),
  _output_1(0.0)
{
}

void IntegratorFilterImplementation::initialize( double initvalue )
{
  _input_1 = _output_1 = initvalue;
}

//------------------------------------------------------------------------------
bool IntegratorFilterImplementation::configure( SGPropertyNode& cfg_node,
                                                const std::string& cfg_name,
                                                SGPropertyNode& prop_root )
{
  if( GainFilterImplementation::configure(cfg_node, cfg_name, prop_root) )
    return true;

  if (cfg_name == "u_min" ) {
    _minInput.push_back(new simgear::Value(prop_root, cfg_node, 1));
    return true;
  }
  if (cfg_name == "u_max" ) {
    _maxInput.push_back(new simgear::Value(prop_root, cfg_node, 1));
    return true;
  }
  return false;
}

double IntegratorFilterImplementation::compute(  double dt, double input )
{
  double output = _output_1 + input *  _gainInput.get_value() * dt;
  double u_min = _minInput.get_value();
  double u_max = _maxInput.get_value();
  if (output >= u_max) output = u_max; // clamping inside "::compute" prevents integrator wind-up
  if (output <= u_min) output = u_min;
  _input_1 = input;
  _output_1 = output;
  return output;

}

void IntegratorFilterImplementation::evalDerivative(double& value, double deriv[2],
                                                    const SGPropertyNode* wrt,
                                                    const simgear::expression::Binding* b) const
{
    // FIXME implement
    std::cout << "IntegratorFilterImplementation::evalDerivative " << (wrt ? wrt->getPath() : "-") << std::endl;
}

/* --------------------------------------------------------------------------------- */
DampedOscillationFilterImplementation::DampedOscillationFilterImplementation() :
  _x0(0.0)
{
}

void DampedOscillationFilterImplementation::initialize( double initvalue )
{
  _x2 = _x1 = _x0 = initvalue;
}

bool DampedOscillationFilterImplementation::configure( SGPropertyNode& cfg_node,
                                                const std::string& cfg_name,
                                                SGPropertyNode& prop_root )
{
  if( GainFilterImplementation::configure(cfg_node, cfg_name, prop_root) )
    return true;

  if (cfg_name == "a" ) {
    _aInput.push_back(new simgear::Value(prop_root, cfg_node, 1));
    return true;
  }
  if (cfg_name == "b" ) {
    _bInput.push_back(new simgear::Value(prop_root, cfg_node, 1));
    return true;
  }
  if (cfg_name == "c" ) {
    _cInput.push_back(new simgear::Value(prop_root, cfg_node, 1));
    return true;
  }
  return false;
}

double DampedOscillationFilterImplementation::compute( double dt, double input )
{
  if (fabs(input) > 1e-15) {
    double dz = dt * input;
    _x0 = _x1 - dz;
    _x2 = _x1 + dz;
  } else {
    double a = _aInput.get_value();
    double b = _bInput.get_value();
    double c = _cInput.get_value();
    _x0 = (_x1 * (2. + dt * (a - b * dt)) - _x2 - c * dt * dt) / (1. + a * dt);
    _x2 = _x1;
    _x1 = _x0;
  }
  return _x0;
}

void DampedOscillationFilterImplementation::evalDerivative(double& value, double deriv[2],
                                                           const SGPropertyNode* wrt,
                                                           const simgear::expression::Binding* b) const
{
    // FIXME implement
    std::cout << "DampedOscillationFilterImplementation::evalDerivative " << (wrt ? wrt->getPath() : "-") << std::endl;
}

/* --------------------------------------------------------------------------------- */

HighPassFilterImplementation::HighPassFilterImplementation() :
  _input_1(0.0),
  _output_1(0.0)

{
}

void HighPassFilterImplementation::initialize( double initvalue )
{
  _input_1 = initvalue;
  _output_1 = initvalue;
}

//double HighPassFilterImplementation::compute(  double dt, double input )
//{
//  input = GainFilterImplementation::compute( dt, input );
//  double tf = _TfInput.get_value();
//
//  double output;
//
//  // avoid negative filter times
//  // and div by zero if -tf == dt
//
//  double alpha = tf > 0.0 ? 1 / ((tf/dt) + 1) : 1.0;
//  output = (1 - alpha) * (input - _input_1 +  _output_1);
//  _input_1 = input;
//  _output_1 = output;
//  return output;
//}

double HighPassFilterImplementation::compute(double dt, double input)
{
    if (SGMiscd::isNaN(input))
        SG_LOG(SG_AUTOPILOT, SG_ALERT, "High pass filter output is NaN.");

    input = GainFilterImplementation::compute(dt, input);
    double tf = _TfInput.get_value();

    double output;

    // avoid negative filter times
    // and div by zero if -tf == dt


    double alpha = tf > 0.0 ? 1 / ((tf / dt) + 1) : 1.0;
    output = (1 - alpha) * (input - _input_1 + _output_1);
    _input_1 = input;

    // Catch NaN before it causes damage

    if (output != output) {
        SG_LOG(SG_AUTOPILOT, SG_ALERT, "High pass filter output is NaN.");
        output = 0.0;
    }
    _output_1 = output;
    return output;
}

void HighPassFilterImplementation::evalDerivative(double& value, double deriv[2],
                                                    const SGPropertyNode* wrt,
                                                    const simgear::expression::Binding* b) const
{
    // FIXME implement
    std::cout << "HighPassFilterImplementation::evalDerivative " << (wrt ? wrt->getPath() : "-") << std::endl;
}
//------------------------------------------------------------------------------
bool HighPassFilterImplementation::configure( SGPropertyNode& cfg_node,
                                              const std::string& cfg_name,
                                              SGPropertyNode& prop_root )
{
  if( GainFilterImplementation::configure(cfg_node, cfg_name, prop_root) )
    return true;

  if (cfg_name == "filter-time" ) {
    _TfInput.push_back(new simgear::Value(prop_root, cfg_node, 1));
    return true;
  }

  return false;
}

/* --------------------------------------------------------------------------------- */

LeadLagFilterImplementation::LeadLagFilterImplementation() :
  _input_1(0.0),
  _output_1(0.0)

{
}

void LeadLagFilterImplementation::initialize( double initvalue )
{
  _input_1 = initvalue;
  _output_1 = initvalue;
}

double LeadLagFilterImplementation::compute(  double dt, double input )
{
  input = GainFilterImplementation::compute( dt, input );
  double tfa = _TfaInput.get_value();
  double tfb = _TfbInput.get_value();

  double output;

  // avoid negative filter times
  // and div by zero if -tf == dt

  double alpha = tfa > 0.0 ? 1 / ((tfa/dt) + 1) : 1.0;
  double beta = tfb > 0.0 ? 1 / ((tfb/dt) + 1) : 1.0;
  output = (1 - beta) * (input / (1 - alpha) - _input_1 + _output_1);
  _input_1 = input;
  _output_1 = output;
  return output;
}

void LeadLagFilterImplementation::evalDerivative(double& value, double deriv[2],
                                                    const SGPropertyNode* wrt,
                                                    const simgear::expression::Binding* b) const
{
    // FIXME implement
    std::cout << "LeadLagFilterImplementation::evalDerivative " << (wrt ? wrt->getPath() : "-") << std::endl;
}

//------------------------------------------------------------------------------
bool LeadLagFilterImplementation::configure( SGPropertyNode& cfg_node,
                                             const std::string& cfg_name,
                                             SGPropertyNode& prop_root )
{
  if( GainFilterImplementation::configure(cfg_node, cfg_name, prop_root) )
    return true;

  if (cfg_name == "filter-time-a" ) {
    _TfaInput.push_back(new simgear::Value(prop_root, cfg_node, 1));
    return true;
  }
  if (cfg_name == "filter-time-b" ) {
    _TfbInput.push_back(new simgear::Value(prop_root, cfg_node, 1));
    return true;
  }
  return false;
}


/* --------------------------------------------------------------------------------- */

CoherentNoiseFilterImplementation::CoherentNoiseFilterImplementation() : _amplitude(1.0),
                                                                         _numDiscreteValues(1024)
{
}

void CoherentNoiseFilterImplementation::initialize(double initvalue)
{
    // allocate the array one bigger so we don't need to worry about
    // wrapping.bound checking the discrete +1 lookup
    _discreteValues.resize(_numDiscreteValues + 1);
    std::generate(_discreteValues.begin(), _discreteValues.end(), []() {
        return (sg_random() * 2.0) - 1.0;
    });
}

static double lerp(double a, double b, double t)
{
    return (a * (1.0 - t)) + (b * t);
}

double CoherentNoiseFilterImplementation::compute(double dt, double input)
{
    const double a = _amplitude.get_value();

    const double t = input * _numDiscreteValues;
    const int i = static_cast<int>(floor(t)) % _numDiscreteValues;
    const auto v0 = _discreteValues.at(i);
    const auto v1 = _discreteValues.at(i + 1);

    const auto weight = t - floor(t);
    const double output = lerp(v0, v1, weight);

    return _absoluteVal ? fabs(output) * a : output * a;
}

void CoherentNoiseFilterImplementation::evalDerivative(double& value, double deriv[2],
                                                    const SGPropertyNode* wrt,
                                                    const simgear::expression::Binding* b) const
{
    // FIXME implement
    std::cout << "CoherentNoiseFilterImplementation::evalDerivative " << (wrt ? wrt->getPath() : "-") << std::endl;
}

//------------------------------------------------------------------------------
bool CoherentNoiseFilterImplementation::configure(SGPropertyNode& cfg_node,
                                                  const std::string& cfg_name,
                                                  SGPropertyNode& prop_root)
{
    if (cfg_name == "discrete-resolution") {
        _numDiscreteValues = cfg_node.getIntValue();
        return true;
    }

    if (cfg_name == "amplitude") {
        _amplitude.push_back(new simgear::Value(prop_root, cfg_node, 1.0));
        return true;
    }

    if (cfg_name == "absolute") {
        _absoluteVal = cfg_node.getBoolValue();
        return true;
    }
    return false;
}

/* -------------------------------------------------------------------------- */
/* Reverse Modifier Implementation                                            */
/* -------------------------------------------------------------------------- */

class DigitalFilter::ReverseModifier : public SGIKVariable::ModifyHandler
{
public:
    typedef SGIKVariable::ModifyHandler Super;

    ReverseModifier(DigitalFilter* filter,
                    SGPropertyNode* propertyNode) :
        Super(propertyNode),
        _filter(filter)
    {
    }

    bool isEnabled() const override
    {
        return _filter->isPropertyEnabled();
    }

    bool isKinematic() const override
    {
        return _filter->isKinematic();
    }

    double modify(double value, bool recurse) override
    {
        return _filter->reverse(value);
    }

    void collectDependentProperties(std::set<const SGPropertyNode*>& props,
                                    int minRev) const override
    {
        _filter->collectDependentProperties(props, minRev);
    }

    void evalDerivative(double& value, double deriv[2], const SGPropertyNode* wrt,
                        const simgear::expression::Binding* b) const override
    {
        _filter->evalDerivative(value, deriv, wrt, b);
        std::cout << "DigitalFilter::ReverseModifier::evalDerivative " << _filter->_srcLocation << " wrt " << (wrt?wrt->getPath():"N/A") << ": " << deriv[0] << "," << deriv[1] << " @" << value << std::endl;
    }

    SGItBreaks evalIterativeBreaks(const SGPropertyNode* wrt,
                                   const simgear::expression::Binding* b,
                                   const SGItBreaks& breaks) const override
    {
        return _filter->evalIterativeBreaks(wrt, b, breaks);
    }

private:
    DigitalFilter* _filter;
};

/* -------------------------------------------------------------------------- */
/* Digital Filter Component Implementation                                    */
/* -------------------------------------------------------------------------- */

DigitalFilter::DigitalFilter() :
    AnalogComponent(),
    _initializeTo(INITIALIZE_INPUT)
{
}

DigitalFilter::~DigitalFilter()
{
}

//------------------------------------------------------------------------------
template<class DigitalFilterType>
DigitalFilterImplementation* digitalFilterFactory()
{
  return new DigitalFilterType();
}

typedef std::map<std::string, DigitalFilterImplementation*(*)()>
DigitalFilterMap;
static DigitalFilterMap componentForge;

//------------------------------------------------------------------------------
bool DigitalFilter::configure( SGPropertyNode& prop_root,
                               SGPropertyNode& cfg )
{
  _srcLocation = cfg.getLocation();
  if( componentForge.empty() )
  {
    componentForge["gain"               ] = digitalFilterFactory<GainFilterImplementation>;
    componentForge["exponential"        ] = digitalFilterFactory<ExponentialFilterImplementation>;
    componentForge["double-exponential" ] = digitalFilterFactory<ExponentialFilterImplementation>;
    componentForge["moving-average"     ] = digitalFilterFactory<MovingAverageFilterImplementation>;
    componentForge["noise-spike"        ] = digitalFilterFactory<NoiseSpikeFilterImplementation>;
    componentForge["rate-limit"         ] = digitalFilterFactory<RateLimitFilterImplementation>;
    componentForge["reciprocal"         ] = digitalFilterFactory<ReciprocalFilterImplementation>;
    componentForge["derivative"         ] = digitalFilterFactory<DerivativeFilterImplementation>;
    componentForge["high-pass"          ] = digitalFilterFactory<HighPassFilterImplementation>;
    componentForge["lead-lag"           ] = digitalFilterFactory<LeadLagFilterImplementation>;
    componentForge["integrator"         ] = digitalFilterFactory<IntegratorFilterImplementation>;
    componentForge["damped-oscillation" ] = digitalFilterFactory<DampedOscillationFilterImplementation>;
    componentForge["coherent-noise"] = digitalFilterFactory<CoherentNoiseFilterImplementation>;
  }

  const auto type = simgear::strutils::strip(cfg.getStringValue("type"));
  DigitalFilterMap::iterator component_factory = componentForge.find(type);
  if( component_factory == componentForge.end() )
  {
    SG_LOG(SG_AUTOPILOT, SG_WARN, "unhandled filter type '" << type << "'");
    return false;
  }

  _implementation = (*component_factory->second)();
  _implementation->setDigitalFilter( this );

  for( int i = 0; i < cfg.nChildren(); ++i )
  {
    SGPropertyNode_ptr child = cfg.getChild(i);
    std::string cname(child->getNameString());
    bool ok = false;
    if (!ok) ok = _implementation->configure(*child, cname, prop_root);
    if (!ok) ok = configure(*child, cname, prop_root);
    if (!ok) ok = (cname == "type");
    if (!ok) ok = (cname == "params");   // 'params' is usually used to specify parameters in PropertList files.
    if (!ok) {
      SG_LOG
      (
        SG_AUTOPILOT,
        SG_ALERT,
        "DigitalFilter: unknown config node: " << cname
      );
    }
  }
  
  /* Send information about associations between our input and output
  properties to Highlight. */
  std::set<const SGPropertyNode*>    inputs;
  _implementation->collectDependentProperties(inputs);
  collectDependentProperties(inputs);
  
  auto highlight = globals->get_subsystem<Highlight>();
  if (highlight) {
    for (auto in: inputs) {
      for (auto& out: _output_list) {
          highlight->addPropertyProperty(
                  in->getPath(true /*simplify*/),
                  out->getPath(true /*simplify*/)
                  );
      }
    }
  }

  // And let IK system know how to reverse the filter
  _reverseModifiers.reserve(_output_list.size());
  for (auto output: _output_list)
      _reverseModifiers.push_back(std::make_unique<ReverseModifier>(this, output.get()));
  
  return true;
}

//------------------------------------------------------------------------------
bool DigitalFilter::configure( SGPropertyNode& cfg_node,
                               const std::string& cfg_name,
                               SGPropertyNode& prop_root )
{
  if( cfg_name == "initialize-to" )
  {
      const auto s = simgear::strutils::strip(cfg_node.getStringValue());
    if( s == "input" )
      _initializeTo = INITIALIZE_INPUT;
    else if( s == "output" )
      _initializeTo = INITIALIZE_OUTPUT;
    else if( s == "none" )
      _initializeTo = INITIALIZE_NONE;
    else
      SG_LOG
      (
        SG_AUTOPILOT,
        SG_WARN, "DigitalFilter: initialize-to (" << s << ") ignored"
      );

    return true;
  }

  return AnalogComponent::configure(cfg_node, cfg_name, prop_root);
}

//------------------------------------------------------------------------------
void DigitalFilter::update( bool firstTime, double dt)
{
  if( _implementation == NULL ) return;

  // Skip if held
  bool held = false;
  for (auto& modifier: _reverseModifiers) {
      if (modifier && modifier->isHeld()) {
          modifier->release();
          held = true;
      }
  }
  if (held)
      return;

  if( firstTime ) {
    switch( _initializeTo ) {

      case INITIALIZE_INPUT:
        SG_LOG(SG_AUTOPILOT,SG_DEBUG, "First time initialization of " << subsystemId() << " to " << _valueInput.get_value() );
        _implementation->initialize( _valueInput.get_value() );
        break;

      case INITIALIZE_OUTPUT:
        SG_LOG(SG_AUTOPILOT,SG_DEBUG, "First time initialization of " << subsystemId() << " to " << get_output_value() );
        _implementation->initialize( get_output_value() );
        break;

      default:
        SG_LOG(SG_AUTOPILOT,SG_DEBUG, "First time initialization of " << subsystemId() << " to (uninitialized)" );
        break;
    }
  }

  double input = _valueInput.get_value() - _referenceInput.get_value();
  if (SGMiscd::isNaN(input))
      input = _valueInput.get_value() - _referenceInput.get_value();
  double output = _implementation->compute( dt, input );

  set_output_value( output );

  if(_debug) {
    std::cout << subsystemId() << ": input=" << input
              << "\toutput=" << output << std::endl;
  }
}

//------------------------------------------------------------------------------
double DigitalFilter::reverse(double value)
{
    set_output_value(value);
    value = get_output_value();
    _implementation->initialize(value);
    if (isPropertyEnabled()) {
        std::cout << _srcLocation << std::endl;
        std::cout << "DigitalFilter::reverse(" << value << ")" << std::endl;
        // FIXME we lie about the value...
        /*return*/ _valueInput.set_value(value);
    }
    return value;
}

//------------------------------------------------------------------------------
void DigitalFilter::evalDerivative(double& value, double deriv[2], const SGPropertyNode* wrt,
                                   const simgear::expression::Binding* b) const
{
    _valueInput.evalDerivative(value, deriv, wrt, b);

    double refVal, refDeriv[2];
    _referenceInput.evalDerivative(refVal, refDeriv, wrt, b);
    value -= refVal;
    if (wrt) {
        deriv[0] -= refDeriv[0];
        deriv[1] -= refDeriv[1];
    }

    _implementation->evalDerivative(value, deriv, wrt, b);
}

//------------------------------------------------------------------------------
SGItBreaks DigitalFilter::evalIterativeBreaks(const SGPropertyNode* wrt,
                                              const simgear::expression::Binding* b,
                                              const SGItBreaks& breaks) const
{
    double value = _valueInput.get_value(b);
    double ref = _referenceInput.get_value(b);
    auto inputBreaks = _implementation->evalIterativeBreaks(value - ref, b, breaks);
    std::cout << "DigitalFilter::evalIterativeBreaks in " << inputBreaks.first << "," << inputBreaks.second << std::endl;
    // input = value - ref
    // value = input + ref
    // ref = value + input

    // Find iterative breaks of wrt for each of the inputs, assuming the other
    // remains constant

    SGItBreaksInfo<double> inputBreaksValue, inputBreaksRef;
    if (!std::isinf(inputBreaks.first)) {
        inputBreaksValue.insertMin(inputBreaks.first + ref);
        inputBreaksRef.insertMin(inputBreaks.first + value);
    }
    if (!std::isinf(inputBreaks.second)) {
        inputBreaksValue.insertMax(inputBreaks.second + ref);
        inputBreaksRef.insertMax(inputBreaks.second + value);
    }
    auto valueBreaks = _valueInput.evalIterativeBreaks(wrt, b, inputBreaksValue);
    auto refBreaks = _referenceInput.evalIterativeBreaks(wrt, b, inputBreaksRef);
    std::cout << "DigitalFilter::evalIterativeBreaks val " << valueBreaks.first << "," << valueBreaks.second << std::endl;
    std::cout << "DigitalFilter::evalIterativeBreaks ref " << refBreaks.first << "," << refBreaks.second << std::endl;

    // Return combined range
    SGItBreaks ret(std::max(valueBreaks.first, refBreaks.first),
                   std::min(valueBreaks.second, refBreaks.second));
    std::cout << "DigitalFilter::evalIterativeBreaks ret " << ret.first << "," << ret.second << std::endl;
    return ret;
}

//------------------------------------------------------------------------------
bool DigitalFilter::isKinematic() const
{
    return _implementation->isKinematic();
}

// Register the subsystem.
SGSubsystemMgr::Registrant<DigitalFilter> registrantDigitalFilter;
