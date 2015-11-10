// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff/
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_FUNCTIONS_ESV2007_HH
#define DUNE_STUFF_FUNCTIONS_ESV2007_HH

#include <cmath>

#include <type_traits>

#include <dune/geometry/referenceelements.hh>

#include <dune/stuff/common/configuration.hh>
#include <dune/stuff/common/debug.hh>
#include <dune/stuff/common/ranges.hh>
#include <dune/stuff/la/container/eigen.hh>

#include "interfaces.hh"

namespace Dune {
namespace Stuff {
namespace Functions {
namespace ESV2007 {

template <class E, class D, size_t d, class R, size_t r, size_t rC = 1>
class Testcase1Force : public LocalizableFunctionInterface<E, D, d, R, r, rC>
{
  Testcase1Force() { static_assert(AlwaysFalse<E>::value, "Not available for these dimensions!"); }
};

template <class EntityImp, class DomainFieldImp, class RangeFieldImp>
class Testcase1Force<EntityImp, DomainFieldImp, 2, RangeFieldImp, 1, 1>
    : public GlobalFunctionInterface<EntityImp, DomainFieldImp, 2, RangeFieldImp, 1>
{
  typedef Testcase1Force<EntityImp, DomainFieldImp, 2, RangeFieldImp, 1, 1> ThisType;
  typedef GlobalFunctionInterface<EntityImp, DomainFieldImp, 2, RangeFieldImp, 1, 1> BaseType;

public:
  using typename BaseType::DomainFieldType;
  using typename BaseType::DomainType;
  using typename BaseType::RangeType;
  using typename BaseType::JacobianRangeType;

  static const bool available = true;

  static std::string static_id() { return BaseType::static_id() + ".ESV2007.testcase1.force"; }

  static Common::Configuration default_config(const std::string sub_name = "")
  {
    Common::Configuration config;
    config["integration_order"] = "3";
    config["name"] = static_id();
    if (sub_name.empty())
      return config;
    else {
      Common::Configuration tmp;
      tmp.add(config, sub_name);
      return tmp;
    }
  } // ... default_config(...)

  static std::unique_ptr<ThisType> create(const Common::Configuration config = default_config(),
                                          const std::string sub_name = static_id())
  {
    // get correct config
    const Common::Configuration cfg         = config.has_sub(sub_name) ? config.sub(sub_name) : config;
    const Common::Configuration default_cfg = default_config();
    // create
    return Common::make_unique<ThisType>(cfg.get("integration_order", default_cfg.get<size_t>("integration_order")),
                                         cfg.get("name", default_cfg.get<std::string>("name")));
  } // ... create(...)

  Testcase1Force(const size_t ord = default_config().get<size_t>("integration_order"),
                 const std::string nm = static_id())
    : order_(ord), name_(nm)
  {
  }

  Testcase1Force(const ThisType& /*other*/) = default;

  ThisType& operator=(const ThisType& /*other*/) = delete;

  virtual std::string type() const override final { return BaseType::static_id() + ".ESV2007.testcase1.force"; }

  virtual std::string name() const override final { return name_; }

  virtual size_t order() const override final { return order_; }

  /**
   * \brief "0.5 * pi * pi * cos(0.5 * pi * x[0]) * cos(0.5 * pi * x[1])"
   */
  virtual void evaluate(const DomainType& xx, RangeType& ret) const override final
  {
    ret[0] = M_PI_2l * M_PIl * cos(M_PI_2l * xx[0]) * cos(M_PI_2l * xx[1]);
  }

  /**
   * \brief ["-0.25 * pi * pi * pi * sin(0.5 * pi * x[0]) * cos(0.5 * pi * x[1])"
   *         "-0.25 * pi * pi * pi * cos(0.5 * pi * x[0]) * sin(0.5 * pi * x[1])"]
   */
  virtual void jacobian(const DomainType& xx, JacobianRangeType& ret) const override final
  {
    const DomainFieldType pre   = -0.25 * M_PIl * M_PIl * M_PIl;
    const DomainFieldType x_arg = M_PI_2l * xx[0];
    const DomainFieldType y_arg = M_PI_2l * xx[1];
    ret[0][0]                   = pre * sin(x_arg) * cos(y_arg);
    ret[0][1]                   = pre * cos(x_arg) * sin(y_arg);
  } // ... jacobian(...)

private:
  const size_t order_;
  const std::string name_;
}; // class Testcase1Force

template <class E, class D, size_t d, class R, size_t r, size_t rC = 1>
class Testcase1ExactSolution : public LocalizableFunctionInterface<E, D, d, R, r, rC>
{
  Testcase1ExactSolution() { static_assert(AlwaysFalse<E>::value, "Not available for these dimensions!"); }
};

template <class EntityImp, class DomainFieldImp, class RangeFieldImp>
class Testcase1ExactSolution<EntityImp, DomainFieldImp, 2, RangeFieldImp, 1, 1>
    : public GlobalFunctionInterface<EntityImp, DomainFieldImp, 2, RangeFieldImp, 1, 1>
{
  typedef Testcase1ExactSolution<EntityImp, DomainFieldImp, 2, RangeFieldImp, 1, 1> ThisType;
  typedef GlobalFunctionInterface<EntityImp, DomainFieldImp, 2, RangeFieldImp, 1, 1> BaseType;

public:
  using typename BaseType::DomainFieldType;
  using typename BaseType::DomainType;
  using typename BaseType::RangeType;
  using typename BaseType::JacobianRangeType;

  static const bool available = true;

  static std::string static_id() { return BaseType::static_id() + ".ESV2007.testcase1.exactsolution"; }

  static Common::Configuration default_config(const std::string sub_name = "")
  {
    Common::Configuration config;
    config["integration_order"] = "3";
    config["name"] = static_id();
    if (sub_name.empty())
      return config;
    else {
      Common::Configuration tmp;
      tmp.add(config, sub_name);
      return tmp;
    }
  } // ... default_config(...)

  static std::unique_ptr<ThisType> create(const Common::Configuration config = default_config(),
                                          const std::string sub_name = static_id())
  {
    // get correct config
    const Common::Configuration cfg         = config.has_sub(sub_name) ? config.sub(sub_name) : config;
    const Common::Configuration default_cfg = default_config();
    // create
    return Common::make_unique<ThisType>(cfg.get("integration_order", default_cfg.get<size_t>("integration_order")),
                                         cfg.get("name", default_cfg.get<std::string>("name")));
  } // ... create(...)

  Testcase1ExactSolution(const size_t ord = default_config().get<size_t>("integration_order"),
                         const std::string nm = static_id())
    : order_(ord), name_(nm)
  {
  }

  Testcase1ExactSolution(const ThisType& /*other*/) = default;

  ThisType& operator=(const ThisType& /*other*/) = delete;

  virtual std::string type() const override final { return BaseType::static_id() + ".ESV2007.testcase1.exactsolution"; }

  virtual std::string name() const override final { return name_; }

  virtual size_t order() const override final { return order_; }

  /**
   * \brief "cos(0.5 * pi * x[0]) * cos(0.5 * pi * x[1])"
   */
  virtual void evaluate(const DomainType& xx, RangeType& ret) const override final
  {
    ret[0] = cos(M_PI_2l * xx[0]) * cos(M_PI_2l * xx[1]);
  }

  /**
   * \brief ["-0.5 * pi * sin(0.5 * pi * x[0]) * cos(0.5 * pi * x[1])"
   *         "-0.5 * pi * cos(0.5 * pi * x[0]) * sin(0.5 * pi * x[1])"]
   */
  virtual void jacobian(const DomainType& xx, JacobianRangeType& ret) const override final
  {
    const DomainFieldType pre   = -0.5 * M_PIl;
    const DomainFieldType x_arg = M_PI_2l * xx[0];
    const DomainFieldType y_arg = M_PI_2l * xx[1];
    ret[0][0]                   = pre * sin(x_arg) * cos(y_arg);
    ret[0][1]                   = pre * cos(x_arg) * sin(y_arg);
  } // ... jacobian(...)

private:
  const size_t order_;
  const std::string name_;
}; // class Testcase1ExactSolution

template <class DiffusionFactorType, class DiffusionTensorType = void>
class Cutoff;

template <class DiffusionType>
class Cutoff<DiffusionType, void>
    : public LocalizableFunctionInterface<typename DiffusionType::EntityType, typename DiffusionType::DomainFieldType,
                                          DiffusionType::dimDomain, typename DiffusionType::RangeFieldType, 1, 1>
{
  static_assert(std::is_base_of<Tags::LocalizableFunction, DiffusionType>::value,
                "DiffusionType has to be tagged as a LocalizableFunction!");
  typedef typename DiffusionType::EntityType E_;
  typedef typename DiffusionType::DomainFieldType D_;
  static const size_t d_ = DiffusionType::dimDomain;
  typedef typename DiffusionType::RangeFieldType R_;
  typedef LocalizableFunctionInterface<E_, D_, d_, R_, 1> BaseType;
  typedef Cutoff<DiffusionType> ThisType;

  class Localfunction : public LocalfunctionInterface<E_, D_, d_, R_, 1, 1>
  {
    typedef LocalfunctionInterface<E_, D_, d_, R_, 1, 1> BaseType;

  public:
    typedef typename BaseType::EntityType EntityType;

    typedef typename BaseType::DomainFieldType DomainFieldType;
    static const size_t dimDomain = BaseType::dimDomain;
    typedef typename BaseType::DomainType DomainType;

    typedef typename BaseType::RangeFieldType RangeFieldType;
    static const size_t dimRange     = BaseType::dimRange;
    static const size_t dimRangeCols = BaseType::dimRangeCols;
    typedef typename BaseType::RangeType RangeType;

    typedef typename BaseType::JacobianRangeType JacobianRangeType;

  private:
    template <class D, int r, int rR>
    struct Compute
    {
      static_assert(AlwaysFalse<D>::value, "Not implemented for these dimensions!");
    };

    template <class D>
    struct Compute<D, 1, 1>
    {
      static RangeFieldType min_eigenvalue_of(const D& diffusion, const EntityType& ent)
      {
        const auto local_diffusion = diffusion.local_function(ent);
        assert(local_diffusion->order() == 0);
        const auto& reference_element = ReferenceElements<DomainFieldType, dimDomain>::general(ent.type());
        return local_diffusion->evaluate(reference_element.position(0, 0))[0];
      } // ... min_eigenvalue_of_(...)
    };  // class Compute< ..., 1, 1 >

  public:
    Localfunction(const EntityType& ent, const DiffusionType& diffusion, const RangeFieldType poincare_constant)
      : BaseType(ent), value_(0)
    {
      const RangeFieldType min_eigen_value =
          Compute<DiffusionType, DiffusionType::dimRange, DiffusionType::dimRangeCols>::min_eigenvalue_of(diffusion,
                                                                                                          ent);
      assert(min_eigen_value > 0.0);
      const DomainFieldType hh = compute_diameter_of_(ent);
      value_                   = (poincare_constant * hh * hh) / min_eigen_value;
    }

    Localfunction(const Localfunction& /*other*/) = delete;

    Localfunction& operator=(const Localfunction& /*other*/) = delete;

    virtual size_t order() const override final { return 0; }

    virtual void evaluate(const DomainType& UNUSED_UNLESS_DEBUG(xx), RangeType& ret) const override final
    {
      assert(this->is_a_valid_point(xx));
      ret[0] = value_;
    }

    virtual void jacobian(const DomainType& UNUSED_UNLESS_DEBUG(xx), JacobianRangeType& ret) const override final
    {
      assert(this->is_a_valid_point(xx));
      ret *= RangeFieldType(0);
    }

  private:
    static DomainFieldType compute_diameter_of_(const EntityType& ent)
    {
      DomainFieldType ret(0);
      for (auto cc : DSC::valueRange(ent.template count<dimDomain>())) {
        const auto vertex = ent.template subEntity<dimDomain>(cc)->geometry().center();
        for (auto dd : DSC::valueRange(cc + 1, ent.template count<dimDomain>())) {
          const auto other_vertex = ent.template subEntity<dimDomain>(dd)->geometry().center();
          const auto diff         = vertex - other_vertex;
          ret                     = std::max(ret, diff.two_norm());
        }
      }
      return ret;
    } // ... compute_diameter_of_(...)

    RangeFieldType value_;
  }; // class Localfunction

public:
  typedef typename BaseType::EntityType EntityType;
  typedef typename BaseType::LocalfunctionType LocalfunctionType;
  typedef typename BaseType::RangeFieldType RangeFieldType;

  static std::string static_id() { return BaseType::static_id() + ".ESV2007.cutoff"; }

  Cutoff(const DiffusionType& diffusion, const RangeFieldType poincare_constant = 1.0 / (M_PIl * M_PIl),
         const std::string nm = static_id())
    : diffusion_(diffusion), poincare_constant_(poincare_constant), name_(nm)
  {
  }

  Cutoff(const ThisType& other) = default;

  ThisType& operator=(const ThisType& other) = delete;

  virtual std::string name() const override final { return name_; }

  virtual std::unique_ptr<LocalfunctionType> local_function(const EntityType& entity) const override final
  {
    return std::unique_ptr<Localfunction>(new Localfunction(entity, diffusion_, poincare_constant_));
  }

private:
  const DiffusionType& diffusion_;
  const RangeFieldType poincare_constant_;
  std::string name_;
}; // class Cutoff

} // namespace ESV2007
} // namespace Functions
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_FUNCTIONS_ESV2007_HH
