#ifndef DUNE_STUFF_FUNCTION_FIXED_HH
#define DUNE_STUFF_FUNCTION_FIXED_HH

#include <memory>

#include <dune/common/exceptions.hh>

#include <dune/stuff/common/parameter.hh>
#include <dune/stuff/common/color.hh>

#include "interface.hh"

namespace Dune {
namespace Stuff {


template< class DomainFieldImp, int domainDim, class RangeFieldImp, int rangeDimRows, int rangeDimCols >
class FunctionFixedParameter
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
  typedef typename BaseType::ParamType        ParamType;

  typedef GenericStationaryFunctionInterface< DomainFieldType, dimDomain,
                                              RangeFieldType, dimRangeRows, dimRangeCols > WrappedType;

  static std::string id()
  {
    return BaseType::id() + ".fixed";
  }

  FunctionFixedParameter(const std::shared_ptr< const WrappedType > parametricFunction,
                         const ParamType fixedParameter)
    : wrappedFunction_(parametricFunction)
    , fixedParam_(fixedParameter)
  {
    if (!wrappedFunction_->parametric())
      DUNE_THROW(Dune::InvalidStateException,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " given function not parametric!");
    if (fixedParam_.size() != wrappedFunction_->paramSize())
      DUNE_THROW(Dune::InvalidStateException,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " given parameter has wrong size (is " << fixedParam_.size()
                 << ", should be " << wrappedFunction_->paramSize() << ")!");
  }

  virtual std::string name() const
  {
    return "fixed '" + wrappedFunction_->name() + "' for parameter '" + Dune::Stuff::Common::Parameter::report(fixedParam_);
  }

  virtual int order() const
  {
    return wrappedFunction_->order();
  }

  virtual void evaluate(const DomainType& x, RangeType& ret) const
  {
    wrappedFunction_->evaluate(x, fixedParam_, ret);
  }

private:
  const std::shared_ptr< const WrappedType > wrappedFunction_;
  const ParamType fixedParam_;
}; // class FunctionFixedParameter


template< class DomainFieldImp, int domainDim, class RangeFieldImp, int rangeDimRows, int rangeDimCols >
class FunctionFixedTime
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
//  typedef typename BaseType::ParamType        ParamType;

  typedef TimedependentFunctionInterface< DomainFieldType, dimDomain,
                                          RangeFieldType, dimRangeRows, dimRangeCols > WrappedType;

  static std::string id()
  {
    return BaseType::id() + ".fixed";
  }

  FunctionFixedTime(const std::shared_ptr< const WrappedType > timedependentFunction,
                    const double time)
    : wrappedFunction_(timedependentFunction)
    , time_(time)
  {}

  virtual std::string name() const
  {
    return "fixed '" + wrappedFunction_->name() + "' for time '" + Dune::Stuff::Common::toString(time_);
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
