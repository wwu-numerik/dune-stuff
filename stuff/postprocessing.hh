/**

 *  \file   postprocessing.hh
 *  \brief  postprocessing.hh
 **/

#ifndef POSTPROCESSING_HH
#define POSTPROCESSING_HH

#include <dune/fem/operator/projection/l2projection.hh>
#include <dune/fem/io/file/vtkio.hh>
#include <dune/fem/io/file/datawriter.hh>

#include "logging.hh"
#include "misc.hh"
#include "parametercontainer.hh"
#include "customprojection.hh"
#include "printing.hh"

#include <dune/fem/misc/l2norm.hh>
#include <dune/fem/misc/l2error.hh>
#include <dune/stuff/functions.hh>
#include <boost/format.hpp>
#include <cmath>
#include <sstream>
#include <tuple>

//! Error and vtk output wrapper class for Stokes problem/pass
template <  class StokesPassImp, class ProblemImp >
class PostProcessor
{
    public:
        typedef ProblemImp
            ProblemType;

        typedef StokesPassImp
            StokesPassType;
		typedef typename StokesPassType::Traits::DiscreteStokesFunctionSpaceWrapperType
            DiscreteStokesFunctionSpaceWrapperType;
		typedef typename StokesPassType::Traits::DiscreteStokesFunctionWrapperType
            DiscreteStokesFunctionWrapperType;

        typedef typename ProblemType::VelocityType
            ContinuousVelocityType;
        typedef typename ProblemType::PressureType
            ContinuousPressureType;
        typedef typename ProblemType::ForceType
            ForceType;
        typedef typename ProblemType::DirichletDataType
            DirichletDataType;

		typedef typename StokesPassType::Traits::GridPartType
            GridPartType;
        typedef typename GridPartType::GridType
            GridType;

        typedef Dune::VTKIO<GridPartType>
            VTKWriterType;

		typedef typename StokesPassType::Traits::DiscreteVelocityFunctionType
            DiscreteVelocityFunctionType;
		typedef typename StokesPassType::Traits::DiscreteVelocityFunctionSpaceType
            DiscreteVelocityFunctionSpaceType;

		typedef typename StokesPassType::Traits::DiscretePressureFunctionType
            DiscretePressureFunctionType;
		typedef typename StokesPassType::Traits::DiscretePressureFunctionSpaceType
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
            current_refine_level_( std::numeric_limits<int>::min() ),
            l2_error_pressure_( - std::numeric_limits<double>::max() ),
            l2_error_velocity_( - std::numeric_limits<double>::max() ),
            vtkWriter_( wrapper.gridPart() ),
            datadir_( Parameters().getParam( "fem.io.datadir", std::string("data") ) + "/" )
        {
            Stuff::testCreateDirectory( datadir_ );
        }

        ~PostProcessor()
        {
        }

		/** \brief analytical data is L2 projected
			\todo only use Stuff::CustomProjection when really necessary
			**/
        void assembleExactSolution()
        {
			Stuff::CustomProjection::project( problem_.dirichletData(), discreteExactDirichlet_ );

            typedef Dune::L2Projection< double, double, ContinuousVelocityType, DiscreteVelocityFunctionType > ProjectionV;
                ProjectionV projectionV;
            projectionV( problem_.velocity(), discreteExactVelocity_ );

            typedef Dune::L2Projection< double, double, ForceType, DiscreteVelocityFunctionType > ProjectionF;
                ProjectionF projectionF;
            projectionF( problem_.force(), discreteExactForce_ );

            typedef Dune::L2Projection< double, double, ContinuousPressureType, DiscretePressureFunctionType > ProjectionP;
                ProjectionP projectionP;
            projectionP( problem_.pressure(), discreteExactPressure_ );
			if ( Parameters().getParam( "save_matrices", false ) ) {
				Stuff::Logging::MatlabLogStream& matlabLogStream = Logger().Matlab();
				Stuff::printDiscreteFunctionMatlabStyle( discreteExactVelocity_, "u_exakt", matlabLogStream );
				Stuff::printDiscreteFunctionMatlabStyle( discreteExactPressure_, "p_exakt", matlabLogStream );
			}
        }

		//! output function that 'knows' function output mode; assembles filename
        template <class Function>
        void vtk_write( const Function& f ) {
			if ( Function::FunctionSpaceType::DimRange > 1 ) {
                vtkWriter_.addVectorVertexData( f );
				vtkWriter_.addVectorCellData( f );
			}
			else {
                vtkWriter_.addVertexData( f );
				vtkWriter_.addCellData( f );
			}

            std::stringstream path;
            if ( Parameters().getParam( "per-run-output", false ) )
                path    << datadir_ << "/ref"
                        <<  current_refine_level_ << "_" << f.name();
            else
                path << datadir_ << "/" << f.name();

            vtkWriter_.write( path.str().c_str() );
            vtkWriter_.clear();
        }

		//! use this function if no reference (ie. coarser/finer) solution is available, or an analytical one is
        void save( const GridType& grid, const DiscreteStokesFunctionWrapperType& wrapper, int refine_level )
        {
            if ( ProblemType:: hasMeaningfulAnalyticalSolution ) {
                if ( !solutionAssembled_ || current_refine_level_ != refine_level ) //re-assemble solution if refine level has changed
                    assembleExactSolution();
                current_refine_level_ = refine_level;

				calcError( wrapper );
                vtk_write( discreteExactVelocity_ );
                vtk_write( discreteExactPressure_ );
                vtk_write( discreteExactForce_ );
                vtk_write( discreteExactDirichlet_ );
                vtk_write( errorFunc_pressure_ );
                vtk_write( errorFunc_velocity_ );
            }

            save_common( grid, wrapper, refine_level );
        }

		//! use this save in eoc runs with no analytical solution available
        void save( const GridType& grid, const DiscreteStokesFunctionWrapperType& wrapper, const DiscreteStokesFunctionWrapperType& reference, int refine_level )
        {
            current_refine_level_ = refine_level;
            calcError( wrapper, reference );
            vtk_write( discreteExactVelocity_ );
            vtk_write( discreteExactPressure_ );
            vtk_write( errorFunc_pressure_ );
            vtk_write( errorFunc_velocity_ );

            save_common( grid, wrapper, refine_level );
        }

		//! used by both PostProcessor::save modes, outputs solutions (in grape/vtk form), but no errors or analytical functions
        void save_common( const GridType& grid, const DiscreteStokesFunctionWrapperType& wrapper, int refine_level )
        {
            current_refine_level_ = refine_level;

            vtk_write( wrapper.discretePressure() );
            vtk_write( wrapper.discreteVelocity() );

            typedef Dune::tuple< const DiscreteVelocityFunctionType*, const DiscretePressureFunctionType* >
				IOTupleType;
			IOTupleType dataTup ( &wrapper.discreteVelocity(), &wrapper.discretePressure() );

            typedef Dune::DataWriter< GridType, IOTupleType >
                DataWriterType;
            DataWriterType datawriter ( grid, dataTup );
            datawriter.writeData( 0, "" );
#ifndef NLOG
			entityColoration();
#endif
        }

		void calcError( const DiscreteStokesFunctionWrapperType& wrapper )
		{
			calcError( wrapper.discretePressure() , wrapper.discreteVelocity() );
		}

		//! proxy function that is to be used if no analytical solutions are availble to calculate errors against
        void calcError( const DiscreteStokesFunctionWrapperType& computed, const DiscreteStokesFunctionWrapperType& reference )
        {
            discreteExactPressure_.assign( reference.discretePressure() );
            discreteExactVelocity_.assign( reference.discreteVelocity() );
			//set to to true so calcError call does not try to assemble exact solutions again
            solutionAssembled_ = true;
            calcError( computed.discretePressure(), computed.discreteVelocity() );
        }

		//! print and save L2 error(functions)
        void calcError( const DiscretePressureFunctionType& pressure, const DiscreteVelocityFunctionType& velocity )
        {
            if ( !solutionAssembled_ )
                assembleExactSolution();

            errorFunc_pressure_.assign( discreteExactPressure_ );
            errorFunc_pressure_ -= pressure;
            errorFunc_velocity_.assign( discreteExactVelocity_ );
            errorFunc_velocity_ -= velocity;

            Dune::L2Norm< GridPartType > l2_Error( gridPart_ );

            l2_error_pressure_ = l2_Error.norm( errorFunc_pressure_ );
            l2_error_velocity_ = l2_Error.norm( errorFunc_velocity_ );

			const double boundaryInt = Stuff::boundaryIntegral( problem_.dirichletData(), discreteExactVelocity_.space() );
			const double pressureMean = Stuff::integralAndVolume( pressure, pressure.space() ).first;
			const double exactPressureMean = Stuff::integralAndVolume( problem_.pressure(), discreteExactPressure_.space() ).first;

            Logger().Info().Resume();
            Logger().Info() << "L2-Error Pressure: " << std::setw(8) << l2_error_pressure_ << "\n"
                            << "L2-Error Velocity: " << std::setw(8) << l2_error_velocity_ << "\n"
							<< boost::format( "Pressure volume integral: %f (discrete), %f (exact)\n") % pressureMean % exactPressureMean
							<< boost::format( "g_D boundary integral: %f\n") % boundaryInt;
        }

		//! used to sore errors in runinfo structure (for eoc latex output)
        std::vector<double> getError()
        {
            std::vector<double> ret;
            ret.push_back( l2_error_velocity_ );
            ret.push_back( l2_error_pressure_ );
            return ret;
        }

		//! assign each entity it's 'id' int and save/(vtk)output it in a discrete function
        void entityColoration()
        {
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
                typename DiscretePressureFunctionType::LocalFunctionType
                    lf = cl.localFunction( entity );

                for ( int i = 0; i < lf.numDofs(); ++i ){
                    lf[i] = numberOfEntities;
                }
            }
            vtk_write( cl );
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
        int current_refine_level_;
        double l2_error_pressure_;
        double l2_error_velocity_;
        VTKWriterType vtkWriter_;
        std::string datadir_;
};

#undef vtk_write

#endif // end of postprocessing.hh
