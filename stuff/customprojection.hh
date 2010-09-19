#ifndef CUSTOMPROJECTION_HH
#define CUSTOMPROJECTION_HH

#include <dune/fem/quadrature/cachingquadrature.hh>
#include <dune/fem/operator/common/operator.hh>
#include <dune/fem/function/common/discretefunction.hh>
#include <dune/fem/function/common/discretefunctionadapter.hh>
#include <dune/fem/operator/1order/localmassmatrix.hh>

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
		const double time = 0.0;
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
		const BOOST_AUTO( gridPart_ , space_.gridPart() );
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
//! basically the fem L2Projection with a function evaluate that converts between compatible types instead of failing
class BetterL2Projection {
public:
	template <class FunctionImp, class DiscreteFunctionImp>
	static void project(const FunctionImp& func,
								DiscreteFunctionImp& discFunc,
								int polOrd = -1)
	{
	  typedef typename DiscreteFunctionImp::DiscreteFunctionSpaceType DiscreteFunctionSpaceType;
	  typedef typename DiscreteFunctionImp::LocalFunctionType LocalFuncType;
	  typedef typename DiscreteFunctionSpaceType::Traits::GridPartType GridPartType;
	  typedef typename DiscreteFunctionSpaceType::Traits::IteratorType Iterator;
	  typedef typename DiscreteFunctionSpaceType::BaseFunctionSetType BaseFunctionSetType ;
	  typedef typename GridPartType::GridType GridType;

	  typedef typename FunctionImp::LocalFunctionType LocalFType;

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
		// get local function of argument
		const LocalFType f = func.localFunction(en);

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
		  typename FunctionImp::DiscreteFunctionSpaceType::RangeType
				  dummy;
		  f.evaluate(quad[qP], dummy);
		  ret =dummy;

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
#endif // CUSTOMPROJECTION_HH
