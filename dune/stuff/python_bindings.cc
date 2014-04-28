// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff/
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "config.h"

#include <iostream>

#include <boost/python.hpp>


namespace Dune {
namespace Stuff {
namespace LA {


class Foo
{
public:
  Foo(const int)
  {}

  void hello() const
  {
    std::cout << "Hello world!" << std::endl;
  }
};


}
namespace LL {


class Bar
{
public:
  Bar(const int)
  {}

  void hello() const
  {
    std::cout << "Hello world!" << std::endl;
  }
};


}
}
}


BOOST_PYTHON_MODULE(libdunestuffpythonbindings)
{
  boost::python::object module_namespace = boost::python::scope();
  module_namespace.attr("__path__") = "libdunestuffpythonbindings";

  boost::python::object la_namespace(boost::python::handle<>(boost::python::borrowed(PyImport_AddModule("libdunestuffpythonbindings.la"))));
  boost::python::object ll_namespace(boost::python::handle<>(boost::python::borrowed(PyImport_AddModule("libdunestuffpythonbindings.ll"))));
  boost::python::scope().attr("la") = la_namespace;
  boost::python::scope().attr("ll") = ll_namespace;

  boost::python::scope la_scope = la_namespace;

  boost::python::class_< Dune::Stuff::LA::Foo >("Foo", boost::python::init< const int >())
      .def("hello", &Dune::Stuff::LA::Foo::hello);

  boost::python::scope ll_scope = ll_namespace;

  boost::python::class_< Dune::Stuff::LL::Bar >("Bar", boost::python::init< const int >())
      .def("hello", &Dune::Stuff::LL::Bar::hello);


} // BOOST_PYTHON_MODULE
