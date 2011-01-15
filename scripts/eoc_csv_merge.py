#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
from csv import DictReader as CsvReader, DictWriter as CsvWriter

out_fn = "out.csv"
tex_fn = "out.tex"

argnum = len(sys.argv) -1
scheme_count = argnum/2
csv_filenames = sys.argv[1:scheme_count+1]
titles = sys.argv[1+scheme_count:]

prefixes = ["L2t_velocity", "L2t_pressure", "L2t_h1_velocity"]
origin_column_header = ["refine"] + prefixes
prefixes = [ "%s_"%x for x in prefixes ]
print prefixes
target_column_header = ["refine"] 
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
			print l_row,title
			if c_id == 0:#skip header row
				continue
			for key,value in l_row.iteritems():
				if key == "refine":
					target_rows[c_id][key] = value
					continue
				t_key = "%s_%s"%(key,title)
				if not t_key in target_column_header:
					continue
				target_rows[c_id][t_key] = value
	for row in target_rows[1:]:
		target_csv.writerow( row )
