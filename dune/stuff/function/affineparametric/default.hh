#ifndef DUNE_STUFF_FUNCTION_AFFINEPARAMETRIC_DEFAULT_HH
#define DUNE_STUFF_FUNCTION_AFFINEPARAMETRIC_DEFAULT_HH

#include <memory>
#include <vector>

#include <dune/stuff/common/parameter.hh>
#include <dune/stuff/common/string.hh>
#include <dune/stuff/common/color.hh>
#include <dune/stuff/common/parameter/tree.hh>

#include <dune/stuff/function.hh>
#include "coefficient.hh"

namespace Dune {
namespace Stuff {


// forward to allow for specialization
template< class DomainFieldImp, int domainDim, class RangeFieldImp, int rangeDimRows, int rangeDimCols = 1>
class AffineParametricFunctionDefault;


template< class DomainFieldImp, int domainDim, class RangeFieldImp, int rangeDim >
class AffineParametricFunctionDefault< DomainFieldImp, domainDim, RangeFieldImp, rangeDim, 1 >
  : public AffineParametricFunctionInterface< DomainFieldImp, domainDim, RangeFieldImp, rangeDim, 1 >
{
  typedef AffineParametricFunctionInterface< DomainFieldImp, domainDim, RangeFieldImp, rangeDim, 1 >  BaseType;
public:
  typedef AffineParametricFunctionDefault< DomainFieldImp, domainDim, RangeFieldImp, rangeDim, 1 >    ThisType;

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
    return BaseType::id() + ".default";
  }

  AffineParametricFunctionDefault(const size_t _paramSize,
                                  const std::vector< ParamType > _paramRange,
                                  const std::vector< std::shared_ptr< const ComponentType > > _components,
                                  const std::vector< std::shared_ptr< const CoefficientType > > _coefficients,
                                  const std::vector< std::string > _parameterExplanation = std::vector< std::string >(),
                                  const int _order = -1,
                                  const std::string _name = id())
    : paramSize_(_paramSize)
    , paramRange_(_paramRange)
    , components_(_components)
    , coefficients_(_coefficients)
    , hasAffinePart_(false)
    , order_(_order)
    , name_(_name)
  {
    // some checks
    assert(paramSize_ > 0);
    assert(components_.size() > 0);
    assert(coefficients_.size() > 0);
    assert(components_.size() == coefficients_.size());
    assert(paramRange_.size() == 2 && "Vector has wrong size!");
    assert(paramRange_[0].size() == paramSize_ && "Vector has wrong size!");
    assert(paramRange_[1].size() == paramSize_ && "Vector has wrong size!");
    for (size_t qq = 0; qq < paramSize_; ++qq) {
      assert(paramRange_[0][qq] <= paramRange_[1][qq]
          && "Given minimum parameter has to be piecewise <= maximum parameter!");
    }
    for (size_t qq = 0; qq < components_.size(); ++qq) {
      if (components_[qq]->parametric())
        DUNE_THROW(Dune::RangeError,
                   "\n" << DSC::colorStringRed("ERROR:")
                   << " component " << qq << " is parametric!");
    }
    // process parameter explanation
    for (size_t qq = 0; qq < std::min(paramSize_, _parameterExplanation.size()); ++qq)
      parameterExplanation_.push_back(_parameterExplanation[qq]);
    for (size_t qq = std::min(paramSize_, _parameterExplanation.size()); qq < paramSize_; ++qq)
      parameterExplanation_.push_back("parameter_component_" + DSC::toString(qq));
  } // FunctionAffineParametricDefault(...)

  AffineParametricFunctionDefault(const size_t _paramSize,
                                  const std::vector< ParamType > _paramRange,
                                  const std::vector< std::shared_ptr< const ComponentType > > _components,
                                  const std::vector< std::shared_ptr< const CoefficientType > > _coefficients,
                                  const std::shared_ptr< const ComponentType > _affinePart,
                                  const std::vector< std::string > _parameterExplanation = std::vector< std::string >(),
                                  const int _order = -1,
                                  const std::string _name = id())
    : paramSize_(_paramSize)
    , paramRange_(_paramRange)
    , components_(_components)
    , coefficients_(_coefficients)
    , hasAffinePart_(true)
    , order_(_order)
    , name_(_name)
    , affinePart_(_affinePart)

  {
    // some checks
    assert(paramSize_ > 0);
    assert(components_.size() > 0);
    assert(coefficients_.size() > 0);
    assert(components_.size() == coefficients_.size());
    assert(paramRange_.size() == 2 && "Vector has wrong size!");
    assert(paramRange_[0].size() == paramSize_ && "Vector has wrong size!");
    assert(paramRange_[1].size() == paramSize_ && "Vector has wrong size!");
    for (size_t qq = 0; qq < paramSize_; ++qq) {
      assert(paramRange_[0][qq] <= paramRange_[1][qq]
          && "Given minimum parameter has to be piecewise <= maximum parameter!");
    }
    for (size_t qq = 0; qq < components_.size(); ++qq) {
      if (components_[qq]->parametric())
        DUNE_THROW(Dune::RangeError,
                   "\n" << DSC::colorStringRed("ERROR:")
                   << " component " << qq << " is parametric!");
    }
    if (affinePart_->parametric())
      DUNE_THROW(Dune::RangeError,
                 "\n" << DSC::colorStringRed("ERROR:")
                 << " affinePart must not be parametric!");
    // process parameter explanation
    for (size_t qq = 0; qq < std::min(paramSize_, _parameterExplanation.size()); ++qq)
      parameterExplanation_.push_back(_parameterExplanation[qq]);
    for (size_t qq = std::min(paramSize_, _parameterExplanation.size()); qq < paramSize_; ++qq)
      parameterExplanation_.push_back("parameter_component_" + DSC::toString(qq));
  } // FunctionAffineParametricDefault(...)

  static Dune::ParameterTree defaultSettings(const std::string subName = "")
  {
    Dune::ParameterTree description;
    description["name"] = id();
    description["order"] = "1";
    description["component.0"] = "2.0 * x[0]";
    description["coefficient.0"] = "mu[0] + mu[1]";
    description["affinePart"] = "sin(x[0])";
    description["paramSize"] = "2";
    description["paramMin"] = "[0.1; 0.1]";
    description["paramMax"] = "[1.0; 1.0]";
    description["paramExplanation"] = "[first_parameter; second_parameter]";
    if (subName.empty())
      return description;
    DSC::ExtendedParameterTree extendedDescription;
    extendedDescription.add(description, subName);
    return extendedDescription;
  } // ... defaultSettings(...)

  static ThisType* create(const DSC::ExtendedParameterTree settings)
  {
    // first of all, read the optional stuff
    const std::string _name = settings.get< std::string >("name", id());
    const int _order = settings.get< int >("order", -1);
    // then, read what has to exist
    // * components
    std::vector< std::shared_ptr< const ComponentType > > _components;
    bool continue_search = true;
    while (continue_search) {
      // lets see if there is a component with this index
      const std::string key = "component." + DSC::toString(_components.size());
      if (settings.hasSub(key)) {
        // there is a sub
        Dune::ParameterTree componentSettings = settings.sub(key);
        // lets see if it has a type
        if (componentSettings.hasKey("type")) {
          const std::string type = componentSettings.get< std::string >("type");
          if (type == "function.affineparametric.default")
            DUNE_THROW(Dune::IOError,
                       "\n" << DSC::colorStringRed("ERROR:")
                       << " can not create a 'function.affineparametric.default' inside itself!");
          _components.emplace_back(Functions< DomainFieldType, dimDomain,
                                              RangeFieldType, dimRange >::create(type, componentSettings));
        } else {
          // since it does not have a type, treat it as an expression function
          if (!(componentSettings.hasKey("expression") || componentSettings.hasSub("expression")))
            DUNE_THROW(Dune::IOError,
                       "\n" << DSC::colorStringRed("ERROR:")
                       << " could not interpret component." << _components.size()
                       << "in the following settings:\n"
                       << settings.reportString("  "));
          if (!componentSettings.hasKey("variable"))
            componentSettings["variable"] = "x";
          if (!componentSettings.hasKey("name"))
            componentSettings["name"] = _name;
          if (!componentSettings.hasKey("order"))
            componentSettings["order"] = DSC::toString(_order);
          _components.emplace_back(Functions< DomainFieldType, dimDomain,
                                              RangeFieldType, dimRange >::create("function.expression",
                                                                               componentSettings));
        } // if (componentSettings.hasKey("type"))
      } else if (settings.hasKey(key)) {
        // there is only one key, interpret it as an entry for an expression function with variable x
        Dune::ParameterTree componentSettings;
        componentSettings["name"] = _name;
        componentSettings["order"] = DSC::toString(_order);
        componentSettings["variable"] = "x";
        componentSettings["expression"] = settings.get< std::string >(key);
        _components.emplace_back(Functions< DomainFieldType, dimDomain,
                                            RangeFieldType, dimRange >::create("function.expression",
                                                                             componentSettings));
      } else {
        // stop the search
        continue_search = false;
      }
    } // while (continue_search)
    if (_components.size() == 0)
      DUNE_THROW(Dune::IOError,
                 "\n" << DSC::colorStringRed("ERROR:")
                 << " no 'component' found in the following settings:\n" << settings.reportString("  "));
    // check that all components are nonparametric
    for (size_t qq = 0; qq < _components.size(); ++qq)
      if (_components[qq]->parametric())
        DUNE_THROW(Dune::RangeError,
                   "\n" << DSC::colorStringRed("ERROR:")
                   << " component " << qq << " is parametric!");
    // * coefficients
    std::vector< std::shared_ptr< const CoefficientType > > _coefficients;
    continue_search = true;
    while (continue_search) {
      // lets see if there is a coefficient with this index
      const std::string key = "coefficient." + DSC::toString(_coefficients.size());
      if (settings.hasKey(key)) {
        _coefficients.emplace_back(new CoefficientType(settings.get< std::string >(key)));
      } else
        continue_search = false;
    } // while (continue_search)
    if (_coefficients.size() == 0)
      DUNE_THROW(Dune::IOError,
                 "\n" << DSC::colorStringRed("ERROR:")
                 << " no 'coefficient' found in the following settings:\n" << settings.reportString("  "));
    else if (!(_coefficients.size() == _components.size()))
      DUNE_THROW(Dune::IOError,
                 "\n" << DSC::colorStringRed("ERROR:")
                 << " wrong number of 'coefficient' found in the following settings:\n"
                 << settings.reportString("  "));
    // the affine part
    std::shared_ptr< const ComponentType > _affinePart;
    bool _hasAffinePart = false;
    if (settings.hasSub("affinePart")) {
      _hasAffinePart = true;
      // there is a sub
      Dune::ParameterTree affinePartSettings = settings.sub("affinePart");
      // lets see if it has a type
      if (affinePartSettings.hasKey("type")) {
        const std::string type = affinePartSettings.get< std::string >("type");
        if (type == "function.affineparametric.default")
          DUNE_THROW(Dune::IOError,
                     "\n" << DSC::colorStringRed("ERROR:")
                     << " can not create a 'function.affineparametric.default' inside itself!");
        _affinePart = std::shared_ptr< ComponentType >(Functions< DomainFieldType, dimDomain,
                                                                  RangeFieldType, dimRange >::create(type, affinePartSettings));
      } else {
        // since it does not have a type, treat it as an expression function
        if (!(affinePartSettings.hasKey("expression") || affinePartSettings.hasSub("expression")))
          DUNE_THROW(Dune::IOError,
                     "\n" << DSC::colorStringRed("ERROR:")
                     << " could not interpret affinePart in the following settings:\n"
                     << settings.reportString("  "));
        if (!affinePartSettings.hasKey("variable"))
          affinePartSettings["variable"] = "x";
        if (!affinePartSettings.hasKey("name"))
          affinePartSettings["name"] = _name;
        if (!affinePartSettings.hasKey("order"))
          affinePartSettings["order"] = DSC::toString(_order);
        _affinePart = std::shared_ptr< ComponentType >(Functions< DomainFieldType, dimDomain,
                                                                  RangeFieldType, dimRange >::create("function.expression", affinePartSettings));
      } // if (componentSettings.hasKey("type"))
    } else if (settings.hasKey("affinePart")) {
      _hasAffinePart = true;
      // there is only one key, interpret it as an entry for an expression function with variable x
      Dune::ParameterTree affinePartSettings;
      affinePartSettings["name"] = _name;
      affinePartSettings["order"] = DSC::toString(_order);
      affinePartSettings["variable"] = "x";
      affinePartSettings["expression"] = settings.get< std::string >("affinePart");
      _affinePart = std::shared_ptr< ComponentType >(Functions< DomainFieldType, dimDomain,
                                                                RangeFieldType, dimRange >::create("function.expression", affinePartSettings));
    }
    // * paramSize
    const size_t _paramSize = settings.get< size_t >("paramSize");
    // * paramRange
    const std::vector< ParamFieldType > _paramMins = settings.getVector< ParamFieldType >("paramMin", _paramSize);
    const std::vector< ParamFieldType > _paramMaxs = settings.getVector< ParamFieldType >("paramMax", _paramSize);
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
    if (settings.hasVector("paramExplanation"))
      _paramExplanation = settings.getVector< std::string >("paramExplanation", 1);
    else if (settings.hasKey("paramExplanation"))
      _paramExplanation.push_back(settings.get< std::string >("paramExplanation"));
    if (_hasAffinePart)
      return new ThisType(_paramSize, _paramRange, _components, _coefficients, _affinePart, _paramExplanation, _order, _name);
    else
      return new ThisType(_paramSize, _paramRange, _components, _coefficients, _paramExplanation, _order, _name);
  } // ... create(...)

  virtual bool parametric() const
  {
    return true;
  }

  virtual bool affineparametric() const
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

  virtual const std::vector< std::shared_ptr< const ComponentType > >& components() const
  {
    return components_;
  }

  virtual const std::vector< std::shared_ptr< const CoefficientType > >& coefficients() const
  {
    return coefficients_;
  }

  virtual bool hasAffinePart() const
  {
    return hasAffinePart_;
  }

  virtual const std::shared_ptr< const ComponentType > & affinePart() const
  {
    DUNE_THROW(Dune::InvalidStateException,
               "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
               << " do not call affinePart() if hasAffinePart() is false!");
    return affinePart_;
  }

  virtual void evaluate(const DomainType& _x, const ParamType& _mu, RangeType& _ret) const
  {
    assert(_mu.size() == paramSize_);
    _ret = RangeFieldType(0);
    RangeType tmpComponentValue;
    RangeFieldType tmpCoefficientValue;
    for (size_t qq = 0; qq < coefficients_.size(); ++qq) {
      components_[qq]->evaluate(_x, tmpComponentValue);
      coefficients_[qq]->evaluate(_mu, tmpCoefficientValue);
      tmpComponentValue *= tmpCoefficientValue;
      _ret += tmpComponentValue;
    }
    if (hasAffinePart_) {
      affinePart_->evaluate(_x, tmpComponentValue);
      _ret += tmpComponentValue;
    }
  } // virtual void evaluate(const DomainType& _x, const ParamType& _mu, RangeType& _ret) const

private:
  size_t paramSize_;
  std::vector< ParamType > paramRange_;
  std::vector< std::shared_ptr< const ComponentType > > components_;
  std::vector< std::shared_ptr< const CoefficientType > > coefficients_;
  bool hasAffinePart_;
  const int order_;
  const std::string name_;
  std::shared_ptr< const ComponentType > affinePart_;
  std::vector< std::string > parameterExplanation_;
}; // class AffineParametricFunctionDefault


} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_FUNCTION_AFFINEPARAMETRIC_DEFAULT_HH
