#ifndef DUNE_STUFF_FUNCTION_PARAMETRIC_SEPARABLE_COEFFICIENT_HH
#define DUNE_STUFF_FUNCTION_PARAMETRIC_SEPARABLE_COEFFICIENT_HH

#ifdef HAVE_CMAKE_CONFIG
  #include "cmake_config.h"
#else
  #include "config.h"
#endif // ifdef HAVE_CMAKE_CONFIG

#include <dune/common/dynvector.hh>
#include <dune/common/fvector.hh>

#include <dune/stuff/common/parameter.hh>
#include <dune/stuff/common/parameter/tree.hh>

#include "../../expression/base.hh"
#include "../../interface.hh"


namespace Dune {
namespace Stuff {
namespace Function {


template< class RangeFieldImp = double >
class Coefficient
  : public ExpressionBase< Common::Parameter::FieldType, Common::Parameter::maxDim, RangeFieldImp, 1 >
{
public:
  typedef Coefficient< RangeFieldImp >                                                                ThisType;
  typedef ExpressionBase< Common::Parameter::FieldType, Common::Parameter::maxDim, RangeFieldImp, 1 > BaseType;

  typedef Common::Parameter::Type ParamType;
  typedef RangeFieldImp           RangeFieldType;

  static std::string id()
  {
    return "function.parametric.separable.coefficient";
  }

  Coefficient(const std::string _expression)
    : BaseType("mu", _expression)
  {}

  Coefficient(const ThisType& other)
    : BaseType(other)
  {}

  void evaluate(const ParamType& _mu, RangeFieldType& _ret) const
  {
    BaseType::evaluate(_mu, _ret);
  }

  RangeFieldType evaluate(const ParamType& _mu) const
  {
    RangeFieldType ret;
    BaseType::evaluate(_mu, ret);
    return ret;
  }
}; // class Coefficient


} // namespace Function
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_FUNCTION_PARAMETRIC_SEPARABLE_COEFFICIENT_HH
