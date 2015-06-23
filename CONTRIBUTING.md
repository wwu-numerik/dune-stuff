```
# This file is part of the dune-stuff project:
#   https://github.com/wwu-numerik/dune-stuff
# Copyright holders: Rene Milk, Felix Schindler
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
```

# Coding style

# whitespace and indentation

* use whitespace instead of tabs
* indentation width is 2 white spaces
* do not indent wihtin namespaces
* leave 2 blank lines between namespaces and enclosed stuff
* leave 1 blank line between namespaces and external stuff
```c++
#include "header.hh";

namespace Foo {
namespace Bar {


struct IamNotIndented {};


} // namespace Foo
} // namespace Bar
```

## cmake

Lowercase calls, one whitespace before `()`, none within, uppercase within `()`, e.g.
```
if (NOT "foo")
```
instead of
```
IF( not "foo" )
```

