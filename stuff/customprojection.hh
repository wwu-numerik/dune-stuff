#ifndef CUSTOMPROJECTION_HH
#define CUSTOMPROJECTION_HH

#include <dune/fem/quadrature/cachingquadrature.hh>
#include <dune/fem/operator/common/operator.hh>
#include <dune/fem/function/common/discretefunction.hh>
#include <dune/fem/function/common/gridfunctionadapter.hh>
#include <dune/fem/operator/1order/localmassmatrix.hh>
#include <dune/stuff/static_assert.hh>
#include <dune/stuff/math.hh>

namespace Stuff {

/** A custom projection of an analytical function that uses a non-standard evalute signature:\n
		<pre>template < class IntersectionIteratorType >\n
		void evaluate( const DomainType& arg, RangeType& ret, const IntersectionIteratorType& faceIter ) const</pre>\n
	\note example being our boundary functions
	\note output currently somewhat meaningless
	\see analyticaldata.hh
**/
class CustomProjection {

public:
	template < class OriginFunctionType, class DestinationFunctionType >
	static void project (const OriginFunctionType& f, DestinationFunctionType& discFunc)
	{
		typedef typename DestinationFunctionType::FunctionSpaceType
			DiscreteFunctionSpace;
		typedef typename DiscreteFunctionSpace::GridPartType
			GridPart;
		typedef typename GridPart::template Codim< 0 >::IteratorType
			EntityIteratorType;
		typedef typename GridPart::GridType::template Codim< 0 >::Entity
			EntityType;
		typedef typename GridPart::IntersectionIteratorType
			IntersectionIteratorType;
		typedef typename IntersectionIteratorType::EntityPointer
			EntityPointer;
		typedef typename DestinationFunctionType::LocalFunctionType
			LocalFunctionType;
		typedef Dune::CachingQuadrature< GridPart, 1 >
			FaceQuadratureType;
		typedef typename DiscreteFunctionSpace::BaseFunctionSetType
			BaseFunctionSetType;
		typedef typename DiscreteFunctionSpace::RangeType
			RangeType;
		const DiscreteFunctionSpace& space_ = discFunc.space();
		const GridPart& gridPart_ = space_.gridPart();
		RangeType phi (0.0);
		EntityIteratorType entityItEndLog = space_.end();
		for (   EntityIteratorType it = space_.begin();
				it != entityItEndLog;
				++it )
		{
			EntityType& e = *it;
			LocalFunctionType lf = discFunc.localFunction( e );
			BaseFunctionSetType baseFunctionset = space_.baseFunctionSet( *it );
			unsigned int intersection_count = 0;
			IntersectionIteratorType intItEnd = gridPart_.iend( *it );
			for (   IntersectionIteratorType intIt = gridPart_.ibegin( *it );
					intIt != intItEnd;
					++intIt ) {
				intersection_count++;
				FaceQuadratureType faceQuadrature( gridPart_,
												   *intIt,
												   ( 4 * space_.order() ) + 1,
												   FaceQuadratureType::INSIDE );
				typename DestinationFunctionType::RangeType ret;
				for ( size_t qP = 0; qP < faceQuadrature.nop(); ++qP ) {
					const double intel =
						 faceQuadrature.weight(qP) * e.geometry().integrationElement( faceQuadrature.point(qP) ); // general case

					if ( intIt->boundary() )
					{
						f.evaluate( faceQuadrature.point(qP), ret, *intIt );

						for ( int i = 0; i < baseFunctionset.numBaseFunctions(); ++i ) {
							baseFunctionset.evaluate(i, faceQuadrature[qP], phi);
							lf[i] += intel * (ret * phi) ;
						}
					}
				}
			}
		}
	}
};
}//end namespace Stuff

namespace Dune {
/**
	 basically the fem L2Projection with a function evaluate that converts between compatible types instead of failing

	 **/
class BetterL2Projection {
protected:
	template < class FunctionType >
	struct DefaultEvaluationFunctor;
public:
	template <class FunctionImp, class DiscreteFunctionImp>
	static void project(const FunctionImp& func,
								DiscreteFunctionImp& discFunc,
								int polOrd = -1)
	{
		dune_static_assert( !(Conversion<FunctionImp, IsDiscreteFunction> ::exists), "TimeAwareL2Projection_not_implemented_for_discrete_source_functions" );
		DefaultEvaluationFunctor< FunctionImp > functor( func );
		projectCommon( functor, discFunc, polOrd );
	}
	template < class TimeProviderType, class FunctionImp, class DiscreteFunctionImp>
	static void project(	const TimeProviderType& timeProvider,
							const FunctionImp& func,
							DiscreteFunctionImp& discFunc,
							int polOrd = -1)
	{
		dune_static_assert( !(Conversion<FunctionImp, IsDiscreteFunction> ::exists), "TimeAwareL2Projection_not_implemented_for_discrete_source_functions" );
		TimeEvaluationFunctor< FunctionImp, TimeProviderType > functor( func, timeProvider );
		projectCommon( functor, discFunc, polOrd );
	}

protected:
	template < class FunctionType >
	struct DefaultEvaluationFunctor
	{
		const FunctionType& function_;
		DefaultEvaluationFunctor( const FunctionType& function )
			:function_( function )
		{}

		void evaluate( const typename FunctionType::DomainType& arg, typename FunctionType::RangeType& ret ) const
		{
			function_.evaluate( arg, ret );
		}
	};
	template < class FunctionType, class TimeProviderType >
	struct TimeEvaluationFunctor
	{
		const FunctionType& function_;
		const TimeProviderType& timeProvider_;
		TimeEvaluationFunctor(	const FunctionType& function,
								const TimeProviderType& timeProvider)
			:function_( function ),
			timeProvider_( timeProvider )
		{}
		void evaluate( const typename FunctionType::DomainType& arg, typename FunctionType::RangeType& ret ) const
		{
			function_.evaluate( timeProvider_.subTime(), arg, ret );
		}
	};
	template < class FunctionType >
	struct TimeEvaluationFunctor<FunctionType,double>
	{
		const FunctionType& function_;
		const double time_;
		TimeEvaluationFunctor(	const FunctionType& function,
								const double time )
			:function_( function ),
			time_( time )
		{}
		void evaluate( const typename FunctionType::DomainType& arg, typename FunctionType::RangeType& ret ) const
		{
			function_.evaluate( time_, arg, ret );
		}
	};

	template < class DiscreteFunctionImp, class EvaluationFunctorType >
	static void projectCommon(	const EvaluationFunctorType& evalutionFunctor,
								DiscreteFunctionImp& discFunc,
								int polOrd = -1)
	{
	  typedef typename DiscreteFunctionImp::DiscreteFunctionSpaceType DiscreteFunctionSpaceType;
	  typedef typename DiscreteFunctionImp::LocalFunctionType LocalFuncType;
	  typedef typename DiscreteFunctionSpaceType::Traits::GridPartType GridPartType;
	  typedef typename DiscreteFunctionSpaceType::Traits::IteratorType Iterator;
	  typedef typename DiscreteFunctionSpaceType::BaseFunctionSetType BaseFunctionSetType ;
	  typedef typename GridPartType::GridType GridType;

//	  typedef typename FunctionImp::LocalFunctionType LocalFType;

	  typename DiscreteFunctionSpaceType::RangeType ret (0.0);
	  typename DiscreteFunctionSpaceType::RangeType phi (0.0);
	  const DiscreteFunctionSpaceType& space =  discFunc.space();

	  // type of quadrature
	  typedef CachingQuadrature<GridPartType,0> QuadratureType;
	  // type of local mass matrix
	  typedef LocalDGMassMatrix< DiscreteFunctionSpaceType, QuadratureType > LocalMassMatrixType;

	  const int quadOrd = (polOrd == -1) ? (2 * space.order()) : polOrd;

	  // create local mass matrix object
	  LocalMassMatrixType massMatrix( space, quadOrd );

	  // check whether geometry mappings are affine or not
	  const bool affineMapping = massMatrix.affine();

	  // clear destination
	  discFunc.clear();

	  const Iterator endit = space.end();
	  for(Iterator it = space.begin(); it != endit ; ++it)
	  {
		// get entity
		const typename GridType::template Codim<0>::Entity& en = *it;
		// get geometry
		const typename GridType::template Codim<0>::Geometry& geo = en.geometry();

		// get quadrature
		QuadratureType quad(en, quadOrd);

		// get local function of destination
		LocalFuncType lf = discFunc.localFunction(en);

		// get base function set
		const BaseFunctionSetType & baseset = lf.baseFunctionSet();

		const int quadNop = quad.nop();
		const int numDofs = lf.numDofs();

		for(int qP = 0; qP < quadNop ; ++qP)
		{
		  const double intel = (affineMapping) ?
			   quad.weight(qP) : // affine case
			   quad.weight(qP) * geo.integrationElement( quad.point(qP) ); // general case

		  // evaluate function
		  typename DiscreteFunctionSpaceType::DomainType x = geo.global( quad.point( qP ) );
		  evalutionFunctor.evaluate( x, ret );

		  // do projection
		  for(int i=0; i<numDofs; ++i)
		  {
			baseset.evaluate(i, quad[qP], phi);
			lf[i] += intel * (ret * phi) ;
		  }
		}

		// in case of non-linear mapping apply inverse
		if ( ! affineMapping )
		{
		  massMatrix.applyInverse( en, lf );
		}
	  }
	}
};

}//end namespace Dune

namespace Stuff {
	namespace Experimental {
		template < int dim, class RangeType, class JacobianRangeType >
		struct GradientJacobianToLaplacian : public RangeType
		{
			GradientJacobianToLaplacian( const JacobianRangeType& jacobian )
			{
			  //Dune::CompileTimeChecker< ( dim == 1 || dim > 3 ) > NotImplemented;
				dune_static_assert(( dim == 1 || dim > 3 ), "GradientJacobianToLaplacian not implemented for this dimension!");
			}
		};

		template < class RangeType, class JacobianRangeType >
		struct GradientJacobianToLaplacian< 2, RangeType, JacobianRangeType>  : public RangeType
		{
			GradientJacobianToLaplacian( const JacobianRangeType& jacobian )
			{
				(*this)[0] = jacobian[0][0];
				(*this)[1] = jacobian[3][1];
			}
		};

		template < class RangeType, class JacobianRangeType >
		struct GradientJacobianToLaplacian< 3, RangeType, JacobianRangeType>  : public RangeType
		{
			GradientJacobianToLaplacian( const JacobianRangeType& jacobian )
			{
				(*this)[0] = jacobian[0][0];
				(*this)[1] = jacobian[4][1];
				(*this)[2] = jacobian[8][2];
			}
		};

		template <	class TimeProviderType,
					class DiscreteVelocityFunctionType,
					class SigmaFunctionType >
		class GradientAdapterFunction :
				public SigmaFunctionType
		{
			protected:
				typedef GradientAdapterFunction <	TimeProviderType,
													DiscreteVelocityFunctionType,
													SigmaFunctionType >
					ThisType;
				typedef SigmaFunctionType
					BaseType;
				const TimeProviderType& timeProvider_;

			public:
				GradientAdapterFunction ( const TimeProviderType& timeProvider,
									  const DiscreteVelocityFunctionType& velocity,
									  SigmaFunctionType& dummy,
									  int polOrd = -1 )
					: BaseType( "grad" , dummy.space()),
					timeProvider_( timeProvider )
				{
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
						BaseFunctionSetType ;
					typedef typename GridPartType::IntersectionIteratorType
						IntersectionIteratorType;
					typedef typename DiscreteVelocityFunctionType::LocalFunctionType
						LocalFType;

					const DiscreteFunctionSpaceType space ( velocity.space().gridPart() );
					const GridPartType& gridPart = space.gridPart();
					// type of quadrature
					typedef Dune::CachingQuadrature<GridPartType,0> VolumeQuadratureType;
					typedef Dune::CachingQuadrature<GridPartType,1> FaceQuadratureType;
					// type of local mass matrix
					typedef Dune::LocalDGMassMatrix< DiscreteFunctionSpaceType, VolumeQuadratureType> LocalMassMatrixType;

					const int quadOrd = (polOrd == -1) ? (2 * space.order()) : polOrd;

					// create local mass matrix object
					LocalMassMatrixType massMatrix( space, quadOrd );

					// check whether geometry mappings are affine or not
					const bool affineMapping = massMatrix.affine();

					// clear destination
					BaseType::clear();

					const Iterator endit = space.end();
					for(Iterator it = space.begin(); it != endit ; ++it)
					{
						// get entity
						const typename GridPartType::GridType::template Codim<0>::Entity& entity = *it;
						// get geometry
						typedef  typename GridPartType::GridType::template Codim<0>::Geometry
							Geometry;
						const Geometry& geo = entity.geometry();

						// get quadrature
						VolumeQuadratureType quad(entity, quadOrd);

						// get local function of destination
						LocalFuncType self_local = BaseType::localFunction(entity);
						// get local function of argument
						const LocalFType velocity_local = velocity.localFunction(entity);

						// get base function set
						const BaseFunctionSetType & baseset = self_local.baseFunctionSet();

						const int quadNop = quad.nop();
						const int numDofs = self_local.numDofs();

						//volume part
						for(int qP = 0; qP < quadNop ; ++qP)
						{
							const typename DiscreteFunctionSpaceType::DomainType xLocal = quad.point(qP);

							const double intel = (affineMapping) ?
								quad.weight(qP): // affine case
								quad.weight(qP)* geo.integrationElement( xLocal ); // general case

							typename DiscreteFunctionSpaceType::DomainType
								xWorld = geo.global( xLocal );

							// evaluate function
							typename DiscreteVelocityFunctionType::DiscreteFunctionSpaceType::RangeType
								velocity_eval;
							velocity_local.evaluate( quad[qP], velocity_eval );

							typename DiscreteVelocityFunctionType::DiscreteFunctionSpaceType::JacobianRangeType
								velocity_jacobian_eval;
							velocity_local.jacobian( quad[qP], velocity_jacobian_eval );

							// do projection
							for(int i=0; i<numDofs; ++i)
							{
								typename DiscreteFunctionType::DiscreteFunctionSpaceType::RangeType phi (0.0);
								baseset.evaluate(i, quad[qP], phi);
								self_local[i] += intel * ( Stuff::colonProduct(velocity_jacobian_eval, phi) );
							}
						}

						// in case of non-linear mapping apply inverse
						if ( ! affineMapping )
						{
							massMatrix.applyInverse( entity, self_local );
						}
					}
				}

		};
	} //ns Experimental
}//ns Stuff

#endif // CUSTOMPROJECTION_HH
