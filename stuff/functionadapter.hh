#ifndef DUNE_STUFF_FUNCTIONADAPTER_HH
#define DUNE_STUFF_FUNCTIONADAPTER_HH

#include <vector>
#include <boost/shared_ptr.hpp>
#include <dune/stuff/printing.hh>

namespace Stuff {

/** DiscreteFunction container \f$G\f$ that takes a \$f(dxd)\$f matrix valued function \f$f\f$ as input that splits itself into \$fd\$f
	DiscreteFunctions that are each \$fd\$f valued.
  * \f{eqnarray*}f\rightarrow (a_{ij})_{1<=i,j<=d}\\
  * G_k= \sum_K\int_K \sum_{l=1..d}a_{kl} \phi_{l} dx\f}
  **/
template < class DiscreteFunctionType, class DiscreteGradientFunctionType >
class GradientSplitterFunction : public std::vector< boost::shared_ptr< DiscreteFunctionType > >
{
	typedef std::vector< DiscreteFunctionType >
		BaseType;
	typedef GradientSplitterFunction< DiscreteFunctionType, DiscreteGradientFunctionType >
		ThisType;
	typedef typename boost::shared_ptr< DiscreteFunctionType >
		PointerType;

public:
	GradientSplitterFunction(	typename DiscreteFunctionType::FunctionSpaceType& space,
								const DiscreteGradientFunctionType&  gradient )
	{
		typedef typename DiscreteFunctionType::FunctionSpaceType::DomainType
			DomainType;
		const size_t dim = DomainType::dimension;
		for ( size_t d = 0; d < dim; ++d ) {
			PointerType p( new DiscreteFunctionType( (boost::format("%s_%s") % gradient.name() % Stuff::dimToAxisName(d,true) ).str(),
			                                         space ) );//never use temporary smart pointers
			push_back( p );
		}

		typedef typename DiscreteFunctionType::FunctionSpaceType
			DiscreteFunctionSpaceType;
		typedef typename DiscreteGradientFunctionType::FunctionSpaceType
			DiscreteGradientFunctionSpaceType;
		typedef typename DiscreteFunctionType::LocalFunctionType
			LocalFunctionType;
		typedef std::vector< LocalFunctionType >
			LocalFunctionVectorType;
		typedef typename DiscreteGradientFunctionType::LocalFunctionType
			LocalGradientFunctionType;
		typedef typename DiscreteFunctionSpaceType::Traits::GridPartType
			GridPartType;
		typedef typename DiscreteFunctionSpaceType::Traits::IteratorType
			Iterator;
		typedef typename DiscreteFunctionSpaceType::BaseFunctionSetType
			BaseFunctionSetType ;
		typedef typename GridPartType::IntersectionIteratorType
			IntersectionIteratorType;

		const GridPartType& gridPart = space.gridPart();
		// type of quadrature
		typedef Dune::CachingQuadrature<GridPartType,0> VolumeQuadratureType;
		typedef Dune::CachingQuadrature<GridPartType,1> FaceQuadratureType;
		// type of local mass matrix
		typedef Dune::LocalDGMassMatrix< DiscreteFunctionSpaceType, VolumeQuadratureType> LocalMassMatrixType;

		const int quadOrd =  (2 * space.order());

		// create local mass matrix object
		LocalMassMatrixType massMatrix( space, quadOrd );

		// check whether geometry mappings are affine or not
		const bool affineMapping = massMatrix.affine();

		const Iterator endit = space.end();
		for(Iterator it = space.begin(); it != endit ; ++it)
		{
			// get entity
			const typename GridType::template Codim<0>::Entity& entity = *it;
			// get geometry
			typedef  typename GridType::template Codim<0>::Geometry
				Geometry;
			const Geometry& geo = entity.geometry();

			// get quadrature
			VolumeQuadratureType quad(entity, quadOrd);

			// get local function of destination

			// get local function of argument
			const LocalGradientFunctionType gradient_local = gradient.localFunction(entity);

			for ( size_t d = 0; d < dim; ++d ) {
				LocalFunctionType local_function = this->at(d)->localFunction( entity );

				const BaseFunctionSetType & baseset = local_function.baseFunctionSet();

				const int quadNop = quad.nop();
				const int numDofs = local_function.numDofs();

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
					typename DiscreteGradientFunctionType::RangeType
						gradient_eval;
					gradient_local.evaluate( quad[qP], gradient_eval );

					typename DiscreteFunctionSpaceType::RangeType
					        jacobian_row;
					for ( size_t e = 0; e < dim; ++e ) {
						jacobian_row[e] = gradient_eval(d,e);
					}
					// do projection
					for(int i=0; i<numDofs; ++i)
					{
						typename DiscreteFunctionType::RangeType phi (0.0);
						baseset.evaluate(i, quad[qP], phi);
						local_function[i] += intel * ( jacobian_row * phi );
					}
				}

				// in case of non-linear mapping apply inverse
				if ( ! affineMapping )
				{
					massMatrix.applyInverse( entity, local_function );
				}
			}
		}
	}
};

}

#endif // DUNE_STUFF_FUNCTIONADAPTER_HH
