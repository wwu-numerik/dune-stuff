#ifndef DUNE_STUFF_COMMON_PARAMETER_HH
#define DUNE_STUFF_COMMON_PARAMETER_HH

#include <sstream>

#include <dune/common/dynvector.hh>

namespace Dune {
namespace Stuff {
namespace Common {


/**
 *  \brief  parameter class representing mu
 *          This is mainly needed for the static maxDim (in the Function::Parametric::Separable::Coefficient).
 */
class Parameter
{
public:
  typedef double                            FieldType;
#ifdef DUNE_STUFF_FUNCTION_PARAMMAXDIM
  static const int                          maxDim = DUNE_STUFF_FUNCTION_PARAMMAXDIM;
#else
  static const int                          maxDim = 50;
#endif
  typedef Dune::DynamicVector< FieldType >  Type;

  static std::string report(const Type& mu)
  {
    std::stringstream ret;
    ret << "[";
    for (size_t ii = 0; ii < mu.size() - 1; ++ii)
      ret << mu[ii] << "; ";
    if (mu.size() > 1)
      ret << mu[mu.size() - 1];
    ret << "]";
    return ret.str();
  }

  static std::string reportWOwhitespace(const Type& mu)
  {
    std::stringstream ret;
    for (size_t ii = 0; ii < mu.size() - 1; ++ii)
      ret << mu[ii] << "_";
    if (mu.size() > 1)
      ret << mu[mu.size() - 1];
    return ret.str();
  }

};


} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_PARAMETER_HH
