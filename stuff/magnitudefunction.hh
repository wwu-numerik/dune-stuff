#ifndef DUNE_STUFF_MAGNITUDEFUNCTION_HH
#define DUNE_STUFF_MAGNITUDEFUNCTION_HH

#include <dune/fem/function/common/function.hh>
#include <dune/fem/function/common/gridfunctionadapter.hh>
#include <dune/fem/space/common/functionspace.hh>
#include <dune/fem/space/dgspace.hh>
#include <dune/stuff/customprojection.hh>
#include <dune/stuff/debug.hh>

#include "localmassmatrix.hh"

namespace Stuff {

    //! Wrap a DiscreteFunction to a scalar function representing its two-norm
    template< class DiscreteFunctionImp, int polOrder = 3 >
	class MagnitudeFunction
	{
	public:
		typedef Dune::FunctionSpace< typename DiscreteFunctionImp::FunctionSpaceType::DomainFieldType,
																double,
																DiscreteFunctionImp::FunctionSpaceType::dimDomain,
																1 >
			MagnitudeSpaceType;
		typedef DiscreteFunctionImp
			DiscreteFunctionType;
		typedef typename DiscreteFunctionType::DiscreteFunctionSpaceType::GridPartType
			GridPartType;
        typedef Dune::DiscontinuousGalerkinSpace< MagnitudeSpaceType, GridPartType, polOrder >
            MagnitudeDiscreteFunctionSpaceType;
        typedef Dune::AdaptiveDiscreteFunction< MagnitudeDiscreteFunctionSpaceType >
			MagnitudeDiscreteFunctionType;

        //! constructor taking discrete function
        MagnitudeFunction ( const DiscreteFunctionType& discreteFunction )
            : magnitude_disretefunctionspace_( discreteFunction.space().gridPart() ),
              magnitude_disretefunction_( discreteFunction.name() + "-magnitude", magnitude_disretefunctionspace_ )
        {
            typedef typename DiscreteFunctionImp::DiscreteFunctionSpaceType DiscreteFunctionSpaceType;
            typedef typename MagnitudeDiscreteFunctionType::LocalFunctionType LocalFuncType;
            typedef typename DiscreteFunctionSpaceType::Traits::GridPartType GridPartType;
            typedef typename DiscreteFunctionSpaceType::Traits::IteratorType Iterator;
            typedef typename MagnitudeDiscreteFunctionSpaceType::BaseFunctionSetType BaseFunctionSetType ;
            typedef typename GridPartType::GridType GridType;

            typename MagnitudeDiscreteFunctionSpaceType::RangeType ret (0.0);
            typename MagnitudeDiscreteFunctionSpaceType::RangeType phi (0.0);
            const DiscreteFunctionSpaceType& space =  discreteFunction.space();

            // type of quadrature
            typedef Dune::CachingQuadrature<GridPartType,0> QuadratureType;
            // type of local mass matrix
            typedef Stuff::LocalMassMatrix< MagnitudeDiscreteFunctionSpaceType, QuadratureType > LocalMassMatrixType;

            const int quadOrd = 2 * space.order()+2;

            // create local mass matrix object
            LocalMassMatrixType massMatrix( magnitude_disretefunctionspace_, quadOrd );

            // check whether geometry mappings are affine or not
            const bool affineMapping = massMatrix.affine();

            // clear destination
            magnitude_disretefunction_.clear();

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
                LocalFuncType lf = magnitude_disretefunction_.localFunction(en);

                // get base function set
                const BaseFunctionSetType & baseset = lf.baseFunctionSet();

                const int quadNop = quad.nop();
                const int numDofs = lf.numDofs();
                for(int qP = 0; qP < quadNop ; ++qP)
                {
                    const double intel = (affineMapping) ?
                                quad.weight(qP) : // affine case
                                quad.weight(qP) * geo.integrationElement( quad.point(qP) ); // general case

                    typename DiscreteFunctionType::RangeType val;
                    discreteFunction.localFunction( en ).evaluate(quad.point( qP ),val);
                    ret = val.two_norm();
                    for(int i=0; i<numDofs; ++i)
                    {
                        baseset.evaluate(i, quad[qP], phi);
                        lf[i] += intel * (ret * phi);
                    }
                }

                // in case of non-linear mapping apply inverse
                if ( ! affineMapping )
                {
                    massMatrix.applyInverse( en, lf );
                }
            }
        }

		//! virtual destructor
		virtual ~MagnitudeFunction () {}

		const MagnitudeDiscreteFunctionType& discreteFunction() const
		{
			return magnitude_disretefunction_;
		}

	private:
		static const MagnitudeSpaceType magnitude_space_;
		MagnitudeDiscreteFunctionSpaceType magnitude_disretefunctionspace_;
		MagnitudeDiscreteFunctionType magnitude_disretefunction_;
	};

	template < class DF, int I > const typename MagnitudeFunction<DF,I>::MagnitudeSpaceType
    MagnitudeFunction<DF,I>::magnitude_space_ = typename MagnitudeFunction<DF,I>::MagnitudeSpaceType();
} //namespace Stuff

#endif // DUNE_STUFF_MAGNITUDEFUNCTION_HH
