// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_FUNCTION_NORM_HH
#define DUNE_STUFF_FUNCTION_NORM_HH

#include <dune/stuff/functions/interfaces.hh>

#if HAVE_DUNE_FEM
# include <dune/stuff/common/disable_warnings.hh>
#   include <dune/fem/function/common/discretefunction.hh>
#   include <dune/fem/misc/l2norm.hh>
# include <dune/stuff/common/reenable_warnings.hh>

# include <dune/fem/misc/h1norm.hh>
#endif

#if HAVE_DUNE_PDELAB
# include <dune/stuff/common/disable_warnings.hh>
#   include <dune/pdelab/test/l2difference.hh>
#   include <dune/pdelab/common/functionutilities.hh>
# include <dune/stuff/common/reenable_warnings.hh>
# include <dune/pdelab/common/functionwrappers.hh>
# include <dune/stuff/functions/pdelabadapter.hh>
#endif

#if HAVE_DUNE_GDT
# include <dune/gdt/products/l2.hh>
# include <dune/gdt/products/h1.hh>
#endif

namespace Dune {
namespace PDELab {
template<typename GFS, typename C>
class ISTLBlockVectorContainer;

} //namespace PDELab

namespace Stuff {

#if HAVE_DUNE_PDELAB
template <class FunctionType_A, class GFS, class C>
typename GFS::Traits::FiniteElementType::Traits::LocalBasisType::Traits::RangeFieldType
l2distance(const FunctionType_A& function_A, const PDELab::ISTLBlockVectorContainer<GFS,C>& function_B)
DUNE_DEPRECATED_MSG("Use the interfaces from interfaces.hh or put this somewhere else!");

template <class FunctionType_A, class GFS, class C>
typename GFS::Traits::FiniteElementType::Traits::LocalBasisType::Traits::RangeFieldType
l2distance(const FunctionType_A& function_A, const PDELab::ISTLBlockVectorContainer<GFS,C>& function_B)
{
  using namespace PDELab;
  typedef DiscreteGridFunction<GFS,ISTLBlockVectorContainer<GFS,C>> DGF;
  DGF disc(function_B.gridFunctionSpace(), function_B);

  const auto grid_f = pdelabAdapted(function_A, function_B.gridFunctionSpace().gridView());
  typedef DifferenceSquaredAdapter<decltype(grid_f), DGF> DifferenceSquared;
  DifferenceSquared differencesquared(grid_f,disc);
  typename DifferenceSquared::Traits::RangeType l2errorsquared(0.0);
  const int magic_number_order = 8;
  integrateGridFunction(differencesquared, l2errorsquared, magic_number_order);
  const auto& comm = function_B.gridFunctionSpace().gridView().comm();
  return sqrt(comm.sum(l2errorsquared));
}

template <class FunctionType_A, class GFS, class C>
typename GFS::Traits::FiniteElementType::Traits::LocalBasisType::Traits::RangeFieldType
h1distance(const FunctionType_A& function_A, const PDELab::ISTLBlockVectorContainer<GFS,C>& function_B)
DUNE_DEPRECATED_MSG("Use the interfaces from interfaces.hh or put this somewhere else!");

template <class FunctionType_A, class GFS, class C>
typename GFS::Traits::FiniteElementType::Traits::LocalBasisType::Traits::RangeFieldType
h1distance(const FunctionType_A& function_A, const PDELab::ISTLBlockVectorContainer<GFS,C>& function_B)
{
//  static_assert(std::is_base_of<Dune::Fem::HasLocalFunction, FunctionType_A>::value, "");
  using namespace PDELab;
  typedef DiscreteGridFunction<GFS,ISTLBlockVectorContainer<GFS,C>> DGF;
  DGF disc(function_B.gridFunctionSpace(), function_B);

  const auto grid_f = pdelabAdapted(function_A, function_B.gridFunctionSpace().gridView());
  typedef H1DifferenceSquaredAdapter<decltype(grid_f), PDELab::ISTLBlockVectorContainer<GFS,C>, GFS> H1DifferenceSquared;
  H1DifferenceSquared h1_difference_squared(grid_f, function_B);
  typename H1DifferenceSquared::Traits::RangeType value(0.0);
  const int magic_number_order = 8;
  integrateGridFunction(h1_difference_squared, value, magic_number_order);
  const auto& comm = function_B.gridFunctionSpace().gridView().comm();
  return std::sqrt(comm.sum(value));
}
#endif // HAVE_DUNE_PDELAB

#if HAVE_DUNE_GDT
template< class SpaceImp, class VectorImp >
static typename VectorImp::RangeFieldType l2norm(const GDT::ConstDiscreteFunction<SpaceImp, VectorImp> function)
{
  return GDT::Products::L2< typename SpaceImp::GridViewType >(function.space().grid_view()).induced_norm(function);
}
#endif

#if HAVE_DUNE_FEM
template <class TraitsType>
static typename TraitsType::RangeFieldType l2norm(const Dune::Fem::DiscreteFunctionInterface<TraitsType>& function)
DUNE_DEPRECATED_MSG("Use the interfaces from interfaces.hh or put this somewhere else!");

template <class TraitsType>
static typename TraitsType::RangeFieldType l2norm(const Dune::Fem::DiscreteFunctionInterface<TraitsType>& function)
{
  return Dune::Fem::L2Norm<typename TraitsType::DiscreteFunctionType::GridPartType>(function.space().gridPart()).norm(function);
}

template <class FunctionType_A, class TraitsType>
typename TraitsType::RangeFieldType l2distance(const FunctionType_A& function_A, const Dune::Fem::DiscreteFunctionInterface<TraitsType>& function_B)
DUNE_DEPRECATED_MSG("Use the interfaces from interfaces.hh or put this somewhere else!");

template <class FunctionType_A, class TraitsType>
typename TraitsType::RangeFieldType l2distance(const FunctionType_A& function_A, const Dune::Fem::DiscreteFunctionInterface<TraitsType>& function_B)
{
  static_assert(std::is_base_of<Dune::Fem::HasLocalFunction, FunctionType_A>::value, "");
  Dune::Fem::L2Norm<typename TraitsType::DiscreteFunctionType::GridPartType> norm(function_B.space().gridPart());
  return norm.distance(function_A, function_B);
}

template <class TraitsType>
static typename TraitsType::RangeFieldType h1norm(const Dune::Fem::DiscreteFunctionInterface<TraitsType>& function)
DUNE_DEPRECATED_MSG("Use the interfaces from interfaces.hh or put this somewhere else!");

template <class TraitsType>
static typename TraitsType::RangeFieldType h1norm(const Dune::Fem::DiscreteFunctionInterface<TraitsType>& function)
{
  const Dune::Fem::H1Norm<typename TraitsType::DiscreteFunctionType::GridPartType> norm(function.space().gridPart());
  return norm.norm(function);
}

template <class FunctionType_A, class TraitsType>
typename TraitsType::RangeFieldType h1distance(const FunctionType_A& function_A, const Dune::Fem::DiscreteFunctionInterface<TraitsType>& function_B)
DUNE_DEPRECATED_MSG("Use the interfaces from interfaces.hh or put this somewhere else!");

template <class FunctionType_A, class TraitsType>
typename TraitsType::RangeFieldType h1distance(const FunctionType_A& function_A, const Dune::Fem::DiscreteFunctionInterface<TraitsType>& function_B)
{
  static_assert(std::is_base_of<Dune::Fem::HasLocalFunction, FunctionType_A>::value, "");
  Dune::Fem::H1Norm<typename TraitsType::DiscreteFunctionType::GridPartType> norm(function_B.space().gridPart());
  return norm.distance(function_A, function_B);
}
#endif

} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_FUNCTION_NORM_HH
