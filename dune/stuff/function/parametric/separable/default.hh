#ifndef DUNE_STUFF_FUNCTION_PARAMETRIC_SEPARABLE_DEFAULT_HH
#define DUNE_STUFF_FUNCTION_PARAMETRIC_SEPARABLE_DEFAULT_HH

#ifdef HAVE_CMAKE_CONFIG
  #include "cmake_config.h"
#elif defined (HAVE_CONFIG_H)
  #include <config.h>
#endif // ifdef HAVE_CMAKE_CONFIG

#include <vector>

#include <dune/common/shared_ptr.hh>

#include <dune/stuff/common/parameter.hh>
#include <dune/stuff/common/string.hh>
#include <dune/stuff/common/color.hh>
#include <dune/stuff/common/parameter/tree.hh>

#include <dune/stuff/function.hh>
#include "coefficient.hh"

namespace Dune {
namespace Stuff {
namespace Function {


template< class DomainFieldImp, int domainDim, class RangeFieldImp, int rangeDim >
class SeparableDefault
  : public Interface< DomainFieldImp, domainDim, RangeFieldImp, rangeDim >
{
public:
  typedef SeparableDefault< DomainFieldImp, domainDim, RangeFieldImp, rangeDim >  ThisType;
  typedef Interface< DomainFieldImp, domainDim, RangeFieldImp, rangeDim >         BaseType;

  typedef typename BaseType::DomainFieldType  DomainFieldType;
  static const int                            dimDomain = BaseType::dimDomain;
  typedef typename BaseType::DomainType       DomainType;
  typedef typename BaseType::RangeFieldType   RangeFieldType;
  static const int                            dimRange = BaseType::dimRange;
  typedef typename BaseType::RangeType        RangeType;
  typedef typename BaseType::ParamFieldType   ParamFieldType;
  typedef typename BaseType::ParamType        ParamType;

  typedef typename BaseType::ComponentType    ComponentType;
  typedef typename BaseType::CoefficientType  CoefficientType;

  static const std::string id()
  {
    return "function.parametric.separable.default";
  }

  SeparableDefault(const size_t _paramSize,
                   const std::vector< ParamType > _paramRange,
                   const std::vector< Dune::shared_ptr< const ComponentType > > _components,
                   const std::vector< Dune::shared_ptr< const CoefficientType > > _coefficients,
                   const std::vector< std::string > _parameterExplanation = std::vector< std::string >(),
                   const int _order = -1,
                   const std::string _name = "function.parametric.separable.default")
    : paramSize_(_paramSize)
    , paramRange_(_paramRange)
    , components_(_components)
    , coefficients_(_coefficients)
    , order_(_order)
    , name_(_name)
  {
    // some checks
    assert(paramSize_ > 0);
    assert(components_.size() > 0);
    assert(coefficients_.size() > 0);
    assert((components_.size() == coefficients_.size())
           || (components_.size() == coefficients_.size() + 1));
    assert(paramRange_.size() == 2 && "Vector has wrong size!");
    assert(paramRange_[0].size() == paramSize_ && "Vector has wrong size!");
    assert(paramRange_[1].size() == paramSize_ && "Vector has wrong size!");
    for (size_t qq = 0; qq < paramSize_; ++qq)
      assert(paramRange_[0][qq] <= paramRange_[1][qq]
          && "Given minimum parameter has to be piecewise <= maximum parameter!");
    for (size_t qq = 0; qq < components_.size(); ++qq)
      if (components_[qq]->parametric())
        DUNE_THROW(Dune::RangeError,
                   "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                   << " component " << qq << " is parametric!");
    // process parameter explanation
    for (size_t qq = 0; qq < std::min(paramSize_, _parameterExplanation.size()); ++qq)
      parameterExplanation_.push_back(_parameterExplanation[qq]);
    for (size_t qq = std::min(paramSize_, _parameterExplanation.size()); qq < paramSize_; ++qq)
      parameterExplanation_.push_back("parameter_component_" + Dune::Stuff::Common::toString(qq));
  }

  SeparableDefault(const ThisType& other)
    : paramSize_(other.paramSize_)
    , paramRange_(other.paramRange_)
    , components_(other.components_)
    , coefficients_(other.coefficients_)
    , order_(other.order_)
    , name_(other.name_)
    , parameterExplanation_(other.parameterExplanation_)
  {}

  ThisType& operator=(const ThisType& other)
  {
    if (this != &other) {
      paramSize_ = other.paramSize();
      paramRange_ = other.paramRange();
      components_ = other.components();
      coefficients_ = other.coefficients();
      order_ = other.order();
      name_ = other.name();
      parameterExplanation_ = other.parameterExplanation();
    }
    return this;
  }

  static Dune::ParameterTree createSampleDescription(const std::string subName = "")
  {
    Dune::ParameterTree description;
    description["name"] = id();
    description["order"] = "1";
    description["component.0"] = "2.0 * x[0]";
    description["component.1"] = "sin(x[0])";
    description["coefficient.0"] = "mu[0] + mu[1]";
    description["paramSize"] = "2";
    description["paramMin"] = "[0.1; 0.1]";
    description["paramMax"] = "[1.0; 1.0]";
    description["paramExplanation"] = "[first_parameter; second_parameter]";
    if (subName.empty())
      return description;
    else {
      Dune::Stuff::Common::ExtendedParameterTree extendedDescription;
      extendedDescription.add(description, subName);
      return extendedDescription;
    }
  }

  static ThisType createFromDescription(const Dune::ParameterTree _description)
  {
    Stuff::Common::ExtendedParameterTree description(_description);
    // first of all, read the optional stuff
    const std::string _name = description.get< std::string >("name", id());
    const int _order = description.get< int >("order", -1);
    // then, read what has to exist
    // * components
    std::vector< Dune::shared_ptr< const ComponentType > > _components;
    bool continue_search = true;
    while (continue_search) {
      // lets see if there is a component with this index
      const std::string key = "component." + Dune::Stuff::Common::toString(_components.size());
      if (description.hasSub(key)) {
        // there is a sub
        Dune::ParameterTree componentDescription = description.sub(key);
        // lets see if it has a type
        if (componentDescription.hasKey("type")) {
          const std::string type = componentDescription.get< std::string >("type");
          if (type == "function.parametric.separable.default")
            DUNE_THROW(Dune::IOError,
                       "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                       << " can not create a 'function.parametric.separable.default' inside itself!");
          _components.push_back(Dune::Stuff::Function::create<  DomainFieldType, dimDomain,
                                                                RangeFieldType, dimRange >(type,
                                                                                           componentDescription));
        } else {
          // since it does not have a type, treat it as an expression function
          if (!(componentDescription.hasKey("expression") || componentDescription.hasSub("expression")))
            DUNE_THROW(Dune::IOError,
                       "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                       << " could not interpret component." << _components.size()
                       << "in the following Dune::ParameterTree:\n"
                       << description.reportString("  "));
          if (!componentDescription.hasKey("variable"))
            componentDescription["variable"] = "x";
          if (!componentDescription.hasKey("name"))
            componentDescription["name"] = _name;
          if (!componentDescription.hasKey("order"))
            componentDescription["order"] = Dune::Stuff::Common::toString(_order);
          _components.push_back(Dune::Stuff::Function::create<  DomainFieldType, dimDomain,
                                                                RangeFieldType, dimRange >("function.expression",
                                                                                           componentDescription));
        } // if (componentDescription.hasKey("type"))
      } else if (description.hasKey(key)) {
        // there is only one key, interpret it as an entry for an expression function with variable x
        Dune::ParameterTree componentDescription;
        componentDescription["name"] = _name;
        componentDescription["order"] = Dune::Stuff::Common::toString(_order);
        componentDescription["variable"] = "x";
        componentDescription["expression"] = description.get< std::string >(key);
        _components.push_back(Dune::Stuff::Function::create<  DomainFieldType, dimDomain,
                                                              RangeFieldType, dimRange >("function.expression",
                                                                                         componentDescription));
      } else {
        // stop the search
        continue_search = false;
      }
    } // while (continue_search)
    if (_components.size() == 0)
      DUNE_THROW(Dune::IOError,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " no 'component' found in the following description:\n" << description.reportString("  "));
    // check that all components are nonparametric
    for (size_t qq = 0; qq < _components.size(); ++qq)
      if (_components[qq]->parametric())
        DUNE_THROW(Dune::RangeError,
                   "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                   << " component " << qq << " is parametric!");
    // * coefficients
    std::vector< Dune::shared_ptr< const CoefficientType > > _coefficients;
    continue_search = true;
    while (continue_search) {
      // lets see if there is a coefficient with this index
      const std::string key = "coefficient." + Dune::Stuff::Common::toString(_coefficients.size());
      if (description.hasKey(key)) {
        _coefficients.push_back(Dune::make_shared< CoefficientType >(description.get< std::string >(key)));
      } else
        continue_search = false;
    } // while (continue_search)
    if (_coefficients.size() == 0)
      DUNE_THROW(Dune::IOError,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " no 'coefficient' found in the following description:\n" << description.reportString("  "));
    else if (!(_coefficients.size() == _components.size()
             || _coefficients.size() == (_components.size() - 1)))
      DUNE_THROW(Dune::IOError,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " wrong number of 'coefficient' found in the following description:\n"
                 << description.reportString("  "));
    // * paramSize
    const size_t _paramSize = description.get< size_t >("paramSize");
    // * paramRange
    const std::vector< ParamFieldType > _paramMins = description.getVector< ParamFieldType >("paramMin", _paramSize);
    const std::vector< ParamFieldType > _paramMaxs = description.getVector< ParamFieldType >("paramMax", _paramSize);
    ParamType _paramMin(_paramSize);
    ParamType _paramMax(_paramSize);
    for (size_t pp = 0; pp < _paramSize; ++pp) {
      _paramMin[pp] = _paramMins[pp];
      _paramMax[pp] = _paramMaxs[pp];
    }
    std::vector< ParamType > _paramRange;
    _paramRange.push_back(_paramMin);
    _paramRange.push_back(_paramMax);
    // * paramExplanation
    std::vector< std::string > _paramExplanation;
    if (description.hasVector("paramExplanation"))
      _paramExplanation = description.getVector< std::string >("paramExplanation", 1);
    else if (description.hasKey("paramExplanation"))
      _paramExplanation.push_back(description.get< std::string >("paramExplanation"));
    return ThisType(_paramSize, _paramRange, _components, _coefficients, _paramExplanation, _order, _name);
  }

  virtual bool parametric() const
  {
    return true;
  }

  virtual bool separable() const
  {
    return true;
  }

  virtual int order() const
  {
    return order_;
  }

  virtual std::string name() const
  {
    return name_;
  }

  virtual size_t paramSize() const
  {
    return paramSize_;
  }

  virtual const std::vector< ParamType >& paramRange() const
  {
    return paramRange_;
  }

  virtual const std::vector< std::string >& paramExplanation() const
  {
    return parameterExplanation_;
  }

  virtual size_t numComponents() const
  {
    return components_.size();
  }

  virtual size_t numCoefficients() const
  {
    return coefficients_.size();
  }

  virtual const std::vector< Dune::shared_ptr< const ComponentType > >& components() const
  {
    return components_;
  }

  virtual const std::vector< Dune::shared_ptr< const CoefficientType > >& coefficients() const
  {
    return coefficients_;
  }

  virtual void evaluate(const DomainType& _x, const ParamType& _mu, RangeType& _ret) const
  {
    assert(_mu.size() == paramSize_);
    _ret = RangeFieldType(0);
    RangeType tmpComponentValue;
    for (size_t qq = 0; qq < numCoefficients(); ++qq) {
      components_[qq]->evaluate(_x, tmpComponentValue);
      tmpComponentValue *= coefficients_[qq]->evaluate(_mu);
      _ret += tmpComponentValue;
    }
    if (numComponents() > numCoefficients()) {
      components_[numCoefficients()]->evaluate(_x, tmpComponentValue);
      _ret += tmpComponentValue;
    }
  } // virtual void evaluate(const DomainType& _x, const ParamType& _mu, RangeType& _ret) const

private:
  size_t paramSize_;
  std::vector< ParamType > paramRange_;
  std::vector< Dune::shared_ptr< const ComponentType > > components_;
  std::vector< Dune::shared_ptr< const CoefficientType > > coefficients_;
  const int order_;
  const std::string name_;
  std::vector< std::string > parameterExplanation_;
}; // class SeparableDefault

} // namespace Function
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_FUNCTION_PARAMETRIC_SEPARABLE_DEFAULT_HH
