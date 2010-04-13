#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os,sys

default_prefixes = ( 'computed__velocity', 'computed__pressure' )
ext = '.vtu'
path = os.getcwd()

pvd_header = '''<?xml version="1.0"?>
<VTKFile type="Collection" version="0.1" byte_order="LittleEndian">
	<Collection>
'''
pvd_footer = '''
	</Collection>
</VTKFile>'''

def writePVD(prefixes):
	for pref in prefixes:
		'only consider files named PREFIXsome_numberEXT'
		try:
			cutoff = int(sys.argv[1])
		except:
			cutoff = None
		fn = '%s.pvd'%pref
		print fn
		if cutoff:
			print '\t\tcutoff: %d'%cutoff
		files = filter( lambda p:  p.startswith( pref ) and p.endswith( ext ) , os.listdir( path ) )
		files.sort()
		with open( fn, 'wb' ) as pvd:
			pvd.write( pvd_header )
			for f in files:
				try:
					'use some_number as timestep'
					step = int( f[f.find(pref)+len(pref):f.find(ext)] )
					if cutoff and step > cutoff:
						break
					pvd.write( '\t\t<DataSet timestep="%d" group="" part="0" file="%s" />\n'%(step, f ) )
				except ValueError:
					print '\tinvalid filename: %s'%f
			pvd.write( pvd_footer )

if __name__=='__main__':
	if len(sys.argv) > 2:
		writePVD( sys.argv[2:] )
	else:
		writePVD( default_prefixes )