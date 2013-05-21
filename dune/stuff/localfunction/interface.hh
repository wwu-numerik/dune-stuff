#ifndef DUNE_STUFF_LOCALFUNCTION_INTERFACE_HH
#define DUNE_STUFF_LOCALFUNCTION_INTERFACE_HH

#include <dune/common/bartonnackmanifcheck.hh>
#include <dune/common/fvector.hh>
#include <dune/common/fmatrix.hh>

namespace Dune {
namespace Stuff {


/**
 *  \brief  Interface for matrix valued globalvalued functions, which can be evaluated locally on one Entity.
 *
 *          This is the interface for matrixvalued functions, see the specialization for rangeDimCols = 1 for scalar
 *          and vector valued functions.
 */
template< class Traits, class DomainFieldImp, int domainDim, class RangeFieldImp, int rangeDimRows, int rangeDimCols >
class LocalFunctionInterface
{
public:
  typedef typename Traits::derived_type derived_type;
  typedef typename Traits::EntityType   EntityType;

  typedef DomainFieldImp                                  DomainFieldType;
  static const unsigned int                               dimDomain = domainDim;
  typedef Dune::FieldVector< DomainFieldType, dimDomain > DomainType;

  typedef RangeFieldImp                                                   RangeFieldType;
  static const unsigned int                                               dimRangeRows = rangeDimRows;
  static const unsigned int                                               dimRangeCols = rangeDimCols;
  typedef Dune::FieldMatrix< RangeFieldType, dimRangeRows, dimRangeCols > RangeType;

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

  RangeType evaluate(const DomainType& x) const
  {
    RangeType ret(0);
    evaluate(x, ret);
    return ret;
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


/**
 *  \brief  Interface for scalar and vector valued globalvalued functions, which can be evaluated locally on one Entity.
 */
template< class Traits, class DomainFieldImp, int domainDim, class RangeFieldImp, int rangeDim >
class LocalFunctionInterface< Traits, DomainFieldImp, domainDim, RangeFieldImp, rangeDim, 1 >
{
public:
  typedef typename Traits::derived_type derived_type;
  typedef typename Traits::EntityType   EntityType;

  typedef DomainFieldImp                                  DomainFieldType;
  static const unsigned int                               dimDomain = domainDim;
  typedef Dune::FieldVector< DomainFieldType, dimDomain > DomainType;

  typedef RangeFieldImp                                 RangeFieldType;
  static const unsigned int                             dimRange = rangeDim;
  static const unsigned int                             dimRangeRows = dimRange;
  static const unsigned int                             dimRangeCols = 1;
  typedef Dune::FieldVector< RangeFieldType, dimRange > RangeType;

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

  RangeType evaluate(const DomainType& x) const
  {
    RangeType ret(0);
    evaluate(x, ret);
    return ret;
  }

  derived_type& asImp()
  {
    return static_cast< derived_type& >(*this);
  }

  const derived_type& asImp() const
  {
    return static_cast< const derived_type& >(*this);
  }
}; // class LocalFunctionInterface< ..., 1 >


/**
 *  \brief  Flag for all functions which provide a localFunction(entity) method.
 */
class LocalizableFunction
{};


} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_LOCALFUNCTION_INTERFACE_HH
