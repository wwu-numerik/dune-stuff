#ifndef DUNE_STUFF_LA_CONTAINER_SEPARABLE_HH
#define DUNE_STUFF_LA_CONTAINER_SEPARABLE_HH

#include <dune/common/exceptions.hh>
#include <dune/common/shared_ptr.hh>

#include <dune/stuff/la/container/eigen.hh>
#include <dune/stuff/common/parameter.hh>
#include <dune/stuff/function/parametric/separable/coefficient.hh>

namespace Dune {
namespace Stuff {
namespace LA {
namespace Container {


#if HAVE_EIGEN
template< class EigenContainerImp >
class Separable
{
public:
  typedef Separable< EigenContainerImp >  ThisType;

  typedef typename LA::Container::EigenInterface< typename EigenContainerImp::Traits >::derived_type  ComponentType;
  typedef Stuff::Function::Coefficient< typename EigenContainerImp::ElementType >                     CoefficientType;

  typedef typename Stuff::Common::Parameter::Type ParamType;

public:
  Separable(const size_t _paramSize,
            std::vector< Dune::shared_ptr< ComponentType > >& _components,
            const std::vector< Dune::shared_ptr< const CoefficientType > >& _coefficients)
    : paramSize_(_paramSize)
    , components_(_components)
    , coefficients_(_coefficients)
  {
    // sanity checks
    if (components_.size() < 1)
      DUNE_THROW(Dune::RangeError,
                 "\nERROR: not enough '_components' given!");
    if (!(coefficients_.size() == components_.size()
          || coefficients_.size() == (components_.size() - 1)))
      DUNE_THROW(Dune::RangeError,
                 "\nERROR: wrong number of 'coefficients_' given!");
    if (coefficients_.size() == 0) {
      if (paramSize_ > 0)
        DUNE_THROW(Dune::RangeError,
                   "\nERROR: '_paramSize' has to be zero!");
    } else {
      if (paramSize_ < 1)
        DUNE_THROW(Dune::RangeError,
                   "\nERROR: '_paramSize' has to be positive!");
    }
  }

  Separable(Dune::shared_ptr< ComponentType > _component)
    : paramSize_(0)
  {
    components_.push_back(_component);
  }

  bool parametric() const
  {
    return numCoefficients() > 0;
  }

  const size_t paramSize() const
  {
    return paramSize_;
  }

  const size_t numComponents() const
  {
    return components_.size();
  }

  std::vector< Dune::shared_ptr< ComponentType > > components()
  {
    return components_;
  }

  const std::vector< Dune::shared_ptr< ComponentType > >& components() const
  {
    return components_;
  }

  const size_t numCoefficients() const
  {
    return coefficients_.size();
  }

  const std::vector< Dune::shared_ptr< const CoefficientType > >& coefficients() const
  {
    return coefficients_;
  }

  Dune::shared_ptr< ComponentType > fix(const ParamType _mu = ParamType()) const
  {
    // in any case, there exists at least one component
    Dune::shared_ptr< ComponentType > ret = Dune::make_shared< ComponentType >(*(components_[0]));
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
        ret->backend() += components_[qq + 1]->backend();
    } // if (parametric())
    return ret;
  } // Dune::shared_ptr< ComponentType > fix(const ParamType& mu) const

private:
  const size_t paramSize_;
  std::vector< Dune::shared_ptr< ComponentType > > components_;
  const std::vector< Dune::shared_ptr< const CoefficientType > > coefficients_;
}; // class Separable
#endif // HAVE_EIGEN


} // namespace Container
} // namespace LA
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_LA_CONTAINER_SEPARABLE_HH
