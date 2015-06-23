```
# This file is part of the dune-stuff project:
#   https://github.com/wwu-numerik/dune-stuff
# Copyright holders: Rene Milk, Felix Schindler
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
```

Thank you for your interest in dune-stuff!

# Help

If you are experiencing problems or
would like to get help, open up a new [issue](https://github.com/wwu-numerik/dune-stuff/issues)
and tag it accordingly. Please note that dune-stuff is not one of the core
DUNE modules. Thus, you will not get any support from the official channels
(such as the DUNE mailinglist, the DUNE bugtracker, etc...).
When submitting bugs, please read these [general guidellines](http://users.dune-project.org/projects/main-wiki/wiki/Guides_bug_reporting)
beforehand.

# Contributing

We very much encourage and welcome contributions!
If you would like to contribute to dune-stuff, please fork the repository and
open up a pull request, explaining your changes and why they should be included.

# Coding style

We do not strictly enforce a coding style at the moment.
However, when editing existing files (or if in doubt), take a look around and try to
stick to the surrounding coding style. In general (or when creating new files), we
try to stick to the rules below.
Please only commit white space changes of existing code separately and provide an
appropriate commit message, e.g., if editing `dune/stuff/common/vector.hh`:
```
[common.vector] whitespace
```

## whitespace and indentation

* use spaces instead of tabs
* indentation width is 2 spaces
* do not indent within namespaces
* leave 2 blank lines between namespaces and enclosed stuff
* leave 1 blank line between namespaces and external stuff
```c++
#include "header.hh";

namespace Foo {
namespace Bar {


struct IamNotIndented {};


} // namespace Bar
} // namespace Foo
```

## cmake

Lowercase calls, one whitespace before `()`, none within, e.g.
```
if (NOT "foo")
```
instead of
```
IF( NOT "foo" )
```

