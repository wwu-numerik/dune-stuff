// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_MAGNITUDEFUNCTION_HH
#define DUNE_STUFF_MAGNITUDEFUNCTION_HH

#if HAVE_DUNE_FEM

#include <boost/numeric/conversion/cast.hpp>

#include <dune/fem/function/common/function.hh>
#include <dune/fem/function/common/gridfunctionadapter.hh>
#include <dune/fem/space/common/functionspace.hh>
#include <dune/fem/space/discontinuousgalerkin.hh>
#include <dune/fem/function/adaptivefunction.hh>

#include <dune/stuff/fem/customprojection.hh>
#include <dune/stuff/common/debug.hh>
#include <dune/stuff/common/ranges.hh>
#include <dune/stuff/fem/localmassmatrix.hh>
#include <dune/stuff/aliases.hh>
#include <dune/stuff/fem/namespace.hh>

namespace Dune {
namespace Stuff {
namespace Fem {

//! Wrap a DiscreteFunction to a scalar function representing its two-norm
template< class DiscreteFunctionImp, int polOrder = 3 >
class MagnitudeFunction
{
public:
  typedef Dune::Fem::FunctionSpace< typename DiscreteFunctionImp::FunctionSpaceType::DomainFieldType,
                               double,
                               DiscreteFunctionImp::FunctionSpaceType::dimDomain,
                               1 >
      MagnitudeSpaceType;
  typedef DiscreteFunctionImp DiscreteFunctionType;
  typedef typename DiscreteFunctionType::DiscreteFunctionSpaceType::GridPartType GridPartType;
  typedef Dune::Fem::DiscontinuousGalerkinSpace< MagnitudeSpaceType, GridPartType, polOrder >
    MagnitudeDiscreteFunctionSpaceType;
  typedef Dune::Fem::AdaptiveDiscreteFunction< MagnitudeDiscreteFunctionSpaceType >
    MagnitudeDiscreteFunctionType;

  //! constructor taking discrete function
  MagnitudeFunction(const DiscreteFunctionType& _discreteFunction)
    : magnitude_disretefunctionspace_( _discreteFunction.space().gridPart() )
    , magnitude_disretefunction_(_discreteFunction.name() + "-magnitude", magnitude_disretefunctionspace_)
  {
    typedef Dune::Fem::CachingQuadrature< GridPartType, 0 > QuadratureType;
    typename MagnitudeDiscreteFunctionSpaceType::RangeType ret(0.0);
    typename MagnitudeDiscreteFunctionSpaceType::RangeType phi(0.0);
    const auto& space = _discreteFunction.space();

    const auto quadOrd = boost::numeric_cast< int >(2 * space.order() + 2);
    DSFe::LocalMassMatrix< MagnitudeDiscreteFunctionSpaceType, QuadratureType > massMatrix(magnitude_disretefunctionspace_, quadOrd);
    const bool affineMapping = massMatrix.affine();
    magnitude_disretefunction_.clear();

    for (const auto& en : space)
    {
      const auto& geo = en.geometry();
      const QuadratureType quad(en, quadOrd);
      auto lf = magnitude_disretefunction_.localFunction(en);
      const auto& baseset = lf.baseFunctionSet();
      const auto numDofs = lf.numDofs();
      for (auto qP : DSC::valueRange(quad.nop()))
      {
        const double intel = (affineMapping) ?
                             quad.weight(qP) :    // affine case
                             quad.weight(qP) * geo.integrationElement( quad.point(qP) );    // general case

        typename DiscreteFunctionType::RangeType val;
        _discreteFunction.localFunction(en).evaluate(quad.point(qP), val);
        ret = val.two_norm();
        for (auto i : DSC::valueRange(numDofs))
        {
          baseset.evaluate(i, quad[qP], phi);
          lf[i] += intel * (ret * phi);
        }
      }

      // in case of non-linear mapping apply inverse
      if (!affineMapping)
      {
        massMatrix.applyInverse(en, lf);
      }
    }
  }


  virtual ~MagnitudeFunction() {}

  const MagnitudeDiscreteFunctionType& discreteFunction() const {
    return magnitude_disretefunction_;
  }

private:
  static const MagnitudeSpaceType magnitude_space_;
  MagnitudeDiscreteFunctionSpaceType magnitude_disretefunctionspace_;
  MagnitudeDiscreteFunctionType magnitude_disretefunction_;
};

template< class DF, int I >
const typename MagnitudeFunction< DF, I >::MagnitudeSpaceType
MagnitudeFunction< DF, I >::magnitude_space_ = typename MagnitudeFunction< DF, I >::MagnitudeSpaceType();

} // namespace Fem
} // namespace Stuff
} // namespace Dune

#endif // #if HAVE_DUNE_FEM

#endif // DUNE_STUFF_MAGNITUDEFUNCTION_HH
