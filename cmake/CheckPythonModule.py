#! /usr/bin/env python
#
# This file is part of the dune-stuff project:
#   https://github.com/wwu-numerik/dune-stuff
# The copyright lies with the authors of this file (see below).
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
# Authors:
#   Felix Schindler (2013, 2015)
#   Rene Milk       (2015)

import sys
try:
    X = __import__(sys.argv[1])
except:
    sys.exit(1)
sys.exit(0)
