// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_FEMEOC_HH
#define DUNE_STUFF_FEMEOC_HH

#if HAVE_DUNE_FEM

#include <cassert>
#include <sstream>
#include <fstream>
#include <vector>
#include <limits>

#include <boost/format.hpp>

#include <dune/common/fvector.hh>

#include <dune/fem/io/file/iointerface.hh>

namespace Dune {
namespace Stuff {
namespace Fem {
/**
   *  @ingroup HelperClasses
   *  \brief Write a self contained tex table
   *  for eoc runs with timing information.
   *
   *  Constructor takes base name (filename) of file and
   *  generates two files:
   *  filename.tex and filename_body.tex.
   *  The file filename_body.tex hold the actual body
   *  of the eoc table which is included in filename.tex
   *  but can also be used to combine e.g. runs with different
   *  parameters or for plotting using gnuplot.
   *
   *  The class is singleton and thus new errors for eoc
   *  computations can be added in any part of the program.
   *  To add a new entry for eoc computations use one of the
   *  addEntry methods. These return a unique unsinged int
   *  which can be used to add error values to the table
   *  with the setErrors methods.
   *  The method write is used to write a single line
   *  to the eoc table.
   *  \note copy/paste from fem with certain adjustments
   */
class FemEoc
{
  std::ofstream outputFile_;
  int level_;
  std::vector< double > prevError_;
  std::vector< double > error_;
  std::vector< std::string > description_;
  double prevh_;
  bool initial_;
  std::vector< int > pos_;

  FemEoc();
  void init(const std::string& path,
            const std::string& name, const std::string& descript, const std::string& inputFile);
  void init(const std::string& name, const std::string& descript, const std::string& inputFile);

  template< class VectorType >
  void seterrors(size_t id, const VectorType& err, size_t size) {
    int pos = pos_[id];

    for (size_t i = 0; i < size; ++i)
      error_[pos + i] = err[i];
  }

  template< int SIZE >
  void seterrors(size_t id, const Dune::FieldVector< double, SIZE >& err) {
    int pos = pos_[id];

    for (int i = 0; i < SIZE; ++i)
      error_[pos + i] = err[i];
  }

  void seterrors(size_t id, const double& err);
  void writeerr(double h, double size, double time, int counter);

  template< class Writer >
  void writeerr(Writer& writer, bool last) {
    if (initial_)
    {
      writer.putHeader(outputFile_);
    }

    writer.putStaticCols(outputFile_);

    for (unsigned int i = 0; i < 2; ++i)
    {
      writer.putErrorCol(outputFile_, prevError_[i], error_[i], prevh_, initial_);
      prevError_[i] = error_[i];
      error_[i] = -1;    // uninitialized
    }

    writer.putLineEnd(outputFile_);

    if (last)
      writer.endTable(outputFile_);

    prevh_ = writer.get_h();
    level_++;
    initial_ = false;
  } // writeerr

  template< class StrVectorType >
  size_t addentry(const StrVectorType& descript, size_t size) {
    if (!initial_)
      DUNE_THROW(Dune::InvalidStateException, "");
    assert(error_.size() < std::numeric_limits< int >::max());
    pos_.push_back( int(error_.size()) );
    for (size_t i = 0; i < size; ++i)
    {
      error_.push_back(0);
      prevError_.push_back(0);
      description_.push_back(descript[i]);
    }
    return pos_.size() - 1;
  } // addentry

  size_t addentry(const std::string& descript);

public:
  ~FemEoc();

  static FemEoc& instance() {
    static FemEoc instance_;
    return instance_;
  }

  //! open file path/name and write a description string into tex file
  static void initialize(const std::string& path,
                         const std::string& name,
                         const std::string& descript,
                         const std::string& templateFilename) {
    instance().init(path, name, descript, templateFilename);
  }

  //! open file name and write description string into tex file
  static void initialize(const std::string& name,
                         const std::string& descript,
                         const std::string& templateFilename) {
    instance().init(name, descript, templateFilename);
  }

  /** \brief add a vector of new eoc values
     *
     *  \tparam  StrVectorType a vector type with operator[]
     *           returning a string (a C style array can be used)
     *           the size of the vector is given as parameter
     *  \return  a unique index used to add the error values
     */
  template< class StrVectorType >
  static size_t addEntry(const StrVectorType& descript, size_t size) {
    return instance().addentry(descript, size);
  }

  /** \brief add a vector of new eoc values
     *
     *  \tparam  StrVectorType a vector type with size() and operator[]
     *           returning a string
     *  \return  a unique index used to add the error values
     */

  template< class StrVectorType >
  static size_t addEntry(const StrVectorType& descript) {
    return instance().addentry( descript, descript.size() );
  }

  /** \brief add a single new eoc output
     *
     *  \return  a unique index used to add the error values
     */
  static size_t addEntry(const std::string& descript) {
    return instance().addentry(descript);
  }

  /** \brief add a single new eoc output
     *
     *  \return  a unique index used to add the error values
     */
  static size_t addEntry(const char* descript) {
    return addEntry( std::string(descript) );
  }

  /** \brief add a vector of error values for the given id (returned by
     *         addEntry)
     *  \tparam  VectorType a vector type with an operator[]
     *           returning a double (C style array can be used)
     */
  template< class VectorType >
  static void setErrors(size_t id, const VectorType& err, int size) {
    instance().seterrors(id, err, size);
  }

  /** \brief add a vector of error values for the given id (returned bywrite(GridWidth::calcGridWidth(gridPart),
     *                 grid.size(0),runTime,0);
     *
     *         addEntry)
     *  \tparam  VectorType a vector type with a size() and an operator[]
     *           returning a double
     */
  template< class VectorType >
  static void setErrors(size_t id, const VectorType& err) {
    instance().seterrors( id, err, err.size() );
  }

  /** \brief add a vector in a FieldVector of error values for the given id (returned by
     *         addEntry)
     */
  template< int SIZE >
  static void setErrors(size_t id, const Dune::FieldVector< double, SIZE >& err) {
    instance().seterrors(id, err);
  }

  /** \brief add a single error value for the given id (returned by
     *         addEntry)
     */
  static void setErrors(size_t id, const double& err) {
    instance().seterrors(id, err);
  }

  /** \brief commit a line to the eoc file
     */
  static void write(double h, double size, double time, int counter) {
    instance().writeerr(h, size, time, counter);
  }

  template< class Writer >
  static void write(Writer& writer, bool last = false) {
    instance().writeerr(writer, last);
  }
};

} // namespace Stuff
} // namespace Fem
} // namespace Dune

#endif // HAVE_DUNE_FEM

#endif // ifndef DUNE_STUFF_FEMEOC_HH
