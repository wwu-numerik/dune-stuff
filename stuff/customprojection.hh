#ifndef CUSTOMPROJECTION_HH
#define CUSTOMPROJECTION_HH

#include <dune/fem/quadrature/cachequad.hh>
#include <dune/fem/operator/common/operator.hh>
#include <dune/fem/function/common/discretefunction.hh>
#include <dune/fem/function/common/discretefunctionadapter.hh>
#include <dune/fem/operator/1order/localmassmatrix.hh>

namespace Stuff {

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
		const DiscreteFunctionSpace& space_ = discFunc.space();
		const GridPart& gridPart_ = space_.gridPart();
		EntityIteratorType entityItEndLog = space_.end();
		for (   EntityIteratorType it = space_.begin();
				it != entityItEndLog;
				++it )
		{
			LocalFunctionType lf = discFunc.localFunction( *it );
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
				for ( int i = 0; i < baseFunctionset.numBaseFunctions(); ++i ) {
					typename DestinationFunctionType::RangeType value;
					for ( int quad = 0; quad < faceQuadrature.nop(); ++quad ) {
						//intel,weight, point->global
						if ( intIt.boundary() ) {
							f.evaluate( faceQuadrature.point(quad), value, intIt );
						}
						lf[i] = value;
					}
				}
			}
		}
	}
};

}//end namespace Stuff
#endif // CUSTOMPROJECTION_HH
