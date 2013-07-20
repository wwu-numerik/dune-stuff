#ifndef DUNE_STUFF_FUNCTION_LOCAL_HH
#define DUNE_STUFF_FUNCTION_LOCAL_HH

#include <dune/stuff/localfunction/interface.hh>

namespace Dune {
namespace Stuff {


// forward, includes are below
template< class DomainFieldImp, int domainDim, class RangeFieldImp, int rangeDimRows, int rangeDimCols = 1 >
class FunctionInterface;


// forward, to be used in the traits
template< class EntityImp, class DomainFieldImp, int domainDim, class RangeFieldImp, int rangeDimRows, int rangeDimCols >
class LocalizedFunction;


/**
 *  \brief Matrix valued Traits for LocalizedFunction.
 *
 *        See spacialization for rangeDimRows = 1 for scalar and vector valued Traits.
 */
template< class EntityImp, class DomainFieldImp, int domainDim, class RangeFieldImp, int rangeDimRows, int rangeDimCols = 1 >
class LocalizedFunctionTraits
{
public:
  typedef LocalizedFunction< EntityImp, DomainFieldImp, domainDim, RangeFieldImp, rangeDimRows, rangeDimCols > derived_type;
  typedef EntityImp EntityType;

  typedef DomainFieldImp                                  DomainFieldType;
  static const unsigned int                               dimDomain = domainDim;
  typedef Dune::FieldVector< DomainFieldType, dimDomain > DomainType;

  typedef RangeFieldImp                                                   RangeFieldType;
  static const unsigned int                                               dimRangeRows = rangeDimRows;
  static const unsigned int                                               dimRangeCols = rangeDimCols;
  typedef Dune::FieldMatrix< RangeFieldType, dimRangeRows, dimRangeCols > RangeType;
}; // class LocalizedFunctionTraits


/**
 *  \brief Scalar and vector valued Traits for LocalizedFunction.
 */
template< class EntityImp, class DomainFieldImp, int domainDim, class RangeFieldImp, int rangeDim >
class LocalizedFunctionTraits< EntityImp, DomainFieldImp, domainDim, RangeFieldImp, rangeDim, 1 >
{
public:
  typedef LocalizedFunction< EntityImp, DomainFieldImp, domainDim, RangeFieldImp, rangeDim, 1 > derived_type;
  typedef EntityImp EntityType;

  typedef DomainFieldImp                                  DomainFieldType;
  static const unsigned int                               dimDomain = domainDim;
  typedef Dune::FieldVector< DomainFieldType, dimDomain > DomainType;

  typedef RangeFieldImp                                                   RangeFieldType;
  static const unsigned int                                               dimRange = rangeDim;
  static const unsigned int                                               dimRangeRows = dimRange;
  static const unsigned int                                               dimRangeCols = 1;
  typedef Dune::FieldVector< RangeFieldType, dimRange > RangeType;
}; // class LocalizedFunctionTraits


template< class EntityImp, class DomainFieldImp, int domainDim, class RangeFieldImp, int rangeDimRows, int rangeDimCols >
class LocalizedFunction
  : public LocalFunctionInterface<  LocalizedFunctionTraits< EntityImp, DomainFieldImp, domainDim, RangeFieldImp, rangeDimRows, rangeDimCols >,
                                    DomainFieldImp, domainDim, RangeFieldImp, rangeDimRows, rangeDimCols >
{
  typedef LocalizedFunctionTraits< EntityImp, DomainFieldImp, domainDim, RangeFieldImp, rangeDimRows, rangeDimCols >  Traits;
  typedef LocalFunctionInterface< Traits, DomainFieldImp, domainDim, RangeFieldImp, rangeDimRows, rangeDimCols >      BaseType;
public:
  typedef LocalizedFunction< EntityImp, DomainFieldImp, domainDim, RangeFieldImp, rangeDimRows, rangeDimCols > Type;

  typedef typename Traits::DomainFieldType DomainFieldType;
  static const unsigned int dimDomain = Traits::dimDomain;
  typedef typename Traits::DomainType DomainType;

  typedef typename Traits::RangeFieldType RangeFieldType;
  static const unsigned int dimRangeRows = Traits::dimRangeRows;
  static const unsigned int dimRangeCols = Traits::dimRangeCols;
  typedef typename Traits::RangeType RangeType;

  typedef typename Traits::EntityType EntityType;

  typedef FunctionInterface< DomainFieldType, dimDomain, RangeFieldType, dimRangeRows, dimRangeCols > FunctionType;

  LocalizedFunction(const FunctionType& func, const EntityType& en)
    : function_(func)
    , entity_(en)
  {}

  const EntityType& entity() const
  {
    return entity_;
  }

  virtual int order() const
  {
    return function_.order();
  }

  void evaluate(const DomainType& x, RangeType& ret) const
  {
    function_.evaluate(entity_.geometry().global(x), ret);
  }

  using BaseType::evaluate;

private:
  const FunctionType& function_;
  const EntityType& entity_;
}; // class LocalizedFunction


} // namespace Stuff
} // namespace Dune

#include "interface.hh"

#endif // DUNE_STUFF_FUNCTION_LOCAL_HH
