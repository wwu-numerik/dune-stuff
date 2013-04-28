#ifndef DUNE_STUFF_LOCALFUNCTION_INTERFACE_HH
#define DUNE_STUFF_LOCALFUNCTION_INTERFACE_HH

#include <dune/common/bartonnackmanifcheck.hh>
#include <dune/common/fvector.hh>
#include <dune/common/fmatrix.hh>

namespace Dune {
namespace Stuff {


template< class Traits >
class LocalFunctionInterface
{
public:
  typedef LocalFunctionInterface< Traits >  ThisType;
  typedef typename Traits::derived_type     derived_type;

  typedef typename Traits::EntityType EntityType;

  typedef Traits::DomainFieldType                         DomainFieldType;
  static const unsigned int                               dimDomain = Traits::dimDomain;
  typedef Dune::FieldVector< DomainFieldType, dimDomain > DomainType;

  typedef Traits::RangeFieldType                          RangeFieldType;
  static const unsigned int                               dimRange = Traits::dimRange;
  typedef Dune::FieldVector< RangeFieldType, dimDomain >  RangeType;

  const EntityType& entity() const
  {
    CHECK_INTERFACE_IMPLEMENTATION(asImp().entity());
    return asImp().entity();
  }

  virtual int order() const
  {
    return -1;
  }

  void evaluate(const DomainType& x, RangeType& ret) const
  {
    CHECK_INTERFACE_IMPLEMENTATION(asImp().evaluate(x, ret));
    asImp().evaluate(x, ret);
  }

  derived_type& asImp()
  {
    return static_cast< derived_type& >(*this);
  }

  const derived_type& asImp() const
  {
    return static_cast< const derived_type& >(*this);
  }
}; // class LocalFunctionInterface


template< class Traits >
class LocalDifferentiableFunctionInterface
  : public LocalFunctionInterface< Traits >
{
public:
  typedef LocalDifferentiableFunctionInterface< Traits >  ThisType;
  typedef LocalFunctionInterface< Traits >                BaseType;
  typedef typename Traits::derived_type                   derived_type;
  typedef typename Traits::EntityType EntityType;

  typedef Traits::DomainFieldType                         DomainFieldType;
  static const unsigned int                               dimDomain = Traits::dimDomain;
  typedef Dune::FieldVector< DomainFieldType, dimDomain > DomainType;

  typedef Traits::RangeFieldType                          RangeFieldType;
  static const unsigned int                               dimRange = Traits::dimRange;
  typedef Dune::FieldVector< RangeFieldType, dimDomain >  RangeType;

  typedef Dune::FieldMatrix< RangeFieldType, dimRange, dimDomain > JacobianRangeType;

  using BaseType::entity;
  using BaseType::evaluate;
  using BaseType::asImp;

  void jacobian(const DomainType& x, JacobianRangeType& ret) const
  {
    CHECK_INTERFACE_IMPLEMENTATION(asImp().jacobian(x, ret));
    asImp().jacobian(x, ret);
  }
}; // class LocalDifferentiableFunctionInterface


} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_LOCALFUNCTION_INTERFACE_HH
