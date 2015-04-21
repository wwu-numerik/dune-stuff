// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff/
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_PLAYGROUND_FUNCTIONS_ESV2007_HH
#define DUNE_STUFF_PLAYGROUND_FUNCTIONS_ESV2007_HH

#include <limits>

#include <boost/numeric/conversion/cast.hpp>

#if HAVE_EIGEN

# include <Eigen/Eigenvalues>

# include <dune/geometry/quadraturerules.hh>

# include <dune/stuff/functions/ESV2007.hh>
# include <dune/stuff/common/debug.hh>
# include <dune/stuff/common/ranges.hh>

namespace Dune {
namespace Stuff {
namespace Functions {


namespace ESV2007 {


template< class DiffusionFactorType, class DiffusionTensorType >
class Cutoff
  : public LocalizableFunctionInterface< typename DiffusionFactorType::EntityType
                                       , typename DiffusionFactorType::DomainFieldType, DiffusionFactorType::dimDomain
                                       , typename DiffusionFactorType::RangeFieldType, 1, 1 >
{
  static_assert(std::is_base_of< Tags::LocalizableFunction, DiffusionFactorType >::value,
                "DiffusionFactorType has to be tagged as a LocalizableFunction!");
  static_assert(std::is_base_of< Tags::LocalizableFunction, DiffusionTensorType >::value,
                "DiffusionTensorType has to be tagged as a LocalizableFunction!");
  typedef typename DiffusionFactorType::EntityType E_;
  typedef typename DiffusionFactorType::DomainFieldType D_;
  static const size_t d_ = DiffusionFactorType::dimDomain;
  typedef typename DiffusionFactorType::RangeFieldType R_;
  typedef LocalizableFunctionInterface< E_, D_, d_, R_, 1 >  BaseType;
  typedef Cutoff< DiffusionFactorType, DiffusionTensorType > ThisType;

  static_assert(DiffusionFactorType::dimRange == 1, "The diffusion factor has to be scalar!");
  static_assert(DiffusionFactorType::dimRangeCols == 1, "The diffusion factor has to be scalar!");

  static_assert(std::is_same< typename DiffusionTensorType::EntityType, E_ >::value, "Types do not match!");
  static_assert(std::is_same< typename DiffusionTensorType::DomainFieldType, D_ >::value, "Types do not match!");
  static_assert(DiffusionTensorType::dimDomain == d_, "Dimensions do not match!");
  static_assert(std::is_same< typename DiffusionTensorType::RangeFieldType, R_ >::value, "Types do not match!");

  static_assert(DiffusionTensorType::dimRange == d_, "The diffusion tensor has to be a matrix!");
  static_assert(DiffusionTensorType::dimRangeCols == d_, "The diffusion tensor has to be a matrix!");

  class Localfunction
    : public LocalfunctionInterface< E_, D_, d_, R_, 1, 1 >
  {
    typedef LocalfunctionInterface< E_, D_, d_, R_, 1, 1 > BaseType;
  public:
    typedef typename BaseType::EntityType EntityType;

    typedef typename BaseType::DomainFieldType DomainFieldType;
    static const size_t                        dimDomain = BaseType::dimDomain;
    typedef typename BaseType::DomainType      DomainType;

    typedef typename BaseType::RangeFieldType RangeFieldType;
    static const size_t                       dimRange = BaseType::dimRange;
    static const size_t                       dimRangeCols = BaseType::dimRangeCols;
    typedef typename BaseType::RangeType      RangeType;

    typedef typename BaseType::JacobianRangeType JacobianRangeType;

  private:
    template< class DF, size_t r, size_t rR >
    struct ComputeDiffusionFactor
    {
      static_assert(AlwaysFalse< DF >::value, "Not implemented for these dimensions!");
    };

    template< class DF >
    struct ComputeDiffusionFactor< DF, 1, 1 >
    {
      /**
       * We try to find the minimum of a polynomial of given order by evaluating it at the points of a quadrature that
       * would integrate this polynomial exactly.
       * \todo These are just some heuristics and should be replaced by something proper.
       */
      static RangeFieldType min_of(const DF& diffusion_factor, const EntityType& ent)
      {
        typename DF::RangeType tmp_value(0);
        RangeFieldType minimum = std::numeric_limits< RangeFieldType >::max();
        const auto local_diffusion_factor = diffusion_factor.local_function(ent);
        const size_t ord = local_diffusion_factor->order();
        const auto& quadrature = QuadratureRules< DomainFieldType, dimDomain >::rule(ent.type(),
                                                                                     boost::numeric_cast< int >(ord));
        const auto quad_point_it_end = quadrature.end();
        for (auto quad_point_it = quadrature.begin(); quad_point_it != quad_point_it_end; ++quad_point_it) {
          local_diffusion_factor->evaluate(quad_point_it->position(), tmp_value);
          minimum = std::min(minimum, tmp_value[0]);
        }
        return minimum;
      } // ... min_of(...)
    }; // class ComputeDiffusionFactor< ..., 1, 1 >

    template< class DT, size_t r, size_t rR >
    struct ComputeDiffusionTensor
    {
      static_assert(AlwaysFalse< DT >::value, "Not implemented for these dimensions!");
    };

    template< class DT, size_t d >
    struct ComputeDiffusionTensor< DT, d, d >
    {
      static RangeFieldType min_eigenvalue_of(const DT& diffusion_tensor, const EntityType& ent)
      {
#if !HAVE_EIGEN
        static_assert(AlwaysFalse< DT >::value, "You are missing eigen!");
#endif
        const auto local_diffusion_tensor = diffusion_tensor.local_function(ent);
        assert(local_diffusion_tensor->order() == 0);
        const auto& reference_element = ReferenceElements< DomainFieldType, dimDomain >::general(ent.type());
        const Stuff::LA::EigenDenseMatrix< RangeFieldType >
            tensor = local_diffusion_tensor->evaluate(reference_element.position(0, 0));
        ::Eigen::EigenSolver< typename Stuff::LA::EigenDenseMatrix< RangeFieldType >::BackendType >
            eigen_solver(tensor.backend());
        assert(eigen_solver.info() == ::Eigen::Success);
        const auto eigenvalues = eigen_solver.eigenvalues(); // <- this should be an Eigen vector of std::complex
        RangeFieldType min_ev = std::numeric_limits< RangeFieldType >::max();
        for (size_t ii = 0; ii < boost::numeric_cast< size_t >(eigenvalues.size()); ++ii) {
          // assert this is real
          assert(std::abs(eigenvalues[ii].imag()) < 1e-15);
          // assert that this eigenvalue is positive
          const RangeFieldType eigenvalue = eigenvalues[ii].real();
          assert(eigenvalue > 1e-15);
          min_ev = std::min(min_ev, eigenvalue);
        }
        return min_ev;
      } // ... min_eigenvalue_of_(...)
    }; // class Compute< ..., d, d >

  public:
    Localfunction(const EntityType& ent,
                  const DiffusionFactorType& diffusion_factor,
                  const DiffusionTensorType& diffusion_tensor,
                  const RangeFieldType poincare_constant)
      : BaseType(ent)
      , value_(0)
    {
      const RangeFieldType min_diffusion_factor
          = ComputeDiffusionFactor< DiffusionFactorType,
                                    DiffusionFactorType::dimRange,
                                    DiffusionFactorType::dimRangeCols >::min_of(diffusion_factor, ent);
      const RangeFieldType min_eigen_value_diffusion_tensor
          = ComputeDiffusionTensor< DiffusionTensorType,
                                    DiffusionTensorType::dimRange,
                                    DiffusionTensorType::dimRangeCols >::min_eigenvalue_of(diffusion_tensor, ent);
      assert(min_diffusion_factor > RangeFieldType(0));
      assert(min_eigen_value_diffusion_tensor > RangeFieldType(0));
      const DomainFieldType hh = compute_diameter_of_(ent);
      value_ = (poincare_constant * hh * hh) / (min_diffusion_factor * min_eigen_value_diffusion_tensor);
    } // Localfunction(...)

    Localfunction(const Localfunction& /*other*/) = delete;

    Localfunction& operator=(const Localfunction& /*other*/) = delete;

    virtual size_t order() const override final
    {
      return 0;
    }

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
      for (auto cc : DSC::valueRange(ent.template count< dimDomain >())) {
        const auto vertex = ent.template subEntity< dimDomain >(cc)->geometry().center();
        for (auto dd : DSC::valueRange(cc + 1, ent.template count< dimDomain >())) {
          const auto other_vertex = ent.template subEntity< dimDomain >(dd)->geometry().center();
          const auto diff = vertex - other_vertex;
          ret = std::max(ret, diff.two_norm());
        }
      }
      return ret;
    } // ... compute_diameter_of_(...)

    RangeFieldType value_;
  }; // class Localfunction

public:
  typedef typename BaseType::EntityType         EntityType;
  typedef typename BaseType::LocalfunctionType  LocalfunctionType;

  typedef typename BaseType::DomainFieldType DomainFieldType;
  static const size_t                        dimDomain = BaseType::dimDomain;
  typedef typename BaseType::DomainType      DomainType;

  typedef typename BaseType::RangeFieldType RangeFieldType;
  static const size_t                       dimRange = BaseType::dimRange;
  static const size_t                       dimRangeCols = BaseType::dimRangeCols;
  typedef typename BaseType::RangeType      RangeType;

  static std::string static_id()
  {
    return BaseType::static_id() + ".ESV2007.cutoff";
  }

  Cutoff(const DiffusionFactorType& diffusion_factor,
         const DiffusionTensorType& diffusion_tensor,
         const RangeFieldType poincare_constant = 1.0 / (M_PIl * M_PIl),
         const std::string nm = static_id())
    : diffusion_factor_(diffusion_factor)
    , diffusion_tensor_(diffusion_tensor)
    , poincare_constant_(poincare_constant)
    , name_(nm)
  {}

  Cutoff(const ThisType& other) = default;

  ThisType& operator=(const ThisType& other) = delete;

  virtual std::string name() const override final
  {
    return name_;
  }

  virtual std::unique_ptr< LocalfunctionType > local_function(const EntityType& entity) const override final
  {
    return std::unique_ptr< Localfunction >(new Localfunction(entity,
                                                              diffusion_factor_,
                                                              diffusion_tensor_,
                                                              poincare_constant_));
  }

private:
  const DiffusionFactorType& diffusion_factor_;
  const DiffusionTensorType& diffusion_tensor_;
  const RangeFieldType poincare_constant_;
  std::string name_;
}; // class Cutoff


} // namespace ESV2007
} // namespace Functions
} // namespace Stuff
} // namespace Dune

#endif // HAVE_EIGEN

#endif // DUNE_STUFF_PLAYGROUND_FUNCTIONS_ESV2007_HH
