// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2013 - 2015)
//   Rene Milk       (2014 - 2015)
//   Tobias Leibner  (2014)

#include "main.hxx"
#include "functions.hh"

#include <memory>

#include <dune/common/exceptions.hh>

#include <dune/stuff/functions/interfaces.hh>
#include <dune/stuff/functions/checkerboard.hh>                                                              

#if HAVE_DUNE_GRID
#include <dune/grid/yaspgrid.hh>
#if HAVE_ALUGRID
#include <dune/grid/alugrid.hh>
#endif //HAVE_ALUGRID

struct CheckerboardFunctionTest : public DS::FunctionTest< TESTFUNCTIONTYPE >
{
    void check() const
    {
        const std::unique_ptr<const FunctionType> function(
                    FunctionType::create(FunctionType::default_config()));
    }
};

TEST_F(CheckerboardFunctionTest, provides_required_methods) { this->check(); }

#else // HAVE_DUNE_GRID

TEST(DISABLED_CheckerboardFunctionTest, provides_required_methods) {}

#endif // HAVE_DUNE_GRID
