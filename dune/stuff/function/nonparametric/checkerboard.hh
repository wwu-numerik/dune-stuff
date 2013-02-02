#ifndef DUNE_STUFF_FUNCTION_CHECKERBOARD_HH
#define DUNE_STUFF_FUNCTION_CHECKERBOARD_HH

#include <vector>

#include "../interface.hh"

namespace Dune {
namespace Stuff {
namespace Function {


template< class DomainFieldImp, int domainDim, class RangeFieldImp, int rangeDim >
class NonparametricCheckerboard;


template< class DomainFieldImp, int domainDim, class RangeFieldImp >
class NonparametricCheckerboard< DomainFieldImp, domainDim, RangeFieldImp, 1 >
  : public Interface< DomainFieldImp, domainDim, RangeFieldImp, 1 >
{
public:
  typedef Interface< DomainFieldImp, domainDim, RangeFieldImp, 1 >                  BaseType;
  typedef NonparametricCheckerboard< DomainFieldImp, domainDim, RangeFieldImp, 1 >  ThisType;

  typedef typename BaseType::DomainType     DomainType;
  static const int                          dimDomain = domainDim;
  typedef typename BaseType::RangeFieldType RangeFieldType;
  typedef typename BaseType::RangeType      RangeType;

  NonparametricCheckerboard(const DomainType _lowerLeft,
                            const DomainType _upperRight,
                            const std::vector< unsigned int > _numElements,
                            const std::vector< RangeFieldType > _values)
    : lowerLeft_(_lowerLeft)
    , upperRight_(_upperRight)
    , numElements_(_numElements)
    , values_(_values)
  {
    // get total number of subdomains
    unsigned int totalSubdomains = 1;
    for (int d = 0; d < dimDomain; ++d) {
      totalSubdomains *= numElements_[d];
    }
    assert(totalSubdomains <= values_.size() && "Please provide at least as many components as subdomains!");
  }

  NonparametricCheckerboard(const ThisType& _other)
    : lowerLeft_(_other.lowerLeft_)
    , upperRight_(_other.upperRight_)
    , numElements_(_other.numElements_)
    , values_(_other.values_)
  {
    // no cheks necessary, since they have been carried out in the constructor of other
  }

  ThisType& operator=(const ThisType& other)
  {
    if (this != &other) {
      lowerLeft_ = other.lowerLeft();
      upperRight_ = other.upperRight();
      numElements_ = other.numElements();
      values_ = other.values();
    }
    return this;
  }

  const DomainType& lowerLeft() const
  {
    return lowerLeft_;
  }

  const DomainType& upperRight() const
  {
    return upperRight_;
  }

  const std::vector< unsigned int >& numElements() const
  {
    return numElements_;
  }

  const std::vector< RangeFieldType >& values() const
  {
    return values_;
  }

  virtual bool parametric() const
  {
    return false;
  }

  virtual std::string name() const
  {
    return "function.nonparametric.checkerboard";
  }

  virtual int order() const
  {
    return 0;
  }

  virtual void evaluate(const DomainType& x, RangeType& ret) const
  {
    // decide on the subdomain the point x belongs to
    std::vector< unsigned int > whichPartition;
    for (int d = 0; d < dimDomain; ++d)
    {
      whichPartition.push_back(std::floor(numElements_[d]*((x[d] - lowerLeft_[d])/(upperRight_[d] - lowerLeft_[d]))));
    }
    unsigned int subdomain = 0;
    if (dimDomain == 1)
      subdomain = whichPartition[0];
    else if (dimDomain == 2)
      subdomain = whichPartition[0] + whichPartition[1]*numElements_[0];
    else if (dimDomain == 3)
      subdomain = whichPartition[0] + whichPartition[1]*numElements_[0] + whichPartition[2]*numElements_[1]*numElements_[0];
    else
    {
      DUNE_THROW(Dune::NotImplemented, "\nError: not implemented for grid dimensions other than 1, 2 or 3!");
    } // decide on the subdomain the point x belongs to
    // return the component that belongs to the subdomain of x
    ret = values_[subdomain];
  } // virtual void evaluate(const DomainType& x, RangeType& ret) const

private:
  DomainType lowerLeft_;
  DomainType upperRight_;
  std::vector< unsigned int > numElements_;
  std::vector< RangeFieldType > values_;
}; // class Checkerboard

} // namespace Function
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_FUNCTION_CHECKERBOARD_HH
