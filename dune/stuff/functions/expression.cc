// This file is part of the dune-stuff project:
//   http://users.dune-project.org/projects/dune-stuff/
// Copyright Holders: Felix Albrecht
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "expression.hh"

namespace Dune {
namespace Stuff {
namespace Function {


// ======================
// ===== Expression =====
// ======================
template< class E, class D, int d, class R, int r, int rC >
std::string Expression< E, D, d, R, r, rC >::static_id()
{
  return BaseType::static_id() + ".expression";
}

template< class E, class D, int d, class R, int r, int rC >
Expression< E, D, d, R, r, rC >::Expression(const std::string variable,
                                            const std::string expression,
                                            const size_t ord,
                                            const std::string nm)
  : function_(new MathExpressionFunctionType(variable, expression))
  , order_(ord)
  , name_(nm)
{}

template< class E, class D, int d, class R, int r, int rC >
Expression< E, D, d, R, r, rC >::Expression(const std::string variable,
                                            const std::vector< std::string > expressions,
                                            const size_t ord,
                                            const std::string nm)
  : function_(new MathExpressionFunctionType(variable, expressions))
  , order_(ord)
  , name_(nm)
{}

template< class E, class D, int d, class R, int r, int rC >
Expression< E, D, d, R, r, rC >::Expression(const ThisType& other)
  : function_(other.function_)
  , order_(other.order_)
  , name_(other.name_)
{}

template< class E, class D, int d, class R, int r, int rC >
    typename Expression< E, D, d, R, r, rC >::ThisType&
Expression< E, D, d, R, r, rC >::operator=(const ThisType& other)
{
  if (this != &other) {
    function_ = other.function_;
    order_ = other.order_;
    name_ = other.name_;
  }
  return *this;
}

template< class E, class D, int d, class R, int r, int rC >
std::string Expression< E, D, d, R, r, rC >::name() const
{
  return name_;
}

template< class E, class D, int d, class R, int r, int rC >
    std::unique_ptr< typename Expression< E, D, d, R, r, rC >::LocalfunctionType >
Expression< E, D, d, R, r, rC >::local_function(const EntityType& entity) const
{
  return std::unique_ptr< Localfunction >(new Localfunction(entity, function_, order_));
}


// ================================
// ===== Expression< ..., 1 > =====
// ================================
template< class E, class D, int d, class R, int r >
std::string Expression< E, D, d, R, r >::static_id()
{
  return BaseType::static_id() + ".expression";
}

template< class E, class D, int d, class R, int r >
Dune::ParameterTree Expression< E, D, d, R, r >::defaultSettings(const std::string subName)
{
  Dune::ParameterTree description;
  description["variable"] = "x";
  description["expression"] = "[x[0]; sin(x[0]); exp(x[0])]";
  description["order"] = "1";
  description["name"] = static_id();
  if (subName.empty())
    return description;
  else {
    Dune::Stuff::Common::ExtendedParameterTree extendedDescription;
    extendedDescription.add(description, subName);
    return extendedDescription;
  }
} // ... defaultSettings(...)

template< class E, class D, int d, class R, int r >
    typename Expression< E, D, d, R, r >::ThisType*
Expression< E, D, d, R, r >::create(const DSC::ExtendedParameterTree settings)
{
  // get necessary
  const std::string _variable = settings.get< std::string >("variable", "x");
  std::vector< std::string > _expressions;
  // lets see, if there is a key or vector
  if (settings.hasVector("expression")) {
    const std::vector< std::string > expr = settings.getVector< std::string >("expression", 1);
    for (size_t ii = 0; ii < expr.size(); ++ii)
      _expressions.push_back(expr[ii]);
  } else if (settings.hasKey("expression")) {
    const std::string expr = settings.get< std::string >("expression");
    _expressions.push_back(expr);
  } else
    DUNE_THROW(Dune::IOError,
               "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
               << " neither key nor vector 'expression' found in the following settings:\n"
               << settings.reportString("  "));
  // get optional
  const int order = settings.get< int >("order", -1);
  const std::string name = settings.get< std::string >("name", "function.expression");
  // create and return
  return new ThisType(_variable, _expressions, order, name);
} // ... create(...)

template< class E, class D, int d, class R, int r >
Expression< E, D, d, R, r >::Expression(const std::string variable,
                                        const std::string expression,
                                        const size_t ord,
                                        const std::string nm,
                                        const std::vector< std::vector< std::string > > gradient_expressions)
  : function_(new MathExpressionFunctionType(variable, expression))
  , order_(ord)
  , name_(nm)
{
  build_gradients(variable, gradient_expressions);
}

template< class E, class D, int d, class R, int r >
Expression< E, D, d, R, r >::Expression(const std::string variable,
                                        const std::vector< std::string > expressions,
                                        const size_t ord,
                                        const std::string nm,
                                        const std::vector< std::vector< std::string > > gradient_expressions)
  : function_(new MathExpressionFunctionType(variable, expressions))
  , order_(ord)
  , name_(nm)
{
  build_gradients(variable, gradient_expressions);
}

template< class E, class D, int d, class R, int r >
Expression< E, D, d, R, r >::Expression(const ThisType& other)
  : function_(other.function_)
  , order_(other.order_)
  , name_(other.name_)
  , gradients_(other.gradients_)
{}

template< class E, class D, int d, class R, int r >
    typename Expression< E, D, d, R, r >::ThisType&
Expression< E, D, d, R, r >::operator=(const ThisType& other)
{
  if (this != &other) {
    function_ = other.function_;
    order_ = other.order_;
    name_ = other.name_;
    gradients_ = other.gradients_;
  }
  return *this;
}

template< class E, class D, int d, class R, int r >
typename Expression< E, D, d, R, r >::ThisType* Expression< E, D, d, R, r >::copy() const
{
  return new ThisType(*this);
}

template< class E, class D, int d, class R, int r >
std::string Expression< E, D, d, R, r >::name() const
{
  return name_;
}

template< class E, class D, int d, class R, int r >
    std::unique_ptr< typename Expression< E, D, d, R, r >::LocalfunctionType >
Expression< E, D, d, R, r >::local_function(const EntityType& entity) const
{
  return std::unique_ptr< Localfunction >(new Localfunction(entity, function_, gradients_, order_));
}

template< class E, class D, int d, class R, int r >
void Expression< E, D, d, R, r >::build_gradients(const std::string variable,
                                                  const std::vector< std::vector< std::string > >& gradient_expressions)
{
  assert(gradient_expressions.size() == 0 || gradient_expressions.size() == dimRange);
  for (const auto& gradient_expression : gradient_expressions) {
    assert(gradient_expression.size() == dimDomain);
    gradients_.emplace_back(new MathExpressionGradientType(variable, gradient_expression));
  }
} // ... build_gradients(...)


} // namespace Function
} // namespace Stuff
} // namespace Dune

#define DSF_LIST_DIMRANGE(etype, ddim) \
  DSF_LIST_DIMRANGECOLS(Dune::Stuff::Function::Expression, etype, ddim, 1) \
  DSF_LIST_DIMRANGECOLS(Dune::Stuff::Function::Expression, etype, ddim, 2) \
  DSF_LIST_DIMRANGECOLS(Dune::Stuff::Function::Expression, etype, ddim, 3)

#define DSF_LIST_DIMRANGECOLS(cname, etype, ddim, rdim) \
  DSF_LIST_DOMAINFIELDTYPES(cname, etype, ddim, rdim, 2) \
  DSF_LIST_DOMAINFIELDTYPES(cname, etype, ddim, rdim, 3)

#define DSF_LIST_DOMAINFIELDTYPES(cname, etype, ddim, rdim, rcdim) \
  DSF_LIST_RANGEFIELDTYPES(cname, etype, double, ddim, rdim, rcdim)

#define DSF_LIST_RANGEFIELDTYPES(cname, etype, dftype, ddim, rdim, rcdim) \
  DSF_LAST_EXPANSION(cname, etype, dftype, ddim, double, rdim, rcdim) \
  DSF_LAST_EXPANSION(cname, etype, dftype, ddim, long double, rdim, rcdim)

#define DSF_LAST_EXPANSION(cname, etype, dftype, ddim, rftype, rdim, rcdim) \
  template class cname< etype, dftype, ddim, rftype, rdim, rcdim >;

#ifdef HAVE_DUNE_GRID

# include <dune/grid/sgrid.hh>

typedef typename Dune::SGrid< 1, 1 >::template Codim< 0 >::Entity DuneSGrid1dEntityType;
typedef typename Dune::SGrid< 2, 2 >::template Codim< 0 >::Entity DuneSGrid2dEntityType;
typedef typename Dune::SGrid< 3, 3 >::template Codim< 0 >::Entity DuneSGrid3dEntityType;

DSF_LIST_DIMRANGE(DuneSGrid1dEntityType, 1)
DSF_LIST_DIMRANGE(DuneSGrid2dEntityType, 2)
DSF_LIST_DIMRANGE(DuneSGrid3dEntityType, 3)

# include <dune/grid/yaspgrid.hh>

typedef typename Dune::YaspGrid< 1 >::template Codim< 0 >::Entity DuneYaspGrid1dEntityType;
typedef typename Dune::YaspGrid< 2 >::template Codim< 0 >::Entity DuneYaspGrid2dEntityType;
typedef typename Dune::YaspGrid< 3 >::template Codim< 0 >::Entity DuneYaspGrid3dEntityType;

DSF_LIST_DIMRANGE(DuneYaspGrid1dEntityType, 1)
DSF_LIST_DIMRANGE(DuneYaspGrid2dEntityType, 2)
DSF_LIST_DIMRANGE(DuneYaspGrid3dEntityType, 3)

# if HAVE_ALUGRID_SERIAL_H || HAVE_ALUGRID_PARALLEL_H
#   define ALUGRID_CONFORM 1
#   define ENABLE_ALUGRID 1

#   include <dune/grid/alugrid.hh>

typedef typename Dune::ALUSimplexGrid< 2, 2 >::template Codim< 0 >::Entity DuneAluSimplexGrid2dEntityType;
typedef typename Dune::ALUSimplexGrid< 3, 3 >::template Codim< 0 >::Entity DuneAluSimplexGrid3dEntityType;
typedef typename Dune::ALUCubeGrid< 3, 3 >::template Codim< 0 >::Entity DuneAluCubeGrid3dEntityType;

DSF_LIST_DIMRANGE(DuneAluSimplexGrid2dEntityType, 2)
DSF_LIST_DIMRANGE(DuneAluSimplexGrid3dEntityType, 3)
DSF_LIST_DIMRANGE(DuneAluCubeGrid3dEntityType, 3)

#   undef ENABLE_ALUGRID
#   undef ALUGRID_CONFORM

# endif // HAVE_ALUGRID_SERIAL_H || HAVE_ALUGRID_PARALLEL_H
#endif // HAVE_DUNE_GRID

#undef DSF_LAST_EXPANSION
#undef DSF_LIST_RANGEFIELDTYPES
#undef DSF_LIST_DOMAINFIELDTYPES
#undef DSF_LIST_DIMRANGECOLS
#undef DSF_LIST_DIMRANGE

#define DSF_LIST_DIMRANGE(etype, ddim) \
  DSF_LIST_DOMAINFIELDTYPES(Dune::Stuff::Function::Expression, etype, ddim, 1) \
  DSF_LIST_DOMAINFIELDTYPES(Dune::Stuff::Function::Expression, etype, ddim, 2) \
  DSF_LIST_DOMAINFIELDTYPES(Dune::Stuff::Function::Expression, etype, ddim, 3)

#define DSF_LIST_DOMAINFIELDTYPES(cname, etype, ddim, rdim) \
  DSF_LIST_RANGEFIELDTYPES(cname, etype, double, ddim, rdim)

#define DSF_LIST_RANGEFIELDTYPES(cname, etype, dftype, ddim, rdim) \
  DSF_LAST_EXPANSION(cname, etype, dftype, ddim, double, rdim) \
  DSF_LAST_EXPANSION(cname, etype, dftype, ddim, long double, rdim)

#define DSF_LAST_EXPANSION(cname, etype, dftype, ddim, rftype, rdim) \
  template class cname< etype, dftype, ddim, rftype, rdim >;


#ifdef HAVE_DUNE_GRID

# include <dune/grid/sgrid.hh>

typedef typename Dune::SGrid< 1, 1 >::template Codim< 0 >::Entity DuneSGrid1dEntityType;
typedef typename Dune::SGrid< 2, 2 >::template Codim< 0 >::Entity DuneSGrid2dEntityType;
typedef typename Dune::SGrid< 3, 3 >::template Codim< 0 >::Entity DuneSGrid3dEntityType;

DSF_LIST_DIMRANGE(DuneSGrid1dEntityType, 1)
DSF_LIST_DIMRANGE(DuneSGrid2dEntityType, 2)
DSF_LIST_DIMRANGE(DuneSGrid3dEntityType, 3)

# include <dune/grid/yaspgrid.hh>

typedef typename Dune::YaspGrid< 1 >::template Codim< 0 >::Entity DuneYaspGrid1dEntityType;
typedef typename Dune::YaspGrid< 2 >::template Codim< 0 >::Entity DuneYaspGrid2dEntityType;
typedef typename Dune::YaspGrid< 3 >::template Codim< 0 >::Entity DuneYaspGrid3dEntityType;

DSF_LIST_DIMRANGE(DuneYaspGrid1dEntityType, 1)
DSF_LIST_DIMRANGE(DuneYaspGrid2dEntityType, 2)
DSF_LIST_DIMRANGE(DuneYaspGrid3dEntityType, 3)

# if HAVE_ALUGRID_SERIAL_H || HAVE_ALUGRID_PARALLEL_H
#   define ALUGRID_CONFORM 1
#   define ENABLE_ALUGRID 1

#   include <dune/grid/alugrid.hh>

typedef typename Dune::ALUSimplexGrid< 2, 2 >::template Codim< 0 >::Entity DuneAluSimplexGrid2dEntityType;
typedef typename Dune::ALUSimplexGrid< 3, 3 >::template Codim< 0 >::Entity DuneAluSimplexGrid3dEntityType;
typedef typename Dune::ALUCubeGrid< 3, 3 >::template Codim< 0 >::Entity DuneAluCubeGrid3dEntityType;

DSF_LIST_DIMRANGE(DuneAluSimplexGrid2dEntityType, 2)
DSF_LIST_DIMRANGE(DuneAluSimplexGrid3dEntityType, 3)
DSF_LIST_DIMRANGE(DuneAluCubeGrid3dEntityType, 3)

# endif // HAVE_ALUGRID_SERIAL_H || HAVE_ALUGRID_PARALLEL_H
#endif // HAVE_DUNE_GRID
