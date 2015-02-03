// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_FEM_FUNCTIONS_CUSTOMPROJECTION_HH
#define DUNE_STUFF_FEM_FUNCTIONS_CUSTOMPROJECTION_HH

#if HAVE_DUNE_FEM

#include <dune/fem/quadrature/cachingquadrature.hh>
#include <dune/fem/function/common/discretefunction.hh>
#include <dune/fem/function/common/gridfunctionadapter.hh>
#include <dune/fem/operator/common/operator.hh>

#include <dune/stuff/common/math.hh>
#include <dune/stuff/fem/namespace.hh>

#include "localmassmatrix.hh"

namespace Dune {
namespace Stuff {
namespace Fem {

  /** A custom projection of an analytical function that uses a non-standard evalute signature:\n
     *  <pre>template < class IntersectionIteratorType >\n
     *  void evaluate( const DomainType& arg, RangeType& ret, const IntersectionIteratorType& faceIter ) const</pre>\n
     * \note example being our boundary functions
     * \note output currently somewhat meaningless
     * \see analyticaldata.hh
     **/
class CustomProjection
{
public:
  template< class OriginFunctionType, class DestinationFunctionType >
  static void project(const OriginFunctionType& f, DestinationFunctionType& discFunc) {
    typedef Dune::Fem::CachingQuadrature< typename DestinationFunctionType::DiscreteFunctionSpaceType::GridPartType, 1 >
        FaceQuadratureType;
    const auto& space_ = discFunc.space();
    const auto& gridPart_ = space_.gridPart();
    typename DestinationFunctionType::DiscreteFunctionSpaceType::RangeType phi(0.0);

    for (const auto& e : space_)
    {
      auto lf = discFunc.localFunction(e);
      auto baseFunctionset = space_.baseFunctionSet(e);
      size_t intersection_count = 0;
      const auto intItEnd = gridPart_.iend(e);
      for (auto intIt = gridPart_.ibegin(e);
           intIt != intItEnd;
           ++intIt)
      {
        intersection_count++;
        FaceQuadratureType faceQuadrature(gridPart_,
                                          *intIt,
                                          ( 4 * space_.order() ) + 1,
                                          FaceQuadratureType::INSIDE);
        typename DestinationFunctionType::RangeType ret;
        for (size_t qP = 0; qP < faceQuadrature.nop(); ++qP)
        {
          const double intel
            = faceQuadrature.weight(qP) * e.geometry().integrationElement( faceQuadrature.point(qP) ); // general case

          if ( intIt->boundary() )
          {
            f.evaluate(faceQuadrature.point(qP), ret, *intIt);

            for (size_t i = 0; i < baseFunctionset.size(); ++i)
            {
              baseFunctionset.evaluate(i, faceQuadrature[qP], phi);
              lf[i] += intel * (ret * phi);
            }
          }
        }
      }
    }
  } // project
};

//! basically the fem L2Projection with a function evaluate that converts between compatible types instead of failing
class BetterL2Projection
{
protected:
  template< class FunctionType >
  struct DefaultEvaluationFunctor;

public:
  template< class FunctionImp, class DiscreteFunctionImp >
  static void project(const FunctionImp& func,
                      DiscreteFunctionImp& discFunc,
                      int polOrd = -1)
  {
    static_assert(!(Conversion< FunctionImp, Dune::Fem::IsDiscreteFunction >::exists),
                       "TimeAwareL2Projection_not_implemented_for_discrete_source_functions");
    DefaultEvaluationFunctor< FunctionImp > functor(func);
    projectCommon(functor, discFunc, polOrd);
  }

  template< class TimeProviderType, class FunctionImp, class DiscreteFunctionImp >
  static void project(const TimeProviderType& timeProvider,
                      const FunctionImp& func,
                      DiscreteFunctionImp& discFunc,
                      int polOrd = -1)
  {
    static_assert(!(Conversion< FunctionImp, Dune::Fem::IsDiscreteFunction >::exists),
                       "TimeAwareL2Projection_not_implemented_for_discrete_source_functions");
    TimeEvaluationFunctor< FunctionImp, TimeProviderType > functor(func, timeProvider);
    projectCommon(functor, discFunc, polOrd);
  }

protected:
  template< class FunctionType >
  struct DefaultEvaluationFunctor
  {
    const FunctionType& function_;
    DefaultEvaluationFunctor(const FunctionType& function)
      : function_(function)
    {}

    void evaluate(const typename FunctionType::DomainType& arg, typename FunctionType::RangeType& ret) const {
      function_.evaluate(arg, ret);
    }
  };
  template< class FunctionType, class TimeProviderType >
  struct TimeEvaluationFunctor
  {
    const FunctionType& function_;
    const TimeProviderType& timeProvider_;
    TimeEvaluationFunctor(const FunctionType& function,
                          const TimeProviderType& timeProvider)
      : function_(function)
        , timeProvider_(timeProvider)
    {}
    void evaluate(const typename FunctionType::DomainType& arg, typename FunctionType::RangeType& ret) const {
      function_.evaluate(timeProvider_.subTime(), arg, ret);
    }
  };
  template< class FunctionType >
  struct TimeEvaluationFunctor< FunctionType, double >
  {
    const FunctionType& function_;
    const double time_;
    TimeEvaluationFunctor(const FunctionType& function,
                          const double time)
      : function_(function)
        , time_(time)
    {}
    void evaluate(const typename FunctionType::DomainType& arg, typename FunctionType::RangeType& ret) const {
      function_.evaluate(time_, arg, ret);
    }
  };

  template< class DiscreteFunctionImp, class EvaluationFunctorType >
  static void projectCommon(const EvaluationFunctorType& evalutionFunctor,
                            DiscreteFunctionImp& discFunc,
                            int polOrd = -1) {
    typedef typename DiscreteFunctionImp::DiscreteFunctionSpaceType  DiscreteFunctionSpaceType;
    typedef typename DiscreteFunctionSpaceType::Traits::GridPartType GridPartType;
    typedef Dune::Fem::CachingQuadrature< GridPartType, 0 > QuadratureType;

    typename DiscreteFunctionSpaceType::RangeType ret(0.0);
    typename DiscreteFunctionSpaceType::RangeType phi(0.0);
    const auto& space = discFunc.space();
    const auto quadOrd = std::max(2 * space.order() + 2, polOrd);
    const Dune::Fem::LocalMassMatrix< DiscreteFunctionSpaceType, QuadratureType > massMatrix(space, quadOrd);
    const bool affineMapping = massMatrix.affine();
    discFunc.clear();

    for (const auto& en : space)
    {
      const auto& geo = en.geometry();
      const QuadratureType quad(en, quadOrd);
      auto lf = discFunc.localFunction(en);
      const auto& baseset = lf.baseFunctionSet();
      const auto quadNop = quad.nop();
      const auto numDofs = lf.numDofs();
      for (decltype(quadNop) qP = 0; qP < quadNop; ++qP)
      {
        const double intel = (affineMapping) ?
                             quad.weight(qP) : // affine case
                             quad.weight(qP) * geo.integrationElement( quad.point(qP) ); // general case

        const auto x = geo.global( quad.point(qP) );
        evalutionFunctor.evaluate(x, ret);
        for (decltype(numDofs) i = 0; i < numDofs; ++i)
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
  } // projectCommon
};

namespace Experimental {

template< int dim, class RangeType, class JacobianRangeType >
struct GradientJacobianToLaplacian
  : public RangeType
{
  GradientJacobianToLaplacian(const JacobianRangeType& /*jacobian*/) {
    // Dune::CompileTimeChecker< ( dim == 1 || dim > 3 ) > NotImplemented;
    static_assert( (dim == 1 || dim > 3), "GradientJacobianToLaplacian not implemented for this dimension!" );
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

template< class TimeProviderType,
          class DiscreteVelocityFunctionType,
          class SigmaFunctionType >
class GradientAdapterFunction
  : public SigmaFunctionType
{
protected:
  typedef GradientAdapterFunction< TimeProviderType,
                                   DiscreteVelocityFunctionType,
                                   SigmaFunctionType >
  ThisType;
  typedef SigmaFunctionType
  BaseType;
  const TimeProviderType& timeProvider_;

public:
  GradientAdapterFunction(const TimeProviderType& timeProvider,
                          const DiscreteVelocityFunctionType& velocity,
                          SigmaFunctionType& dummy,
                          int polOrd = -1)
    : BaseType( "grad", dummy.space() )
      , timeProvider_(timeProvider) {
    typedef SigmaFunctionType
    DiscreteFunctionType;
    typedef typename SigmaFunctionType::DiscreteFunctionSpaceType
    DiscreteFunctionSpaceType;
    typedef typename DiscreteFunctionType::LocalFunctionType
    LocalFuncType;
    typedef typename DiscreteFunctionSpaceType::Traits::GridPartType
    GridPartType;
    typedef typename DiscreteFunctionSpaceType::Traits::IteratorType
    Iterator;
    typedef typename DiscreteFunctionSpaceType::BaseFunctionSetType
    BaseFunctionSetType;
//    typedef typename GridPartType::IntersectionIteratorType
//    IntersectionIteratorType;
    typedef typename DiscreteVelocityFunctionType::LocalFunctionType
    LocalFType;

    const DiscreteFunctionSpaceType space( velocity.space().gridPart() );
    const GridPartType& gridPart = space.gridPart();
    // type of quadrature
    typedef Dune::Fem::CachingQuadrature< GridPartType, 0 > VolumeQuadratureType;
//    typedef Dune::Fem::CachingQuadrature< GridPartType, 1 > FaceQuadratureType;
    // type of local mass matrix
    typedef Dune::Stuff::Fem::LocalMassMatrix< DiscreteFunctionSpaceType, VolumeQuadratureType > LocalMassMatrixType;

    const auto quadOrd = std::max(2 * space.order() + 2, polOrd);

    // create local mass matrix object
    LocalMassMatrixType massMatrix(space, quadOrd);

    // check whether geometry mappings are affine or not
    const bool affineMapping = massMatrix.affine();

    // clear destination
    BaseType::clear();

    const Iterator endit = space.end();
    for (Iterator it = space.begin(); it != endit; ++it)
    {
      // get entity
      const typename GridPartType::GridType::template Codim< 0 >::Entity& entity = *it;
      // get geometry
      typedef  typename GridPartType::GridType::template Codim< 0 >::Geometry
      Geometry;
      const Geometry& geo = entity.geometry();

      // get quadrature
      VolumeQuadratureType quad(entity, quadOrd);

      // get local function of destination
      LocalFuncType self_local = BaseType::localFunction(entity);
      // get local function of argument
      const LocalFType velocity_local = velocity.localFunction(entity);

      // get base function set
      const BaseFunctionSetType& baseset = self_local.baseFunctionSet();

      const auto quadNop = quad.nop();
      const auto numDofs = self_local.numDofs();

      // volume part
      for (decltype(quadNop) qP = 0; qP < quadNop; ++qP)
      {
        const typename DiscreteFunctionSpaceType::DomainType xLocal = quad.point(qP);

        const double intel = (affineMapping) ?
                             quad.weight(qP) : // affine case
                             quad.weight(qP) * geo.integrationElement(xLocal); // general case

        typename DiscreteFunctionSpaceType::DomainType
        xWorld = geo.global(xLocal);

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
          self_local[i] += intel * ( Dune::Stuff::Common::colonProduct(velocity_jacobian_eval, phi) );
        }
      }

      // in case of non-linear mapping apply inverse
      if (!affineMapping)
      {
        massMatrix.applyInverse(entity, self_local);
      }
    }
  }
};

} // namespace Experimental

} // namespace Fem
} // namespace Stuff
} // namespace Dune

#endif // HAVE_DUNE_FEM

#endif // DUNE_STUFF_FEM_FUNCTIONS_CUSTOMPROJECTION_HH
