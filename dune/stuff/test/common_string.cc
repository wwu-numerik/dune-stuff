// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "main.hxx"

#include <vector>

#include <dune/common/fmatrix.hh>
#include <dune/common/densematrix.hh>
#include <dune/common/fvector.hh>
#include <dune/common/dynmatrix.hh>
#include <dune/common/dynvector.hh>
#include <dune/common/densevector.hh>

#include <dune/stuff/common/string.hh>
#include <dune/stuff/common/logstreams.hh>
#include <dune/stuff/common/exceptions.hh>
#include <dune/stuff/common/fvector.hh>
#include <dune/stuff/common/fmatrix.hh>
#include <dune/stuff/la/container/common.hh>
#include <dune/stuff/la/container/eigen.hh>
#include <dune/stuff/la/container/istl.hh>

using namespace Dune::Stuff::Common;
using namespace std;


// define types and expected values for the typed tests
template <class MatrixType>
struct MatrixStringTestDouble : public ::testing::Test
{
  void check() const
  {
    EXPECT_EQ("[1 2; 3 4]", to_string(from_string<MatrixType>("[1.0 2; 3.0 4]")));
    EXPECT_THROW(from_string<MatrixType>("[1 2; 3 4]", 3, 3), Dune::Stuff::Exceptions::conversion_error);
  }
};

template <class MatrixType>
struct MatrixStringTestChar : public ::testing::Test
{
  void check() const
  {
    EXPECT_EQ("[1 2; 3 4]", to_string(from_string<MatrixType>("[1 2; 3 4]")));
    EXPECT_THROW(from_string<MatrixType>("[1 2; 3 4]", 3, 3), Dune::Stuff::Exceptions::conversion_error);
  }
};

template <class VectorType>
struct VectorStringTestDouble : public ::testing::Test
{
  void check() const
  {
    EXPECT_EQ("[1 2 3]", to_string(from_string<VectorType>("[1.0 2 3.0]")));
    EXPECT_THROW(from_string<VectorType>("[1.0 2 3.0]", 4), Dune::Stuff::Exceptions::conversion_error);
  }
};

template <class VectorType>
struct VectorStringTestInt : public ::testing::Test
{
  void check() const
  {
    EXPECT_EQ("[1 2 3]", to_string(from_string<VectorType>("[1 2 3]")));
    EXPECT_THROW(from_string<VectorType>("[1 2 3]", 4), Dune::Stuff::Exceptions::conversion_error);
  }
};

typedef testing::Types<std::vector<double>,
                       Dune::Stuff::LA::CommonDenseVector<double>,
                       Dune::DynamicVector<double>,
                       Dune::FieldVector<double, 3>,
                       Dune::Stuff::Common::FieldVector<double, 3>
#if HAVE_EIGEN
                       ,
                       Dune::Stuff::LA::EigenDenseVector<double>,
                       Dune::Stuff::LA::EigenMappedDenseVector<double>
#endif
#if HAVE_DUNE_ISTL
                       ,
                       Dune::Stuff::LA::IstlDenseVector<double>
#endif
                       >
    VectorTypesDouble;

typedef testing::Types<std::vector<int>,
                       Dune::Stuff::LA::CommonDenseVector<int>,
                       Dune::DynamicVector<int>,
                       Dune::FieldVector<int, 3>,
                       Dune::Stuff::Common::FieldVector<int, 3>
#if HAVE_DUNE_ISTL
                       ,
                       Dune::Stuff::LA::IstlDenseVector<int>
#endif
                       >
    VectorTypesInt;

typedef testing::Types<Dune::DynamicMatrix<double>,
                       Dune::Stuff::LA::CommonDenseMatrix<double>,
                       Dune::Stuff::Common::FieldMatrix<double, 2, 2>,
                       Dune::FieldMatrix<double, 2, 2>
#if HAVE_EIGEN
                       ,
                       Dune::Stuff::LA::EigenDenseMatrix<double>
#endif
                       >
    MatrixTypesDouble;

typedef testing::Types<Dune::DynamicMatrix<char>,
                       Dune::Stuff::Common::FieldMatrix<char, 2, 2>,
                       Dune::FieldMatrix<char, 2, 2>>
    MatrixTypesChar;

// from_string/to_string tests for vector and matrix types
TYPED_TEST_CASE(MatrixStringTestDouble, MatrixTypesDouble);
TYPED_TEST(MatrixStringTestDouble, CheckDouble)
{
  this->check();
}

TYPED_TEST_CASE(MatrixStringTestChar, MatrixTypesChar);
TYPED_TEST(MatrixStringTestChar, CheckChar)
{
  this->check();
}

TYPED_TEST_CASE(VectorStringTestDouble, VectorTypesDouble);
TYPED_TEST(VectorStringTestDouble, CheckDouble)
{
  this->check();
}

TYPED_TEST_CASE(VectorStringTestInt, VectorTypesInt);
TYPED_TEST(VectorStringTestInt, CheckInt)
{
  this->check();
}

// Additional from_string/to_string tests
TEST(StringTest, ConvertTo)
{
  EXPECT_EQ("9", to_string(from_string<int>("9")));
  EXPECT_EQ("P", to_string(from_string<char>("P")));
  EXPECT_EQ(double(0.1), from_string<double>(to_string<double>(0.1)));
  EXPECT_EQ("0.100006", to_string(from_string<double>("0.1000055511151231257827021181583404541015625")));
  EXPECT_EQ(1e-14, from_string<double>("1e-14"));
  EXPECT_EQ(1e-14, from_string<double>("1E-14"));
  EXPECT_EQ("1", to_string(from_string<bool>("1")));
  EXPECT_EQ("0", to_string(from_string<bool>("0")));

  EXPECT_EQ("-1", to_string(from_string<long>("-1")));
}

TEST(StringTest, ConvertFrom)
{
  EXPECT_EQ(9, from_string<int>("9"));
  EXPECT_EQ(0, from_string<int>("0"));
  EXPECT_EQ('p', from_string<char>(to_string('p')));
  EXPECT_EQ(-1, from_string<char>(to_string(char(-1))));
  EXPECT_THROW(from_string<char>("sd"), Dune::Stuff::Exceptions::conversion_error);
  EXPECT_EQ(true, from_string<bool>("1"));
  EXPECT_EQ(true, from_string<bool>("true"));
  EXPECT_EQ(true, from_string<bool>("True"));
  EXPECT_EQ(false, from_string<bool>("0"));
  EXPECT_EQ(false, from_string<bool>("false"));
  EXPECT_EQ(false, from_string<bool>("False"));
  EXPECT_THROW(from_string<int>(""), std::invalid_argument);
}

// Hex, whitespacify, tokenize, stringFromTime tests
TEST(StringTest, Hex)
{
// disabled the following test for Intel and newer GNU compiler because it causes compilation failures for no obvious
// reason
#ifndef __INTEL_COMPILER || (__GNUC__ < 5)
//  EXPECT_GT(boost::lexical_cast<HexToString<unsigned long> >(std::cout), 0u);
#endif
  EXPECT_EQ(boost::lexical_cast<HexToString<unsigned long>>("0x00000F"), 15u);
}

TEST(StringTest, Whitespace)
{
  EXPECT_EQ("---------", whitespaceify("\t\t\t\t\t\t\t\t\t", '-'));
}

TEST(StringTest, Tokenizer)
{
  const string seps(" \t;");
  const string msg("a t\tkk;;g");
  const vector<string> tokens_default = {"a", "t", "kk", "", "g"};
  const vector<string> tokens_compressed = {"a", "t", "kk", "g"};
  EXPECT_EQ(tokens_default, tokenize(msg, seps, boost::algorithm::token_compress_off));
  EXPECT_EQ(tokens_compressed, tokenize(msg, seps, boost::algorithm::token_compress_on));
  const string num_msg("-1 2;;4");
  vector<int> numbers_default = {-1, 2, 0, 4};
  vector<int> numbers_compressed = {-1, 2, 4};
  EXPECT_EQ(numbers_default, tokenize<int>(num_msg, seps, boost::algorithm::token_compress_off));
  EXPECT_EQ(numbers_compressed, tokenize<int>(num_msg, seps, boost::algorithm::token_compress_on));
}

TEST(StringTest, TimeString)
{
  string ts = stringFromTime(-1);
}
