#ifndef DUNE_STUFF_RUNTIMEFUNCTION_HH
#define DUNE_STUFF_RUNTIMEFUNCTION_HH

#include <dune/stuff/timefunction.hh>
#include <dune/stuff/parametercontainer.hh>
#include <dune/stuff/expressions/mathexpr.h>

namespace Stuff {

/** \brief A Dune::Function derived class that allows almost arbitrary function evaluations in a parameter file.
	\note Yes, this means no more recompiling to change your right-hand side, intial value or whatever functions.

	All you need to provide in your parameter file is for each function a block like this:
	<pre> NAME_{x,y,z}: expression</pre>
	where y,z are optional depending on the dimensionality of the parametrizing function space.
	You can use x,y,z,t in your expression and they'll be properly replaced with current function args when the function is evaluated.
	For list of available functions see: mathexpr.h
	**/
template < class FunctionSpaceImp >
		class RuntimeFunction : public Dune::Function < FunctionSpaceImp , RuntimeFunction< FunctionSpaceImp > >
{
protected:
	typedef Dune::Function < FunctionSpaceImp , RuntimeFunction< FunctionSpaceImp > >
		BaseType;
	typedef double
		FieldType;
	const std::string name_;
	//! these need to be mutable because the Function interface is not flexible enough to provide non-const evaluate
	mutable FieldType arg_x_,arg_y_,arg_z_;
	mutable double time_;

	//! after init these are only ever used again internally
	RVar var_arg_x_,var_arg_y_,var_arg_z_,var_time_;
	//! continious access to var_* pointers
	RVar* vararray_[4];
	//! the real meat, these store the parsed expressions and return calculated results via values stored in vararray_
	ROperation *op_x_,*op_y_,*op_z_;

public:
	typedef typename BaseType::DomainType
		DomainType;
	typedef typename BaseType::RangeType
		RangeType;

	RuntimeFunction( const std::string& name, const FunctionSpaceImp& space )
		: BaseType ( space ),
		name_( name ),
		arg_x_( FieldType(0) ),
		arg_y_( FieldType(0) ),
		arg_z_( FieldType(0) ),
		time_( 0 ),
		var_arg_x_( "x", &arg_x_ ),
		var_arg_y_( "y", &arg_y_ ),
		var_arg_z_( "z", &arg_z_ ),
		var_time_( "t", &time_ ),
		op_x_( 0 ),
		op_y_( 0 ),
		op_z_( 0 )
	{
		vararray_[0] = &var_arg_x_;
		vararray_[1] = &var_arg_y_;
		vararray_[2] = &var_arg_z_;
		vararray_[3] = &var_time_;
		op_x_ = new ROperation ( Parameters().getFunction("func")['x'], 4, vararray_ );
		op_y_ = new ROperation ( Parameters().getFunction("func")['y'], 4, vararray_ );
		op_z_ = new ROperation ( Parameters().getFunction("func")['z'], 4, vararray_ );
	}

	~RuntimeFunction()
	{
		delete op_z_;
		delete op_y_;
		delete op_x_;
	}

	//! this first assigns current arg to arg_* (and thereby to var_arg_*) and then returns values of expressions to ret
	inline void evaluate( const double time, const DomainType& arg, RangeType& ret ) const
	{
		arg_x_ = arg[0];
		if ( arg.dim() > 1 )
			arg_y_ = arg[1];
		if ( arg.dim() > 2 )
			arg_z_ = arg[2];
		time_ = time;
		ret[0] = op_x_->Val();
		if ( arg.dim() > 1 )
			ret[1] = op_y_->Val();
		if ( arg.dim() > 2 )
			ret[2] = op_z_->Val();
	}

	inline void evaluate( const DomainType& arg, RangeType& ret ) const {
		evaluate(0,arg,ret);
	}

};

} // namespace Stuff {

#endif // DUNE_STUFF_RUNTIMEFUNCTION_HH
