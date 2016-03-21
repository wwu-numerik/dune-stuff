// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_FUNCTIONS_AFFINE_HH
#define DUNE_STUFF_FUNCTIONS_AFFINE_HH

#include <memory>

#include <dune/common/fmatrix.hh>
#include <dune/common/fvector.hh>

#include <dune/stuff/common/configuration.hh>
#include <dune/stuff/la/container.hh>
#include <dune/stuff/functions/constant.hh>

#include "interfaces.hh"

namespace Dune {
namespace Stuff {
namespace Functions {

/**
 * \brief Simple affine function of the form f(x) = A*x + b. For dimRangeCols > 1, there has to be a matrix A_i for
 * every column.
 */
template <class EntityImp, class DomainFieldImp, size_t domainDim, class RangeFieldImp, size_t rangeDim,
          size_t rangeDimCols = 1>
class Affine
  : public Dune::Stuff::GlobalFunctionInterface<EntityImp, DomainFieldImp, domainDim, RangeFieldImp, rangeDim, rangeDimCols>
{
  typedef GlobalFunctionInterface< EntityImp, DomainFieldImp, domainDim, RangeFieldImp, rangeDim, rangeDimCols> BaseType;
  typedef Affine< EntityImp, DomainFieldImp, domainDim, RangeFieldImp, rangeDim, rangeDimCols > ThisType;

public:
  using typename BaseType::DomainType;
  using typename BaseType::RangeFieldType;
  using typename BaseType::RangeType;
  using typename BaseType::JacobianRangeType;
  using BaseType::dimDomain;
  using BaseType::dimRange;
  using BaseType::dimRangeCols;
  typedef typename Dune::Stuff::LA::CommonSparseMatrix<RangeFieldType> MatrixType;
  typedef typename Dune::FieldMatrix<RangeFieldType, dimRange, dimDomain> FieldMatrixType;

  using typename BaseType::LocalfunctionType;

  static const bool available = true;

  static std::string static_id() { return BaseType::static_id() + ".affine"; }

  static Common::Configuration default_config(const std::string sub_name = "")
  {
    Common::Configuration config;
    config["A"]      = internal::EyeMatrix<RangeFieldImp, rangeDim, domainDim>::value_str();
    config["b"]      = internal::EyeMatrix<RangeFieldImp, rangeDim, 1>::value_str();
    config["name"]   = static_id();
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
    std::vector<MatrixType> A_vector(dimRangeCols);
    for (size_t cc = 0; cc < dimRangeCols; ++cc) {
      if (cc == 0 && cfg.has_key("A")) {
        A_vector[0] = cfg.get<MatrixType>("A", dimRange, dimDomain);
      } else {
        A_vector[cc] = cfg.get<MatrixType>("A." + DSC::to_string(cc), dimRange, dimDomain);
      }
    }
    return Common::make_unique<ThisType>(A_vector,
                                         cfg.get<RangeType>("b"),
                                         cfg.get("name", default_cfg.get<std::string>("name")));
  } // ... create(...)

  // constructor
  explicit Affine(const std::vector<MatrixType> A,
                  const RangeType b = RangeType(0),
                  const std::string name_in = static_id())
    : A_(A)
    , b_(b)
    , name_(name_in)
    , b_zero_(DSC::FloatCmp::eq(b_, RangeType(0)))
  {
    assert(A.size() >= dimRangeCols);
  }

  explicit Affine(const Dune::FieldVector< FieldMatrixType, dimRangeCols> A,
                  const RangeType b = RangeType(0),
                  const bool prune = true,
                  const std::string name_in = static_id())
    : A_(A.size())
    , b_(b)
    , name_(name_in)
    , b_zero_(DSC::FloatCmp::eq(b_, RangeType(0)))
  {
    for (size_t cc = 0; cc < dimRangeCols; ++cc)
      A_[cc] = MatrixType(A[cc], prune);
  }

  // constructor for dimRangeCols = 1.
  explicit Affine(const MatrixType A,
                  const RangeType b = RangeType(0),
                  const std::string name_in = static_id())
    : Affine(std::vector<MatrixType>(1, A), b, name_in)
  {
    static_assert(dimRangeCols == 1, "Use constructor above for dimRangeCols > 1");
  }

  Affine(const ThisType& other) = default;

// if HAVE_DUNE_FEM is true, GlobalFunctionInterface is derived from Fem::Function which has a deleted copy assignment operator
#if HAVE_DUNE_FEM
  ThisType& operator=(const ThisType& other)
  {
    A_ = other.A_;
    b_ = other.b_;
    b_zero_ = other.b_zero_;
    name_ = other.name_;
    return *this;
  }
#endif

  virtual std::string type() const override final { return BaseType::static_id() + ".affine"; }

  virtual size_t order() const override final { return 1; }

  using BaseType::evaluate;

  virtual void evaluate(const DomainType& x, RangeType& ret) const override final
  {
    evaluate_helper(x, ret, internal::ChooseVariant< dimRangeCols >());
  }

  using BaseType::jacobian;

  virtual void jacobian(const DomainType& x, JacobianRangeType& ret) const override final
  {
    jacobian_helper(x, ret, internal::ChooseVariant< dimRangeCols >());
  }

  virtual std::string name() const override final { return name_; }

private:
  template< size_t rC >
  void evaluate_helper(const DomainType& x, RangeType& ret, const internal::ChooseVariant< rC >) const
  {
    for (size_t cc = 0; cc < rC; ++cc) {
    Dune::FieldVector< RangeFieldType, dimRange > tmp_col;
    A_[cc].mv(x, tmp_col);
    for (size_t rr = 0; rr < dimRange; ++rr)
      ret[rr][cc] = tmp_col[rr];
    }
    if (!b_zero_)
      ret += b_;
  }

  void evaluate_helper(const DomainType& x, RangeType& ret, const internal::ChooseVariant< 1 >) const
  {
    A_[0].mv(x, ret);
    if (!b_zero_)
      ret += b_;
  }

  template< size_t rC >
  void jacobian_helper(const DomainType& /*x*/, JacobianRangeType& ret, const internal::ChooseVariant< rC >) const
  {
    for (size_t cc = 0; cc < rC; ++cc)
      ret[cc] = A_[cc].operator FieldMatrixType();
  }

  void jacobian_helper(const DomainType& /*x*/, JacobianRangeType& ret, const internal::ChooseVariant< 1 >) const
  {
    ret = A_[0].operator FieldMatrixType();
  }

  std::vector< MatrixType > A_;
  RangeType b_;
  bool b_zero_;
  std::string name_;
};

} // namespace Functions
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_FUNCTIONS_AFFINE_HH
