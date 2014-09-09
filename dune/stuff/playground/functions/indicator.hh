// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_FUNCTIONS_INDICATOR_HH
#define DUNE_STUFF_FUNCTIONS_INDICATOR_HH

#include <memory>
#include <vector>
#include <utility>

#include <dune/common/static_assert.hh>

#include <dune/stuff/common/fvector.hh>

#include "../../functions/interfaces.hh"

namespace Dune {
namespace Stuff {
namespace Functions {


template< class E, class D, int d, class R, int r = 1, int rC = 1 >
class Indicator
{
  static_assert(AlwaysFalse< E >::value, "Not implemented for these dimensions!");
};


template< class E, class D, int d, class R >
class Indicator< E, D, d, R, 1 >
  : public LocalizableFunctionInterface< E, D, d, R, 1 >
{
  typedef LocalizableFunctionInterface< E, D, d, R, 1 > BaseType;
  typedef Indicator< E, D, d, R, 1 >                    ThisType;

  class Localfunction
    : public LocalfunctionInterface< E, D, d, R, 1 >
  {
    typedef LocalfunctionInterface< E, D, d, R, 1 > InterfaceType;
  public:
    using typename InterfaceType::EntityType;
    using typename InterfaceType::DomainType;
    using typename InterfaceType::RangeType;
    using typename InterfaceType::JacobianRangeType;

    Localfunction(const EntityType& entity, const RangeType& value)
      : InterfaceType(entity)
      , value_(value)
    {}

    virtual size_t order() const DS_OVERRIDE DS_FINAL
    {
      return 0;
    }

    virtual void evaluate(const DomainType& xx, RangeType& ret) const DS_OVERRIDE DS_FINAL
    {
      assert(this->is_a_valid_point(xx));
      ret = value_;
    }

    virtual void jacobian(const DomainType& xx, JacobianRangeType& ret) const DS_OVERRIDE DS_FINAL
    {
      assert(this->is_a_valid_point(xx));
      ret *= 0.0;
    }

  private:
    const RangeType value_;
  }; // class Localfunction

public:
  using typename BaseType::EntityType;
  using typename BaseType::DomainType;
  using typename BaseType::RangeType;
  using typename BaseType::LocalfunctionType;

  Indicator(std::vector< std::pair< std::pair< DomainType, DomainType >, R > > values,
            const std::string name = "indicator")
    : values_(values)
    , name_(name)
  {}

  Indicator(std::vector< std::pair< std::pair< Common::FieldVector< D, d >, Common::FieldVector< D, d > >, R > > values,
            const std::string name = "indicator")
    : values_(values)
    , name_(name)
  {}

  virtual ~Indicator() {}

  virtual ThisType* copy() const DS_OVERRIDE DS_FINAL
  {
    DUNE_THROW(NotImplemented, "");
  }

  virtual std::string name() const DS_OVERRIDE DS_FINAL
  {
    return name_;
  }

  virtual std::unique_ptr< LocalfunctionType > local_function(const EntityType& entity) const DS_OVERRIDE DS_FINAL
  {
    const auto center = entity.geometry().center();
    for (const auto& element : values_) {
      const auto& domain = element.first;
      const auto& lower_left = domain.first;
      const auto& upper_right = domain.second;
      if (Common::FloatCmp::le(lower_left, center) && Common::FloatCmp::lt(center, upper_right)) {
        const auto& value = element.second;
        return Common::make_unique< Localfunction >(entity, value);
      }
    }
    return Common::make_unique< Localfunction >(entity, 0.0);
  } // ... local_function(...)

private:
  const std::vector< std::pair< std::pair< DomainType, DomainType >, R > > values_;
  const std::string name_;
}; // class Indicator


} // namespace Functions
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_FUNCTIONS_INDICATOR_HH
