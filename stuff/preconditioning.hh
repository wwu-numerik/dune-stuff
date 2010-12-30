#ifndef DUNE_STUFF_PRECONDITIONING_HH
#define DUNE_STUFF_PRECONDITIONING_HH

namespace Stuff {

template <class Operator, template < class T, class F> class Solver, class RangeDiscreteFunctionType>
class OperatorBasedPreconditioner {

	Operator& operator_;
	const typename RangeDiscreteFunctionType::FunctionSpaceType& range_space_;
	const bool right_preconditioning_;
	typedef Solver<RangeDiscreteFunctionType,Operator>
		SolverType;
	SolverType solver_;

public:
	OperatorBasedPreconditioner(Operator& op,
								const typename RangeDiscreteFunctionType::FunctionSpaceType& range_space,
								const bool right_preconditioning = false,
								double solver_accuracy = 1e-7 )
		: operator_(op),
		  range_space_(range_space),
		  right_preconditioning_(right_preconditioning),
		  solver_( operator_,
			  solver_accuracy /*rel limit*/,
			  solver_accuracy /*abs limit*/,
			  1 /*not working iteration limit*/,
			  false /*verbose*/)
	{}

	template <class VecType>
	void precondition( const VecType* tmp, VecType* dest ) const
	{
		multOEM(tmp,dest);
	}

	template <class VECtype>
	void multOEM(const VECtype *x, VECtype * ret) const
	{
//		RangeDiscreteFunctionType arg("arg", range_space_, x);
//		RangeDiscreteFunctionType dest("dest", range_space_);
//		solver_.apply(arg,dest);
//		for(int i=0;i<range_space_.size();++i)
//			ret[i] = dest.leakPointer()[i];
		operator_.multOEM(x,ret);
	}

	bool rightPrecondition() const
	{
		return right_preconditioning_;
	}
};

} //end namespace Stuff
#endif // DUNE_STUFF_PRECONDITIONING_HH
