// This file is part of the dune-stuff project:
//   http://users.dune-project.org/projects/dune-stuff/
// Copyright Holders: Felix Albrecht
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "config.h"

#include "spe10.hh"

namespace Dune {
namespace Stuff {
namespace Function {


// ========================================
// ==== Spe10Model1< ..., 2..., 1, 1 > ====
// ========================================

template< class E, class D, class R >
const size_t Spe10Model1< E, D, 2, R, 1, 1 >::numXelements = 100;

template< class E, class D, class R >
const size_t Spe10Model1< E, D, 2, R, 1, 1 >::numYelements = 1;

template< class E, class D, class R >
const size_t Spe10Model1< E, D, 2, R, 1, 1 >::numZelements = 20;

template< class E, class D, class R >
const typename Spe10Model1< E, D, 2, R, 1, 1 >::RangeFieldType Spe10Model1< E, D, 2, R, 1, 1 >::minValue = 0.001;

template< class E, class D, class R >
const typename Spe10Model1< E, D, 2, R, 1, 1 >::RangeFieldType Spe10Model1< E, D, 2, R, 1, 1 >::maxValue = 998.915;

template< class E, class D, class R >
std::string Spe10Model1< E, D, 2, R, 1, 1 >::static_id()
{
  return BaseType::static_id() + ".spe10.model1";
}

template< class E, class D, class R >
    std::vector< typename Spe10Model1< E, D, 2, R, 1, 1 >::RangeType >
Spe10Model1< E, D, 2, R, 1, 1 >::read_values_from_file(const std::string& filename,
                                                       const RangeFieldType& min,
                                                       const RangeFieldType& max)
{
  if (!(max > min))
    DUNE_THROW(Dune::RangeError,
               "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
               << " max (is " << max << ") has to be larger than min (is " << min << ")!");
  const RangeFieldType scale = (max - min) / (maxValue - minValue);
  const RangeType shift = min - scale*minValue;
  // read all the data from the file
  std::ifstream datafile(filename);
  if (datafile.is_open()) {
    static const size_t entriesPerDim = numXelements*numYelements*numZelements;
    // create storage (there should be exactly 6000 values in the file, but we onyl read the first 2000)
    std::vector< RangeType > data(entriesPerDim, RangeFieldType(0));
    double tmp = 0;
    size_t counter = 0;
    while (datafile >> tmp && counter < entriesPerDim) {
      data[counter] = (tmp * scale) + shift;
      ++counter;
    }
    datafile.close();
    if (counter != entriesPerDim)
      DUNE_THROW(Dune::IOError,"\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " wrong number of entries in '" << filename << "' (are "
                 << counter << ", should be " << entriesPerDim << ")!");
    return data;
  } else
    DUNE_THROW(Dune::IOError,
               "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
               << " could not open '" << filename << "'!");
} // Spe10Model1()

template< class E, class D, class R >
Spe10Model1< E, D, 2, R, 1, 1 >::Spe10Model1(const std::string& filename,
                                             std::vector< DomainFieldType >&& lowerLeft,
                                             std::vector< DomainFieldType >&& upperRight,
                                             const RangeFieldType min,
                                             const RangeFieldType max,
                                             const std::string nm)
  : BaseType(std::move(lowerLeft),
             std::move(upperRight),
             {numXelements, numZelements},
             read_values_from_file(filename, min, max), nm)
{}

template< class E, class D, class R >
typename Spe10Model1< E, D, 2, R, 1, 1 >::ThisType* Spe10Model1< E, D, 2, R, 1, 1 >::copy() const
{
  return new ThisType(*this);
}

template< class E, class D, class R >
Dune::ParameterTree Spe10Model1< E, D, 2, R, 1, 1 >::defaultSettings(const std::string subName)
{
  Dune::ParameterTree description;
  description["filename"] = "perm_case1.dat";
  description["lowerLeft"] = "[0.0; 0.0]";
  description["upperRight"] = "[762.0; 15.24]";
  description["minValue"] = "0.001";
  description["maxValue"] = "998.915";
  description["name"] = static_id();
  if (subName.empty())
    return description;
  else {
    Dune::Stuff::Common::ExtendedParameterTree extendedDescription;
    extendedDescription.add(description, subName);
    return extendedDescription;
  }
} // ... defaultSettings(...)

template< class E, class D, class R >
    typename Spe10Model1< E, D, 2, R, 1, 1 >::ThisType*
Spe10Model1< E, D, 2, R, 1, 1 >::create(const Dune::Stuff::Common::ExtendedParameterTree settings)
{
  // get data
  const std::string filename = settings.get< std::string >("filename");
  std::vector< DomainFieldType > lowerLeft = settings.getVector< DomainFieldType >("lowerLeft", dimDomain);
  std::vector< DomainFieldType > upperRight = settings.getVector< DomainFieldType >("upperRight", dimDomain);
  const RangeFieldType minVal = settings.get< RangeFieldType >("minValue", minValue);
  const RangeFieldType maxVal = settings.get< RangeFieldType >("maxValue", maxValue);
  const std::string nm = settings.get< std::string >("name", static_id());
  // create and return, leave the checks to the constructor
  return new ThisType(filename, std::move(lowerLeft), std::move(upperRight), minVal, maxVal, nm);
} // ... create(...)

} // namespace Function
} // namespace Stuff
} // namespace Dune

#define DSF_LIST_DOMAINFIELDTYPES(etype, ddim, rdim, rcdim) \
  DSF_LIST_RANGEFIELDTYPES(etype, double, ddim, rdim, rcdim)

#define DSF_LIST_RANGEFIELDTYPES(etype, dftype, ddim, rdim, rcdim) \
  DSF_LAST_EXPANSION(etype, dftype, ddim, double, rdim, rcdim) \
  DSF_LAST_EXPANSION(etype, dftype, ddim, long double, rdim, rcdim)

#define DSF_LAST_EXPANSION(etype, dftype, ddim, rftype, rdim, rcdim) \
  template class Dune::Stuff::Function::Spe10Model1< etype, dftype, ddim, rftype, rdim, rcdim >;

#ifdef HAVE_DUNE_GRID

# include <dune/grid/sgrid.hh>

typedef typename Dune::SGrid< 2, 2 >::template Codim< 0 >::Entity DuneSGrid2dEntityType;

DSF_LIST_DOMAINFIELDTYPES(DuneSGrid2dEntityType, 2, 1, 1)

# include <dune/grid/yaspgrid.hh>

typedef typename Dune::YaspGrid< 2 >::template Codim< 0 >::Entity DuneYaspGrid2dEntityType;

DSF_LIST_DOMAINFIELDTYPES(DuneYaspGrid2dEntityType, 2, 1, 1)

# if HAVE_ALUGRID_SERIAL_H || HAVE_ALUGRID_PARALLEL_H
#   define ALUGRID_CONFORM 1
#   define ENABLE_ALUGRID 1

#   include <dune/grid/alugrid.hh>

typedef typename Dune::ALUSimplexGrid< 2, 2 >::template Codim< 0 >::Entity DuneAluSimplexGrid2dEntityType;

DSF_LIST_DOMAINFIELDTYPES(DuneAluSimplexGrid2dEntityType, 2, 1, 1)

# endif // HAVE_ALUGRID_SERIAL_H || HAVE_ALUGRID_PARALLEL_H
#endif // HAVE_DUNE_GRID
