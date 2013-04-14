#! /usr/bin/env python

import os
import sys

sys.path.append(os.getcwd())

if __name__ == '__main__':
    print os.getcwd()

    import dunecontainermanager as dune

    container = dune.ContainerManager()

    vectorID = container.createDenseVector(10)
    matrixID = container.createDenseMatrix(10, 10)

    # x <- Ax
    container.trmv(vectorID, matrixID)
