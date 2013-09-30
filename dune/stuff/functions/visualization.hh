#ifndef DUNE_STUFF_FUNCTIONS_VISUALIZATION_HH
#define DUNE_STUFF_FUNCTIONS_VISUALIZATION_HH

#include <dune/grid/io/file/vtk/function.hh>
#include <dune/grid/io/file/vtk/vtkwriter.hh>

#include "interfaces.hh"

namespace Dune {
namespace Stuff {
namespace Function {


template< class GridViewType, int dimRange >
class VisualizationAdapter
  : public VTKFunction< GridViewType >
{
public:
  typedef typename GridViewType::template Codim< 0 >::Entity EntityType;

  typedef typename GridViewType::ctype              DomainFieldType;
  static const unsigned int                         dimDomain = GridViewType::dimension;
  typedef FieldVector< DomainFieldType, dimDomain > DomainType;

  typedef LocalizableFunctionInterface< EntityType, DomainFieldType, dimDomain, double, dimRange > FunctionType;

  VisualizationAdapter(const FunctionType& function)
    : function_(function)
    , tmp_value_(0)
  {}

  virtual int ncomps () const override
  {
    return dimRange;
  }

  virtual std::string name() const override
  {
    return function_.name();
  }

  virtual double evaluate (int comp, const EntityType& en, const DomainType& xx) const override
  {
    assert(comp >= 0);
    assert(comp < dimRange);
    const auto local_func = function_.local_function(en);
    local_func->evaluate(xx, tmp_value_);
    return tmp_value_[comp];
  }

private:
  const FunctionType& function_;
  mutable FieldVector< double, dimRange > tmp_value_;
}; // class VisualizationAdapter


} // namespace Function
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_FUNCTIONS_VISUALIZATION_HH
