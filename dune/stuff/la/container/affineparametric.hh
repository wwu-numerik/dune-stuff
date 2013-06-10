#ifndef DUNE_STUFF_LA_CONTAINER_AFFINEPARAMETRIC_HH
#define DUNE_STUFF_LA_CONTAINER_AFFINEPARAMETRIC_HH

#include <memory>

#include <dune/common/exceptions.hh>
#include <dune/common/shared_ptr.hh>

#include <dune/stuff/la/container/eigen.hh>
#include <dune/stuff/common/parameter.hh>
#include <dune/stuff/function/affineparametric/coefficient.hh>

namespace Dune {
namespace Stuff {
namespace LA {


#if HAVE_EIGEN
template< class EigenContainerImp >
class AffineParametricContainer
{
public:
  typedef AffineParametricContainer< EigenContainerImp >  ThisType;

  typedef typename LA::EigenInterface< typename EigenContainerImp::Traits >::derived_type  ComponentType;
  typedef Stuff::FunctionAffineSeparablCoefficient< typename EigenContainerImp::ElementType >         CoefficientType;

  typedef typename Stuff::Common::Parameter::Type ParamType;

public:
  AffineParametricContainer(const size_t _paramSize,
                   std::vector< std::shared_ptr< ComponentType > >& _components,
                   const std::vector< std::shared_ptr< const CoefficientType > >& _coefficients)
    : paramSize_(_paramSize)
    , components_(_components)
    , coefficients_(_coefficients)
  {
    // sanity checks
    if (components_.size() < 1)
      DUNE_THROW(Dune::RangeError,
                 "\nERROR: not enough 'components' given!");
    if (!(coefficients_.size() == components_.size()
          || coefficients_.size() == (components_.size() - 1)))
      DUNE_THROW(Dune::RangeError,
                 "\nERROR: wrong number of 'coefficients' given!");
    if (coefficients_.size() == 0) {
      if (paramSize_ > 0)
        DUNE_THROW(Dune::RangeError,
                   "\nERROR: 'paramSize' has to be zero!");
    } else {
      if (paramSize_ < 1)
        DUNE_THROW(Dune::RangeError,
                   "\nERROR: 'paramSize' has to be positive!");
    }
  } // AffineParametric(...)

  AffineParametricContainer(std::shared_ptr< ComponentType > _component)
    : paramSize_(0)
  {
    components_.push_back(_component);
  }

  bool parametric() const
  {
    return numCoefficients() > 0;
  }

  size_t paramSize() const
  {
    return paramSize_;
  }

  size_t numComponents() const
  {
    return components_.size();
  }

  std::vector< std::shared_ptr< ComponentType > > components()
  {
    return components_;
  }

  const std::vector< std::shared_ptr< ComponentType > >& components() const
  {
    return components_;
  }

  size_t numCoefficients() const
  {
    return coefficients_.size();
  }

  const std::vector< std::shared_ptr< const CoefficientType > >& coefficients() const
  {
    return coefficients_;
  }

  std::shared_ptr< ComponentType > fix(const ParamType _mu = ParamType()) const
  {
    // in any case, there exists at least one component
    auto ret = std::make_shared< ComponentType >(*(components_[0]));
    // if we are parametric, we have to do some more
    if (parametric()) {
      assert(_mu.size() == paramSize());
      // since we are parametric, at least one coefficient has to exist as well
      ret->backend() *= coefficients_[0]->evaluate(_mu);
      // sum over the rest of the component/coefficient combinations
      size_t qq = 1;
      for (; qq < numCoefficients(); ++qq)
        ret->backend() += components_[qq]->backend() * coefficients_[qq]->evaluate(_mu);
      // add the last lonely component if necesarry
      if (numComponents() > qq)
        ret->backend() += components_[qq]->backend();
    } // if (parametric())
    return ret;
  } // std::shared_ptr< ComponentType > fix(const ParamType& mu) const

private:
  const size_t paramSize_;
  std::vector< std::shared_ptr< ComponentType > > components_;
  const std::vector< std::shared_ptr< const CoefficientType > > coefficients_;
}; // class Separable
#endif // HAVE_EIGEN


} // namespace LA
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_LA_CONTAINER_AFFINEPARAMETRIC_HH
