#include "eigenmanager.hh"

Dune::Stuff::LA::Container::EigenManager::EigenManager()
  : denseVectorId_(0)
  , denseMatrixId_(0)
{}

std::string Dune::Stuff::LA::Container::EigenManager::createDenseVector(const int size)
{
  const std::string vectorId = "denseVector_" + Dune::Stuff::Common::toString(denseVectorId_);
  denseVectorMap_.insert(std::make_pair(vectorId, std::make_shared< DenseVectorType >(size)));
  denseVectorId_++;
  return vectorId;
}

std::string Dune::Stuff::LA::Container::EigenManager::createDenseMatrix(const int rows, const int cols)
{
  const std::string matrixId = "denseMatrix_" + Dune::Stuff::Common::toString(denseMatrixId_);
  denseMatrixMap_.insert(std::make_pair(matrixId, std::make_shared< DenseMatrixType >(rows, cols)));
  denseMatrixId_++;
  return matrixId;
}

std::string Dune::Stuff::LA::Container::EigenManager::dot(const std::string x, const std::string y)
{
  // get x
  const auto searchX = denseVectorMap_.find(x);
  if (searchX == denseVectorMap_.end())
    return "error: vector '" + x + "' undefined!";
  const auto& vectorX = *(searchX->second);
  // get y
  const auto searchY = denseVectorMap_.find(y);
  if (searchY == denseVectorMap_.end())
    return "error: vector '" + y + "' undefined!";
  const auto& vectorY = *(searchY->second);
  // compute
  try {
    const double result = vectorX.backend().transpose() * vectorY.backend();
    return Dune::Stuff::Common::toString(result);
  } catch (...) {
    return "error: unknown exception thrown!";
  }
}
