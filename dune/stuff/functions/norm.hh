#ifndef DUNE_STUFF_FUNCTION_NORM_HH
#define DUNE_STUFF_FUNCTION_NORM_HH

#include "interfaces.hh"

#if HAVE_DUNE_FEM
# include <dune/fem/function/common/discretefunction.hh>
# include <dune/fem/misc/l2norm.hh>
# include <dune/fem/misc/h1norm.hh>
#endif

namespace Dune {
namespace Stuff {

#if HAVE_DUNE_FEM
template <class TraitsType>
static typename TraitsType::RangeFieldType l2norm(const Dune::Fem::DiscreteFunctionInterface<TraitsType>& function)
{
  return Dune::Fem::L2Norm<typename TraitsType::DiscreteFunctionType::GridPartType>(function.space().gridPart()).norm(function);
}

template <class FunctionType_A, class TraitsType>
typename TraitsType::RangeFieldType l2norm(const FunctionType_A& function_A, const Dune::Fem::DiscreteFunctionInterface<TraitsType>& function_B)
{
  static_assert(std::is_base_of<Dune::Fem::HasLocalFunction, FunctionType_A>::value, "");
  Dune::Fem::L2Norm<typename TraitsType::DiscreteFunctionType::GridPartType> norm(function_B.space().gridPart());
  return norm.distance(function_A, function_B);
}

template <class TraitsType>
static typename TraitsType::RangeFieldType h1norm(const Dune::Fem::DiscreteFunctionInterface<TraitsType>& function)
{
  const Dune::Fem::H1Norm<typename TraitsType::DiscreteFunctionType::GridPartType> norm(function.space().gridPart());
  return norm.norm(function);
}

template <class FunctionType_A, class TraitsType>
typename TraitsType::RangeFieldType h1norm(const FunctionType_A& function_A, const Dune::Fem::DiscreteFunctionInterface<TraitsType>& function_B)
{
  static_assert(std::is_base_of<Dune::Fem::HasLocalFunction, FunctionType_A>::value, "");
  Dune::Fem::H1Norm<typename TraitsType::DiscreteFunctionType::GridPartType> norm(function_B.space().gridPart());
  return norm.distance(function_A, function_B);
}
#endif

} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_FUNCTION_NORM_HH
