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

#define LS6(etype) \
  LS5(Dune::Stuff::LocalfunctionSetInterface, etype) \
  LS5(Dune::Stuff::LocalfunctionInterface, etype) \
  LS5(Dune::Stuff::LocalizableFunctionInterface, etype)

#define LS5(cname, etype) \
  LS4(cname, etype, double)

#define LS4(cname, etype, dftype) \
  LS3(cname, etype, dftype, double) \
  LS3(cname, etype, dftype, long double)

#define LS3(cname, etype, dftype, rftype) \
  LS2(cname, etype, dftype, rftype, 1) \
  LS2(cname, etype, dftype, rftype, 2) \
  LS2(cname, etype, dftype, rftype, 3)

#define LS2(cname, etype, dftype, rftype, ddim) \
  LS1(cname, etype, dftype, ddim, rftype, 1) \
  LS1(cname, etype, dftype, ddim, rftype, 2) \
  LS1(cname, etype, dftype, ddim, rftype, 3)

#define LS1(cname, etype, dftype, ddim, rftype, rdim) \
  LS0(cname, etype, dftype, ddim, rftype, rdim, 1) \
  LS0(cname, etype, dftype, ddim, rftype, rdim, 2) \
  LS0(cname, etype, dftype, ddim, rftype, rdim, 3)

#define LS0(cname, etype, dftype, ddim, rftype, rdim, rcdim) \
  template class cname< etype, dftype, ddim, rftype, rdim, rcdim >;

#include <dune/stuff/grid/fakeentity.hh>

typedef Dune::Stuff::Grid::FakeEntity< 1 > DuneStuffFake1dEntityType;
typedef Dune::Stuff::Grid::FakeEntity< 2 > DuneStuffFake2dEntityType;
typedef Dune::Stuff::Grid::FakeEntity< 3 > DuneStuffFake3dEntityType;

#define LS_FAKE(dimw) \
  LS6(Dune::Stuff::Grid::FakeEntity< dimw >)

LS_FAKE(1)
LS_FAKE(2)
LS_FAKE(3)

#ifdef HAVE_DUNE_GRID

# include <dune/grid/sgrid.hh>

typedef typename Dune::SGrid< 1, 1 >::template Codim< 0 >::Entity DuneSGrid1dEntityType;
typedef typename Dune::SGrid< 2, 2 >::template Codim< 0 >::Entity DuneSGrid2dEntityType;
typedef typename Dune::SGrid< 3, 3 >::template Codim< 0 >::Entity DuneSGrid3dEntityType;

template class Dune::Stuff::LocalfunctionSetInterface< DuneSGrid1dEntityType, double, 1, double, 1, 1 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneSGrid1dEntityType, double, 1, double, 1, 2 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneSGrid1dEntityType, double, 1, double, 1, 3 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneSGrid1dEntityType, double, 1, double, 2, 1 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneSGrid1dEntityType, double, 1, double, 2, 2 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneSGrid1dEntityType, double, 1, double, 2, 3 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneSGrid1dEntityType, double, 1, double, 3, 1 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneSGrid1dEntityType, double, 1, double, 3, 2 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneSGrid1dEntityType, double, 1, double, 3, 3 >;

template class Dune::Stuff::LocalfunctionInterface< DuneSGrid1dEntityType, double, 1, double, 1, 1 >;
template class Dune::Stuff::LocalfunctionInterface< DuneSGrid1dEntityType, double, 1, double, 1, 2 >;
template class Dune::Stuff::LocalfunctionInterface< DuneSGrid1dEntityType, double, 1, double, 1, 3 >;
template class Dune::Stuff::LocalfunctionInterface< DuneSGrid1dEntityType, double, 1, double, 2, 1 >;
template class Dune::Stuff::LocalfunctionInterface< DuneSGrid1dEntityType, double, 1, double, 2, 2 >;
template class Dune::Stuff::LocalfunctionInterface< DuneSGrid1dEntityType, double, 1, double, 2, 3 >;
template class Dune::Stuff::LocalfunctionInterface< DuneSGrid1dEntityType, double, 1, double, 3, 1 >;
template class Dune::Stuff::LocalfunctionInterface< DuneSGrid1dEntityType, double, 1, double, 3, 2 >;
template class Dune::Stuff::LocalfunctionInterface< DuneSGrid1dEntityType, double, 1, double, 3, 3 >;

template class Dune::Stuff::LocalizableFunctionInterface< DuneSGrid1dEntityType, double, 1, double, 1, 1 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneSGrid1dEntityType, double, 1, double, 1, 2 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneSGrid1dEntityType, double, 1, double, 1, 3 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneSGrid1dEntityType, double, 1, double, 2, 1 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneSGrid1dEntityType, double, 1, double, 2, 2 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneSGrid1dEntityType, double, 1, double, 2, 3 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneSGrid1dEntityType, double, 1, double, 3, 1 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneSGrid1dEntityType, double, 1, double, 3, 2 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneSGrid1dEntityType, double, 1, double, 3, 3 >;


template class Dune::Stuff::LocalfunctionSetInterface< DuneSGrid2dEntityType, double, 2, double, 1, 1 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneSGrid2dEntityType, double, 2, double, 1, 2 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneSGrid2dEntityType, double, 2, double, 1, 3 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneSGrid2dEntityType, double, 2, double, 2, 1 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneSGrid2dEntityType, double, 2, double, 2, 2 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneSGrid2dEntityType, double, 2, double, 2, 3 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneSGrid2dEntityType, double, 2, double, 3, 1 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneSGrid2dEntityType, double, 2, double, 3, 2 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneSGrid2dEntityType, double, 2, double, 3, 3 >;

template class Dune::Stuff::LocalfunctionInterface< DuneSGrid2dEntityType, double, 2, double, 1, 1 >;
template class Dune::Stuff::LocalfunctionInterface< DuneSGrid2dEntityType, double, 2, double, 1, 2 >;
template class Dune::Stuff::LocalfunctionInterface< DuneSGrid2dEntityType, double, 2, double, 1, 3 >;
template class Dune::Stuff::LocalfunctionInterface< DuneSGrid2dEntityType, double, 2, double, 2, 1 >;
template class Dune::Stuff::LocalfunctionInterface< DuneSGrid2dEntityType, double, 2, double, 2, 2 >;
template class Dune::Stuff::LocalfunctionInterface< DuneSGrid2dEntityType, double, 2, double, 2, 3 >;
template class Dune::Stuff::LocalfunctionInterface< DuneSGrid2dEntityType, double, 2, double, 3, 1 >;
template class Dune::Stuff::LocalfunctionInterface< DuneSGrid2dEntityType, double, 2, double, 3, 2 >;
template class Dune::Stuff::LocalfunctionInterface< DuneSGrid2dEntityType, double, 2, double, 3, 3 >;

template class Dune::Stuff::LocalizableFunctionInterface< DuneSGrid2dEntityType, double, 2, double, 1, 1 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneSGrid2dEntityType, double, 2, double, 1, 2 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneSGrid2dEntityType, double, 2, double, 1, 3 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneSGrid2dEntityType, double, 2, double, 2, 1 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneSGrid2dEntityType, double, 2, double, 2, 2 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneSGrid2dEntityType, double, 2, double, 2, 3 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneSGrid2dEntityType, double, 2, double, 3, 1 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneSGrid2dEntityType, double, 2, double, 3, 2 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneSGrid2dEntityType, double, 2, double, 3, 3 >;


template class Dune::Stuff::LocalfunctionSetInterface< DuneSGrid3dEntityType, double, 3, double, 1, 1 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneSGrid3dEntityType, double, 3, double, 1, 2 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneSGrid3dEntityType, double, 3, double, 1, 3 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneSGrid3dEntityType, double, 3, double, 2, 1 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneSGrid3dEntityType, double, 3, double, 2, 2 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneSGrid3dEntityType, double, 3, double, 2, 3 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneSGrid3dEntityType, double, 3, double, 3, 1 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneSGrid3dEntityType, double, 3, double, 3, 2 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneSGrid3dEntityType, double, 3, double, 3, 3 >;

template class Dune::Stuff::LocalfunctionInterface< DuneSGrid3dEntityType, double, 3, double, 1, 1 >;
template class Dune::Stuff::LocalfunctionInterface< DuneSGrid3dEntityType, double, 3, double, 1, 2 >;
template class Dune::Stuff::LocalfunctionInterface< DuneSGrid3dEntityType, double, 3, double, 1, 3 >;
template class Dune::Stuff::LocalfunctionInterface< DuneSGrid3dEntityType, double, 3, double, 2, 1 >;
template class Dune::Stuff::LocalfunctionInterface< DuneSGrid3dEntityType, double, 3, double, 2, 2 >;
template class Dune::Stuff::LocalfunctionInterface< DuneSGrid3dEntityType, double, 3, double, 2, 3 >;
template class Dune::Stuff::LocalfunctionInterface< DuneSGrid3dEntityType, double, 3, double, 3, 1 >;
template class Dune::Stuff::LocalfunctionInterface< DuneSGrid3dEntityType, double, 3, double, 3, 2 >;
template class Dune::Stuff::LocalfunctionInterface< DuneSGrid3dEntityType, double, 3, double, 3, 3 >;

template class Dune::Stuff::LocalizableFunctionInterface< DuneSGrid3dEntityType, double, 3, double, 1, 1 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneSGrid3dEntityType, double, 3, double, 1, 2 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneSGrid3dEntityType, double, 3, double, 1, 3 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneSGrid3dEntityType, double, 3, double, 2, 1 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneSGrid3dEntityType, double, 3, double, 2, 2 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneSGrid3dEntityType, double, 3, double, 2, 3 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneSGrid3dEntityType, double, 3, double, 3, 1 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneSGrid3dEntityType, double, 3, double, 3, 2 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneSGrid3dEntityType, double, 3, double, 3, 3 >;

# include <dune/grid/yaspgrid.hh>

typedef typename Dune::YaspGrid< 1 >::template Codim< 0 >::Entity DuneYaspGrid1dEntityType;
typedef typename Dune::YaspGrid< 2 >::template Codim< 0 >::Entity DuneYaspGrid2dEntityType;
typedef typename Dune::YaspGrid< 3 >::template Codim< 0 >::Entity DuneYaspGrid3dEntityType;

template class Dune::Stuff::LocalfunctionSetInterface< DuneYaspGrid1dEntityType, double, 1, double, 1, 1 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneYaspGrid1dEntityType, double, 1, double, 1, 2 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneYaspGrid1dEntityType, double, 1, double, 1, 3 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneYaspGrid1dEntityType, double, 1, double, 2, 1 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneYaspGrid1dEntityType, double, 1, double, 2, 2 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneYaspGrid1dEntityType, double, 1, double, 2, 3 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneYaspGrid1dEntityType, double, 1, double, 3, 1 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneYaspGrid1dEntityType, double, 1, double, 3, 2 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneYaspGrid1dEntityType, double, 1, double, 3, 3 >;

template class Dune::Stuff::LocalfunctionInterface< DuneYaspGrid1dEntityType, double, 1, double, 1, 1 >;
template class Dune::Stuff::LocalfunctionInterface< DuneYaspGrid1dEntityType, double, 1, double, 1, 2 >;
template class Dune::Stuff::LocalfunctionInterface< DuneYaspGrid1dEntityType, double, 1, double, 1, 3 >;
template class Dune::Stuff::LocalfunctionInterface< DuneYaspGrid1dEntityType, double, 1, double, 2, 1 >;
template class Dune::Stuff::LocalfunctionInterface< DuneYaspGrid1dEntityType, double, 1, double, 2, 2 >;
template class Dune::Stuff::LocalfunctionInterface< DuneYaspGrid1dEntityType, double, 1, double, 2, 3 >;
template class Dune::Stuff::LocalfunctionInterface< DuneYaspGrid1dEntityType, double, 1, double, 3, 1 >;
template class Dune::Stuff::LocalfunctionInterface< DuneYaspGrid1dEntityType, double, 1, double, 3, 2 >;
template class Dune::Stuff::LocalfunctionInterface< DuneYaspGrid1dEntityType, double, 1, double, 3, 3 >;

template class Dune::Stuff::LocalizableFunctionInterface< DuneYaspGrid1dEntityType, double, 1, double, 1, 1 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneYaspGrid1dEntityType, double, 1, double, 1, 2 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneYaspGrid1dEntityType, double, 1, double, 1, 3 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneYaspGrid1dEntityType, double, 1, double, 2, 1 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneYaspGrid1dEntityType, double, 1, double, 2, 2 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneYaspGrid1dEntityType, double, 1, double, 2, 3 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneYaspGrid1dEntityType, double, 1, double, 3, 1 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneYaspGrid1dEntityType, double, 1, double, 3, 2 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneYaspGrid1dEntityType, double, 1, double, 3, 3 >;


template class Dune::Stuff::LocalfunctionSetInterface< DuneYaspGrid2dEntityType, double, 2, double, 1, 1 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneYaspGrid2dEntityType, double, 2, double, 1, 2 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneYaspGrid2dEntityType, double, 2, double, 1, 3 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneYaspGrid2dEntityType, double, 2, double, 2, 1 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneYaspGrid2dEntityType, double, 2, double, 2, 2 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneYaspGrid2dEntityType, double, 2, double, 2, 3 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneYaspGrid2dEntityType, double, 2, double, 3, 1 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneYaspGrid2dEntityType, double, 2, double, 3, 2 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneYaspGrid2dEntityType, double, 2, double, 3, 3 >;

template class Dune::Stuff::LocalfunctionInterface< DuneYaspGrid2dEntityType, double, 2, double, 1, 1 >;
template class Dune::Stuff::LocalfunctionInterface< DuneYaspGrid2dEntityType, double, 2, double, 1, 2 >;
template class Dune::Stuff::LocalfunctionInterface< DuneYaspGrid2dEntityType, double, 2, double, 1, 3 >;
template class Dune::Stuff::LocalfunctionInterface< DuneYaspGrid2dEntityType, double, 2, double, 2, 1 >;
template class Dune::Stuff::LocalfunctionInterface< DuneYaspGrid2dEntityType, double, 2, double, 2, 2 >;
template class Dune::Stuff::LocalfunctionInterface< DuneYaspGrid2dEntityType, double, 2, double, 2, 3 >;
template class Dune::Stuff::LocalfunctionInterface< DuneYaspGrid2dEntityType, double, 2, double, 3, 1 >;
template class Dune::Stuff::LocalfunctionInterface< DuneYaspGrid2dEntityType, double, 2, double, 3, 2 >;
template class Dune::Stuff::LocalfunctionInterface< DuneYaspGrid2dEntityType, double, 2, double, 3, 3 >;

template class Dune::Stuff::LocalizableFunctionInterface< DuneYaspGrid2dEntityType, double, 2, double, 1, 1 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneYaspGrid2dEntityType, double, 2, double, 1, 2 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneYaspGrid2dEntityType, double, 2, double, 1, 3 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneYaspGrid2dEntityType, double, 2, double, 2, 1 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneYaspGrid2dEntityType, double, 2, double, 2, 2 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneYaspGrid2dEntityType, double, 2, double, 2, 3 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneYaspGrid2dEntityType, double, 2, double, 3, 1 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneYaspGrid2dEntityType, double, 2, double, 3, 2 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneYaspGrid2dEntityType, double, 2, double, 3, 3 >;


template class Dune::Stuff::LocalfunctionSetInterface< DuneYaspGrid3dEntityType, double, 3, double, 1, 1 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneYaspGrid3dEntityType, double, 3, double, 1, 2 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneYaspGrid3dEntityType, double, 3, double, 1, 3 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneYaspGrid3dEntityType, double, 3, double, 2, 1 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneYaspGrid3dEntityType, double, 3, double, 2, 2 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneYaspGrid3dEntityType, double, 3, double, 2, 3 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneYaspGrid3dEntityType, double, 3, double, 3, 1 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneYaspGrid3dEntityType, double, 3, double, 3, 2 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneYaspGrid3dEntityType, double, 3, double, 3, 3 >;

template class Dune::Stuff::LocalfunctionInterface< DuneYaspGrid3dEntityType, double, 3, double, 1, 1 >;
template class Dune::Stuff::LocalfunctionInterface< DuneYaspGrid3dEntityType, double, 3, double, 1, 2 >;
template class Dune::Stuff::LocalfunctionInterface< DuneYaspGrid3dEntityType, double, 3, double, 1, 3 >;
template class Dune::Stuff::LocalfunctionInterface< DuneYaspGrid3dEntityType, double, 3, double, 2, 1 >;
template class Dune::Stuff::LocalfunctionInterface< DuneYaspGrid3dEntityType, double, 3, double, 2, 2 >;
template class Dune::Stuff::LocalfunctionInterface< DuneYaspGrid3dEntityType, double, 3, double, 2, 3 >;
template class Dune::Stuff::LocalfunctionInterface< DuneYaspGrid3dEntityType, double, 3, double, 3, 1 >;
template class Dune::Stuff::LocalfunctionInterface< DuneYaspGrid3dEntityType, double, 3, double, 3, 2 >;
template class Dune::Stuff::LocalfunctionInterface< DuneYaspGrid3dEntityType, double, 3, double, 3, 3 >;

template class Dune::Stuff::LocalizableFunctionInterface< DuneYaspGrid3dEntityType, double, 3, double, 1, 1 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneYaspGrid3dEntityType, double, 3, double, 1, 2 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneYaspGrid3dEntityType, double, 3, double, 1, 3 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneYaspGrid3dEntityType, double, 3, double, 2, 1 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneYaspGrid3dEntityType, double, 3, double, 2, 2 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneYaspGrid3dEntityType, double, 3, double, 2, 3 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneYaspGrid3dEntityType, double, 3, double, 3, 1 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneYaspGrid3dEntityType, double, 3, double, 3, 2 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneYaspGrid3dEntityType, double, 3, double, 3, 3 >;

# if HAVE_ALUGRID_SERIAL_H || HAVE_ALUGRID_PARALLEL_H
#   define ALUGRID_CONFORM 1
#   define ENABLE_ALUGRID 1

#   include <dune/grid/alugrid.hh>

typedef typename Dune::ALUSimplexGrid< 2, 2 >::template Codim< 0 >::Entity DuneAluSimplexGrid2dEntityType;
typedef typename Dune::ALUSimplexGrid< 3, 3 >::template Codim< 0 >::Entity DuneAluSimplexGrid3dEntityType;
typedef typename Dune::ALUCubeGrid< 3, 3 >::template Codim< 0 >::Entity DuneAluCubeGrid3dEntityType;

template class Dune::Stuff::LocalfunctionSetInterface< DuneAluSimplexGrid2dEntityType, double, 2, double, 1, 1 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneAluSimplexGrid2dEntityType, double, 2, double, 1, 2 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneAluSimplexGrid2dEntityType, double, 2, double, 1, 3 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneAluSimplexGrid2dEntityType, double, 2, double, 2, 1 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneAluSimplexGrid2dEntityType, double, 2, double, 2, 2 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneAluSimplexGrid2dEntityType, double, 2, double, 2, 3 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneAluSimplexGrid2dEntityType, double, 2, double, 3, 1 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneAluSimplexGrid2dEntityType, double, 2, double, 3, 2 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneAluSimplexGrid2dEntityType, double, 2, double, 3, 3 >;

template class Dune::Stuff::LocalfunctionInterface< DuneAluSimplexGrid2dEntityType, double, 2, double, 1, 1 >;
template class Dune::Stuff::LocalfunctionInterface< DuneAluSimplexGrid2dEntityType, double, 2, double, 1, 2 >;
template class Dune::Stuff::LocalfunctionInterface< DuneAluSimplexGrid2dEntityType, double, 2, double, 1, 3 >;
template class Dune::Stuff::LocalfunctionInterface< DuneAluSimplexGrid2dEntityType, double, 2, double, 2, 1 >;
template class Dune::Stuff::LocalfunctionInterface< DuneAluSimplexGrid2dEntityType, double, 2, double, 2, 2 >;
template class Dune::Stuff::LocalfunctionInterface< DuneAluSimplexGrid2dEntityType, double, 2, double, 2, 3 >;
template class Dune::Stuff::LocalfunctionInterface< DuneAluSimplexGrid2dEntityType, double, 2, double, 3, 1 >;
template class Dune::Stuff::LocalfunctionInterface< DuneAluSimplexGrid2dEntityType, double, 2, double, 3, 2 >;
template class Dune::Stuff::LocalfunctionInterface< DuneAluSimplexGrid2dEntityType, double, 2, double, 3, 3 >;

template class Dune::Stuff::LocalizableFunctionInterface< DuneAluSimplexGrid2dEntityType, double, 2, double, 1, 1 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneAluSimplexGrid2dEntityType, double, 2, double, 1, 2 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneAluSimplexGrid2dEntityType, double, 2, double, 1, 3 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneAluSimplexGrid2dEntityType, double, 2, double, 2, 1 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneAluSimplexGrid2dEntityType, double, 2, double, 2, 2 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneAluSimplexGrid2dEntityType, double, 2, double, 2, 3 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneAluSimplexGrid2dEntityType, double, 2, double, 3, 1 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneAluSimplexGrid2dEntityType, double, 2, double, 3, 2 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneAluSimplexGrid2dEntityType, double, 2, double, 3, 3 >;


template class Dune::Stuff::LocalfunctionSetInterface< DuneAluSimplexGrid3dEntityType, double, 3, double, 1, 1 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneAluSimplexGrid3dEntityType, double, 3, double, 1, 2 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneAluSimplexGrid3dEntityType, double, 3, double, 1, 3 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneAluSimplexGrid3dEntityType, double, 3, double, 2, 1 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneAluSimplexGrid3dEntityType, double, 3, double, 2, 2 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneAluSimplexGrid3dEntityType, double, 3, double, 2, 3 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneAluSimplexGrid3dEntityType, double, 3, double, 3, 1 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneAluSimplexGrid3dEntityType, double, 3, double, 3, 2 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneAluSimplexGrid3dEntityType, double, 3, double, 3, 3 >;

template class Dune::Stuff::LocalfunctionInterface< DuneAluSimplexGrid3dEntityType, double, 3, double, 1, 1 >;
template class Dune::Stuff::LocalfunctionInterface< DuneAluSimplexGrid3dEntityType, double, 3, double, 1, 2 >;
template class Dune::Stuff::LocalfunctionInterface< DuneAluSimplexGrid3dEntityType, double, 3, double, 1, 3 >;
template class Dune::Stuff::LocalfunctionInterface< DuneAluSimplexGrid3dEntityType, double, 3, double, 2, 1 >;
template class Dune::Stuff::LocalfunctionInterface< DuneAluSimplexGrid3dEntityType, double, 3, double, 2, 2 >;
template class Dune::Stuff::LocalfunctionInterface< DuneAluSimplexGrid3dEntityType, double, 3, double, 2, 3 >;
template class Dune::Stuff::LocalfunctionInterface< DuneAluSimplexGrid3dEntityType, double, 3, double, 3, 1 >;
template class Dune::Stuff::LocalfunctionInterface< DuneAluSimplexGrid3dEntityType, double, 3, double, 3, 2 >;
template class Dune::Stuff::LocalfunctionInterface< DuneAluSimplexGrid3dEntityType, double, 3, double, 3, 3 >;

template class Dune::Stuff::LocalizableFunctionInterface< DuneAluSimplexGrid3dEntityType, double, 3, double, 1, 1 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneAluSimplexGrid3dEntityType, double, 3, double, 1, 2 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneAluSimplexGrid3dEntityType, double, 3, double, 1, 3 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneAluSimplexGrid3dEntityType, double, 3, double, 2, 1 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneAluSimplexGrid3dEntityType, double, 3, double, 2, 2 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneAluSimplexGrid3dEntityType, double, 3, double, 2, 3 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneAluSimplexGrid3dEntityType, double, 3, double, 3, 1 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneAluSimplexGrid3dEntityType, double, 3, double, 3, 2 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneAluSimplexGrid3dEntityType, double, 3, double, 3, 3 >;


template class Dune::Stuff::LocalfunctionSetInterface< DuneAluCubeGrid3dEntityType, double, 3, double, 1, 1 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneAluCubeGrid3dEntityType, double, 3, double, 1, 2 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneAluCubeGrid3dEntityType, double, 3, double, 1, 3 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneAluCubeGrid3dEntityType, double, 3, double, 2, 1 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneAluCubeGrid3dEntityType, double, 3, double, 2, 2 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneAluCubeGrid3dEntityType, double, 3, double, 2, 3 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneAluCubeGrid3dEntityType, double, 3, double, 3, 1 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneAluCubeGrid3dEntityType, double, 3, double, 3, 2 >;
template class Dune::Stuff::LocalfunctionSetInterface< DuneAluCubeGrid3dEntityType, double, 3, double, 3, 3 >;

template class Dune::Stuff::LocalfunctionInterface< DuneAluCubeGrid3dEntityType, double, 3, double, 1, 1 >;
template class Dune::Stuff::LocalfunctionInterface< DuneAluCubeGrid3dEntityType, double, 3, double, 1, 2 >;
template class Dune::Stuff::LocalfunctionInterface< DuneAluCubeGrid3dEntityType, double, 3, double, 1, 3 >;
template class Dune::Stuff::LocalfunctionInterface< DuneAluCubeGrid3dEntityType, double, 3, double, 2, 1 >;
template class Dune::Stuff::LocalfunctionInterface< DuneAluCubeGrid3dEntityType, double, 3, double, 2, 2 >;
template class Dune::Stuff::LocalfunctionInterface< DuneAluCubeGrid3dEntityType, double, 3, double, 2, 3 >;
template class Dune::Stuff::LocalfunctionInterface< DuneAluCubeGrid3dEntityType, double, 3, double, 3, 1 >;
template class Dune::Stuff::LocalfunctionInterface< DuneAluCubeGrid3dEntityType, double, 3, double, 3, 2 >;
template class Dune::Stuff::LocalfunctionInterface< DuneAluCubeGrid3dEntityType, double, 3, double, 3, 3 >;

template class Dune::Stuff::LocalizableFunctionInterface< DuneAluCubeGrid3dEntityType, double, 3, double, 1, 1 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneAluCubeGrid3dEntityType, double, 3, double, 1, 2 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneAluCubeGrid3dEntityType, double, 3, double, 1, 3 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneAluCubeGrid3dEntityType, double, 3, double, 2, 1 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneAluCubeGrid3dEntityType, double, 3, double, 2, 2 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneAluCubeGrid3dEntityType, double, 3, double, 2, 3 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneAluCubeGrid3dEntityType, double, 3, double, 3, 1 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneAluCubeGrid3dEntityType, double, 3, double, 3, 2 >;
template class Dune::Stuff::LocalizableFunctionInterface< DuneAluCubeGrid3dEntityType, double, 3, double, 3, 3 >;

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
