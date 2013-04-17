#! /usr/bin/env python

import sys, os
#, shutil
import pybindgen
#from pybindgen.gccxmlparser import ModuleParser
from pybindgen import FileCodeSink
#from pygccxml.declarations.class_declaration import class_t
#from pybindgen.typehandlers import base as typehandlers
#from pybindgen import ReturnValue, Parameter, Module, Function, FileCodeSink
#from pybindgen import CppMethod, CppConstructor, CppClass, Enum
#from pygccxml.declarations.calldef import free_function_t, member_function_t, constructor_t, calldef_t

def generate_my_module(inputdir, outputdir, includedirs):
    includedirs = includedirs[0].split(';')
    generator_filename = os.path.join(outputdir, 'dunecontainermanager.cc')
    module = pybindgen.Module('dunecontainermanager')
    module.add_include('"eigenmanager.hh"')
    #module.add_function('run', None, [])
    namespace = module.add_cpp_namespace('Dune').add_cpp_namespace('Stuff').add_cpp_namespace('LA').add_cpp_namespace('Container')
    EigenManager = namespace.add_class('EigenManager')
    EigenManager.add_constructor([])
    EigenManager.add_method('createDenseVector',
                            pybindgen.retval('std::string'),
                            [pybindgen.param('int', 'size')])
    EigenManager.add_method('dot',
                            pybindgen.retval('std::string'),
                            [pybindgen.param('std::string', 'x'),
                             pybindgen.param('std::string', 'y')])

    with open(generator_filename, 'wb') as output:
        module.generate(FileCodeSink(output))

    ## check if generate() was successfull
    #print os.stat(generator_fn).st_size

if __name__ == '__main__':
    generate_my_module(sys.argv[1], sys.argv[2], sys.argv[3:])
