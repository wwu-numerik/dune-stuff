// This file is part of the dune-stuff project:
//   http://users.dune-project.org/projects/dune-stuff/
// Copyright Holders: Felix Albrecht, Rene Milk
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "interfaces.hh"

#include <dune/geometry/genericreferenceelements.hh>

namespace Dune {
namespace Stuff {


// =====================================
// ===== LocalfunctionSetInterface =====
// =====================================
template< class E, class D, int d, class R, int r, int rC >
LocalfunctionSetInterface< E, D, d, R, r, rC >::LocalfunctionSetInterface(const EntityType& ent)
  : entity_(ent)
{}

template< class E, class D, int d, class R, int r, int rC >
LocalfunctionSetInterface< E, D, d, R, r, rC >::~LocalfunctionSetInterface()
{}

template< class E, class D, int d, class R, int r, int rC >
    const typename LocalfunctionSetInterface< E, D, d, R, r, rC >::EntityType&
LocalfunctionSetInterface< E, D, d, R, r, rC >::entity() const
{
  return entity_;
}

template< class E, class D, int d, class R, int r, int rC >
    std::vector< typename LocalfunctionSetInterface< E, D, d, R, r, rC >::RangeType >
LocalfunctionSetInterface< E, D, d, R, r, rC >::evaluate(const DomainType& xx) const
{
  std::vector< RangeType > ret(size(), RangeType(0));
  evaluate(xx, ret);
  return ret;
}

template< class E, class D, int d, class R, int r, int rC >
    std::vector< typename LocalfunctionSetInterface< E, D, d, R, r, rC >::JacobianRangeType >
LocalfunctionSetInterface< E, D, d, R, r, rC >::jacobian(const DomainType& xx) const
{
  std::vector< JacobianRangeType > ret(size(), JacobianRangeType(0));
  jacobian(xx, ret);
  return ret;
}

template< class E, class D, int d, class R, int r, int rC >
bool LocalfunctionSetInterface< E, D, d, R, r, rC >::is_a_valid_point(const DomainType& xx) const
{
  const auto& reference_element = GenericReferenceElements< DomainFieldType, dimDomain >::general(entity().type());
  return reference_element.checkInside(xx);
}


// ==================================
// ===== LocalfunctionInterface =====
// ==================================
template< class E, class D, int d, class R, int r, int rC >
LocalfunctionInterface< E, D, d, R, r, rC >::LocalfunctionInterface(const EntityType& ent)
  : BaseType(ent)
{}

template< class E, class D, int d, class R, int r, int rC >
LocalfunctionInterface< E, D, d, R, r, rC >::~LocalfunctionInterface()
{}

template< class E, class D, int d, class R, int r, int rC >
size_t LocalfunctionInterface< E, D, d, R, r, rC >::size() const
{
  return 1;
}

template< class E, class D, int d, class R, int r, int rC >
void LocalfunctionInterface< E, D, d, R, r, rC >::evaluate(const DomainType& xx, std::vector< RangeType >& ret) const
{
  assert(ret.size() >= 1);
  evaluate(xx, ret[0]);
}

template< class E, class D, int d, class R, int r, int rC >
void LocalfunctionInterface< E, D, d, R, r, rC >::jacobian(const DomainType& xx,
                                                           std::vector< JacobianRangeType >& ret) const
{
  assert(ret.size() >= 1);
  jacobian(xx, ret[0]);
}

template< class E, class D, int d, class R, int r, int rC >
    typename LocalfunctionInterface< E, D, d, R, r, rC >::RangeType
LocalfunctionInterface< E, D, d, R, r, rC >::evaluate(const DomainType& xx) const
{
  RangeType ret(0);
  evaluate(xx, ret);
  return ret;
}

template< class E, class D, int d, class R, int r, int rC >
    typename LocalfunctionInterface< E, D, d, R, r, rC >::JacobianRangeType
LocalfunctionInterface< E, D, d, R, r, rC >::jacobian(const DomainType& xx) const
{
  JacobianRangeType ret(0);
  jacobian(xx, ret);
  return ret;
}


// ========================================
// ===== LocalizableFunctionInterface =====
// ========================================
template< class E, class D, int d, class R, int r, int rC >
LocalizableFunctionInterface< E, D, d, R, r, rC >::~LocalizableFunctionInterface()
{}

template< class E, class D, int d, class R, int r, int rC >
std::string LocalizableFunctionInterface< E, D, d, R, r, rC >::static_id()
{
  return "dune.stuff.function";
}

template< class E, class D, int d, class R, int r, int rC >
std::string LocalizableFunctionInterface< E, D, d, R, r, rC >::name() const
{
  return "dune.stuff.function";
}


// =============================
// ===== FunctionInterface =====
// =============================
template< class D, int d, class R, int r >
FunctionInterface< D, d, R, r >::~FunctionInterface()
{}

template< class D, int d, class R, int r >
std::string FunctionInterface< D, d, R, r >::static_id()
{
  return "dune.stuff.function";
}

template< class D, int d, class R, int r >
std::string FunctionInterface< D, d, R, r >::name() const
{
  return "dune.stuff.function";
}

template< class D, int d, class R, int r >
int FunctionInterface< D, d, R, r >::order() const
{
  return -1;
}

template< class D, int d, class R, int r >
typename FunctionInterface< D, d, R, r >::RangeType FunctionInterface< D, d, R, r >::evaluate(const DomainType& x) const
{
  RangeType ret;
  evaluate(x, ret);
  return ret;
}

template< class D, int d, class R, int r >
void FunctionInterface< D, d, R, r >::jacobian(const DomainType& /*x*/, JacobianRangeType& /*ret*/) const
{
  DUNE_THROW(Dune::NotImplemented, "You really have to implement this!");
}

template< class D, int d, class R, int r >
    typename FunctionInterface< D, d, R, r >::JacobianRangeType
FunctionInterface< D, d, R, r >::jacobian(const DomainType& x) const
{
  JacobianRangeType ret;
  jacobian(x, ret);
  return ret;
}


} // namespace Stuff
} // namespace Dune

#define DSF_LIST_DIMDOMAIN(etype) \
  DSF_LIST_CLASSES(etype, 1) \
  DSF_LIST_CLASSES(etype, 2) \
  DSF_LIST_CLASSES(etype, 3)

#define DSF_LIST_CLASSES(etype, ddim) \
  DSF_LIST_DIMRANGE(Dune::Stuff::LocalfunctionSetInterface, etype, ddim) \
  DSF_LIST_DIMRANGE(Dune::Stuff::LocalfunctionInterface, etype, ddim) \
  DSF_LIST_DIMRANGE(Dune::Stuff::LocalizableFunctionInterface, etype, ddim)

#define DSF_LIST_DIMRANGE(cname, etype, ddim) \
  DSF_LIST_DIMRANGECOLS(cname, etype, ddim, 1) \
  DSF_LIST_DIMRANGECOLS(cname, etype, ddim, 2) \
  DSF_LIST_DIMRANGECOLS(cname, etype, ddim, 3)

#define DSF_LIST_DIMRANGECOLS(cname, etype, ddim, rdim) \
  DSF_LIST_DOMAINFIELDTYPES(cname, etype, ddim, rdim, 1) \
  DSF_LIST_DOMAINFIELDTYPES(cname, etype, ddim, rdim, 2) \
  DSF_LIST_DOMAINFIELDTYPES(cname, etype, ddim, rdim, 3)

#define DSF_LIST_DOMAINFIELDTYPES(cname, etype, ddim, rdim, rcdim) \
  DSF_LIST_RANGEFIELDTYPES(cname, etype, double, ddim, rdim, rcdim)

#define DSF_LIST_RANGEFIELDTYPES(cname, etype, dftype, ddim, rdim, rcdim) \
  DSF_LAST_EXPANSION(cname, etype, dftype, ddim, double, rdim, rcdim) \
  DSF_LAST_EXPANSION(cname, etype, dftype, ddim, long double, rdim, rcdim)

#define DSF_LAST_EXPANSION(cname, etype, dftype, ddim, rftype, rdim, rcdim) \
  template class cname< etype, dftype, ddim, rftype, rdim, rcdim >;

#include <dune/stuff/grid/fakeentity.hh>

#define DSF_LIST_FAKEENTITIES(dimw) \
  DSF_LIST_DIMDOMAIN(Dune::Stuff::Grid::FakeEntity< dimw >)

DSF_LIST_FAKEENTITIES(1)
DSF_LIST_FAKEENTITIES(2)
DSF_LIST_FAKEENTITIES(3)

#ifdef HAVE_DUNE_GRID

# include <dune/grid/sgrid.hh>

typedef typename Dune::SGrid< 1, 1 >::template Codim< 0 >::Entity DuneSGrid1dEntityType;
typedef typename Dune::SGrid< 2, 2 >::template Codim< 0 >::Entity DuneSGrid2dEntityType;
typedef typename Dune::SGrid< 3, 3 >::template Codim< 0 >::Entity DuneSGrid3dEntityType;

DSF_LIST_DIMDOMAIN(DuneSGrid1dEntityType)
DSF_LIST_DIMDOMAIN(DuneSGrid2dEntityType)
DSF_LIST_DIMDOMAIN(DuneSGrid3dEntityType)

# include <dune/grid/yaspgrid.hh>

typedef typename Dune::YaspGrid< 1 >::template Codim< 0 >::Entity DuneYaspGrid1dEntityType;
typedef typename Dune::YaspGrid< 2 >::template Codim< 0 >::Entity DuneYaspGrid2dEntityType;
typedef typename Dune::YaspGrid< 3 >::template Codim< 0 >::Entity DuneYaspGrid3dEntityType;

DSF_LIST_DIMDOMAIN(DuneYaspGrid1dEntityType)
DSF_LIST_DIMDOMAIN(DuneYaspGrid2dEntityType)
DSF_LIST_DIMDOMAIN(DuneYaspGrid3dEntityType)

# if HAVE_ALUGRID_SERIAL_H || HAVE_ALUGRID_PARALLEL_H
#   define ALUGRID_CONFORM 1
#   define ENABLE_ALUGRID 1

#   include <dune/grid/alugrid.hh>

typedef typename Dune::ALUSimplexGrid< 2, 2 >::template Codim< 0 >::Entity DuneAluSimplexGrid2dEntityType;
typedef typename Dune::ALUSimplexGrid< 3, 3 >::template Codim< 0 >::Entity DuneAluSimplexGrid3dEntityType;
typedef typename Dune::ALUCubeGrid< 3, 3 >::template Codim< 0 >::Entity DuneAluCubeGrid3dEntityType;

DSF_LIST_CLASSES(DuneAluSimplexGrid2dEntityType, 2)
DSF_LIST_CLASSES(DuneAluSimplexGrid3dEntityType, 3)
DSF_LIST_CLASSES(DuneAluCubeGrid3dEntityType, 3)

# endif // HAVE_ALUGRID_SERIAL_H || HAVE_ALUGRID_PARALLEL_H
#endif // HAVE_DUNE_GRID

template class Dune::Stuff::FunctionInterface< double, 1, double, 1 >;
template class Dune::Stuff::FunctionInterface< double, 1, double, 2 >;
template class Dune::Stuff::FunctionInterface< double, 1, double, 3 >;

template class Dune::Stuff::FunctionInterface< double, 2, double, 1 >;
template class Dune::Stuff::FunctionInterface< double, 2, double, 2 >;
template class Dune::Stuff::FunctionInterface< double, 2, double, 3 >;

template class Dune::Stuff::FunctionInterface< double, 3, double, 1 >;
template class Dune::Stuff::FunctionInterface< double, 3, double, 2 >;
template class Dune::Stuff::FunctionInterface< double, 3, double, 3 >;
