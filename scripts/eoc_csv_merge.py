#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
from csv import DictReader as CsvReader, DictWriter as CsvWriter

out_fn = "out.csv"

argnum = len(sys.argv) -1
scheme_count = argnum/2
csv_filenames = sys.argv[1:scheme_count+1]
#print csv_filenames 
titles = sys.argv[1+scheme_count:]
#print titles

scheme_to_fn = dict( zip(titles,csv_filenames) )
#print scheme_to_fn

origin_column_header = ["refine", "EOC_velocity", "EOC_velocity_avg", "EOC_pressure", "EOC_pressure_avg", "EOC_velocity_h1"]

target_column_header = ["refine"] 
for x in titles:
	target_column_header.append("EOC_velocity_%s"%x)
	target_column_header.append("EOC_pressure_%s"%x)
 
print target_column_header

with open(out_fn,'wb') as out_file :
	readers = dict( zip(titles, [ CsvReader( open(fn,'rb'),delimiter='\t', fieldnames=origin_column_header ) for fn in csv_filenames ] ) )
	#print readers
	out_file.write( '\t'.join(target_column_header) + '\n' )
	target_csv = CsvWriter( out_file, target_column_header, delimiter='\t' )
	
