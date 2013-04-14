#! /usr/bin/env python

import sys, os, shutil
import pybindgen
from pybindgen.gccxmlparser import ModuleParser
from pybindgen import FileCodeSink
#from pygccxml.declarations.class_declaration import class_t
#from pybindgen.typehandlers import base as typehandlers
#from pybindgen import ReturnValue, Parameter, Module, Function, FileCodeSink
#from pybindgen import CppMethod, CppConstructor, CppClass, Enum
#from pygccxml.declarations.calldef import free_function_t, member_function_t, constructor_t, calldef_t

def my_module_gen(inputdir, outputdir, includedirs):
    tmpincludedirs = includedirs
    includedirs = [tmpincludedirs[0]]
    includedirs.extend(tmpincludedirs[1].split(';'))
    generator_fn = os.path.join(outputdir, 'dunecontainermanager.cc')
    module_parser = ModuleParser('dunecontainermanager')

    with open(generator_fn, 'wb') as output:
        module = module_parser.parse([os.path.join(inputdir, 'manager.hh')],
                                     include_paths=includedirs,
                                     includes=['"manager.hh"'])
        #gccxml_options={'cflags':'-std=c++0x', 'compiler':'gcc-4.6'})
        module.generate(FileCodeSink(output))

    # check if generate() was successfull
    print os.stat(generator_fn).st_size

if __name__ == '__main__':
    my_module_gen(sys.argv[1], sys.argv[2], sys.argv[3:])
