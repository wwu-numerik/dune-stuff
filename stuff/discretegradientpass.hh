/**
 *  \file   discretegradientpass.hh
 *  \brief  contains a class DiscreteGradientPass
 *
 *          see R. Eymard, T. Gallouet, R. Herbin, 2006, "A cell-centered
 *          finite-volume approximation for anisotropic diffusion operators on
 *          unstructured meshes in any space dimension" for details
 **/

#ifndef DUNE_STUFF_DISCRETEGRADIENTPASS_HH
#define DUNE_STUFF_DISCRETEGRADIENTPASS_HH

#include <dune/fem/pass/pass.hh>
#include <dune/fem/space/dgspace.hh>

#ifndef NLOG
    #include <dune/stuff/printing.hh>
    #include <dune/stuff/misc.hh>
	#include <dune/stuff/logging.hh>
#endif

#if( POLORDER != 0 )
    #warning "discrete gradient method is only defined for constant functions!"
#endif

namespace Dune{

// forward decraration
template < class DiscreteGradientModelTraitsImp >
class DiscreteGradientModel;

/**
 *  \brief DiscreteGradientModelTraits
 *  \todo   doc
 **/
template < class GridPartImp, class DiscreteFunctionImp >
class DiscreteGradientModelTraits
{
    public:

        //! CRTP trick
        typedef DiscreteGradientModel< DiscreteGradientModelTraits >
            DiscreteModelType;

        //! grid part type
        typedef GridPartImp
            GridPartType;

        /**
         *  \name   types needed for the pass
         *  \{
         **/
        //! return type of the pass
        typedef DiscreteFunctionImp
            DestinationType;
        /**
         *  \}
         **/

};

/**
 *  \brief  DiscreteGradientModel
 *  \todo   doc
 **/
template < class DiscreteGradientModelTraitsImp >
class DiscreteGradientModel
{
    public:

        //! traits
        typedef DiscreteGradientModelTraitsImp
            Traits;

        //! grid part type
        typedef typename Traits::GridPartType
            GridPartType;

};

/**
 *  \brief  DiscreteGradientPass
 *  \todo   doc
 **/
template <  class DiscreteModelImp,
            class PreviousPassImp,
            int PassID = 0 >
class DiscreteGradientPass
    : public Pass < DiscreteModelImp, PreviousPassImp, PassID >
{
    public:

        //! base type
        typedef Pass < DiscreteModelImp, PreviousPassImp, PassID >
            BaseType;

        //! previous pass type
        typedef PreviousPassImp
            PreviousPassType;

        //! discrete model type
        typedef DiscreteModelImp
            DiscreteModelType;

        /**
         *  \name typedefs needed for interface compliance
         *  \{
         **/
        typedef typename BaseType::DestinationType
            DestinationType;

        typedef typename BaseType::DomainType
            DomainType;

        typedef typename BaseType::RangeType
            RangeType;

        typedef typename BaseType::TotalArgumentType
            TotalArgumentType;
        /**
         *  \}
         **/

    private:

        //! grd part type
        typedef typename DiscreteModelType::GridPartType
            GridPartType;

        typedef typename GridPartType::GridType
            GridType;

        typedef DomainType
            DiscreteFunctionType;

        typedef typename DiscreteFunctionType::DiscreteFunctionSpaceType
            DiscreteFunctionSpaceType;

        typedef typename DiscreteFunctionType::DomainType
            DiscreteFunctionDomainType;

        typedef DiscreteFunctionDomainType
            WorldCoordianteType;

        typedef typename DiscreteFunctionType::RangeType
            DiscreteFunctionRangeType;

        typedef RangeType
            DiscreteGradientType;

        typedef typename DiscreteGradientType::DiscreteFunctionSpaceType
            DiscreteGradientSpaceType;

        typedef typename DiscreteGradientType::DomainType
            DiscreteGradientDomainType;

        typedef typename DiscreteGradientType::RangeType
            DiscreteGradientRangeType;

    public:


        /**
         *  \brief  constructor
         *  \todo   doc
         **/
        DiscreteGradientPass(   PreviousPassType& prevPass,
                                DiscreteModelType& discreteModel,
                                GridPartType& gridPart )
            : BaseType( prevPass ),
            discreteModel_( discreteModel ),
            gridPart_( gridPart ),
            grid_( gridPart_.grid() ){}

        /**
         *  \brief destructor
         *  \todo   doc
         **/
        ~DiscreteGradientPass(){}

        /**
         *  \brief main method
         *  \todo   doc
         **/
        virtual void apply( const DomainType &arg, RangeType &dest) const
        {

#ifndef NLOG
            // logging stuff
            Logging::LogStream& infoStream = Logger().Info();
            Logging::LogStream& debugStream = Logger().Dbg();
            Logging::LogStream& errorStream = Logger().Err();
            infoStream << "  - entering DiscreteGradientPass::apply()" << std::endl;
#endif

            // clear return
            dest.clear();

            //! type of the grid
            typedef typename GridPartType::GridType
                GridType;

            //! type of codim 0 entity
            typedef typename GridType::template Codim< 0 >::Entity
                EntityType;

            //! entity geometry type
            typedef typename EntityType::Geometry
                EntityGeometryType;

            //! entity iterator of the gridpart
            typedef typename GridPartType::template Codim< 0 >::IteratorType
                EntityIteratorType;

            //! Intersection iterator of the gridpart
            typedef typename GridPartType::IntersectionIteratorType
                IntersectionIteratorType;

            //! local function type
            typedef typename DomainType::LocalFunctionType
                LocalFunctionType;

            //! local gradient type
            typedef typename RangeType::LocalFunctionType
                LocalGradientType;

            //! discrete function base fucntion set type
            typedef typename DiscreteFunctionSpaceType::BaseFunctionSetType
                DiscreteFunctionBaseFunctionSetType;

            //! discrete gradient base function set type
            typedef typename DiscreteGradientSpaceType::BaseFunctionSetType
                DiscreteGradientBaseFunctionSetType;

            //! we use caching quadratures for the entities
            typedef Dune::CachingQuadrature< GridPartType, 0 >
                VolumeQuadratureType;

            //! we use caching quadratures for the faces
            typedef Dune::CachingQuadrature< GridPartType, 1 >
                FaceQuadratureType;

            // get the spaces
            const DiscreteFunctionSpaceType& discreteFunctionSpace_ = arg.space();
            const DiscreteGradientSpaceType& discreteGradientSpace_ = dest.space();

            // figure out the right dimensions
            const int dimWorld = GridType::dimensionworld;
            const DiscreteFunctionRangeType dummyFunctionRange( 0.0 );
            const DiscreteGradientRangeType dummyGradientRange( 0.0 );
            const int dimOfDiscreteFunctionRange = dummyFunctionRange.dim();

#if ( dimOfDiscreteFunctionRange == 1 )
    #define RANGETYPE_IS_A_SCALAR
#elif ( dimOfDiscreteFunctionRange == dimWorld )
    #define RANGETYPE_IS_A_VECTOR
#else
    #error "dimOfDiscreteFunctionRange != 1 AND dimOfDiscreteFunctionRange != dimWorld"
#endif

#ifdef RANGETYPE_IS_A_SCALAR
            const int colDimOfDiscreteGradientRange = dummyGradientRange.dim();
            const int rowDimOfDiscreteGradientRange( 1 );
#endif
#ifdef RANGETYPE_IS_A_VECTOR
            const int rowDimOfDiscreteGradientRange = dummyGradientRange.rowdim();
            const int colDimOfDiscreteGradientRange = dummyGradientRange.coldim();
#endif

#ifndef NLOG
            debugStream << "    - dimWorld: " << dimWorld << std::endl
                        << "    - dimOfDiscreteFunctionRange: " << dimOfDiscreteFunctionRange << std::endl
                        << "    - rowDimOfDiscreteGradientRange: " << rowDimOfDiscreteGradientRange << std::endl
                        << "    - colDimOfDiscreteGradientRange: " << colDimOfDiscreteGradientRange << std::endl;
#endif

            // walk the grid
            EntityIteratorType entityItEnd = discreteFunctionSpace_.end();
            for (   EntityIteratorType entityIt = discreteFunctionSpace_.begin();
                    entityIt != entityItEnd;
                    ++entityIt ) {

                // get some infos about the entity
                const EntityType& entity = *entityIt;
                const EntityGeometryType& geometryEntity = entity.geometry();
                const double volumeEntity = geometryEntity.volume();

                // get quadrature
                const VolumeQuadratureType volumeQuadratureEntity( entity, 0 );

                // get the local functions
                LocalFunctionType localFunctionEntity = arg.localFunction( entity );
                LocalGradientType localGradientEntity = dest.localFunction( entity );

                // get the basefunctionsets
                const DiscreteFunctionBaseFunctionSetType discreteFunctionBaseFunctionSetEntity = discreteFunctionSpace_.baseFunctionSet( entity );
                const DiscreteGradientBaseFunctionSetType discreteGradientBaseFunctionSetEntity = discreteGradientSpace_.baseFunctionSet( entity );
                const int numDiscreteFunctionBaseFunctionsEntity = discreteFunctionBaseFunctionSetEntity.numBaseFunctions();
                const int numDiscreteGradientBaseFunctionsEntity = discreteGradientBaseFunctionSetEntity.numBaseFunctions();

                // play safe
#ifdef RANGETYPE_IS_A_SCALAR
                assert( numDiscreteFunctionBaseFunctionsEntity == 1 );
                assert( numDiscreteGradientBaseFunctionsEntity == dimWorld );
#endif
#ifdef RANGETYPE_IS_A_VECTOR
                assert( numDiscreteFunctionBaseFunctionsEntity == dimWorld );
                assert( numDiscreteGradientBaseFunctionsEntity == ( dimWorld * dimWorld ) );
#endif

                const WorldCoordianteType centerOfGravityElement = volumeQuadratureEntity.point( 0 );

                // walk the intersections
                IntersectionIteratorType intItEnd = gridPart_.iend( entity );
                for (   IntersectionIteratorType intIt = gridPart_.ibegin( entity );
                        intIt != intItEnd;
                        ++intIt ) {

                    // some info about the intersection
                    const typename IntersectionIteratorType::Geometry& geometryIntersection = intIt.intersectionGlobal();
                    const double volumeIntersection = geometryIntersection.volume();

                    // get quadrature
                    const FaceQuadratureType faceQuadratureElement( gridPart_,
                                                                    intIt,
                                                                    0,
                                                                    FaceQuadratureType::INSIDE );
                    const WorldCoordianteType centerOfGravityFace = faceQuadratureElement.point( 0 );
                    const WorldCoordianteType differenceOfGravityCenters = centerOfGravityFace - centerOfGravityElement;
                    const double normOfDifferenceOfGravityCenters = differenceOfGravityCenters.two_norm();

                    // if we are inside the grid
                    if ( intIt.neighbor() && !intIt.boundary() ) {

                        // get some infos about the neighbour
                        const typename IntersectionIteratorType::EntityPointer neighbourPtr = intIt.outside();
                        const EntityType& neighbour = *neighbourPtr;
                        const EntityGeometryType& geometryNeighbour = neighbour.geometry();

                        // get the local functions
                        LocalFunctionType localFunctionNeighbour = arg.localFunction( neighbour );
                        LocalGradientType localGradientNeighbour = dest.localFunction( neighbour );

                        // get the basefunctionsets
                        const DiscreteFunctionBaseFunctionSetType discreteFunctionBaseFunctionSetNeighbour = discreteFunctionSpace_.baseFunctionSet( neighbour );
                        const DiscreteGradientBaseFunctionSetType discreteGradientBaseFunctionSetNeighbour = discreteGradientSpace_.baseFunctionSet( neighbour );
                        const int numDiscreteFunctionBaseFunctionsNeighbour = discreteFunctionBaseFunctionSetNeighbour.numBaseFunctions();
                        const int numDiscreteGradientBaseFunctionsNeighbour = discreteGradientBaseFunctionSetNeighbour.numBaseFunctions();

                        // play safe
#ifdef RANGETYPE_IS_A_SCALAR
                        assert( numDiscreteFunctionBaseFunctionsNeighbour == 1 );
                        assert( numDiscreteGradientBaseFunctionsNeighbour == dimWorld );
#endif
#ifdef RANGETYPE_IS_A_VECTOR
                        assert( numDiscreteFunctionBaseFunctionsNeighbour == dimWorld );
                        assert( numDiscreteGradientBaseFunctionsNeighbour == ( dimWorld * dimWorld ) );
#endif

                        for ( int i = 0; i < dimWorld; ++i ) {
                            // compute sum
                            DiscreteFunctionRangeType sum = differenceOfGravityCenters;
                            sum *= ( 1.0 / normOfDifferenceOfGravityCenters );
                            sum *= ( volumeIntersection / volumeEntity );
                            sum *= ( localFunctionNeighbour[i] - localFunctionEntity[i] );
                            // add to gradient vector
                            for ( int j = 0; j < dimWorld; ++j ) {
                                localGradientEntity[ ( dimWorld * i ) + j ] += sum[ j ];
                            }
                        }

                    } // end if we are inside

                    // if we are on the boundary of the grid
                    if ( !intIt.neighbor() && intIt.boundary() ) {

                        for ( int i = 0; i < dimWorld; ++i ) {
                            // compute sum
                            DiscreteFunctionRangeType sum = differenceOfGravityCenters;
                            sum *= ( -1.0 / normOfDifferenceOfGravityCenters );
                            sum *= ( volumeIntersection / volumeEntity );
                            sum *= localFunctionEntity[i];
                            // add to gradient vector
                            for ( int j = 0; j < dimWorld; ++j ) {
                                localGradientEntity[ ( dimWorld * i ) + j ] += sum[ j ];
                            }
                        }

                    } // end if we are on the boundary

                } // end of intersection walk

            } // end of gridwalk

#ifndef NLOG
            infoStream  << "  - leaving DiscreteGradientPass::apply()" << std::endl
                        << std::endl;
#endif
        } // end of apply()

        /**
         *  \name methods needed for interface compliance
         *  \{
         **/
        virtual void compute( const TotalArgumentType &arg, DestinationType &dest) const
        {}

        virtual void allocateLocalMemory()
        {}
        /**
         *  \}
         **/


    private:

        DiscreteModelType& discreteModel_;
        GridPartType& gridPart_;
        GridType& grid_;

}; // end of DiscreteGradientPass

} //end ns DUne

#endif // end of dicretegradientpass.hh DUNE_STUFF_DISCRETEGRADIENTPASS_HH
