#ifndef DUNE_STUFF_ADAPTION_HH
#define DUNE_STUFF_ADAPTION_HH

#include <dune/stuff/restrict_prolong.hh>
#include <dune/fem/space/dgspace/dgadaptmanager.hh>
#include <dune/stuff/mixins.hh>

namespace Dune {
/**
 *  \brief encapsulates the adaption handling for our DiscreteStokesFunctionWrapper
 *	Each instance produces its own RestrictProlongPair of operators for one stokesFunctionWrapper, these are in turn managed
	in a static operator set which in turn is the real argument type for the shared Dune::AdaptationManager instance.
	\note this will horribly fail if you have FunctionWrappers of same type living on different grid instances (which could be remedied by keeping a grid instance indexed map of adaption managers instead).
 *
 **/
template < class DiscreteStokesFunctionWrapperImp >
class DiscreteStokesFunctionWrapperAdaptionManager : public Stuff::InstanceCounter< DiscreteStokesFunctionWrapperAdaptionManager<DiscreteStokesFunctionWrapperImp> >
{
	 protected:
		typedef Stuff::InstanceCounter< DiscreteStokesFunctionWrapperAdaptionManager<DiscreteStokesFunctionWrapperImp> >
			InstanceCounterType;
		typedef typename DiscreteStokesFunctionWrapperImp::GridType
			GridType;

		typedef Dune::RestrictProlongDefault< typename DiscreteStokesFunctionWrapperImp::DiscretePressureFunctionType >
			RestrictProlongPressureType;
		typedef Dune::AdaptationManager< GridType, RestrictProlongPressureType >
			PressureAdaptationManagerType;

		typedef Dune::RestrictProlongDefault< typename DiscreteStokesFunctionWrapperImp::DiscreteVelocityFunctionType >
			RestrictProlongVelocityType;
		typedef Dune::AdaptationManager< GridType, RestrictProlongVelocityType >
			VelocityAdaptationManagerType;

		typedef Dune::RestrictProlongPair<RestrictProlongVelocityType&, RestrictProlongPressureType& >
			RestrictProlongPairType;
		typedef RestrictProlongOperatorSet<RestrictProlongPairType>
			RestrictProlongOperatorSetType;

		typedef Dune::AdaptationManager< GridType, RestrictProlongOperatorSetType >
			AdaptationManagerType;

	public:
		DiscreteStokesFunctionWrapperAdaptionManager (  GridType& grid,
														DiscreteStokesFunctionWrapperImp& functionWrapper )
			:
			grid_( grid ),
			function_wrapper_( functionWrapper),
			rpVelocity_             ( functionWrapper.discreteVelocity() ),
			rpPressure_             ( functionWrapper.discretePressure() ),
			restrictPair_( rpVelocity_, rpPressure_ )
		{
			if ( InstanceCounterType::instanceCount() == 1 )
			{
				combined_adaptManager_ = new AdaptationManagerType( grid, restrictOperator_Set_ );
			}
			assert( combined_adaptManager_ );
			restrictOperator_Set_.add( &restrictPair_ );
		}

		~DiscreteStokesFunctionWrapperAdaptionManager()
		{
			restrictOperator_Set_.remove( &restrictPair_ );
			if ( InstanceCounterType::instanceCount() == 1 )
			{
				delete combined_adaptManager_;
				combined_adaptManager_ = 0;
			}
		}

		void adapt()
		{
			assert( combined_adaptManager_ );
			combined_adaptManager_->adapt();
		}

		DiscreteStokesFunctionWrapperAdaptionManager( DiscreteStokesFunctionWrapperAdaptionManager& other )
			:
			grid_( other.grid_ ),
			function_wrapper_( other.function_wrapper_ ),
			rpVelocity_             ( function_wrapper_.discreteVelocity() ),
			rpPressure_             ( function_wrapper_.discretePressure() ),
			restrictPair_( rpVelocity_, rpPressure_ )
		{

		}

	protected:
		GridType& grid_;
		DiscreteStokesFunctionWrapperImp& function_wrapper_;
		RestrictProlongVelocityType rpVelocity_;
		RestrictProlongPressureType rpPressure_;
		RestrictProlongPairType restrictPair_;
		static RestrictProlongOperatorSetType restrictOperator_Set_;
		static AdaptationManagerType* combined_adaptManager_;
 };
template <class T>
	typename DiscreteStokesFunctionWrapperAdaptionManager<T>::RestrictProlongOperatorSetType
		DiscreteStokesFunctionWrapperAdaptionManager<T>::restrictOperator_Set_;

template <class T>
	typename DiscreteStokesFunctionWrapperAdaptionManager<T>::AdaptationManagerType*
		DiscreteStokesFunctionWrapperAdaptionManager<T>::combined_adaptManager_ = 0;

} // end namespace Dune
#endif // DUNE_STUFF_ADAPTION_HH
