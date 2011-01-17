#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
from csv import DictReader as CsvReader, DictWriter as CsvWriter

def beginSubfloat(out, y_axis_logarithmic = True ):
	out.write(  "\\subfloat{\n\\begin{tikzpicture}[scale=\\plotscale]\n" )
	if y_axis_logarithmic:
		out.write(  "\\begin{semilogyaxis}[\n")
	else:
		out.write(  "\\begin{axis}[\n")	
	out.write(  "legend style={ at={(1.02,1)},anchor=north west},\n")

def endSubfloat(out, y_axis_logarithmic = True, breakline = True ):
	if y_axis_logarithmic:
		out.write(  "\\end{semilogyaxis} \n\\end{tikzpicture}}")
	else:
		out.write(  "\\end{axis} \n\\end{tikzpicture}}")
	if breakline:
		out.write(  "\\\\\n")
	else:
		out.write(  "\n")

def texsafe(string):
	return string.replace( '_', '\\_' )

marks = ( "|","x","o","*","-","#","@")
colors = ( "red","blue","green","yellow","cyan","magenta","black" )

from optparse import OptionParser
parser = OptionParser()
parser.add_option("-p", "--prefix", dest="out_prefix", default='out',
                  help="output filename prefix", type='string')
parser.add_option("-r", "--runtype", dest="runtype", default='refine',
                  help="runtype", type='string')
parser.add_option("-c", "--caption", dest="caption", default='',
                  help="caption for figure", type='string')
                  
(options, args) = parser.parse_args()

out_fn = "%s.csv"%options.out_prefix
tex_fn = "%s.tex"%options.out_prefix

argnum = len(args)
scheme_count = argnum/2
csv_filenames = args[:scheme_count]
titles = args[scheme_count:]

first_col = options.runtype

prefixes = ["L2t_velocity", "L2t_pressure", "L2t_h1_velocity","maxV","maxP"]
#prefixes = ["L2t_velocity", "L2t_pressure", "L2t_h1_velocity"]
origin_column_header = [first_col] + prefixes
prefixes = [ "%s_"%x for x in prefixes ]
target_column_header = [first_col] 
for x in titles:
	for s in prefixes:
		target_column_header.append("%s%s"%(s,x))

with open(out_fn,'wb') as out_file :
	#need to read with diff file object, otherwise it'll point o fiel end in iteration below
	row_count = len(list(CsvReader( open(csv_filenames[0]) ,delimiter='\t', fieldnames=origin_column_header)))
	
	readers = dict( zip(titles, [ CsvReader( open(fn,'rb') ,delimiter='\t', fieldnames=origin_column_header ) for fn in csv_filenames ] ) )
	out_file.write( '\t'.join(target_column_header) + '\n' )
	target_csv = CsvWriter( out_file, target_column_header, delimiter='\t',extrasaction='raise' )
	
	target_rows = [ dict() for i in range(row_count)]
	for title,reader in readers.iteritems():
		for (c_id,l_row) in zip(range(row_count),reader):
			if c_id == 0:#skip header row
				continue
			for key,value in l_row.iteritems():
				if key == first_col:
					target_rows[c_id][key] = value
					continue
				t_key = "%s_%s"%(key,title)
				if not t_key in target_column_header:
					continue
				target_rows[c_id][t_key] = value
	for row in target_rows[1:]:
		target_csv.writerow( row )

with open(tex_fn,'wb') as tex_file :
	tex_file.write( '\\begin{figure}' )
	for (i,prefix) in zip(range(len(prefixes)),prefixes):
		beginSubfloat( tex_file )
		tex_file.write("xlabel=%s,\nylabel=$ERR_{%s}$]\n"%(first_col,texsafe(prefix)) )
		for (title,color,mark) in zip(titles,colors,marks):
			tex_file.write( "\\addplot[color=%s,mark=%s]\ntable[x=%s,y=%s%s] {%s};"%(color,mark,first_col,prefix,title,out_fn,) )
			tex_file.write( "\\addlegendentry{%s}\n"%( title ) )
		endSubfloat( tex_file, True, i % 2 !=0 )
	tex_file.write( '\\caption{%s}\\end{figure}\n'%texsafe(options.caption) )
