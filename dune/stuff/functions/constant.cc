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
template< class E, class D, int d, class R, int r >
std::string Constant< E, D, d, R, r >::static_id()
{
  return BaseType::static_id() + ".constant";
}

template< class E, class D, int d, class R, int r >
Dune::ParameterTree Constant< E, D, d, R, r >::defaultSettings(const std::string subName)
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

template< class E, class D, int d, class R, int r >
typename Constant< E, D, d, R, r >::ThisType* Constant< E, D, d, R, r >::create(const DSC::ExtendedParameterTree settings)
{
  typedef typename Constant< E, D, d, R, r >::ThisType ThisType;
  typedef typename Constant< E, D, d, R, r >::RangeFieldType RangeFieldType;
  return new ThisType(settings.get< RangeFieldType >("value", RangeFieldType(0)));
} // ... create(...)

template< class E, class D, int d, class R, int r >
Constant< E, D, d, R, r >::Constant(const RangeFieldType& val, const std::string nm)
  : value_(std::make_shared< RangeType >(val))
  , name_(nm)
{}

template< class E, class D, int d, class R, int r >
Constant< E, D, d, R, r >::Constant(const RangeType& val, const std::string nm)
  : value_(std::make_shared< RangeType >(val))
  , name_(nm)
{}

template< class E, class D, int d, class R, int r >
Constant< E, D, d, R, r >::Constant(const ThisType& other)
  : value_(other.value_)
  , name_(other.name_)
{}

template< class E, class D, int d, class R, int r >
typename Constant< E, D, d, R, r >::ThisType& Constant< E, D, d, R, r >::operator=(const ThisType& other)
{
  if (this != &other) {
    value_ = other.value_;
    name_ = other.name_;
  }
  return *this;
}

template< class E, class D, int d, class R, int r >
typename Constant< E, D, d, R, r >::ThisType* Constant< E, D, d, R, r >::copy() const
{
  return new ThisType(*this);
}

template< class E, class D, int d, class R, int r >
std::string Constant< E, D, d, R, r >::name() const
{
  return name_;
}

template< class E, class D, int d, class R, int r >
    std::unique_ptr< typename Constant< E, D, d, R, r >::LocalfunctionType >
Constant< E, D, d, R, r >::local_function(const EntityType& entity) const
{
  return std::unique_ptr< Localfunction >(new Localfunction(entity, value_));
}


} // namespace Function
} // namespace Stuff
} // namespace Dune

#include <dune/stuff/grid/fakeentity.hh>

typedef Dune::Stuff::Grid::FakeEntity< 1 > DuneStuffFake1dEntityType;
typedef Dune::Stuff::Grid::FakeEntity< 2 > DuneStuffFake2dEntityType;
typedef Dune::Stuff::Grid::FakeEntity< 3 > DuneStuffFake3dEntityType;

template class Dune::Stuff::Function::Constant< DuneStuffFake1dEntityType, double, 1, double, 1 >;
//template class Dune::Stuff::Function::Constant< DuneStuffFake1dEntityType, double, 1, double, 2 >;
//template class Dune::Stuff::Function::Constant< DuneStuffFake1dEntityType, double, 1, double, 3 >;
//template class Dune::Stuff::Function::Constant< DuneStuffFake2dEntityType, double, 2, double, 1 >;
//template class Dune::Stuff::Function::Constant< DuneStuffFake2dEntityType, double, 2, double, 2 >;
//template class Dune::Stuff::Function::Constant< DuneStuffFake2dEntityType, double, 2, double, 3 >;
//template class Dune::Stuff::Function::Constant< DuneStuffFake3dEntityType, double, 3, double, 1 >;
//template class Dune::Stuff::Function::Constant< DuneStuffFake3dEntityType, double, 3, double, 2 >;
//template class Dune::Stuff::Function::Constant< DuneStuffFake3dEntityType, double, 3, double, 3 >;

//#ifdef HAVE_DUNE_GRID

//# include <dune/grid/sgrid.hh>

//typedef typename Dune::SGrid< 1, 1 >::template Codim< 0 >::Entity DuneSGrid1dEntityType;
//typedef typename Dune::SGrid< 2, 2 >::template Codim< 0 >::Entity DuneSGrid2dEntityType;
//typedef typename Dune::SGrid< 3, 3 >::template Codim< 0 >::Entity DuneSGrid3dEntityType;

//template class Dune::Stuff::Function::Constant< DuneSGrid1dEntityType, double, 1, double, 1 >;
//template class Dune::Stuff::Function::Constant< DuneSGrid1dEntityType, double, 1, double, 2 >;
//template class Dune::Stuff::Function::Constant< DuneSGrid1dEntityType, double, 1, double, 3 >;
//template class Dune::Stuff::Function::Constant< DuneSGrid2dEntityType, double, 2, double, 1 >;
//template class Dune::Stuff::Function::Constant< DuneSGrid2dEntityType, double, 2, double, 2 >;
//template class Dune::Stuff::Function::Constant< DuneSGrid2dEntityType, double, 2, double, 3 >;
//template class Dune::Stuff::Function::Constant< DuneSGrid3dEntityType, double, 3, double, 1 >;
//template class Dune::Stuff::Function::Constant< DuneSGrid3dEntityType, double, 3, double, 2 >;
//template class Dune::Stuff::Function::Constant< DuneSGrid3dEntityType, double, 3, double, 3 >;

//# include <dune/grid/yaspgrid.hh>

//typedef typename Dune::YaspGrid< 1 >::template Codim< 0 >::Entity DuneYaspGrid1dEntityType;
//typedef typename Dune::YaspGrid< 2 >::template Codim< 0 >::Entity DuneYaspGrid2dEntityType;
//typedef typename Dune::YaspGrid< 3 >::template Codim< 0 >::Entity DuneYaspGrid3dEntityType;

//template class Dune::Stuff::Function::Constant< DuneYaspGrid1dEntityType, double, 1, double, 1 >;
//template class Dune::Stuff::Function::Constant< DuneYaspGrid1dEntityType, double, 1, double, 2 >;
//template class Dune::Stuff::Function::Constant< DuneYaspGrid1dEntityType, double, 1, double, 3 >;
//template class Dune::Stuff::Function::Constant< DuneYaspGrid2dEntityType, double, 2, double, 1 >;
//template class Dune::Stuff::Function::Constant< DuneYaspGrid2dEntityType, double, 2, double, 2 >;
//template class Dune::Stuff::Function::Constant< DuneYaspGrid2dEntityType, double, 2, double, 3 >;
//template class Dune::Stuff::Function::Constant< DuneYaspGrid3dEntityType, double, 3, double, 1 >;
//template class Dune::Stuff::Function::Constant< DuneYaspGrid3dEntityType, double, 3, double, 2 >;
//template class Dune::Stuff::Function::Constant< DuneYaspGrid3dEntityType, double, 3, double, 3 >;

//# if HAVE_ALUGRID_SERIAL_H || HAVE_ALUGRID_PARALLEL_H
//#   define ALUGRID_CONFORM 1
//#   define ENABLE_ALUGRID 1

//#   include <dune/grid/alugrid.hh>

//typedef typename Dune::ALUSimplexGrid< 2, 2 >::template Codim< 0 >::Entity DuneAluSimplexGrid2dEntityType;
//typedef typename Dune::ALUSimplexGrid< 3, 3 >::template Codim< 0 >::Entity DuneAluSimplexGrid3dEntityType;
//typedef typename Dune::ALUCubeGrid< 3, 3 >::template Codim< 0 >::Entity DuneAluCubeGrid3dEntityType;

//template class Dune::Stuff::Function::Constant< DuneAluSimplexGrid2dEntityType, double, 2, double, 1 >;
//template class Dune::Stuff::Function::Constant< DuneAluSimplexGrid2dEntityType, double, 2, double, 2 >;
//template class Dune::Stuff::Function::Constant< DuneAluSimplexGrid2dEntityType, double, 2, double, 3 >;
//template class Dune::Stuff::Function::Constant< DuneAluSimplexGrid3dEntityType, double, 3, double, 1 >;
//template class Dune::Stuff::Function::Constant< DuneAluSimplexGrid3dEntityType, double, 3, double, 2 >;
//template class Dune::Stuff::Function::Constant< DuneAluSimplexGrid3dEntityType, double, 3, double, 3 >;
//template class Dune::Stuff::Function::Constant< DuneAluCubeGrid3dEntityType, double, 3, double, 1 >;
//template class Dune::Stuff::Function::Constant< DuneAluCubeGrid3dEntityType, double, 3, double, 2 >;
//template class Dune::Stuff::Function::Constant< DuneAluCubeGrid3dEntityType, double, 3, double, 3 >;

//# endif // HAVE_ALUGRID_SERIAL_H || HAVE_ALUGRID_PARALLEL_H
//#endif // HAVE_DUNE_GRID
