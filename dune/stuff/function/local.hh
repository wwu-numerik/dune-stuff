#ifndef DUNE_STUFF_FUNCTION_LOCAL_HH
#define DUNE_STUFF_FUNCTION_LOCAL_HH

#include <memory>

#include <dune/common/exceptions.hh>
#include <dune/common/fvector.hh>

#include <dune/stuff/common/color.hh>

#include "interface.hh"
#include "../localfunction/interface.hh"

namespace Dune {
namespace Stuff {


// forward, to be used in the traits
template< class DomainFieldImp, unsigned int domainDim, class RangeFieldImp, unsigned int rangeDim, class EntityImp >
class LocalFunctionWrapper;


template< class DomainFieldImp, unsigned int domainDim, class RangeFieldImp, unsigned int rangeDim, class EntityImp >
class LocalFunctionWrapperTraits
{
public:
  typedef LocalFunctionWrapper< DomainFieldImp, domainDim, RangeFieldImp, rangeDim, EntityImp > derived_type;
  typedef DomainFieldImp    DomainFieldType;
  static const unsigned int dimDomain = domainDim;
  typedef RangeFieldImp     RangeFieldType;
  static const unsigned int dimRange = rangeDim;
  typedef EntityImp         EntityType;
};


template< class DomainFieldImp, unsigned int domainDim, class RangeFieldImp, unsigned int rangeDim, class EntityImp >
class LocalFunctionWrapper
  : public LocalFunctionInterface< LocalFunctionWrapperTraits< DomainFieldImp, domainDim, RangeFieldImp, rangeDim, EntityImp > >
{
public:
  typedef LocalFunctionWrapper< DomainFieldImp, domainDim, RangeFieldImp, rangeDim, EntityImp >       ThisType;
  typedef LocalFunctionWrapperTraits< DomainFieldImp, domainDim, RangeFieldImp, rangeDim, EntityImp > Traits;
  typedef LocalFunctionInterface< Traits >                                                            InterfaceType;

  typedef typename Traits::EntityType EntityType;

  typedef Traits::DomainFieldType                         DomainFieldType;
  static const unsigned int                               dimDomain = Traits::dimDomain;
  typedef Dune::FieldVector< DomainFieldType, dimDomain > DomainType;

  typedef Traits::RangeFieldType                          RangeFieldType;
  static const unsigned int                               dimRange = Traits::dimRange;
  typedef Dune::FieldVector< RangeFieldType, dimDomain >  RangeType;

  typedef FunctionInterface< DomainFieldType, dimDomain, RangeFieldType, dimRange > WrappedFunctionType;

  LocalFunctionWrapper(const WrappedFunctionType& function, const EntityType& en)
    : wrapped_(function)
    , entity_(en)
  {
    assert(!wrapped_->parametric());
  }

  virtual int order() const
  {
    return wrapped_.order();
  }

  const EntityType& entity() const
  {
    return entity_;
  }

  void evaluate(const DomainType& x, RangeType& ret) const
  {
    wrapped_.evaluate(entity_.geometry().global(x), ret);
  }

private:
  const WrappedFunctionType& wrapped_;
  const EntityType& entity_;
}; // class LocalFunctionWrapper


} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_FUNCTION_LOCAL_HH
