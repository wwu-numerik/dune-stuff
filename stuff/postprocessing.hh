/**

 *  \file   postprocessing.hh
 *  \brief  postprocessing.hh
 **/

#ifndef POSTPROCESSING_HH
#define POSTPROCESSING_HH

#include <dune/fem/operator/projection/l2projection.hh>
#include <dune/fem/io/file/vtkio.hh>

#include "logging.hh"
#include "problem.hh"
#include "parametercontainer.hh"

#include <dune/fem/misc/l2norm.hh>
#include <cmath>

//! simple macro that uses member vtkWriter instance to write file according to variable name
#define VTK_WRITE(z)    vtkWriter_.addVertexData(z); \
                        vtkWriter_.write(( "data/"#z ) ); \
                        vtkWriter_.clear();


template <  class StokesPassImp, class ProblemImp >
class PostProcessor
{
    public:
        typedef ProblemImp
            ProblemType;

        typedef StokesPassImp
            StokesPassType;
        typedef typename StokesPassType::DiscreteStokesFunctionSpaceWrapperType
            DiscreteStokesFunctionSpaceWrapperType;
        typedef typename StokesPassType::DiscreteStokesFunctionWrapperType
            DiscreteStokesFunctionWrapperType;

        typedef typename ProblemType::VelocityType
            ContinuousVelocityType;
        typedef typename ProblemType::PressureType
            ContinuousPressureType;
        typedef typename ProblemType::ForceType
            ForceType;
        typedef typename ProblemType::DirichletDataType
            DirichletDataType;

        typedef typename StokesPassType::GridPartType
            GridPartType;
        typedef typename GridPartType::GridType
            GridType;

        typedef Dune::VTKIO<GridPartType>
            VTKWriterType;

        typedef typename StokesPassType::DiscreteVelocityFunctionType
            DiscreteVelocityFunctionType;
        typedef typename StokesPassType::DiscreteVelocityFunctionSpaceType
            DiscreteVelocityFunctionSpaceType;

        typedef typename StokesPassType::DiscretePressureFunctionType
            DiscretePressureFunctionType;
        typedef typename StokesPassType::DiscretePressureFunctionSpaceType
            DiscretePressureFunctionSpaceType;


        PostProcessor( const DiscreteStokesFunctionSpaceWrapperType& wrapper, const ProblemType& prob )
            : //pass_( pass ),
            problem_( prob ),
            spaceWrapper_( wrapper ),
            gridPart_( wrapper.gridPart() ),
            velocitySpace_ ( wrapper.discreteVelocitySpace() ),
            discreteExactVelocity_( "u_exact", wrapper.discreteVelocitySpace() ),
            discreteExactForce_( "f_exact", wrapper.discreteVelocitySpace() ),
            discreteExactDirichlet_( "gd_exact", wrapper.discreteVelocitySpace() ),
            discreteExactPressure_( "p_exact", wrapper.discretePressureSpace() ),
            errorFunc_velocity_( "err_velocity", wrapper.discreteVelocitySpace() ),
            errorFunc_pressure_( "err_pressure", wrapper.discretePressureSpace() ),
            solutionAssembled_( false ),
            l2_error_pressure_( - std::numeric_limits<double>::max() ),
            l2_error_velocity_( - std::numeric_limits<double>::max() ),
            vtkWriter_( wrapper.gridPart() )
        {

        }

        ~PostProcessor()
        {
        }

        void assembleExactSolution()
        {
            typedef Dune::L2Projection< double, double, ContinuousVelocityType, DiscreteVelocityFunctionType > ProjectionV;
                ProjectionV projectionV;
            projectionV( problem_.velocity(), discreteExactVelocity_ );

            typedef Dune::L2Projection< double, double, DirichletDataType, DiscreteVelocityFunctionType > ProjectionD;
                ProjectionD projectionD;
            projectionD( problem_.dirichletData(), discreteExactDirichlet_ );

            typedef Dune::L2Projection< double, double, ForceType, DiscreteVelocityFunctionType > ProjectionF;
                ProjectionF projectionF;
            projectionF( problem_.force(), discreteExactForce_ );

            typedef Dune::L2Projection< double, double, ContinuousPressureType, DiscretePressureFunctionType > ProjectionP;
                ProjectionP projectionP;
            projectionP( problem_.pressure(), discreteExactPressure_ );
        }

        void save( const GridType& grid, const DiscreteStokesFunctionWrapperType& wrapper )
        {
            if ( !solutionAssembled_ )
                assembleExactSolution();

            calcError( wrapper.discretePressure() , wrapper.discreteVelocity() );

            VTK_WRITE( wrapper.discretePressure() );
            VTK_WRITE( wrapper.discreteVelocity() );
            VTK_WRITE( discreteExactVelocity_ );
			VTK_WRITE( discreteExactPressure_ );
            VTK_WRITE( discreteExactForce_ );
			VTK_WRITE( discreteExactDirichlet_ );
			VTK_WRITE( errorFunc_pressure_ );
			VTK_WRITE( errorFunc_velocity_ );
#ifndef NLOG
			entityColoration();
#endif
        }

        void calcError( const DiscretePressureFunctionType& pressure, const DiscreteVelocityFunctionType& velocity )
        {
            if ( !solutionAssembled_ )
                assembleExactSolution();

            errorFunc_pressure_.assign( discreteExactPressure_ );
            errorFunc_pressure_ -= pressure;
            errorFunc_velocity_.assign( discreteExactVelocity_ );
            errorFunc_velocity_ -= velocity;

            Dune::L2Norm< GridPartType > l2_Error( gridPart_ );
            l2_error_pressure_ =
                l2_Error.norm( errorFunc_pressure_ );
            l2_error_velocity_ =
                l2_Error.norm( errorFunc_velocity_ );

            Logger().Info().Resume();
            Logger().Info() << "L2-Error Pressure: " << std::setw(8) << l2_error_pressure_ << "\n"
                                << "L2-Error Velocity: " << std::setw(8) << l2_error_velocity_ << "\n"
                                << "L2-Error Pressure (sqrt): " << std::setw(8) << std::sqrt( l2_error_pressure_ ) << "\n"
                                << "L2-Error Velocity (sqrt): " << std::setw(8) << std::sqrt( l2_error_velocity_ ) << std::endl;
        }

        std::vector<double> getError()
        {
            std::vector<double> ret;
            ret.push_back( l2_error_velocity_ );
            ret.push_back( l2_error_pressure_ );
            return ret;
        }

        void entityColoration()
        {
            Logging::LogStream& dbg = Logger().Dbg();
            DiscretePressureFunctionType cl ( "entitiy-num", spaceWrapper_.discretePressureSpace() );
            unsigned long numberOfEntities = 0;

            typedef typename GridPartType::GridType::template Codim< 0 >::Entity
                EntityType;
            typedef typename GridPartType::template Codim< 0 >::IteratorType
                EntityIteratorType;
            typedef typename GridPartType::IntersectionIteratorType
                IntersectionIteratorType;

            EntityIteratorType entityItEndLog = velocitySpace_.end();
            for (   EntityIteratorType entityItLog = velocitySpace_.begin();
                    entityItLog != entityItEndLog;
                    ++entityItLog, ++numberOfEntities ) {
                const EntityType& entity = *entityItLog;
                const typename EntityType::Geometry& geo = entity .geometry();

//                dbg << "entity: " << numberOfEntities <<"\n";
                for ( int i = 0; i < geo.corners(); ++i ){
//                    Stuff::printFieldVector( geo[i], "  corner", dbg );
                }
//                dbg << std::endl;

                typename DiscretePressureFunctionType::LocalFunctionType
                    lf = cl.localFunction( entity );

                for ( int i = 0; i < lf.numDofs(); ++i ){
                    lf[i] = numberOfEntities;
                }

                unsigned long numberOfIntersections =0;
                IntersectionIteratorType intItEnd = gridPart_.iend( entity );
                for (   IntersectionIteratorType intIt = gridPart_.ibegin( entity );
                        intIt != intItEnd;
                        ++intIt,++numberOfIntersections ) {
                    // if we are inside the grid
                    if ( intIt.neighbor() && !intIt.boundary() ) {
                        // count inner intersections

                    }
                    // if we are on the boundary of the grid
                    if ( !intIt.neighbor() && intIt.boundary() ) {
                        // count boundary intersections

                    }
                }
            }

            VTK_WRITE( cl );
        }

    private:

        const ProblemType& problem_;
        const DiscreteStokesFunctionSpaceWrapperType& spaceWrapper_;
        const GridPartType& gridPart_;
        const DiscreteVelocityFunctionSpaceType& velocitySpace_;
        DiscreteVelocityFunctionType discreteExactVelocity_;
        DiscreteVelocityFunctionType discreteExactForce_;
        DiscreteVelocityFunctionType discreteExactDirichlet_;
        DiscretePressureFunctionType discreteExactPressure_;
        DiscreteVelocityFunctionType errorFunc_velocity_;
        DiscretePressureFunctionType errorFunc_pressure_;
        bool solutionAssembled_;
        double l2_error_pressure_;
        double l2_error_velocity_;
        VTKWriterType vtkWriter_;

};

#undef VTK_WRITE

#endif // end of postprocessing.hh
