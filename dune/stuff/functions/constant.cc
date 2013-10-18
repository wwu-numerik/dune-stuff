// This file is part of the dune-stuff project:
//   http://users.dune-project.org/projects/dune-stuff/
// Copyright Holders: Felix Albrecht
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifdef HAVE_CMAKE_CONFIG
# include "cmake_config.h"
#elif defined (HAVE_CONFIG_H)
# include "config.h"
#endif

#include "constant.hh"

namespace Dune {
namespace Stuff {
namespace Function {


// ====================
// ===== Constant =====
// ====================
template< class E, class D, int d, class R, int r, int rC >
std::string Constant< E, D, d, R, r, rC >::static_id()
{
  return BaseType::static_id() + ".constant";
}

template< class E, class D, int d, class R, int r, int rC >
Dune::ParameterTree Constant< E, D, d, R, r, rC >::defaultSettings(const std::string subName)
{
  Dune::ParameterTree description;
  description["value"] = "1.0";
  if (subName.empty())
    return description;
  else {
    Dune::Stuff::Common::ExtendedParameterTree extendedDescription;
    extendedDescription.add(description, subName);
    return extendedDescription;
  }
} // ... defaultSettings(...)

template< class E, class D, int d, class R, int r, int rC >
typename Constant< E, D, d, R, r, rC >::ThisType* Constant< E, D, d, R, r, rC >::create(const DSC::ExtendedParameterTree settings)
{
  typedef typename Constant< E, D, d, R, r, rC >::ThisType ThisType;
  typedef typename Constant< E, D, d, R, r, rC >::RangeFieldType RangeFieldType;
  return new ThisType(settings.get< RangeFieldType >("value", RangeFieldType(0)));
} // ... create(...)

template< class E, class D, int d, class R, int r, int rC >
Constant< E, D, d, R, r, rC >::Constant(const RangeFieldType& val, const std::string nm)
  : value_(std::make_shared< RangeType >(val))
  , name_(nm)
{}

template< class E, class D, int d, class R, int r, int rC >
Constant< E, D, d, R, r, rC >::Constant(const RangeType& val, const std::string nm)
  : value_(std::make_shared< RangeType >(val))
  , name_(nm)
{}

template< class E, class D, int d, class R, int r, int rC >
Constant< E, D, d, R, r, rC >::Constant(const ThisType& other)
  : value_(other.value_)
  , name_(other.name_)
{}

template< class E, class D, int d, class R, int r, int rC >
typename Constant< E, D, d, R, r, rC >::ThisType& Constant< E, D, d, R, r, rC >::operator=(const ThisType& other)
{
  if (this != &other) {
    value_ = other.value_;
    name_ = other.name_;
  }
  return *this;
}

template< class E, class D, int d, class R, int r, int rC >
typename Constant< E, D, d, R, r, rC >::ThisType* Constant< E, D, d, R, r, rC >::copy() const
{
  return new ThisType(*this);
}

template< class E, class D, int d, class R, int r, int rC >
std::string Constant< E, D, d, R, r, rC >::name() const
{
  return name_;
}

template< class E, class D, int d, class R, int r, int rC >
    std::unique_ptr< typename Constant< E, D, d, R, r, rC >::LocalfunctionType >
Constant< E, D, d, R, r, rC >::local_function(const EntityType& entity) const
{
  return std::unique_ptr< Localfunction >(new Localfunction(entity, value_));
}


} // namespace Function
} // namespace Stuff
} // namespace Dune

#define DSF_LIST_DIMDOMAIN(etype) \
  DSF_LIST_DIMRANGE(etype, 1) \
  DSF_LIST_DIMRANGE(etype, 2) \
  DSF_LIST_DIMRANGE(etype, 3)

#define DSF_LIST_DIMRANGE(etype, ddim) \
  DSF_LIST_DIMRANGECOLS(Dune::Stuff::Function::Constant, etype, ddim, 1) \
  DSF_LIST_DIMRANGECOLS(Dune::Stuff::Function::Constant, etype, ddim, 2) \
  DSF_LIST_DIMRANGECOLS(Dune::Stuff::Function::Constant, etype, ddim, 3)

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

typedef Dune::Stuff::Grid::FakeEntity< 1 > DuneStuffFake1dEntityType;
typedef Dune::Stuff::Grid::FakeEntity< 2 > DuneStuffFake2dEntityType;
typedef Dune::Stuff::Grid::FakeEntity< 3 > DuneStuffFake3dEntityType;

DSF_LIST_DIMDOMAIN(DuneStuffFake1dEntityType)
DSF_LIST_DIMDOMAIN(DuneStuffFake2dEntityType)
DSF_LIST_DIMDOMAIN(DuneStuffFake3dEntityType)

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

DSF_LIST_DIMRANGE(DuneAluSimplexGrid2dEntityType, 2)
DSF_LIST_DIMRANGE(DuneAluSimplexGrid3dEntityType, 3)
DSF_LIST_DIMRANGE(DuneAluCubeGrid3dEntityType, 3)

# endif // HAVE_ALUGRID_SERIAL_H || HAVE_ALUGRID_PARALLEL_H
#endif // HAVE_DUNE_GRID
