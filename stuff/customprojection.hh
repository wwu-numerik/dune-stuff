#ifndef CUSTOMPROJECTION_HH
#define CUSTOMPROJECTION_HH

#include <dune/fem/quadrature/cachequad.hh>
#include <dune/fem/operator/common/operator.hh>
#include <dune/fem/function/common/discretefunction.hh>
#include <dune/fem/function/common/discretefunctionadapter.hh>
#include <dune/fem/operator/1order/localmassmatrix.hh>

namespace Stuff {

/** A custom projection of an analytical function that uses a non-standard evalute signature:\n
		<pre>template < class IntersectionIteratorType >\n
		void evaluate( const DomainType& arg, RangeType& ret, const IntersectionIteratorType& faceIter ) const<pre>\n
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
												   intIt,
												   ( 4 * space_.order() ) + 1,
												   FaceQuadratureType::INSIDE );
				typename DestinationFunctionType::RangeType ret;
				for ( int qP = 0; qP < faceQuadrature.nop(); ++qP ) {
					const double intel =
						 faceQuadrature.weight(qP) * e.geometry().integrationElement( faceQuadrature.point(qP) ); // general case

					if ( intIt.boundary() )
					{
						f.evaluate( faceQuadrature.point(qP), ret, intIt );

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
#endif // CUSTOMPROJECTION_HH
