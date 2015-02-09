// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#warning Will be removed soon, file an issue on https://github.com/wwu-numerik/dune-stuff/issues if you need this (09.02.2015)!

#ifndef DUNE_STUFF_FUNCTIONADAPTER_HH
#define DUNE_STUFF_FUNCTIONADAPTER_HH

#if HAVE_DUNE_FEM

#include <vector>

#include <boost/numeric/conversion/cast.hpp>
#include <boost/shared_ptr.hpp>

#include <dune/common/deprecated.hh>

#include <dune/stuff/common/math.hh>
#include <dune/stuff/fem/localmassmatrix.hh>
#include <dune/stuff/common/print.hh>
#include <dune/stuff/fem.hh>
#include <dune/stuff/common/ranges.hh>

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
class
  DUNE_DEPRECATED_MSG("Will be removed soon, file an issue on https://github.com/wwu-numerik/dune-stuff/issues if you need this (09.02.2015)!")
      GradientSplitterFunction
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
    // type of local mass matrix
    typedef Dune::Stuff::Fem::LocalMassMatrix< DiscreteFunctionSpaceType, VolumeQuadratureType > LocalMassMatrixType;

    const auto quadOrd = boost::numeric_cast< int >(2*space.order());

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
        const auto numDofs = local_function.numDofs();
        // volume part
        for (auto qP : DSC::valueRange(quad.nop()))
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
          for (auto i : DSC::valueRange(numDofs))
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

template< size_t dim, class RangeType, class JacobianRangeType >
struct
  DUNE_DEPRECATED_MSG("Will be removed soon, file an issue on https://github.com/wwu-numerik/dune-stuff/issues if you need this (09.02.2015)!")
       GradientJacobianToLaplacian
  : public RangeType
{
  GradientJacobianToLaplacian(const JacobianRangeType& /*jacobian*/) {
    static_assert( (dim == 1 || dim > 3), "GradientJacobianToLaplacian is not implemented for this dimension!" );
    // Dune::CompileTimeChecker< ( dim == 1 || dim > 3 ) > NotImplemented;
  }
};

template< class RangeType, class JacobianRangeType >
struct
  DUNE_DEPRECATED_MSG("Will be removed soon, file an issue on https://github.com/wwu-numerik/dune-stuff/issues if you need this (09.02.2015)!")
       GradientJacobianToLaplacian< 2, RangeType, JacobianRangeType >
  : public RangeType
{
  GradientJacobianToLaplacian(const JacobianRangeType& jacobian) {
    (*this)[0] = jacobian[0][0];
    (*this)[1] = jacobian[3][1];
  }
};

template< class RangeType, class JacobianRangeType >
struct
  DUNE_DEPRECATED_MSG("Will be removed soon, file an issue on https://github.com/wwu-numerik/dune-stuff/issues if you need this (09.02.2015)!")
       GradientJacobianToLaplacian< 3, RangeType, JacobianRangeType >
  : public RangeType
{
  GradientJacobianToLaplacian(const JacobianRangeType& jacobian) {
    (*this)[0] = jacobian[0][0];
    (*this)[1] = jacobian[4][1];
    (*this)[2] = jacobian[8][2];
  }
};

struct
  DUNE_DEPRECATED_MSG("Will be removed soon, file an issue on https://github.com/wwu-numerik/dune-stuff/issues if you need this (09.02.2015)!")
       ProductFunctorMatrices
{
  template< class R1, class R2 >
  static double call(const R1& r1, const R2& r2) {
    return Dune::Stuff::Common::colonProduct(r1, r2);
  }
};

struct
  DUNE_DEPRECATED_MSG("Will be removed soon, file an issue on https://github.com/wwu-numerik/dune-stuff/issues if you need this (09.02.2015)!")
       ProductFunctorMatrixVector
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
class
  DUNE_DEPRECATED_MSG("Will be removed soon, file an issue on https://github.com/wwu-numerik/dune-stuff/issues if you need this (09.02.2015)!")
      GradientAdapterFunction
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
    // type of local mass matrix
    typedef DSFe::LocalMassMatrix< DiscreteFunctionSpaceType, VolumeQuadratureType > LocalMassMatrixType;
    const auto quadOrd = std::max(2 * space.order() + 2, polOrd);
    const LocalMassMatrixType massMatrix(space, boost::numeric_cast< int >(quadOrd));
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
      const auto numDofs = self_local.numDofs();

      // volume part
      for (auto qP : DSC::valueRange(quad.nop()))
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
        for (auto i : DSC::valueRange(numDofs))
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
class
  DUNE_DEPRECATED_MSG("Will be removed soon, file an issue on https://github.com/wwu-numerik/dune-stuff/issues if you need this (09.02.2015)!")
      LaplaceAdapterFunction
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
    typedef Dune::Stuff::Fem::LocalMassMatrix< DiscreteFunctionSpaceType, VolumeQuadratureType > LocalMassMatrixType;
    const auto quadOrd = boost::numeric_cast< int >(std::max(2 * space.order() + 2, polOrd));
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
      const auto numDofs = self_local.numDofs();

      // volume part
      for (auto qP : DSC::valueRange(quad.nop()))
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
        for (auto i : DSC::valueRange(numDofs))
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
