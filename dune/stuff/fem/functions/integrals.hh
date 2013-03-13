#ifndef DUNE_STUFF_INTEGRALS_HH
#define DUNE_STUFF_INTEGRALS_HH

#ifdef HAVE_CMAKE_CONFIG
 #include "cmake_config.h"
#else
 #include "config.h"
#endif

#ifdef HAVE_DUNE_FEM

#include <dune/stuff/fem/localmassmatrix.hh>
#include <dune/stuff/common/ranges.hh>
#include <dune/fem/quadrature/cachingquadrature.hh>

namespace Dune {
namespace Stuff {
namespace Fem {

/** \todo RENE needs to doc me **/
template< class FunctionType, class DiscreteFunctionSpaceType >
std::pair< typename FunctionType::RangeType, double > integralAndVolume(const FunctionType& function,
                                                                        const DiscreteFunctionSpaceType& space,
                                                                        const int polOrd = -1) {
  typedef Dune::CachingQuadrature< typename DiscreteFunctionSpaceType::Traits::GridPartType, 0 > QuadratureType;
  typedef Dune::Stuff::Fem::LocalMassMatrix< DiscreteFunctionSpaceType, QuadratureType > LocalMassMatrixType;

  typename FunctionType::RangeType integral_value = typename FunctionType::RangeType(0);
  double total_volume = 0;
  typename DiscreteFunctionSpaceType::RangeType ret(0.0);

  const int quadOrd = std::max(2 * space.order() + 2, polOrd);
  for (const auto& en : space)
  {
    const auto& geo = en.geometry();
    total_volume += geo.volume();
    const QuadratureType quad(en, quadOrd);
    const int quadNop = quad.nop();
    for (int qP = 0; qP < quadNop; ++qP)
    {
      const double intel = quad.weight(qP)
                           * geo.integrationElement( quad.point(qP) ); // general case
      // evaluate function
      typename DiscreteFunctionSpaceType::RangeType dummy;
      const auto xWorld = geo.global( quad.point(qP) );
      function.evaluate(xWorld, dummy);
      ret = dummy;
      ret *= intel;

      integral_value += ret;
    }
  }
  const auto& comm = space.gridPart().grid().comm();
  integral_value = comm.sum(integral_value);
  total_volume = comm.sum(total_volume);
  return std::make_pair(integral_value, total_volume);
} // integralAndVolume

/** \todo RENE needs to doc me **/
template< class FunctionType, class DiscreteFunctionSpaceType >
typename FunctionType::RangeType meanValue(const FunctionType& function,
                                           const DiscreteFunctionSpaceType& space,
                                           const int polOrd = -1) {
  std::pair< typename FunctionType::RangeType, double > pair = integralAndVolume(function, space, polOrd);
  pair.first /= pair.second;
  return pair.first;
}

/** \todo RENE needs to doc me **/
template< class FunctionType, class DiscreteFunctionSpaceType >
double boundaryIntegral(const FunctionType& function, const DiscreteFunctionSpaceType& space, const int polOrd = -1) {
  typedef typename DiscreteFunctionSpaceType::Traits::GridPartType GridPartType;
  typedef Dune::CachingQuadrature< GridPartType, 1 > QuadratureType;
  typedef Dune::Stuff::Fem::LocalMassMatrix< DiscreteFunctionSpaceType,
                                  Dune::CachingQuadrature< GridPartType, 0 > > LocalMassMatrixType;
  double integral_value = 0;
  double total_volume = 0;
  typename DiscreteFunctionSpaceType::RangeType ret(0.0);
  const int quadOrd = std::max(2 * space.order() + 2, polOrd);
  // create local mass matrix object
  LocalMassMatrixType massMatrix(space, quadOrd);
  // check whether geometry mappings are affine or not
  const bool affineMapping = massMatrix.affine();
  for (const auto& entity : space)
  {
    const auto& geo = entity.geometry();
    total_volume += geo.volume();

    const auto intItEnd = space.gridPart().iend(entity);
    for (auto intIt = space.gridPart().ibegin(entity);
         intIt != intItEnd;
         ++intIt)
    {
      if ( !intIt->neighbor() && intIt->boundary() )
      {
        const QuadratureType quad(space.gridPart(),
                                  *intIt,
                                  quadOrd,
                                  QuadratureType::INSIDE);
        const int quadNop = quad.nop();
        for (int qP = 0; qP < quadNop; ++qP)
        {
          const double intel = (affineMapping) ?
                               quad.weight(qP) : // affine case
                               quad.weight(qP) * geo.integrationElement( quad.point(qP) ); // general case

          // evaluate function
          typename DiscreteFunctionSpaceType::RangeType dummy;
          const auto xWorld = geo.global( quad.point(qP) );
          function.evaluate(xWorld, dummy, *intIt);
          ret = dummy;
          ret *= intel;
          integral_value += ret * intIt->unitOuterNormal( quad.localPoint(qP) );
        }
      }
    }
  }
  return integral_value;
} // boundaryIntegral


} // end namespace Fem
} // end namespace Stuff
} // end namespace Dune

#endif // HAVE_DUNE_FEM

#endif // DUNE_STUFF_INTEGRALS_HH

/** Copyright (c) 2012, Felix Albrecht, Rene Milk
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
