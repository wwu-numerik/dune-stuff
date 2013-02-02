#ifndef DUNE_STUFF_FUNCTION_NONPARAMETRIC_EXPRESSION_HH
#define DUNE_STUFF_FUNCTION_NONPARAMETRIC_EXPRESSION_HH

#ifdef HAVE_CMAKE_CONFIG
  #include "cmake_config.h"
#else
  #include "config.h"
#endif // ifdef HAVE_CMAKE_CONFIG

#include <sstream>
#include <vector>

//#if HAVE_EIGEN
//  #include <Eigen/Core>
//#endif // HAVE_EIGEN

#include <dune/common/fvector.hh>
#include <dune/common/dynvector.hh>
#include <dune/common/exceptions.hh>
#include <dune/common/static_assert.hh>

//#ifdef HAVE_DUNE_FEM
//  #include <dune/fem/function/common/function.hh>
//  #include <dune/fem/space/common/functionspace.hh>
//#endif // HAVE_DUNE_FEM

#include <dune/stuff/common/parameter/tree.hh>
#include <dune/stuff/common/string.hh>
#include <dune/stuff/common/color.hh>

#include "expression/mathexpr.hh"
#include "../interface.hh"

namespace Dune {
namespace Stuff {
namespace Function {

/**
  \brief  Provides a function which evaluates a given mathematical expression at runtime.

          Given a mathematical expression as a string, a domain \f$ K_d^{m \geq 1} \f$ and a range \f$ K_r^{n \geq 1}
          \f$ this function represents the map
          \f{eqnarray}
            f:K_d^m \to K_r^n\\
            x = (x_1, \dots, x_m)' \mapsto (f_1(x), \dots f_n(x))',
          \f}
          where \f$ K_d \f$ is the DomainType and \f$ K_r \f$ is the RangeType, usually a power of \f$ \mathcal{R} \f$.
          The name of the variable as well as the \f$ n \f$ expressions of \f$f_1, \dots, f_n\f$ have to be given in a
          Dune::ParameterTree in the following form:
\code variable: x
expression.0: 2*x[0]
expression.1: sin(x[1])*x[0]\endcode
          There have to exist at least \f$n\f$ expressions; the entries of the variable are indexed by \f$[i]\f$ for
          \f$ 0 \leq i \leq m - 1 \f$.
 **/
template< class DomainFieldImp, int domainDim, class RangeFieldImp, int rangeDim >
class NonparametricExpression
  : public Interface< DomainFieldImp, domainDim, RangeFieldImp, rangeDim >
{
public:
  typedef Interface< DomainFieldImp, domainDim, RangeFieldImp, rangeDim >               BaseType;
  typedef NonparametricExpression< DomainFieldImp, domainDim, RangeFieldImp, rangeDim > ThisType;

  typedef typename BaseType::DomainFieldType  DomainFieldType;
  static const int                            dimDomain = BaseType::dimDomain;
  typedef typename BaseType::DomainType       DomainType;

  typedef typename BaseType::RangeFieldType   RangeFieldType;
  static const int                            dimRange = BaseType::dimRange;
  typedef typename BaseType::RangeType        RangeType;

  typedef typename BaseType::ParamType  ParamType;

  NonparametricExpression(const std::string _variable, const std::string _expression,
                          const int _order = -1, const std::string _name = "function.nonparametric.expression")
    : order_(_order)
    , name_(_name)
  {
    const std::vector< std::string > expressions(1, _expression);
    setup(_variable, expressions);
  } // NonparametricExpression(const std::string variable, const std::string expression)

  NonparametricExpression(const std::string _variable, const std::vector< std::string > _expressions,
                          const int _order = -1, const std::string _name = "function.nonparametric.expression")
    : order_(_order)
    , name_(_name)
  {
    setup(_variable, _expressions);
  } // NonparametricExpression(const std::string variable, const std::vector< std::string >& expressions)

  NonparametricExpression(const ThisType& _other)
    : order_(_other.order())
    , name_(_other.name())
  {
    setup(_other.variable(), _other.expression());
  } // NonparametricExpression(const ThisType& other)

  static ThisType createFromParamTree(const Dune::ParameterTree& _paramTree)
  {
    const Dune::Stuff::Common::ExtendedParameterTree paramTree(_paramTree);
    // get necessary
    const std::string _variable = paramTree.get< std::string >("variable");
    const std::vector< std::string > _expressions = paramTree.getVector< std::string >("expression", dimRange);
    // get optional
    const int _order = paramTree.get< int >("order", -1);
    const std::string _name = paramTree.get< std::string >("name", "function.nonparametric.expression");
    // create and return
    return ThisType(_variable, _expressions, _order, _name);
  } // static ThisType createFromParamTree(const Stuff::Common::ExtendedParameterTree& paramTree)

  ThisType& operator=(const ThisType& _other)
  {
    if (this != &_other) {
      cleanup();
      order_ = _other.order();
      name_ = _other.name();
      variable_ = "";
      variables_ = std::vector< std::string >();
      expressions_ = std::vector< std::string >();
      setup(_other.variable(), _other.expression());
    }
    return this;
  } // ThisType& operator=(const ThisType& other)

  ~NonparametricExpression()
  {
    cleanup();
  } // ~NonparametricExpression()

  virtual bool parametric() const
  {
    return false;
  }

  virtual int order() const
  {
    return order_;
  }

  virtual std::string name() const
  {
    return name_;
  }

  void report(const std::string _name = "function.nonparametric.expression",
              std::ostream& _stream = std::cout,
              const std::string& _prefix = "") const
  {
    const std::string tmp = _name + "(" + variable() + ") = ";
    _stream << _prefix << tmp;
    if (expression().size() == 1)
        _stream << expression()[0] << std::endl;
    else {
      _stream << "[ " << expression()[0] << ";" << std::endl;
      const std::string whitespace = Dune::Stuff::Common::whitespaceify(tmp + "[ ");
      for (unsigned int i = 1; i < expression().size() - 1; ++i)
        _stream << _prefix << whitespace << expression()[i] << ";" << std::endl;
      _stream << _prefix << whitespace << expression()[expression().size() -1] << " ]" << std::endl;
    }
  } // void report(const std::string, std::ostream&, const std::string&) const

  std::string variable() const
  {
    return variable_;
  }

  const std::vector< std::string >& expression() const
  {
    return expressions_;
  }

  virtual void evaluate(const DomainType& arg, RangeType& ret) const
  {
    // copy arg
    for (typename DomainType::size_type ii = 0; ii < arg.size(); ++ii)
      *(arg_[ii]) = arg[ii];
    // copy ret
    for (typename RangeType::size_type ii = 0; ii < ret.size(); ++ii)
      ret[ii] = op_[ii]->Val();
  }

  virtual void evaluate(const ParamType& arg, RangeType& ret) const
  {
    assert(arg.size() == dimDomain);
    // copy arg
    for (typename DomainType::size_type ii = 0; ii < arg.size(); ++ii)
      *(arg_[ii]) = arg[ii];
    // copy ret
    for (typename RangeType::size_type ii = 0; ii < ret.size(); ++ii)
      ret[ii] = op_[ii]->Val();
  }

  using BaseType::evaluate;

private:
  void setup(const std::string& _variable, const std::vector< std::string >& _expression)
  {
    dune_static_assert((dimDomain > 0), "Really?");
    dune_static_assert((dimRange > 0), "Really?");
    // set expressions
    if (_expression.size() < dimRange)
      DUNE_THROW(Dune::InvalidStateException,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " '_expression' too short (is " << _expression.size()
                 << ", should be " << dimRange << ")!");
    for (int ii = 0; ii < dimRange; ++ii)
      expressions_.push_back(_expression[ii]);
    // set variable (i.e. "x")
    variable_ = _variable;
    // fill variables (i.e. "x[0]", "x[1]", ...)
    for (int ii = 0; ii < dimDomain; ++ii) {
      std::stringstream variableStream;
      variableStream << variable_ << "[" << ii << "]";
      variables_.push_back(variableStream.str());
    }
    // create epressions
    for (unsigned int ii = 0; ii < dimDomain; ++ii) {
      arg_[ii] = new DomainFieldType(0.0);
      var_arg_[ii] = new RVar(variables_[ii].c_str(), arg_[ii]);
      vararray_[ii] = var_arg_[ii];
    }
    for (unsigned int ii = 0; ii < dimRange; ++ ii) {
      op_[ii] = new ROperation(expressions_[ii].c_str(), dimDomain, vararray_);
    }
  } // void setup(const std::string& variable, const std::vector< std::string >& expressions)

  void cleanup()
  {
    for (int ii = 0; ii < dimRange; ++ii) {
      delete op_[ii];
    }
    for (int ii = 0; ii < dimDomain; ++ii) {
      delete var_arg_[ii];
      delete arg_[ii];
    }
  } // void cleanup()

  const int order_;
  const std::string name_;
  std::string                variable_;
  std::vector< std::string > variables_;
  std::vector< std::string > expressions_;
  unsigned int actualDimRange_;
  mutable DomainFieldType* arg_[dimDomain];
  RVar* var_arg_[dimDomain];
  RVar* vararray_[dimDomain];
  ROperation* op_[dimRange];
}; // class NonparametricExpression


template< class ParamFieldImp, int maxParamDim, class RangeFieldImp >
class Coefficient
  : public NonparametricExpression< ParamFieldImp, maxParamDim, RangeFieldImp, 1 >
{
public:
  typedef NonparametricExpression< ParamFieldImp, maxParamDim, RangeFieldImp, 1 > BaseType;
  typedef Coefficient< ParamFieldImp, maxParamDim, RangeFieldImp >   ThisType;

  Coefficient(const std::string _variable, const std::string _expression,
              const int _order = -1, const std::string _name = "function.coefficient")
    : BaseType(_variable, _expression, _order, _name)
  {}

  Coefficient(const ThisType& _other)
    : BaseType(_other.variable(), _other.expression()[0], _other.order(), _other.name())
  {}

  ThisType& operator=(const ThisType& _other)
  {
    BaseType::operator=(_other);
    return this;
  } // ThisType& operator=(const ThisType& other)
};


} // namespace Function
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_FUNCTION_NONPARAMETRIC_EXPRESSION_HH
