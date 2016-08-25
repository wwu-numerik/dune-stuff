// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff/
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_COMMON_EXCEPTIONS_HH
#define DUNE_STUFF_COMMON_EXCEPTIONS_HH

#include <dune/common/exceptions.hh>
#include <dune/common/parallel/mpihelper.hh>
#include <dune/common/deprecated.hh>

namespace Dune {
namespace Stuff {
namespace Exceptions {


class CRTP_check_failed : public Dune::Exception
{
};

class shapes_do_not_match : public Dune::Exception
{
};

class index_out_of_range : public Dune::Exception
{
};

class you_are_using_this_wrong : public Dune::Exception
{
};

class wrong_input_given : public you_are_using_this_wrong
{
};

class requirements_not_met : public you_are_using_this_wrong
{
};

class configuration_error : public Dune::Exception
{
};

class conversion_error : public Dune::Exception
{
};

class results_are_not_as_expected : public Dune::Exception
{
};

class internal_error : public Dune::Exception
{
};

class external_error : public Dune::Exception
{
};

class linear_solver_failed : public Dune::Exception
{
};

class you_have_to_implement_this : public Dune::NotImplemented
{
};

class reinterpretation_error : public Dune::Exception
{
};


} // namespace Exceptions
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_EXCEPTIONS_HH
