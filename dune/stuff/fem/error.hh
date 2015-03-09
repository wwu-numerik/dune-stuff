// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#warning Will be removed soon, file an issue on https://github.com/wwu-numerik/dune-stuff/issues if you need this (09.02.2015)!

#ifndef DUNE_STUFF_ERROR_HH
#define DUNE_STUFF_ERROR_HH

#if HAVE_DUNE_FEM

#include <utility>

#include <boost/format.hpp>

#include <dune/common/deprecated.hh>

#include <dune/fem/misc/l2norm.hh>
#include <dune/fem/misc/h1norm.hh>

#include <dune/stuff/fem.hh>

namespace Dune {
namespace Stuff {
namespace Fem {

/** \brief Class that facilitates getting absolute and relative error of a pair of DisceteFunctions
   * \tparam NormType
   **/
template< class GridPartType, class NormType = Dune::Fem::L2Norm< GridPartType > >
class
  DUNE_DEPRECATED_MSG("Will be removed soon, file an issue on https://github.com/wwu-numerik/dune-stuff/issues if you need this (09.02.2015)!")
      L2Error
{
  typedef NormType
  L2NormType;
  L2NormType l2norm_;

public:
  L2Error(const GridPartType& gridPart)
    : l2norm_(gridPart)
  {}

  struct Errors
    : public std::pair< double, double >
  {
    typedef std::pair< double, double >
    BaseType;
    std::string name_;
    Errors(double abs, double rel, std::string name)
      : BaseType(abs, rel)
        , name_(name)
    {}

    //!make friend op <<
    std::string str() const {
      return ( boost::format("%s L2 error: %e (abs) | %e (rel)\n") % name_ % absolute() % relative() ).str();
    }

    double absolute() const {
      return BaseType::first;
    }

    double relative() const {
      return BaseType::second;
    }
  };

  template< class DiscreteFunctionType >
  Errors get(const DiscreteFunctionType& function_A, const DiscreteFunctionType& function_B) const {
    DiscreteFunctionType tmp( "L2Error::tmp", function_A.space() );

    return get(function_A, function_B, tmp);
  }

  template< class DiscreteFunctionType >
  Errors get(const DiscreteFunctionType& function_A, const DiscreteFunctionType& function_B,
             DiscreteFunctionType& diff) const {
    diff.assign(function_A);
    diff -= function_B;
    const double abs = l2norm_.norm(diff);
    return Errors(abs,
                  abs / l2norm_.norm(function_B),
                  function_A.name()
                  );
  } // get
};

} // namespace Fem
} // namespace Stuff
} // namespace Dune

#endif // #if HAVE_DUNE_FEM

#endif // DUNE_STUFF_ERROR_HH
