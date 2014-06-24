// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_FUNCTION_FIXED_HH
#define DUNE_STUFF_FUNCTION_FIXED_HH

#include <memory>

#include <dune/common/exceptions.hh>

#include <dune/stuff/common/color.hh>

#include <dune/stuff/functions/interfaces.hh>
#include <dune/stuff/playground/functions/time.hh>

namespace Dune {
namespace Stuff {


template< class DomainFieldImp, int domainDim, class RangeFieldImp, int rangeDimRows, int rangeDimCols >
class
  DUNE_DEPRECATED_MSG("Derive this from LocalizableFunctionInterface or GlobalFunctionInterface or put this somewhere else!")
      FunctionFixedTime
  : public FunctionInterface< DomainFieldImp, domainDim, RangeFieldImp, rangeDimRows, rangeDimCols >
{
  typedef FunctionInterface< DomainFieldImp, domainDim, RangeFieldImp, rangeDimRows, rangeDimCols > BaseType;
public:
  typedef typename BaseType::DomainFieldType  DomainFieldType;
  static const int                            dimDomain = BaseType::dimDomain;
  typedef typename BaseType::DomainType       DomainType;
  typedef typename BaseType::RangeFieldType   RangeFieldType;
  static const int                            dimRangeRows = BaseType::dimRangeRows;
  static const int                            dimRangeCols = BaseType::dimRangeCols;
  typedef typename BaseType::RangeType        RangeType;

  typedef TimedependentFunctionInterface< DomainFieldType, dimDomain,
                                          RangeFieldType, dimRangeRows, dimRangeCols > WrappedType;

  static std::string static_id()
  {
    return BaseType::static_id() + ".fixed";
  }

  FunctionFixedTime(const std::shared_ptr< const WrappedType > timedependentFunction,
                    const double time)
    : wrappedFunction_(timedependentFunction)
    , time_(time)
  {}

  virtual std::string name() const
  {
    return "fixed '" + wrappedFunction_->name() + "' for time '" + Dune::Stuff::Common::toString(time_) + "'";
  }

  virtual int order() const
  {
    return wrappedFunction_->order();
  }

  virtual void evaluate(const DomainType& x, RangeType& ret) const
  {
    wrappedFunction_->evaluate(x, time_, ret);
  }

private:
  const std::shared_ptr< const WrappedType > wrappedFunction_;
  const double time_;
}; // class FunctionFixedTime


} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_FUNCTION_FIXED_HH
