#ifndef DUNE_STUFF_FUNCTION_PARAMETRIC_SEPARABLE_COEFFICIENT_HH
#define DUNE_STUFF_FUNCTION_PARAMETRIC_SEPARABLE_COEFFICIENT_HH

#ifdef HAVE_CMAKE_CONFIG
  #include "cmake_config.h"
#else
  #include "config.h"
#endif // ifdef HAVE_CMAKE_CONFIG

#include <sstream>
#include <vector>

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
  typedef Coefficient< RangeFieldImp >                                                ThisType;
  typedef ExpressionBase< Common::Parameter::FieldType, Common::Parameter::maxDim, RangeFieldImp, 1 > BaseType;

  typedef Common::Parameter::Type ParamType;

  static std::string id()
  {
    return "function.parametric.separable.coefficient";
  }

  Coefficient(const std::string _expression)
    : BaseType("mu", _expression)
  {}

  Coefficient(const std::vector< std::string > _expressions)
    : BaseType("mu", _expressions)
  {}

  Coefficient(const ThisType& other)
    : BaseType(other)
  {}

  static ThisType createFromDescription(const Dune::ParameterTree& _description)
  {
    const Dune::Stuff::Common::ExtendedParameterTree description(_description);
    // get necessary
    const std::vector< std::string > _expressions = description.getVector< std::string >("expression", 1);
    // create and return
    return ThisType(_expressions);
  } // static ThisType createFromDescription(const Dune::ParameterTree& _description)
}; // class Coefficient


} // namespace Function
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_FUNCTION_PARAMETRIC_SEPARABLE_COEFFICIENT_HH
