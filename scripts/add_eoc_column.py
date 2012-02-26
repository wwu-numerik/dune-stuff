#!/usr/bin/env python
import csv
import sys
import os
import math
import copy

out_fn = sys.argv[2]
with open( out_fn, 'wb' ) as out:
	fn = sys.argv[1]
	delimiter = ','
	csv_reader = csv.DictReader( open(fn,'rb'), delimiter=delimiter )
	for row in csv_reader:
		pass
	fieldnames = csv_reader.fieldnames

	fieldnames += ['EOC_velocity', 'EOC_pressure']
	csv_reader = csv.DictReader( open(fn,'rb'), delimiter=delimiter )
	#out.write( '\t'.join(fieldnames) + '\n' )
	writer = csv.DictWriter( out, fieldnames, delimiter=delimiter )
	writer.writeheader()
	lastRow = None
	for row in csv_reader:
		u_eoc = 'nan'
		p_eoc = 'nan'
		if lastRow:
			u_last = float(lastRow['L2_0'])
			p_last = float(lastRow['L2_1'])
			h_last = float(lastRow['grid_width'])
			u_this = float(row['L2_0'])
			p_this = float(row['L2_1'])
			h_this = float(row['grid_width'])
			u_eoc = math.log(u_this/u_last) / math.log(h_this/h_last)
			p_eoc = math.log(p_this/p_last) / math.log(h_this/h_last)
			new = [str(u_eoc), str(p_eoc)]
		row['EOC_velocity'] = u_eoc
		row['EOC_pressure'] = p_eoc
		writer.writerow(row)
		
		lastRow = copy.deepcopy(row)
	out.flush()

os.system( "sed -i 's/L2_0/L2_velocity/g' %s"%out_fn )
os.system( "sed -i 's/L2_1/L2_pressure/g' %s"%out_fn )