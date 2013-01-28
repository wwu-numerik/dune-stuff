#ifndef DUNE_STUFF_LA_CONTAINER_SEPARABLE_HH
#define DUNE_STUFF_LA_CONTAINER_SEPARABLE_HH

#include <dune/common/exceptions.hh>
#include <dune/common/shared_ptr.hh>

#include <dune/stuff/la/container/eigen.hh>
#include <dune/stuff/function/expression.hh>
#include <dune/stuff/common/separable-container.hh>

namespace Dune {
namespace Stuff {
namespace LA {
namespace Container {


#if HAVE_EIGEN
template< class EigenContainerImp, class ParamFieldImp, int maxNumParams >
class Separable
  : public Dune::Stuff::Common::SeparableContainer< EigenContainerImp,
                                                    Dune::Stuff::Function::Expression<  ParamFieldImp,
                                                                                        maxNumParams,
                                                                                        typename EigenContainerImp::ElementType,
                                                                                        1 >,
                                                    typename EigenContainerImp::size_type >
{
public:
  typedef Dune::Stuff::Common::SeparableContainer<  EigenContainerImp,
                                                    Dune::Stuff::Function::Expression<  ParamFieldImp,
                                                                                        maxNumParams,
                                                                                        typename EigenContainerImp::ElementType,
                                                                                        1 >,
                                                    typename EigenContainerImp::size_type >
    BaseType;
  typedef typename Dune::Stuff
      ::LA
      ::Container
      ::EigenInterface< typename EigenContainerImp::Traits >::derived_type                ComponentType;
  typedef ParamFieldImp                                                                   ParamFieldType;
  static const int maxParams = maxNumParams;
  typedef typename ComponentType::ElementType                                             ElementType;
  typedef typename ComponentType::size_type                                               size_type;
  typedef Dune::Stuff::Function::Expression< ParamFieldType, maxParams, ElementType, 1 >  CoefficientType;
  typedef Eigen::Matrix< ParamFieldType, Eigen::Dynamic, 1 >                              ParamType;

private:
  static std::vector< Dune::shared_ptr< const ComponentType > > constify_vector(std::vector< Dune::shared_ptr< ComponentType > > _vector)
  {
    std::vector< Dune::shared_ptr< const ComponentType > > ret(_vector.size());
    for (size_type ii = 0; ii < _vector.size(); ++ii)
      ret[ii] = _vector[ii];
    return ret;
  }

public:
  Separable(const size_type _paramSize,
            std::vector< Dune::shared_ptr< ComponentType > > _components,
            const std::vector< Dune::shared_ptr< const CoefficientType > > _coefficients)
    : BaseType(_paramSize, constify_vector(_components), _coefficients)
    , components_(_components)
  {
    // sanity checks
    if (BaseType::components().size() < 1)
      DUNE_THROW(Dune::RangeError,
                 "\nERROR: not enough '_components' given!");
    if (!(BaseType::coefficients().size() == BaseType::components().size()
          || BaseType::coefficients().size() == (BaseType::components().size() - 1)))
      DUNE_THROW(Dune::RangeError,
                 "\nERROR: wrong number of 'coefficients_' given!");
    if (BaseType::coefficients().size() == 0) {
      if (BaseType::paramSize() > 0)
        DUNE_THROW(Dune::RangeError,
                   "\nERROR: '_paramSize' has to be zero!");
    } else {
      if (BaseType::paramSize() < 1)
        DUNE_THROW(Dune::RangeError,
                   "\nERROR: '_paramSize' has to be positive!");
    }
  }

  Separable(Dune::shared_ptr< ComponentType > _component)
    : BaseType(_component)
  {
    components_.push_back(_component);
  }

  bool parametric() const
  {
    return BaseType::numCoefficients() > 0;
  }

  std::vector< Dune::shared_ptr< ComponentType > >& components()
  {
    return components_;
  }

  Dune::shared_ptr< ComponentType > fix(const ParamType& mu) const
  {
    assert(parametric() && "Call fix() instead of fix(mu) for nonparametric container!");
    assert(mu.size() == BaseType::paramSize());
    Dune::shared_ptr< ComponentType > ret = Dune::make_shared< ComponentType >(*(BaseType::components()[0]));
    // since we are parametric, at leas one coefficient has to exist
    ParamType coefficient = BaseType::coefficients()[0]->evaluate(mu);
    assert(coefficient.size() == 1);
    ret->backend() *= coefficient;
    size_type qq = 1;
    for (; qq < BaseType::numCoefficients(); ++qq) {
      coefficient = BaseType::coefficients()[qq]->evaluate(mu);
      assert(coefficient.size() == 1);
      ret->backend() += BaseType::components()[qq]->backend() * coefficient[0];
    }
    if (BaseType::numComponents() > qq)
      ret->backend() += BaseType::components()[qq + 1]->base();
    return ret;
  } // Dune::shared_ptr< ComponentType > fix(const ParamType& mu) const

  Dune::shared_ptr< const ComponentType > fix() const
  {
    assert(BaseType::paramSize() == 0);
    assert(BaseType::numComponents() == 1);
    assert(BaseType::numCoefficients() == 0);
    return BaseType::components()[0];
  }

private:
  std::vector< Dune::shared_ptr< ComponentType > > components_;
}; // class Separable
#endif // HAVE_EIGEN


} // namespace Container
} // namespace LA
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_LA_CONTAINER_SEPARABLE_HH
