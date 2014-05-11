// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//
// Contributors: Sven Kaulmann

#ifndef DUNE_STUFF_FUNCTION_PDELABADAPTER_HH
#define DUNE_STUFF_FUNCTION_PDELABADAPTER_HH

#if 1 // HAVE_DUNE_PDELAB

#include <dune/pdelab/common/function.hh>
#include <dune/pdelab/gridfunctionspace/gridfunctionspaceutilities.hh>
#include <dune/stuff/functions/global.hh>

#if HAVE_DUNE_FEM
#include <dune/fem/function/common/discretefunction.hh>
#endif

namespace Dune {
namespace Stuff {

template<typename T1, typename T2>
struct
  DUNE_DEPRECATED_MSG("Derive this from LocalizableFunctionInterface or GlobalFunctionInterface or put this somewhere else!")
        DifferenceSquaredAdapter
  : public PDELab::GridFunctionBase< PDELab::GridFunctionTraits<typename T1::Traits::GridViewType,
                                   typename T1::Traits::RangeFieldType,
                                   1,Dune::FieldVector<typename T1::Traits::RangeFieldType,1> >
  , DifferenceSquaredAdapter<T1,T2> >
{
  typedef Dune::PDELab::GridFunctionTraits<typename T1::Traits::GridViewType,
                                           typename T1::Traits::RangeFieldType,
                                           1,Dune::FieldVector<typename T1::Traits::RangeFieldType,1> > Traits;

  DifferenceSquaredAdapter (const T1& t1_, const T2& t2_) : t1(t1_), t2(t2_) {}

  //! \copydoc GridFunctionBase::evaluate()
  inline void evaluate (const typename Traits::ElementType& e,
                        const typename Traits::DomainType& x,
                        typename Traits::RangeType& y) const
  {
    typename T1::Traits::RangeType y1;
    t1.evaluate(e,x,y1);
    typename T2::Traits::RangeType y2;
    t2.evaluate(e,x,y2);
    y1 -= y2;
    y = y1.two_norm2();
  }

  inline const typename Traits::GridViewType& getGridView () const
  {
    return t1.getGridView();
  }

private:
  const T1& t1;
  const T2& t2;
};

//! T1 needs to have a callable local_function.jacobian method and T2 should be something wrappable into a GridFunction
template<typename T1, typename T2, class GFS>
class
  DUNE_DEPRECATED_MSG("Derive this from LocalizableFunctionInterface or GlobalFunctionInterface or put this somewhere else!")
      H1DifferenceSquaredAdapter
  : public PDELab::GridFunctionBase< PDELab::GridFunctionTraits<typename T1::Traits::GridViewType,
                                   typename T1::Traits::RangeFieldType,
                                   1,Dune::FieldVector<typename T1::Traits::RangeFieldType,1> >
  , H1DifferenceSquaredAdapter<T1, T2, GFS>>
{
  typedef PDELab::DiscreteGridFunctionGradient<GFS, T2> T2GradientType;
  typedef PDELab::DiscreteGridFunction<GFS,T2> T2_DGF_Type;
  typedef DifferenceSquaredAdapter<T1,T2_DGF_Type> L2DifferenceType;

public:
  typedef Dune::PDELab::GridFunctionTraits<typename T1::Traits::GridViewType,
                                           typename T1::Traits::RangeFieldType,
                                           1,Dune::FieldVector<typename T1::Traits::RangeFieldType,1> > Traits;

  H1DifferenceSquaredAdapter (const T1& t1, const T2& t2)
    : t1_(t1)
    , t2_(t2)
    , t2_dgf_(t2.gridFunctionSpace(), t2)
    , t2_gradient_(t2.gridFunctionSpace(), t2)
    , l2_difference_(t1, t2_dgf_)
  {}

  //! \copydoc GridFunctionBase::evaluate()
  inline void evaluate (const typename Traits::ElementType& e,
                        const typename Traits::DomainType& x,
                        typename Traits::RangeType& y) const
  {
    l2_difference_.evaluate(e, x, y);
    typename T1::JacobianRangeType j1;
    typename T2GradientType::Traits::RangeType j2;
    t1_.local_function(e)->jacobian(x, j1);
    t2_gradient_.evaluate(e, x, j2);
    j2 -= j1[0];
    y += j2.two_norm2();
  }

private:
  const T1& t1_;
  const T2& t2_;
  const T2_DGF_Type t2_dgf_;
  const T2GradientType t2_gradient_;
  const L2DifferenceType l2_difference_;

public:
  auto getGridView () const -> decltype(t2_.gridFunctionSpace().gridView())
  {
    return t2_.gridFunctionSpace().gridView();
  }

};

template <class DiscreteFunctionType>
struct
  DUNE_DEPRECATED_MSG("Derive this from LocalizableFunctionInterface or GlobalFunctionInterface or put this somewhere else!")
        FemToPDELabAdapterTraits :
  public PDELab::GridFunctionTraits<typename DiscreteFunctionType::GridPartType::GridType::LeafGridView,
                                    typename DiscreteFunctionType::RangeFieldType,
                                    DiscreteFunctionType::RangeType::dimension, typename DiscreteFunctionType::RangeType >
{};

//! wrap a discretefunction so pdelab can call its own loca evalute signature on it
template <class DiscreteFunctionType>
class
  DUNE_DEPRECATED_MSG("Derive this from LocalizableFunctionInterface or GlobalFunctionInterface or put this somewhere else!")
      FemToPDELabAdapter : public
    PDELab::GridFunctionInterface<FemToPDELabAdapterTraits<DiscreteFunctionType>, FemToPDELabAdapter<DiscreteFunctionType>>
{
  typedef PDELab::GridFunctionInterface<FemToPDELabAdapterTraits<DiscreteFunctionType>, FemToPDELabAdapter<DiscreteFunctionType>>
    BaseType;
public:
  typedef FemToPDELabAdapterTraits<DiscreteFunctionType> Traits;

  typedef typename DiscreteFunctionType::JacobianRangeType JacobianRangeType;
  typedef typename Traits::GridViewType GridViewType;

  FemToPDELabAdapter(const DiscreteFunctionType& df, const GridViewType& gridView)
    : BaseType()
    , df_(df)
    , gridView_(gridView)
  {}

  inline void evaluate (const typename Traits::ElementType& e,
            const typename Traits::DomainType& x,
            typename Traits::RangeType& y) const
  {
    df_.localFunction(e).evaluate(x,y);
  }

  inline const GridViewType& getGridView () const
  {
    return gridView_;
  }

  std::unique_ptr<typename DiscreteFunctionType::LocalFunctionType> local_function(const typename Traits::ElementType& e) const {
    return DSC::make_unique<typename DiscreteFunctionType::LocalFunctionType>(df_.localFunction(e));
  }
private:
  const DiscreteFunctionType& df_;
  const GridViewType& gridView_;

};

//! wrap a Stuff::GlobalFunction into something usable as a grid function in PDELab
template <class GridViewType, class GlobalFunctionType>
struct
  DUNE_DEPRECATED_MSG("Derive this from LocalizableFunctionInterface or GlobalFunctionInterface or put this somewhere else!")
        GlobalGridFunctionAdapter
    : public PDELab::AnalyticGridFunctionBase<PDELab::AnalyticGridFunctionTraits<GridViewType,
                                                                                 typename GlobalFunctionType::RangeFieldType,
                                                                                 GlobalFunctionType::dimRange>,
                                              GlobalGridFunctionAdapter<GridViewType,GlobalFunctionType>>
{
  typedef typename GlobalFunctionType::JacobianRangeType JacobianRangeType;
  typedef typename GlobalFunctionType::RangeType RangeType;
  typedef typename GlobalFunctionType::DomainType DomainType;

  typedef PDELab::AnalyticGridFunctionBase<PDELab::AnalyticGridFunctionTraits<GridViewType,
                                                                              typename GlobalFunctionType::RangeFieldType,
                                                                              GlobalFunctionType::dimRange>,
                                           GlobalGridFunctionAdapter<GridViewType,GlobalFunctionType>> BaseType;

  const GlobalFunctionType& f_;

  GlobalGridFunctionAdapter(const GlobalFunctionType& f, const GridViewType& view)
    : BaseType(view)
    , f_(f) {}

  //! PDELab basetype calls local evaluate signature and transforms coord to global before calling this
  void evaluateGlobal(const DomainType& x_global, RangeType& y) const {
    f_.evaluate(x_global, y);
  }

  auto local_function(const typename GridViewType::template Codim<0>::Entity& e) const -> decltype(f_.local_function(e)) {
    return f_.local_function(e);
  }

};

template <class GridViewType, class GlobalFunctionType>
GlobalGridFunctionAdapter<GridViewType, GlobalFunctionType> globalGridFunctionAdapter(const GridViewType& view,
                                                                                      const GlobalFunctionType& f)
DUNE_DEPRECATED_MSG("Use the interfaces from interfaces.hh or put this somewhere else!")
{
  return  GlobalGridFunctionAdapter<GridViewType, GlobalFunctionType>(f, view);
}

template <class GridViewType, class E, class D, int d, class RF, int r>
GlobalGridFunctionAdapter<GridViewType,GlobalFunction<E,D,d,RF,r>> pdelabAdapted(const GlobalFunction<E,D,d,RF,r>& f,
                                                                                 const GridViewType& view)
DUNE_DEPRECATED_MSG("Use the interfaces from interfaces.hh or put this somewhere else!")
{
  return GlobalGridFunctionAdapter<GridViewType,GlobalFunction<E,D,d,RF,r>>(f, view);
}

template <class GridViewType, class TraitsType>
FemToPDELabAdapter<Dune::Fem::DiscreteFunctionInterface<TraitsType>> pdelabAdapted(const Dune::Fem::DiscreteFunctionInterface<TraitsType>& f,
                                                                                 const GridViewType& view)
DUNE_DEPRECATED_MSG("Use the interfaces from interfaces.hh or put this somewhere else!")
{
  return FemToPDELabAdapter<Dune::Fem::DiscreteFunctionInterface<TraitsType> >(f, view);
}



} // namespace Stuff
} // namespace Dune

#endif // HAVE_DUNE_PDELAB

#endif // DUNE_STUFF_FUNCTION_PDELABADAPTER_HH
