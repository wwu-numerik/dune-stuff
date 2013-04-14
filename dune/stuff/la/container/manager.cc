#include "manager.hh"

ContainerManager::ContainerManager() {}

ContainerManager::~ContainerManager() {}

std::string ContainerManager::createDenseVector(const int size)
{
  std::cout << "ContainerManager::createDenseVector(" << size << ")" << std::endl;
  return "";
}

std::string ContainerManager::createDenseMatrix(const int rows, const int cols)
{
  std::cout << "ContainerManager::createDenseMatrix(" << rows << ", " << cols << ")" << std::endl;
  return "";
}

std::string ContainerManager::trmv(const std::string denseVector, const std::string denseMatrix)
{
  std::cout << "ContainerManager::trmv()" << std::endl;
  return "";
}
