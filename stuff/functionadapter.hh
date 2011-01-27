#ifndef DUNE_STUFF_FUNCTIONADAPTER_HH
#define DUNE_STUFF_FUNCTIONADAPTER_HH

#include <vector>
#include <boost/shared_ptr.hpp>
#include <dune/stuff/printing.hh>
#include <dune/stuff/static_assert.hh>

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
	GradientSplitterFunction(	const typename DiscreteFunctionType::FunctionSpaceType& space,
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


	template < int dim, class RangeType, class JacobianRangeType >
	struct GradientJacobianToLaplacian : public RangeType
	{
		GradientJacobianToLaplacian( const JacobianRangeType& jacobian )
		{
			dune_static_assert((dim==1 || dim>3), "GradientJacobianToLaplacian is not implemented for this dimension!");
			// Dune::CompileTimeChecker< ( dim == 1 || dim > 3 ) > NotImplemented;
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

	struct ProductFunctorMatrices {
		template < class R1, class R2 >
		static double call (const R1& r1,const R2& r2 ) {
			return Stuff::colonProduct( r1, r2 );
		}
	};

	struct ProductFunctorMatrixVector {
		template < class R1, class R2 >
		static double call (const R1& r1,const R2& r2 ) {
			Dune::FieldVector< double, 1 > res;
			r1.mv( r2, res ) ;
			return res[0];
		}
	};

	template <	class DiscreteVelocityFunctionType,
				class SigmaFunctionType,
				class ProductFunctor >
	class GradientAdapterFunction :
			public SigmaFunctionType
	{
		protected:
			typedef GradientAdapterFunction <	DiscreteVelocityFunctionType,
												SigmaFunctionType,
												ProductFunctor>
				ThisType;
			typedef SigmaFunctionType
				BaseType;

		public:
			GradientAdapterFunction ( const DiscreteVelocityFunctionType& velocity,
								  SigmaFunctionType& dummy,
								  int polOrd = -1 )
				: BaseType( "grad" , dummy.space())
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
					const typename GridType::template Codim<0>::Entity& entity = *it;
					// get geometry
					typedef  typename GridType::template Codim<0>::Geometry
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
							self_local[i] += intel * ProductFunctor::call( velocity_jacobian_eval, phi );
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

	template <	class DiscreteVelocityFunctionType,
				class SigmaFunctionType,
				class ProductFunctor >
	class LaplaceAdapterFunction :
			public DiscreteVelocityFunctionType
	{
		protected:
			typedef LaplaceAdapterFunction <	DiscreteVelocityFunctionType,
												SigmaFunctionType,
												ProductFunctor>
				ThisType;
			typedef DiscreteVelocityFunctionType
				BaseType;

		public:
			LaplaceAdapterFunction( const DiscreteVelocityFunctionType& velocity,
								  SigmaFunctionType& dummy,
								  int polOrd = -1 )
				: BaseType( "grad" , velocity.space())
			{
				GradientAdapterFunction<	DiscreteVelocityFunctionType,
											SigmaFunctionType,
											ProductFunctor>
				        gradient( velocity, dummy, polOrd );
				init( gradient, polOrd );
			}

//			LaplaceAdapterFunction( const SigmaFunctionType& gradient )
//				: BaseType( "grad" , velocity.space())
//			{
//				init( gradient, -1 );
//			}


			void init( const SigmaFunctionType& gradient, int polOrd )
			{
				typedef DiscreteVelocityFunctionType
					DiscreteFunctionType;
				typedef typename DiscreteFunctionType::DiscreteFunctionSpaceType
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
				typedef typename SigmaFunctionType::LocalFunctionType
					LocalFType;

				const DiscreteFunctionSpaceType& space = BaseType::space();
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
					const typename GridType::template Codim<0>::Entity& entity = *it;
					// get geometry
					typedef  typename GridType::template Codim<0>::Geometry
						Geometry;
					const Geometry& geo = entity.geometry();

					// get quadrature
					VolumeQuadratureType quad(entity, quadOrd);

					// get local function of destination
					LocalFuncType self_local = BaseType::localFunction(entity);
					// get local function of argument
					const LocalFType gradient_local = gradient.localFunction(entity);

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

						typename SigmaFunctionType::DiscreteFunctionSpaceType::JacobianRangeType
							gradient_jacobian_eval;
						gradient_local.jacobian( quad[qP], gradient_jacobian_eval );

						GradientJacobianToLaplacian<	DiscreteFunctionType::RangeType::size,
																typename DiscreteFunctionType::RangeType,
																typename SigmaFunctionType::JacobianRangeType >
								velocity_real_laplacian ( gradient_jacobian_eval );

						// do projection
						for(int i=0; i<numDofs; ++i)
						{
							typename DiscreteFunctionType::DiscreteFunctionSpaceType::RangeType phi (0.0);
							baseset.evaluate(i, quad[qP], phi);
							self_local[i] += 2 * intel * ( velocity_real_laplacian * phi );
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


}

#endif // DUNE_STUFF_FUNCTIONADAPTER_HH
