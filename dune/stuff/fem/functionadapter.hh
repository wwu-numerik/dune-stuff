#ifndef DUNE_STUFF_FUNCTIONADAPTER_HH
#define DUNE_STUFF_FUNCTIONADAPTER_HH

#if HAVE_DUNE_FEM

#include <dune/stuff/common/math.hh>
#include <dune/stuff/fem/localmassmatrix.hh>

#include <vector>
#include <boost/shared_ptr.hpp>
#include <dune/stuff/common/print.hh>
#include <dune/common/static_assert.hh>

#include <dune/stuff/fem/namespace.hh>

namespace Dune {
namespace Stuff {
namespace Fem {

/** DiscreteFunction container \f$G\f$ that takes a \$f(dxd)\$f matrix valued function \f$f\f$ as input that splits
 * itself into \$fd\$f
   * DiscreteFunctions that are each \$fd\$f valued.
   * \f{eqnarray*}f\rightarrow (a_{ij})_{1<=i,j<=d}\\
   * G_k= \sum_K\int_K \sum_{l=1..d}a_{kl} \phi_{l} dx\f}
   **/
template< class DiscreteFunctionType, class DiscreteGradientFunctionType >
class GradientSplitterFunction
  : public std::vector< std::shared_ptr< DiscreteFunctionType > >
{
  typedef std::vector< DiscreteFunctionType >
    BaseType;
  typedef GradientSplitterFunction< DiscreteFunctionType, DiscreteGradientFunctionType >
    ThisType;
  typedef typename std::shared_ptr< DiscreteFunctionType >
    PointerType;

public:
  GradientSplitterFunction(const typename DiscreteFunctionType::FunctionSpaceType& space,
                           const DiscreteGradientFunctionType& gradient) {
    const size_t dim = DiscreteFunctionType::DomainType::dimension;
    for (size_t d = 0; d < dim; ++d)
    {
      PointerType p = std::make_shared<DiscreteFunctionType>(( boost::format("%s_%s")
                                                 % gradient.name()
                                                 % Dune::Stuff::Common::dimToAxisName(d, true) ).str(),
                                               space); // never use temporary smart pointers
      push_back(p);
    }

    typedef typename DiscreteFunctionType::FunctionSpaceType
      DiscreteFunctionSpaceType;
    typedef typename DiscreteFunctionSpaceType::Traits::GridPartType
      GridPartType;

    // type of quadrature
    typedef Dune::Fem::CachingQuadrature< GridPartType, 0 > VolumeQuadratureType;
    typedef Dune::Fem::CachingQuadrature< GridPartType, 1 > FaceQuadratureType;
    // type of local mass matrix
    typedef Dune::Stuff::Fem::LocalMassMatrix< DiscreteFunctionSpaceType, VolumeQuadratureType > LocalMassMatrixType;

    const int quadOrd = ( 2 * space.order() );

    // create local mass matrix object
    const LocalMassMatrixType massMatrix(space, quadOrd);

    // check whether geometry mappings are affine or not
    const bool affineMapping = massMatrix.affine();
    for (auto entity : space)
    {
      const auto& geo = entity.geometry();
      const VolumeQuadratureType quad(entity, quadOrd);
      const auto gradient_local = gradient.localFunction(entity);
      for (size_t d = 0; d < dim; ++d)
      {
        auto local_function = this->at(d)->localFunction(entity);
        const auto& baseset = local_function.baseFunctionSet();
        const int quadNop = quad.nop();
        const int numDofs = local_function.numDofs();
        // volume part
        for (int qP = 0; qP < quadNop; ++qP)
        {
          const auto xLocal = quad.point(qP);
          const double intel = (affineMapping) ?
                               quad.weight(qP) : // affine case
                               quad.weight(qP) * geo.integrationElement(xLocal); // general case

          typename DiscreteGradientFunctionType::RangeType gradient_eval;
          gradient_local.evaluate(quad[qP], gradient_eval);
          typename DiscreteFunctionSpaceType::RangeType jacobian_row;
          for (size_t e = 0; e < dim; ++e)
          {
            jacobian_row[e] = gradient_eval(d, e);
          }
          // do projection
          for (int i = 0; i < numDofs; ++i)
          {
            typename DiscreteFunctionType::RangeType phi(0.0);
            baseset.evaluate(i, quad[qP], phi);
            local_function[i] += intel * (jacobian_row * phi);
          }
        }
        // in case of non-linear mapping apply inverse
        if (!affineMapping)
        {
          massMatrix.applyInverse(entity, local_function);
        }
      }
    }
  }
};

template< int dim, class RangeType, class JacobianRangeType >
struct GradientJacobianToLaplacian
  : public RangeType
{
  GradientJacobianToLaplacian(const JacobianRangeType& /*jacobian*/) {
    dune_static_assert( (dim == 1 || dim > 3), "GradientJacobianToLaplacian is not implemented for this dimension!" );
    // Dune::CompileTimeChecker< ( dim == 1 || dim > 3 ) > NotImplemented;
  }
};

template< class RangeType, class JacobianRangeType >
struct GradientJacobianToLaplacian< 2, RangeType, JacobianRangeType >
  : public RangeType
{
  GradientJacobianToLaplacian(const JacobianRangeType& jacobian) {
    (*this)[0] = jacobian[0][0];
    (*this)[1] = jacobian[3][1];
  }
};

template< class RangeType, class JacobianRangeType >
struct GradientJacobianToLaplacian< 3, RangeType, JacobianRangeType >
  : public RangeType
{
  GradientJacobianToLaplacian(const JacobianRangeType& jacobian) {
    (*this)[0] = jacobian[0][0];
    (*this)[1] = jacobian[4][1];
    (*this)[2] = jacobian[8][2];
  }
};

struct ProductFunctorMatrices
{
  template< class R1, class R2 >
  static double call(const R1& r1, const R2& r2) {
    return Dune::Stuff::Common::colonProduct(r1, r2);
  }
};

struct ProductFunctorMatrixVector
{
  template< class R1, class R2 >
  static double call(const R1& r1, const R2& r2) {
    Dune::FieldVector< double, 1 > res;
    r1.mv(r2, res);
    return res[0];
  }
};

template< class DiscreteVelocityFunctionType,
          class SigmaFunctionType,
          class ProductFunctor >
class GradientAdapterFunction
  : public SigmaFunctionType
{
protected:
  typedef GradientAdapterFunction< DiscreteVelocityFunctionType,
                                   SigmaFunctionType,
                                   ProductFunctor >
  ThisType;
  typedef SigmaFunctionType
  BaseType;

public:
  GradientAdapterFunction(const DiscreteVelocityFunctionType& velocity,
                          SigmaFunctionType& dummy,
                          int polOrd = -1)
    : BaseType( "grad", dummy.space() )
  {
    typedef SigmaFunctionType DiscreteFunctionType;
    typedef typename SigmaFunctionType::DiscreteFunctionSpaceType DiscreteFunctionSpaceType;
    typedef typename DiscreteFunctionSpaceType::Traits::GridPartType GridPartType;

    const DiscreteFunctionSpaceType space( velocity.space().gridPart() );
    // type of quadrature
    typedef Dune::Fem::CachingQuadrature< GridPartType, 0 > VolumeQuadratureType;
    typedef Dune::Fem::CachingQuadrature< GridPartType, 1 > FaceQuadratureType;
    // type of local mass matrix
    typedef DSFe::LocalMassMatrix< DiscreteFunctionSpaceType, VolumeQuadratureType > LocalMassMatrixType;
    const int quadOrd = std::max(2 * space.order() + 2, polOrd);
    const LocalMassMatrixType massMatrix(space, quadOrd);
    const bool affineMapping = massMatrix.affine();
    // clear destination
    BaseType::clear();

    for (const auto& entity : space)
    {
      const auto& geo = entity.geometry();
      const VolumeQuadratureType quad(entity, quadOrd);
      auto self_local = BaseType::localFunction(entity);
      const auto velocity_local = velocity.localFunction(entity);
      const auto& baseset = self_local.baseFunctionSet();
      const int quadNop = quad.nop();
      const int numDofs = self_local.numDofs();

      // volume part
      for (int qP = 0; qP < quadNop; ++qP)
      {
        const auto xLocal = quad.point(qP);
        const double intel = (affineMapping) ?
                             quad.weight(qP) : // affine case
                             quad.weight(qP) * geo.integrationElement(xLocal); // general case

        // evaluate function
        typename DiscreteVelocityFunctionType::DiscreteFunctionSpaceType::RangeType
          velocity_eval;
        velocity_local.evaluate(quad[qP], velocity_eval);
        typename DiscreteVelocityFunctionType::DiscreteFunctionSpaceType::JacobianRangeType
          velocity_jacobian_eval;
        velocity_local.jacobian(quad[qP], velocity_jacobian_eval);
        // do projection
        for (int i = 0; i < numDofs; ++i)
        {
          typename DiscreteFunctionType::DiscreteFunctionSpaceType::RangeType phi(0.0);
          baseset.evaluate(i, quad[qP], phi);
          self_local[i] += intel * ProductFunctor::call(velocity_jacobian_eval, phi);
        }
      }

      // in case of non-linear mapping apply inverse
      if (!affineMapping)
      {
        assert(false);              // fail here since last line is commented because of failing const stuff in
                                    //localdgmatrix
// massMatrix.applyInverse( entity, self_local );
      }
    }
  }
};

template< class DiscreteVelocityFunctionType,
          class SigmaFunctionType,
          class ProductFunctor >
class LaplaceAdapterFunction
  : public DiscreteVelocityFunctionType
{
protected:
  typedef LaplaceAdapterFunction< DiscreteVelocityFunctionType,
                                  SigmaFunctionType,
                                  ProductFunctor >
    ThisType;
  typedef DiscreteVelocityFunctionType BaseType;

public:
  LaplaceAdapterFunction(const DiscreteVelocityFunctionType& velocity,
                         SigmaFunctionType& dummy,
                         int polOrd = -1)
    : BaseType( "grad", velocity.space() )
  {
    GradientAdapterFunction< DiscreteVelocityFunctionType,
                             SigmaFunctionType,
                             ProductFunctor >
    gradient(velocity, dummy, polOrd);
    init(gradient, polOrd);
  }

  void init(const SigmaFunctionType& gradient, int polOrd)
  {
    typedef DiscreteVelocityFunctionType DiscreteFunctionType;
    typedef typename DiscreteFunctionType::DiscreteFunctionSpaceType DiscreteFunctionSpaceType;
    typedef typename DiscreteFunctionSpaceType::Traits::GridPartType GridPartType;

    const DiscreteFunctionSpaceType& space = BaseType::space();
    typedef Dune::Fem::CachingQuadrature< GridPartType, 0 > VolumeQuadratureType;
    typedef Dune::Fem::CachingQuadrature< GridPartType, 1 > FaceQuadratureType;
    typedef Dune::Stuff::Fem::LocalMassMatrix< DiscreteFunctionSpaceType, VolumeQuadratureType > LocalMassMatrixType;
    const int quadOrd = std::max(2 * space.order() + 2, polOrd);
    const LocalMassMatrixType massMatrix(space, quadOrd);
    const bool affineMapping = massMatrix.affine();
    BaseType::clear();

    for (const auto& entity : space)
    {
      const auto& geo = entity.geometry();
      const VolumeQuadratureType quad(entity, quadOrd);
      auto self_local = BaseType::localFunction(entity);
      const auto gradient_local = gradient.localFunction(entity);
      const auto& baseset = self_local.baseFunctionSet();
      const int quadNop = quad.nop();
      const int numDofs = self_local.numDofs();

      // volume part
      for (int qP = 0; qP < quadNop; ++qP)
      {
        const auto xLocal = quad.point(qP);
        const double intel = (affineMapping) ?
                             quad.weight(qP) : // affine case
                             quad.weight(qP) * geo.integrationElement(xLocal); // general case

        typename SigmaFunctionType::DiscreteFunctionSpaceType::JacobianRangeType
          gradient_jacobian_eval;
        gradient_local.jacobian(quad[qP], gradient_jacobian_eval);

        GradientJacobianToLaplacian< DiscreteFunctionType::RangeType::dimension,
                                     typename DiscreteFunctionType::RangeType,
                                     typename SigmaFunctionType::JacobianRangeType >
          velocity_real_laplacian(gradient_jacobian_eval);

        // do projection
        for (int i = 0; i < numDofs; ++i)
        {
          typename DiscreteFunctionType::DiscreteFunctionSpaceType::RangeType phi(0.0);
          baseset.evaluate(i, quad[qP], phi);
          self_local[i] += 2 * intel * (velocity_real_laplacian * phi);
        }
      }

      // in case of non-linear mapping apply inverse
      if (!affineMapping)
      {
        assert(false);              // fail here since last line is commented because of failing const stuff in
                                    //localdgmatrix
// massMatrix.applyInverse( entity, self_local );
      }
    }
  } // init
};

} // namespace Fem
} // namespace Stuff
} // namespace Dune

#endif // HAVE_DUNE_FEM

#endif // DUNE_STUFF_FUNCTIONADAPTER_HH
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
