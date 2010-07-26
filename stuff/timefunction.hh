#ifndef TIMEFUNCTION_HH
#define TIMEFUNCTION_HH

#include <dune/fem/function/common/function.hh>

namespace Dune {
	template < class FunctionSpaceImp, class FunctionImp, class TimeProviderImp >
	class TimeFunction :
				public BartonNackmanInterface< TimeFunction < FunctionSpaceImp, FunctionImp, TimeProviderImp >,
									   FunctionImp >,
				public Function < FunctionSpaceImp , TimeFunction < FunctionSpaceImp, FunctionImp, TimeProviderImp > >
	{
		protected:
			typedef TimeFunction< FunctionSpaceImp, FunctionImp, TimeProviderImp >
				ThisType;
			typedef Function < FunctionSpaceImp , ThisType >
				BaseType;
			typedef BartonNackmanInterface< ThisType, FunctionImp >
				Interface;

			const TimeProviderImp& timeProvider_;

			using Interface::asImp;

		public:
			TimeFunction(const TimeProviderImp& timeProvider, const FunctionSpaceImp& space )
			  :BaseType( space ),
			  timeProvider_( timeProvider )
			{}

			~TimeFunction()
			{}

			virtual inline void evaluate( const typename BaseType::DomainType& x, typename BaseType::RangeType& ret ) const
			{
				const double t = timeProvider_.subTime();
				CHECK_AND_CALL_INTERFACE_IMPLEMENTATION( asImp().evaluateTime( t, x, ret ) );
			}

			/** \brief evaluate the Jacobian of the function
			 *
			 *  \param[in]  x    evaluation point
			 *  \param[out] ret  value of the Jacobian in x
			 */
			inline void jacobian ( const typename BaseType::DomainType &x,
								   typename BaseType::JacobianRangeType &ret ) const
			{
			  CHECK_AND_CALL_INTERFACE_IMPLEMENTATION( asImp().jacobian( x, ret ) );
			}

			/** \brief evaluate a derivative of the function
			 *
			 *  \param[in]  diffVariable  vector describing the partial derivative to
			 *                            evaluate
			 *  \param[in]  x             evaluation point
			 *  \param[out] ret           value of the derivative in x
			 */
			template< int diffOrder >
			inline void evaluate ( const FieldVector< deriType, diffOrder > &diffVariable,
								   const typename BaseType::DomainType &x,
								   typename BaseType::RangeType &ret ) const
			{
			  CHECK_AND_CALL_INTERFACE_IMPLEMENTATION
				( asImp().evaluate( diffVariable, x, ret ) );
			}

	};

}

#endif // TIMEFUNCTION_HH
