
#include <iostream>
#include <string>
#include <map>

#include "eigen.hh"

class ContainerManager
{
public:
  ContainerManager();

  ~ContainerManager();

  std::string createDenseVector(const int size);

  std::string createDenseMatrix(const int rows, const int cols);

  std::string trmv(const std::string denseVector, const std::string denseMatrix);

};
