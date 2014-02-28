#ifndef DUNE_STUFF_MAGNITUDEFUNCTION_HH
#define DUNE_STUFF_MAGNITUDEFUNCTION_HH

#if HAVE_DUNE_FEM

#include <dune/fem/function/common/function.hh>
#include <dune/fem/function/common/gridfunctionadapter.hh>
#include <dune/fem/space/common/functionspace.hh>
#include <dune/fem/space/dgspace.hh>
#include <dune/fem/function/adaptivefunction.hh>
#include <dune/stuff/fem/customprojection.hh>
#include <dune/stuff/common/debug.hh>
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

    const int quadOrd = 2 * space.order() + 2;
    DSFe::LocalMassMatrix< MagnitudeDiscreteFunctionSpaceType, QuadratureType > massMatrix(magnitude_disretefunctionspace_, quadOrd);
    const bool affineMapping = massMatrix.affine();
    magnitude_disretefunction_.clear();

    for (const auto& en : space)
    {
      const auto& geo = en.geometry();
      const QuadratureType quad(en, quadOrd);
      auto lf = magnitude_disretefunction_.localFunction(en);
      const auto& baseset = lf.baseFunctionSet();
      const int quadNop = quad.nop();
      const int numDofs = lf.numDofs();
      for (int qP = 0; qP < quadNop; ++qP)
      {
        const double intel = (affineMapping) ?
                             quad.weight(qP) :    // affine case
                             quad.weight(qP) * geo.integrationElement( quad.point(qP) );    // general case

        typename DiscreteFunctionType::RangeType val;
        _discreteFunction.localFunction(en).evaluate(quad.point(qP), val);
        ret = val.two_norm();
        for (int i = 0; i < numDofs; ++i)
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
/** Copyright (c) 2012, Rene Milk
   * All rights reserved.
   *
   * Redistribution and use in source and binary forms, with or without
   * modification, are permitted provided that the following conditions are met:
   *
   * 1. Redistributions of source code must retain the above copyright notice, this
   *    list of conditions and the following disclaimer.
   * 2. Redistributions in binary form must reproduce the above copyright notice,
   *    this list of conditions and the following disclaimer in the documentation
   *    and/or other materials provided with the distribution.
   *
   * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
   * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   *
   * The views and conclusions contained in the software and documentation are those
   * of the authors and should not be interpreted as representing official policies,
   * either expressed or implied, of the FreeBSD Project.
   **/
