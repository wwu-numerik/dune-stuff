// This file is part of the dune-gdt project:
//   http://users.dune-project.org/projects/dune-gdt
// Copyright holders: Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_COMMON_TMP_STORAGE_HH
#define DUNE_STUFF_COMMON_TMP_STORAGE_HH

#include <vector>

#include <dune/common/dynmatrix.hh>
#include <dune/common/dynvector.hh>

#include <dune/stuff/common/parallel/threadstorage.hh>

namespace Dune {
namespace Stuff {
namespace Common {


template< class FieldType = double >
class TmpMatricesStorage
{
public:
  typedef DynamicMatrix< FieldType > LocalMatrixType;
protected:
  typedef std::vector< std::vector< LocalMatrixType > > LocalMatrixContainerType;

public:

  /**
   * \brief constructs the appropriate amount of matrix and indices storage
   *
   *        We construct 4 indices vectors since this is the maximum amount needed for discretizations (at least that
   *        we know of atm), namely one for each entity/neighbor and ansatz/test combination.
   */
  TmpMatricesStorage(const std::vector< size_t >& num_tmp_objects,
                     const size_t max_rows,
                     const size_t max_cols)
    : matrices_(LocalMatrixContainerType({std::vector< LocalMatrixType >(num_tmp_objects.at(0),
                                                                         LocalMatrixType(max_rows, max_cols, FieldType(0))),
                                          std::vector< LocalMatrixType >(num_tmp_objects.at(1),
                                                                         LocalMatrixType(max_rows, max_cols, FieldType(0)))}))
    , indices_(4, Dune::DynamicVector< size_t >(std::max(max_rows, max_cols)))
  {}

  virtual ~TmpMatricesStorage() {}

  std::vector< std::vector< LocalMatrixType > >& matrices()
  {
    return *matrices_;
  }

  std::vector< Dune::DynamicVector< size_t > >& indices()
  {
    return *indices_;
  }

protected:
  PerThreadValue< LocalMatrixContainerType > matrices_;
  PerThreadValue< std::vector< DynamicVector< size_t > > > indices_;
}; // class Matrices


template< class FieldType = double >
class TmpVectorsStorage
{
public:
  typedef DynamicVector< FieldType > LocalVectorType;
protected:
  typedef std::vector< std::vector< LocalVectorType > > LocalVectorContainerType;
public:
  TmpVectorsStorage(const std::vector< size_t >& num_tmp_objects,
          const size_t max_size)
    : vectors_(LocalVectorContainerType({std::vector< LocalVectorType >(num_tmp_objects.at(0),
                                                                        LocalVectorType(max_size, FieldType(0))),
                                         std::vector< LocalVectorType >(num_tmp_objects.at(1),
                                                                        LocalVectorType(max_size, FieldType(0)))}))
    , indices_(max_size)
  {}

  virtual ~TmpVectorsStorage() {}

  std::vector< std::vector< LocalVectorType > >& vectors()
  {
    return *vectors_;
  }

  Dune::DynamicVector< size_t >& indices()
  {
    return *indices_;
  }

protected:
  PerThreadValue< std::vector< std::vector< LocalVectorType > > > vectors_;
  PerThreadValue< Dune::DynamicVector< size_t > > indices_;
}; // class Vectors


} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_TMP_STORAGE_HH
