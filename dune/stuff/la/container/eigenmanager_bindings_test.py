#! /usr/bin/env python

from __future__ import print_function

import sys

print('importing \'dunecontainermanager\'... ', end='')
try:
    import dunecontainermanager as dune
    print('done')
except:
    print('failed')
    sys.exit(1)

print('creating manager... ', end='')
try:
    manager = dune.Dune.Stuff.LA.Container.EigenManager()
    print('done')
except:
    print('failed')
    sys.exit(1)

print('creating vector: ', end='')
try:
    vectorX = manager.createDenseVector(10)
    print(vectorX)
except:
    print('failed')
    sys.exit(1)
