// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//
// Contributors: Kirsten Weber

#ifndef DUNE_STUFF_FUNCTION_FROMFILE_HH
#define DUNE_STUFF_FUNCTION_FROMFILE_HH

#include <sstream>
#include <iostream>
#include <memory>

#include <dune/common/exceptions.hh>

#include <dune/stuff/common/parameter/tree.hh>
#include <dune/stuff/common/color.hh>
#include <dune/stuff/common/string.hh>
#include <dune/stuff/common/logging.hh>

#include "interfaces.hh"


namespace Dune {
namespace Stuff {

template< class DomainFieldImp, int domainDim, class RangeFieldImp, int rangeDimRows, int rangeDimCols >
class FunctionFromFile
  : public FunctionInterface< DomainFieldImp, domainDim, RangeFieldImp, rangeDimRows >
{
  typedef FunctionInterface< DomainFieldImp, domainDim, RangeFieldImp, rangeDimRows > BaseType;
public:
  typedef FunctionFromFile< DomainFieldImp, domainDim, RangeFieldImp, rangeDimRows, rangeDimCols >  ThisType;

  typedef DomainFieldImp                      DomainFieldType;
  static const int                            dimDomain = BaseType::dimDomain;
  typedef typename BaseType::DomainType       DomainType;
  typedef RangeFieldImp                       RangeFieldType;
  static const int                            dimRange = BaseType::dimRange;
  typedef typename BaseType::RangeType        RangeType;

  static std::string static_id()
  {
    return BaseType::static_id() + ".fromfile";
  }

public:
  FunctionFromFile(const std::string _filename,
                   const DomainType& _lowerLeft,
                   const DomainType& _upperRight,
                   const std::vector< size_t >& _numElements,
                   const std::string _name = static_id(),
                   const int _order = 0,
                   std::ostream& out = Dune::Stuff::Common::Logger().devnull())
    : filename_(_filename)
    , lowerLeft_(_lowerLeft)
    , upperRight_(_upperRight)
    , numElements_(_numElements)
    , name_(_name)
    , order_(_order)
  {
    // sanity checks
    std::stringstream msg;
    size_t throw_up = 0;
    for (int dd = 0; dd < dimDomain; ++dd) {
      if (!(lowerLeft_[dd] < upperRight_[dd])) {
        ++throw_up;
        msg << "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
            << " lowerLeft[" << dd << "] (" << lowerLeft_[dd]
            << ") has to be smaller than upperRight[" << dd << "] (" << upperRight_[dd] << ")!";
      }
      if (!(numElements_[dd] > 0)) {
        ++throw_up;
        msg << "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
            << " numElements[" << dd << "] has to be positive (is " << numElements_[dd] << ")!";
      }
    }

    // read all the data (functionvalues per voxel) from the file
    std::ifstream datafile(filename_);
    if (datafile.is_open()) {
      // geht das ganze so nur, wenn das ascii_==true ist? ascii_ benutze ich bisher noch nicht!
      std::string line;
      int numberOfVoxels = 0;
      const int dim = rangeDimCols*rangeDimRows;
      RangeType values;

      out << "Reading " << filename_ << " ...   " << std::flush;
      while(std::getline(datafile,line))
      {
        numberOfVoxels++;
        const std::vector<double> items = Dune::Stuff::Common::tokenize<double>(line, " ");
        if (items.size() != dimDomain + dim)
          DUNE_THROW(Dune::IOError,
                     "Error: " << items.size() << " = items.size() != dimDomain + rangeDimCols*rangeDimRows = " << dimDomain + dim << "!");
        for (unsigned int ii = 0; ii < dimDomain; ++ii){
          // the numbering of the voxels starts with 1
          if (!( (1 <= items[ii]) && (items[ii] <= numElements_[ii]) ))
            DUNE_THROW(Dune::IOError,
                       "Error: " << ii << ". index of the voxel (" << items[ii]
                       << ") does not lie between 1 and numElements[" << ii << "] ("
                       << numElements_[ii] << ") !");
        }
        for (unsigned int ii = 0; ii < rangeDimRows; ++ii){
          for (unsigned int jj = 0; jj < rangeDimCols; ++jj){
            DUNE_THROW(InvalidStateException, "assignment below doesn't compile");
//            values[ii][jj] = items[dimDomain + jj + rangeDimCols*ii];
          }
        }
        data_.push_back(values);
      }
      out << "done: function values for " << numberOfVoxels << " voxels read." << std::endl;
    } else { // if (datafile)
      ++throw_up;
      msg << "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
          << " could not open '" << filename_ << "'!";
    } // if (datafile)
    // throw up, if needed
    if (throw_up)
      DUNE_THROW(Dune::RangeError, msg.str());
  } // FunctionFromFile()

  const DomainType& lowerLeft() const
  {
    return lowerLeft_;
  }

  const DomainType& upperRight() const
  {
    return upperRight_;
  }

  const std::vector< size_t >& numElements() const
  {
    return numElements_;
  }

  virtual std::string name() const
  {
    return name_;
  }

  virtual int order() const
  {
    return order_;
  }

  virtual void evaluate(const DomainType& x, RangeType& ret) const
  {
    // decide on the voxel x belongs to
    Dune::FieldVector< size_t, dimDomain > voxel;
    for (size_t ii=0; ii < dimDomain; ++ii){
      voxel[ii] = std::floor(numElements_[ii]*( (x[ii] - lowerLeft_[ii])/(upperRight_[ii] - lowerLeft_[ii]) ));
      if (!( (0 <= voxel[ii]) && (voxel[ii] < numElements_[ii]) ))
        DUNE_THROW(Dune::RangeError,
                   "Error: " << ii << ". index of the voxel (" << voxel[ii]
                   << ") does not lie between 0 and numElements[" << ii << "]-1 ("
                   << numElements_[ii]-1 << ") !");
    }
    int index;
    if (dimDomain == 1){
      index = voxel[0];
    }
    else if ( dimDomain==2 ){
      index = voxel[1] + voxel[0]*numElements_[1];
    }
    else if ( dimDomain==3 ){
      index = voxel[2] + voxel[1]*numElements_[2] + voxel[0]*numElements_[1]*numElements_[2];
    }
    else {
      DUNE_THROW(Dune::RangeError, "evaluate() does not support dimDomains other than 1,2 or 3!");
    }

    ret = data_[index];
  } // virtual void evaluate(const DomainType& x, RangeType& ret) const

private:
  const std::string filename_;
  const DomainType lowerLeft_;
  const DomainType upperRight_;
  const std::vector< size_t > numElements_;
  const std::string name_;
  const int order_;
  std::vector< RangeType > data_;
}; // class FunctionFromFile


template< class DomainFieldImp, int domainDim, class RangeFieldImp>
class FunctionFromFile< DomainFieldImp, domainDim, RangeFieldImp, 1, 1 >
  : public FunctionInterface< DomainFieldImp, domainDim, RangeFieldImp, 1 >
{
  typedef FunctionInterface< DomainFieldImp, domainDim, RangeFieldImp, 1 > BaseType;
public:
  typedef FunctionFromFile< DomainFieldImp, domainDim, RangeFieldImp, 1, 1 >  ThisType;

  typedef DomainFieldImp                      DomainFieldType;
  static const int                            dimDomain = BaseType::dimDomain;
  typedef typename BaseType::DomainType       DomainType;
  typedef RangeFieldImp                       RangeFieldType;
  static const int                            dimRange = BaseType::dimRange;
  typedef typename BaseType::RangeType        RangeType;

  static std::string static_id()
  {
    return BaseType::static_id() + ".fromfile";
  }

public:
  FunctionFromFile(const std::string _filename,
                   const DomainType& _lowerLeft,
                   const DomainType& _upperRight,
                   const std::vector< size_t >& _numElements,
                   const std::string _name = static_id(),
                   const int _order = 0,
                   std::ostream& out = Dune::Stuff::Common::Logger().devnull())
    : filename_(_filename)
    , lowerLeft_(_lowerLeft)
    , upperRight_(_upperRight)
    , numElements_(_numElements)
    , name_(_name)
    , order_(_order)
  {
    // sanity checks
    std::stringstream msg;
    size_t throw_up = 0;
    for (int dd = 0; dd < dimDomain; ++dd) {
      if (!(lowerLeft_[dd] < upperRight_[dd])) {
        ++throw_up;
        msg << "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
            << " lowerLeft[" << dd << "] (" << lowerLeft_[dd]
            << ") has to be smaller than upperRight[" << dd << "] (" << upperRight_[dd] << ")!";
      }
      if (!(numElements_[dd] > 0)) {
        ++throw_up;
        msg << "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
            << " numElements[" << dd << "] has to be positive (is " << numElements_[dd] << ")!";
      }
    }

    // read all the data (functionvalues per voxel) from the file
    std::ifstream datafile(filename_);
    if (datafile.is_open()) {
      // geht das ganze so nur, wenn das ascii_==true ist? ascii_ benutze ich bisher noch nicht!
      std::string line;
      int numberOfVoxels = 0;
      RangeType value;

      out << "Reading " << filename_ << " ...   " << std::flush;
      while(std::getline(datafile,line))
      {
        numberOfVoxels++;
        const std::vector<double> items = Dune::Stuff::Common::tokenize<double>(line, " ");
        if (items.size() != dimDomain + 1)
          DUNE_THROW(Dune::IOError,
                     "Error: " << items.size() << " = items.size() != dimDomain + 1 = " << dimDomain + 1 << "!");
        for (unsigned int ii = 0; ii < dimDomain; ++ii){
          // the numbering of the voxels starts with 1
          if (!( (1 <= items[ii]) && (items[ii] <= numElements_[ii]) ))
            DUNE_THROW(Dune::IOError,
                       "Error: " << ii << ". index of the voxel (" << items[ii]
                       << ") does not lie between 1 and numElements[" << ii << "] ("
                       << numElements_[ii] << ") !");
        }
        value = items[dimDomain];
        data_.push_back(value);
      }
      out << "done: function values for " << numberOfVoxels << " voxels read." << std::endl;
    } else { // if (datafile)
      ++throw_up;
      msg << "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
          << " could not open '" << filename_ << "'!";
    } // if (datafile)
    // throw up, if needed
    if (throw_up)
      DUNE_THROW(Dune::RangeError, msg.str());
  } // FunctionFromFile()

  const DomainType& lowerLeft() const
  {
    return lowerLeft_;
  }

  const DomainType& upperRight() const
  {
    return upperRight_;
  }

  const std::vector< size_t >& numElements() const
  {
    return numElements_;
  }

  virtual std::string name() const
  {
    return name_;
  }

  virtual int order() const
  {
    return order_;
  }

  virtual void evaluate(const DomainType& x, RangeType& ret) const
  {
    // decide on the voxel x belongs to
    Dune::FieldVector< size_t, dimDomain > voxel;
    for (size_t ii=0; ii < dimDomain; ++ii){
      voxel[ii] = std::floor(numElements_[ii]*( (x[ii] - lowerLeft_[ii])/(upperRight_[ii] - lowerLeft_[ii]) ));
      if (!( (0 <= voxel[ii]) && (voxel[ii] < numElements_[ii]) ))
        DUNE_THROW(Dune::RangeError,
                   "Error: " << ii << ". index of the voxel (" << voxel[ii]
                   << ") does not lie between 0 and numElements[" << ii << "]-1 ("
                   << numElements_[ii]-1 << ") !");
    }
    int index;
    if (dimDomain == 1){
      index = voxel[0];
    }
    else if ( dimDomain==2 ){
      index = voxel[1] + voxel[0]*numElements_[1];
    }
    else if ( dimDomain==3 ){
      index = voxel[2] + voxel[1]*numElements_[2] + voxel[0]*numElements_[1]*numElements_[2];
    }
    else {
      DUNE_THROW(Dune::RangeError, "evaluate() does not support dimDomains other than 1,2 or 3!");
    }

    ret = data_[index];
  } // virtual void evaluate(const DomainType& x, RangeType& ret) const

private:
  const std::string filename_;
  const DomainType lowerLeft_;
  const DomainType upperRight_;
  const std::vector< size_t > numElements_;
  const std::string name_;
  const int order_;
  std::vector< RangeType > data_;
}; // class FunctionFromFile< ..., 1, 1 >


template< class DomainFieldImp, int domainDim, class RangeFieldImp, int rangeDim>
class FunctionFromFile< DomainFieldImp, domainDim, RangeFieldImp, rangeDim, 1 >
  : public FunctionInterface< DomainFieldImp, domainDim, RangeFieldImp, rangeDim >
{
  typedef FunctionInterface< DomainFieldImp, domainDim, RangeFieldImp, rangeDim > BaseType;
public:
  typedef FunctionFromFile< DomainFieldImp, domainDim, RangeFieldImp, rangeDim, 1 >  ThisType;

  typedef DomainFieldImp                      DomainFieldType;
  static const int                            dimDomain = BaseType::dimDomain;
  typedef typename BaseType::DomainType       DomainType;
  typedef RangeFieldImp                       RangeFieldType;
  static const int                            dimRange = BaseType::dimRange;
  typedef typename BaseType::RangeType        RangeType;
  static const int                            rangeDimRows = dimRange;

  static std::string id()
  {
    return BaseType::id() + ".fromfile";
  }

public:
  FunctionFromFile(const std::string _filename,
                   const DomainType& _lowerLeft,
                   const DomainType& _upperRight,
                   const std::vector< size_t >& _numElements,
                   const std::string _name = id(),
                   const int _order = 0,
                   std::ostream& out = Dune::Stuff::Common::Logger().devnull())
    : filename_(_filename)
    , lowerLeft_(_lowerLeft)
    , upperRight_(_upperRight)
    , numElements_(_numElements)
    , name_(_name)
    , order_(_order)
  {
    // sanity checks
    std::stringstream msg;
    size_t throw_up = 0;
    for (int dd = 0; dd < dimDomain; ++dd) {
      if (!(lowerLeft_[dd] < upperRight_[dd])) {
        ++throw_up;
        msg << "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
            << " lowerLeft[" << dd << "] (" << lowerLeft_[dd]
            << ") has to be smaller than upperRight[" << dd << "] (" << upperRight_[dd] << ")!";
      }
      if (!(numElements_[dd] > 0)) {
        ++throw_up;
        msg << "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
            << " numElements[" << dd << "] has to be positive (is " << numElements_[dd] << ")!";
      }
    }

    // read all the data (functionvalues per voxel) from the file
    std::ifstream datafile(filename_);
    if (datafile.is_open()) {
      // geht das ganze so nur, wenn das ascii_==true ist? ascii_ benutze ich bisher noch nicht!
      std::string line;
      int numberOfVoxels = 0;
      RangeType values;

      out << "Reading " << filename_ << " ...   " << std::flush;
      while(std::getline(datafile,line))
      {
        numberOfVoxels++;
        const std::vector<double> items = Dune::Stuff::Common::tokenize<double>(line, " ");
        if (items.size() != rangeDimRows + dimDomain)
          DUNE_THROW(Dune::IOError,
                     "Error: " << items.size() << " = items.size() != rangeDimRows + dimDomain = " << rangeDimRows + dimDomain << "!");
        for (unsigned int ii = 0; ii < dimDomain; ++ii){
          // the numbering of the voxels starts with 1
          if (!( (1 <= items[ii]) && (items[ii] <= numElements_[ii]) ))
            DUNE_THROW(Dune::IOError,
                       "Error: " << ii << ". index of the voxel (" << items[ii]
                       << ") does not lie between 1 and numElements[" << ii << "] ("
                       << numElements_[ii] << ") !");
        }
        for (unsigned int ii = 0; ii < rangeDimRows; ++ii){
          values[ii] = items[dimDomain + ii];
        }
        data_.push_back(values);
      }
      out << "done: function values for " << numberOfVoxels << " voxels read." << std::endl;
    } else { // if (datafile)
      ++throw_up;
      msg << "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
          << " could not open '" << filename_ << "'!";
    } // if (datafile)
    // throw up, if needed
    if (throw_up)
      DUNE_THROW(Dune::RangeError, msg.str());
  } // FunctionFromFile()

  const DomainType& lowerLeft() const
  {
    return lowerLeft_;
  }

  const DomainType& upperRight() const
  {
    return upperRight_;
  }

  const std::vector< size_t >& numElements() const
  {
    return numElements_;
  }

  virtual std::string name() const
  {
    return name_;
  }

  virtual int order() const
  {
    return order_;
  }

  virtual void evaluate(const DomainType& x, RangeType& ret) const
  {
    // decide on the voxel x belongs to
    Dune::FieldVector< size_t, dimDomain > voxel;
    for (size_t ii=0; ii < dimDomain; ++ii){
      voxel[ii] = std::floor(numElements_[ii]*( (x[ii] - lowerLeft_[ii])/(upperRight_[ii] - lowerLeft_[ii]) ));
      if (!( (0 <= voxel[ii]) && (voxel[ii] < numElements_[ii]) ))
        DUNE_THROW(Dune::RangeError,
                   "Error: " << ii << ". index of the voxel (" << voxel[ii]
                   << ") does not lie between 0 and numElements[" << ii << "]-1 ("
                   << numElements_[ii]-1 << ") !");
    }
    int index;
    if (dimDomain == 1){
      index = voxel[0];
    }
    else if ( dimDomain==2 ){
      index = voxel[1] + voxel[0]*numElements_[1];
    }
    else if ( dimDomain==3 ){
      index = voxel[2] + voxel[1]*numElements_[2] + voxel[0]*numElements_[1]*numElements_[2];
    }
    else {
      DUNE_THROW(Dune::RangeError, "evaluate() does not support dimDomains other than 1,2 or 3!");
    }

    ret = data_[index];
  } // virtual void evaluate(const DomainType& x, RangeType& ret) const

private:
  const std::string filename_;
  const DomainType lowerLeft_;
  const DomainType upperRight_;
  const std::vector< size_t > numElements_;
  const std::string name_;
  const int order_;
  std::vector< RangeType > data_;
}; // class FunctionFromFile< ..., 1>


} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_FUNCTION_FROMFILE_HH
