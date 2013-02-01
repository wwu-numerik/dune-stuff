#ifndef DUNE_STUFF_LA_CONTAINER_SEPARABLE_HH
#define DUNE_STUFF_LA_CONTAINER_SEPARABLE_HH

#include <dune/common/exceptions.hh>
#include <dune/common/shared_ptr.hh>

#include <dune/stuff/la/container/eigen.hh>
#include <dune/stuff/function/nonparametric/expression.hh>
#include <dune/stuff/common/separable-container.hh>

namespace Dune {
namespace Stuff {
namespace LA {
namespace Container {


#if HAVE_EIGEN
template< class EigenContainerImp, class ParamFieldImp, int maxNumParams >
class Separable
  : public Common::SeparableContainer<  EigenContainerImp,
                                        Dune::Stuff::Function::Coefficient< ParamFieldImp,
                                                                            maxNumParams,
                                                                            typename EigenContainerImp::ElementType >,
                                        typename EigenContainerImp::size_type >
{
public:
  typedef Common::SeparableContainer< EigenContainerImp,
                                      Dune::Stuff::Function::Coefficient< ParamFieldImp,
                                                                          maxNumParams,
                                                                          typename EigenContainerImp::ElementType >,
                                      typename EigenContainerImp::size_type >
                                                                      BaseType;
  typedef Separable< EigenContainerImp, ParamFieldImp, maxNumParams > ThisType;

  typedef typename LA::Container::EigenInterface< typename EigenContainerImp::Traits >::derived_type  ComponentType;
  typedef Dune::Stuff::Function::Coefficient< ParamFieldImp,
                                              maxNumParams,
                                              typename EigenContainerImp::ElementType > CoefficientType;
  typedef typename CoefficientType::ParamType ParamType;
  typedef typename CoefficientType::size_type size_type;

private:
  static std::vector< Dune::shared_ptr< const ComponentType > > constify_vector(std::vector< Dune::shared_ptr< ComponentType > > _vector)
  {
    std::vector< Dune::shared_ptr< const ComponentType > > ret(_vector.size());
    for (typename std::vector< Dune::shared_ptr< ComponentType > >::size_type ii = 0; ii < _vector.size(); ++ii)
      ret[ii] = _vector[ii];
    return ret;
  }

public:
  Separable(const size_type _paramSize,
            std::vector< Dune::shared_ptr< ComponentType > > _components,
            const std::vector< Dune::shared_ptr< const CoefficientType > > _coefficients)
    : BaseType(_paramSize, constify_vector(_components), _coefficients)
    , components_(_components)
  {}

  Separable(Dune::shared_ptr< ComponentType > _component)
    : BaseType(_component)
  {
    components_.push_back(_component);
  }

  std::vector< Dune::shared_ptr< ComponentType > > components()
  {
    return components_;
  }

  using BaseType::components;

  Dune::shared_ptr< ComponentType > fix(const ParamType& _mu) const
  {
    assert(false && "Implement me!");
//    assert(parametric() && "Call fix() instead of fix(mu) for nonparametric container!");
//    assert(mu.size() == BaseType::paramSize());
//    Dune::shared_ptr< ComponentType > ret = Dune::make_shared< ComponentType >(*(BaseType::components()[0]));
//    // since we are parametric, at leas one coefficient has to exist
//    ParamType coefficient = BaseType::coefficients()[0]->evaluate(mu);
//    assert(coefficient.size() == 1);
//    ret->backend() *= coefficient[0];
//    size_type qq = 1;
//    for (; qq < BaseType::numCoefficients(); ++qq) {
//      coefficient = BaseType::coefficients()[qq]->evaluate(mu);
//      assert(coefficient.size() == 1);
//      ret->backend() += BaseType::components()[qq]->backend() * coefficient[0];
//    }
//    if (BaseType::numComponents() > qq)
//      ret->backend() += BaseType::components()[qq + 1]->backend();
//    return ret;
  } // Dune::shared_ptr< ComponentType > fix(const ParamType& mu) const

//  Dune::shared_ptr< ComponentType > fix() const
//  {
//    assert(BaseType::paramSize() == 0);
//    assert(BaseType::numComponents() == 1);
//    assert(BaseType::numCoefficients() == 0);
//    return Dune::make_shared< ComponentType >(*(BaseType::components()[0]));
//  }

private:
  std::vector< Dune::shared_ptr< ComponentType > > components_;
}; // class Separable
#endif // HAVE_EIGEN


} // namespace Container
} // namespace LA
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_LA_CONTAINER_SEPARABLE_HH
