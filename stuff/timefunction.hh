#ifndef STUFF_TIMEFUNCTION_HH
#define STUFF_TIMEFUNCTION_HH

#include <dune/fem/function/common/function.hh>
#include <dune/common/bartonnackmanifcheck.hh>

namespace Dune {
    struct ConstTimeProvider {
        const double time_;
        ConstTimeProvider( double time = 0.0 )
            : time_(time){}
        double subTime() const { return time_; }
    };

	/** \brief a Base class for Dune::Fem::Function compatible classes that need automatic time awareness via TimeProvider
	  *
	  **/
    template < class FunctionSpaceImp, class FunctionImp, class TimeProviderImp >
	class TimeFunction :
				public BartonNackmanInterface< TimeFunction < FunctionSpaceImp, FunctionImp, TimeProviderImp >,
									   FunctionImp >,
                public Fem::Function < FunctionSpaceImp , TimeFunction < FunctionSpaceImp, FunctionImp, TimeProviderImp > >
	{
		protected:
			typedef TimeFunction< FunctionSpaceImp, FunctionImp, TimeProviderImp >
				ThisType;
            typedef Fem::Function < FunctionSpaceImp , ThisType >
				BaseType;
			typedef BartonNackmanInterface< ThisType, FunctionImp >
				Interface;

            TimeProviderImp* timeProviderPtr_;//in case we cosntruct our own
            const TimeProviderImp& timeProvider_;

			using Interface::asImp;
			static const int dim_ = FunctionSpaceImp::dimDomain ;

		public:
			typedef TimeProviderImp
				TimeProviderType;

            TimeFunction(const TimeProviderImp& timeProvider, const FunctionSpaceImp& /*space*/ )
                : BaseType(),
                timeProviderPtr_( nullptr ),
                timeProvider_( timeProvider )
			{}
            TimeFunction(const TimeProviderImp& timeProvider )
                : BaseType(),
                timeProviderPtr_( nullptr ),
                timeProvider_( timeProvider )
            {}
            explicit TimeFunction(double t)
                : BaseType(),
                timeProviderPtr_( new TimeProviderType( t ) ),
                timeProvider_( *timeProviderPtr_ )
            {}

            virtual ~TimeFunction()
            {
                delete timeProviderPtr_;
            }

			inline void evaluate( const typename BaseType::DomainType& x, typename BaseType::RangeType& ret ) const
			{
				const double t = timeProvider_.subTime();
				CHECK_AND_CALL_INTERFACE_IMPLEMENTATION( asImp().evaluateTime( t, x, ret ) );
			}

			inline void evaluate( double time, const typename BaseType::DomainType& x, typename BaseType::RangeType& ret ) const
			{
				CHECK_AND_CALL_INTERFACE_IMPLEMENTATION( asImp().evaluateTime( time, x, ret ) );
			}

			/** \brief evaluate the Jacobian of the function
			 *
			 *  \param[in]  x    evaluation point
			 *  \param[out] ret  value of the Jacobian in x
			 */
			void jacobian ( const typename BaseType::DomainType &x,
								   typename BaseType::JacobianRangeType &ret ) const
			{
				const double t = timeProvider_.subTime();
                CHECK_AND_CALL_INTERFACE_IMPLEMENTATION( asImp().jacobianTime( t, x, ret ) );
			}

			/** \brief evaluate a derivative of the function
			 *
			 *  \param[in]  diffVariable  vector describing the partial derivative to
			 *                            evaluate
			 *  \param[in]  x             evaluation point
			 *  \param[out] ret           value of the derivative in x
			 */
			template< int diffOrder >
            inline void evaluate ( const FieldVector< int, diffOrder > &diffVariable,
								   const typename BaseType::DomainType &x,
								   typename BaseType::RangeType &ret ) const
			{
			  CHECK_AND_CALL_INTERFACE_IMPLEMENTATION
				( asImp().evaluate( diffVariable, x, ret ) );
			}

			const TimeProviderType& timeProvider() const
			{
				return timeProvider_;
			}

	};

	/** \brief basically the same as TimeFunction, but with an evaluate signature that additionally gets an IntersectionIterator
	  *
	  **/
	template < class FunctionSpaceImp, class FunctionImp, class TimeProviderImp >
	class IntersectionTimeFunction :
				public BartonNackmanInterface< IntersectionTimeFunction < FunctionSpaceImp, FunctionImp, TimeProviderImp >,
									   FunctionImp >,
                public Fem::Function < FunctionSpaceImp , IntersectionTimeFunction < FunctionSpaceImp, FunctionImp, TimeProviderImp > >
	{
		protected:
			typedef IntersectionTimeFunction< FunctionSpaceImp, FunctionImp, TimeProviderImp >
				ThisType;
            typedef Fem::Function < FunctionSpaceImp , ThisType >
				BaseType;
			typedef BartonNackmanInterface< ThisType, FunctionImp >
				Interface;

			const TimeProviderImp& timeProvider_;

			using Interface::asImp;
			static const int dim_ = FunctionSpaceImp::dimDomain ;

		public:
			typedef TimeProviderImp
				TimeProviderType;

            IntersectionTimeFunction(const TimeProviderImp& timeProvider, const FunctionSpaceImp& /*space*/ )
              :BaseType(),
			  timeProvider_( timeProvider )
			{}

			~IntersectionTimeFunction()
			{}

			template < class IntersectionType >
			inline void evaluate( const typename BaseType::DomainType& x,
								  typename BaseType::RangeType& ret,
								  const IntersectionType& intersection ) const
			{
				const double t = timeProvider_.subTime();
				CHECK_AND_CALL_INTERFACE_IMPLEMENTATION( asImp().evaluateTime( t, x, ret, intersection ) );
			}

			template < class IntersectionType >
			inline void evaluate( double time,
								  const typename BaseType::DomainType& x,
								  typename BaseType::RangeType& ret,
								  const IntersectionType& intersection ) const
			{
				CHECK_AND_CALL_INTERFACE_IMPLEMENTATION( asImp().evaluateTime( time, x, ret, intersection ) );
			}

			inline void evaluate( const typename BaseType::DomainType& /*x*/, typename BaseType::RangeType& /*ret*/ ) const
			{
				DUNE_THROW( Dune::NotImplemented, "__FUNCTION__ only present for interface compat" );
			}

			inline void evaluate( double time,
								  const typename BaseType::DomainType& x,
								  typename BaseType::RangeType& ret ) const
			{
				CHECK_AND_CALL_INTERFACE_IMPLEMENTATION( asImp().evaluateTime( time, x, ret ) );
			}


			/** \brief evaluate the Jacobian of the function
			 *
			 *  \param[in]  x    evaluation point
			 *  \param[out] ret  value of the Jacobian in x
			 */
			inline void jacobian ( const typename BaseType::DomainType& /*x*/,
								   typename BaseType::JacobianRangeType& /*ret*/ ) const
			{
				DUNE_THROW( Dune::NotImplemented, "__CLASS__ needs to be extended" );
			}

			/** \brief evaluate a derivative of the function
			 *
			 *  \param[in]  diffVariable  vector describing the partial derivative to
			 *                            evaluate
			 *  \param[in]  x             evaluation point
			 *  \param[out] ret           value of the derivative in x
			 */
			template< int diffOrder >
            inline void evaluate ( const FieldVector< int, diffOrder >& /*diffVariable*/,
								   const typename BaseType::DomainType& /*x*/,
								   typename BaseType::RangeType& /*ret*/ ) const
			{
				DUNE_THROW( Dune::NotImplemented, "__CLASS__ needs to be extended" );
			}

			const TimeProviderType& timeProvider() const
			{
				return timeProvider_;
			}

	};

}

#endif // STUFF_TIMEFUNCTION_HH
