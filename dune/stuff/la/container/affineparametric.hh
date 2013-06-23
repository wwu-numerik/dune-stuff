#ifndef DUNE_STUFF_LA_CONTAINER_AFFINEPARAMETRIC_HH
#define DUNE_STUFF_LA_CONTAINER_AFFINEPARAMETRIC_HH

#include <memory>

#include <dune/common/exceptions.hh>
#include <dune/common/shared_ptr.hh>

#include <dune/stuff/la/container/eigen.hh>
#include <dune/stuff/common/parameter.hh>
#include <dune/stuff/function/affineparametric/coefficient.hh>
#include <dune/stuff/common/color.hh>

namespace Dune {
namespace Stuff {
namespace LA {


#if HAVE_EIGEN
template< class EigenContainerImp >
class AffineParametricContainer
{
public:
  typedef AffineParametricContainer< EigenContainerImp >  ThisType;

  typedef typename LA::EigenContainerInterface< typename EigenContainerImp::Traits >::derived_type  ComponentType;
  typedef Stuff::AffineParametricCoefficientFunction< typename EigenContainerImp::ElementType >     CoefficientType;

  typedef typename Stuff::Common::Parameter::Type ParamType;

  AffineParametricContainer(const size_t _paramSize,
                            std::vector< std::shared_ptr< ComponentType > >& _components,
                            const std::vector< std::shared_ptr< const CoefficientType > >& _coefficients)
    : paramSize_(_paramSize)
    , components_(_components)
    , coefficients_(_coefficients)
    , hasAffinePart_(false)
  {
    // sanity checks
    if (components_.size() < 1)
      DUNE_THROW(Dune::RangeError,
                 "\nERROR: not enough 'components' given!");
    if (!coefficients_.size() == components_.size())
      DUNE_THROW(Dune::RangeError,
                 "\nERROR: wrong number of 'coefficients' given!");
    if (paramSize_ < 1)
      DUNE_THROW(Dune::RangeError,
                 "\nERROR: 'paramSize' has to be positive!");
  } // AffineParametric(...)

  AffineParametricContainer(const size_t _paramSize,
                            std::vector< std::shared_ptr< ComponentType > >& _components,
                            const std::vector< std::shared_ptr< const CoefficientType > >& _coefficients,
                            std::shared_ptr< ComponentType > _affinePart)
    : paramSize_(_paramSize)
    , components_(_components)
    , coefficients_(_coefficients)
    , hasAffinePart_(true)
    , affinePart_(_affinePart)
  {
    // sanity checks
    if (components_.size() < 1)
      DUNE_THROW(Dune::RangeError,
                 "\nERROR: not enough 'components' given!");
    if (!coefficients_.size() == components_.size())
      DUNE_THROW(Dune::RangeError,
                 "\nERROR: wrong number of 'coefficients' given!");
    if (paramSize_ < 1)
      DUNE_THROW(Dune::RangeError,
                 "\nERROR: 'paramSize' has to be positive!");
  } // AffineParametric(...)

  AffineParametricContainer(std::shared_ptr< ComponentType > _affinePart)
    : paramSize_(0)
    , components_(0)
    , coefficients_(0)
    , hasAffinePart_(true)
    , affinePart_(_affinePart)
  {}

  bool parametric() const
  {
    return (coefficients_.size() > 0);
  }

  size_t paramSize() const
  {
    return paramSize_;
  }

  std::vector< std::shared_ptr< ComponentType > > components()
  {
    return components_;
  }

  const std::vector< std::shared_ptr< ComponentType > >& components() const
  {
    return components_;
  }

  const std::vector< std::shared_ptr< const CoefficientType > >& coefficients() const
  {
    return coefficients_;
  }

  bool hasAffinePart() const
  {
    return hasAffinePart_;
  }

  std::shared_ptr< ComponentType > affinePart()
  {
    if (hasAffinePart_)
      return affinePart_;
    else
      DUNE_THROW(Dune::InvalidStateException,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " do not call affinePart() if hasAffinePart() is false!");
  }

  std::shared_ptr< const ComponentType >& affinePart() const
  {
    if (hasAffinePart_)
      return affinePart_;
    else
      DUNE_THROW(Dune::InvalidStateException,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " do not call affinePart() if hasAffinePart() is false!");
  }

  std::shared_ptr< ComponentType > fix(const ParamType mu = ParamType()) const
  {
    if (components_.size() == 0) {
      assert(hasAffinePart_);
      assert(mu.size() == 0);
      return std::make_shared< ComponentType >(*affinePart_);
    } else {
      assert(mu.size() == paramSize_);
      assert(components_.size() == coefficients_.size());
      // here we have at least one component/coefficient
      auto ret = make_shared< ComponentType >(*components()[0]);
      ret->backend() *= coefficients_[0]->evaluate(mu);
      // then we do the rest
      for (size_t qq = 1; qq < components_.size(); ++qq)
        ret->backend() += components_[qq]->backend() * coefficients_[qq]->evaluate(mu);
      // and the affine part
      if (hasAffinePart_)
        ret->backend() += affinePart_->backend();
      return ret;
    }
  } // std::shared_ptr< ComponentType > fix(const ParamType& mu) const

private:
  const size_t paramSize_;
  std::vector< std::shared_ptr< ComponentType > > components_;
  const std::vector< std::shared_ptr< const CoefficientType > > coefficients_;
  const bool hasAffinePart_;
  std::shared_ptr< ComponentType > affinePart_;
}; // class Separable
#endif // HAVE_EIGEN


} // namespace LA
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_LA_CONTAINER_AFFINEPARAMETRIC_HH
