#ifndef DUNE_STUFF_COMMON_SEPARABLE_CONTAINER_HH
#define DUNE_STUFF_COMMON_SEPARABLE_CONTAINER_HH

#include <vector>

#include <dune/common/exceptions.hh>
#include <dune/common/shared_ptr.hh>

namespace Dune {
namespace Stuff {
namespace Common {


template< class ComponentImp, class CoefficientImp, class size_t = std::size_t >
class SeparableContainer
{
public:
  typedef ComponentImp ComponentType;

  typedef CoefficientImp CoefficientType;

  typedef size_t size_type;

  SeparableContainer(const size_type _paramSize,
                     const std::vector< Dune::shared_ptr< const ComponentType > > _components,
                     const std::vector< Dune::shared_ptr< const CoefficientType > > _coefficients)
    : paramSize_(_paramSize)
    , components_(_components)
    , coefficients_(_coefficients)
  {
    // sanity checks
    if (components_.size() < 1)
      DUNE_THROW(Dune::RangeError,
                 "\nERROR: not enough '_components' given!");
    if (!(coefficients_.size() == components_.size()
          || coefficients_.size() == (components_.size() - 1)))
      DUNE_THROW(Dune::RangeError,
                 "\nERROR: wrong number of 'coefficients_' given!");
    if (coefficients_.size() == 0) {
      if (paramSize_ > 0)
        DUNE_THROW(Dune::RangeError,
                   "\nERROR: '_paramSize' has to be zero!");
    } else {
      if (paramSize_ < 1)
        DUNE_THROW(Dune::RangeError,
                   "\nERROR: '_paramSize' has to be positive!");
    }
  }

  SeparableContainer(const Dune::shared_ptr< const ComponentType > _component)
    : paramSize_(0)
  {
    components_.push_back(_component);
  }

  bool parametric() const
  {
    return numCoefficients() > 0;
  }

  const size_type paramSize() const
  {
    return paramSize_;
  }

  const size_type numComponents() const
  {
    return components_.size();
  }

  const std::vector< Dune::shared_ptr< const ComponentType > >& components() const
  {
    return components_;
  }

  const size_type numCoefficients() const
  {
    return coefficients_.size();
  }

  const std::vector< Dune::shared_ptr< const CoefficientType > >& coefficients() const
  {
    return coefficients_;
  }

private:
  const size_type paramSize_;
  std::vector< Dune::shared_ptr< const ComponentType > > components_;
  const std::vector< Dune::shared_ptr< const CoefficientType > > coefficients_;
}; // class SeparableContainer


} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_SEPARABLE_CONTAINER_HH
