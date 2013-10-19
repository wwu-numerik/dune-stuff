// This file is part of the dune-stuff project:
//   http://users.dune-project.org/projects/dune-stuff/
// Copyright Holders: Felix Albrecht
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "checkerboard.hh"

namespace Dune {
namespace Stuff {
namespace Function {


// ========================
// ===== Checkerboard =====
// ========================
template< class E, class D, int d, class R, int r, int rC >
std::string Checkerboard< E, D, d, R, r, rC >::static_id()
{
  return BaseType::static_id() + ".checkerboard";
}

template< class E, class D, int d, class R, int r, int rC >
Dune::ParameterTree Checkerboard< E, D, d, R, r, rC >::defaultSettings(const std::string subName)
{
  Dune::ParameterTree description;
  description["lowerLeft"] = "[0.0; 0.0; 0.0]";
  description["upperRight"] = "[1.0; 1.0; 1.0]";
  description["numElements"] = "[2; 2; 2]";
  description["values"] = "[1.0; 2.0; 3.0; 4.0; 5.0; 6.0; 7.0; 8.0]";
  description["name"] = static_id();
  if (subName.empty())
    return description;
  else {
    Dune::Stuff::Common::ExtendedParameterTree extendedDescription;
    extendedDescription.add(description, subName);
    return extendedDescription;
  }
} // ... defaultSettings(...)

template< class E, class D, int d, class R, int r, int rC >
    typename Checkerboard< E, D, d, R, r, rC >::ThisType*
Checkerboard< E, D, d, R, r, rC >::create(const DSC::ExtendedParameterTree settings)
{
  // get data
  std::vector< DomainFieldType > lowerLeft = settings.getVector("lowerLeft", DomainFieldType(0), dimDomain);
  std::vector< DomainFieldType > upperRight = settings.getVector("upperRight",
                                                                 DomainFieldType(1),
                                                                 dimDomain);
  std::vector< size_t > numElements = settings.getVector("numElements", size_t(1), dimDomain);
  std::vector< RangeFieldType > values_rf = settings.getVector("values", RangeFieldType(1), 1);
  std::vector< RangeType > values(values_rf.size());
  for (size_t ii = 0; ii < values_rf.size(); ++ii)
    values[ii] = RangeType(values_rf[ii]);
  // create and return, leave the checks to the base constructor
  return new ThisType(std::move(lowerLeft), std::move(upperRight), std::move(numElements), std::move(values));
} // ... create(...)

template< class E, class D, int d, class R, int r, int rC >
Checkerboard< E, D, d, R, r, rC >::Checkerboard(std::vector< DomainFieldType >&& lowerLeft,
                                                std::vector< DomainFieldType >&& upperRight,
                                                std::vector< size_t >&& numElements,
                                                std::vector< RangeType >&& values,
                                                std::string nm)
  : lowerLeft_(new std::vector< DomainFieldType >(std::move(lowerLeft)))
  , upperRight_(new std::vector< DomainFieldType >(std::move(upperRight)))
  , numElements_(new std::vector< size_t >(std::move(numElements)))
  , values_(new std::vector< RangeType >(std::move(values)))
  , name_(nm)
{
  // checks
  if (lowerLeft_->size() < dimDomain)
    DUNE_THROW(Dune::RangeError,
               "lowerLeft too small (is " << lowerLeft_->size() << ", should be " << dimDomain << ")");
  if (upperRight_->size() < dimDomain)
    DUNE_THROW(Dune::RangeError,
               "upperRight too small (is " << upperRight_->size() << ", should be " << dimDomain << ")");
  if (numElements_->size() < dimDomain)
    DUNE_THROW(Dune::RangeError,
               "numElements too small (is " << numElements_->size() << ", should be " << dimDomain << ")");
  size_t totalSubdomains = 1;
  for (size_t dd = 0; dd < dimDomain; ++dd) {
    const auto& ll = (*lowerLeft_)[dd];
    const auto& ur = (*upperRight_)[dd];
    const auto& ne = (*numElements_)[dd];
    if (!(ll < ur))
      DUNE_THROW(Dune::RangeError, "lowerLeft has to be elementwise smaller than upperRight!");
    totalSubdomains *= ne;
  }
  if (values_->size() < totalSubdomains)
    DUNE_THROW(Dune::RangeError,
               "values too small (is " << values_->size() << ", should be " << totalSubdomains << ")");
} // Checkerboard(...)

template< class E, class D, int d, class R, int r, int rC >
Checkerboard< E, D, d, R, r, rC >::Checkerboard(const ThisType& other)
  : lowerLeft_(other.lowerLeft_)
  , upperRight_(other.upperRight_)
  , numElements_(other.numElements_)
  , values_(other.values_)
  , name_(other.name_)
{}

template< class E, class D, int d, class R, int r, int rC >
    typename Checkerboard< E, D, d, R, r, rC >::ThisType&
Checkerboard< E, D, d, R, r, rC >::operator=(const ThisType& other)
{
  if (this != &other) {
    lowerLeft_ = other.lowerLeft_;
    upperRight_ = other.upperRight_;
    numElements_ = other.numElements_;
    values_ = other.values_;
    name_ = other.name_;
  }
  return *this;
}

template< class E, class D, int d, class R, int r, int rC >
typename Checkerboard< E, D, d, R, r, rC >::ThisType* Checkerboard< E, D, d, R, r, rC >::copy() const
{
  return new ThisType(*this);
}

template< class E, class D, int d, class R, int r, int rC >
std::string Checkerboard< E, D, d, R, r, rC >::name() const
{
  return name_;
}

template< class E, class D, int d, class R, int r, int rC >
    std::unique_ptr< typename Checkerboard< E, D, d, R, r, rC >::LocalfunctionType >
Checkerboard< E, D, d, R, r, rC >::local_function(const EntityType& entity) const
{
  // decide on the subdomain the center of the entity belongs to
  const auto center = entity.geometry().center();
  std::vector< size_t > whichPartition(dimDomain, 0);
  const auto& ll = *lowerLeft_;
  const auto& ur = *upperRight_;
  const auto& ne = *numElements_;
  for (size_t dd = 0; dd < dimDomain; ++dd) {
    // for points that are on upperRight_[d], this selects one partition too much
    // so we need to cap this
    whichPartition[dd] = std::min(size_t(std::floor(ne[dd]*((center[dd] - ll[dd])/(ur[dd] - ll[dd])))),
                                  ne[dd] - 1);
  }
  size_t subdomain = 0;
  if (dimDomain == 1)
    subdomain = whichPartition[0];
  else if (dimDomain == 2)
    subdomain = whichPartition[0] + whichPartition[1]*ne[0];
  else
    subdomain = whichPartition[0] + whichPartition[1]*ne[0] + whichPartition[2]*ne[1]*ne[0];
  // return the component that belongs to the subdomain
  return std::unique_ptr< Localfunction >(new Localfunction(entity, (*values_)[subdomain]));
} // ... local_function(...)


} // namespace Function
} // namespace Stuff
} // namespace Dune


#define DSF_LIST_DIMRANGE(etype, ddim) \
  DSF_LIST_DIMRANGECOLS(Dune::Stuff::Function::Checkerboard, etype, ddim, 1) \
  DSF_LIST_DIMRANGECOLS(Dune::Stuff::Function::Checkerboard, etype, ddim, 2) \
  DSF_LIST_DIMRANGECOLS(Dune::Stuff::Function::Checkerboard, etype, ddim, 3)

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

#ifdef HAVE_DUNE_GRID

# include <dune/grid/sgrid.hh>

typedef typename Dune::SGrid< 1, 1 >::template Codim< 0 >::Entity DuneSGrid1dEntityType;
typedef typename Dune::SGrid< 2, 2 >::template Codim< 0 >::Entity DuneSGrid2dEntityType;
typedef typename Dune::SGrid< 3, 3 >::template Codim< 0 >::Entity DuneSGrid3dEntityType;

DSF_LIST_DIMRANGE(DuneSGrid1dEntityType, 1)
DSF_LIST_DIMRANGE(DuneSGrid2dEntityType, 2)
DSF_LIST_DIMRANGE(DuneSGrid3dEntityType, 3)

# include <dune/grid/yaspgrid.hh>

typedef typename Dune::YaspGrid< 1 >::template Codim< 0 >::Entity DuneYaspGrid1dEntityType;
typedef typename Dune::YaspGrid< 2 >::template Codim< 0 >::Entity DuneYaspGrid2dEntityType;
typedef typename Dune::YaspGrid< 3 >::template Codim< 0 >::Entity DuneYaspGrid3dEntityType;

DSF_LIST_DIMRANGE(DuneYaspGrid1dEntityType, 1)
DSF_LIST_DIMRANGE(DuneYaspGrid2dEntityType, 2)
DSF_LIST_DIMRANGE(DuneYaspGrid3dEntityType, 3)

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
