#!/usr/bin/env python

fn = 'timeseries.csv'
col = 'L2-Pres_1'
scale = 10
import csv

out = open( 'new_'+fn, 'wb' )
csv_reader = csv.DictReader( open(fn,'rb'),delimiter='\t' )
for row in csv_reader:
	pass
fieldnames = csv_reader.fieldnames

csv_reader = csv.DictReader( open(fn,'rb'),delimiter='\t' )
out.write( '\t'.join(fieldnames) + '\n' )
csv_writer = csv.DictWriter( out,fieldnames,delimiter='\t' )

for row in csv_reader:
	copy = row
	copy[col] = str(scale * float(copy[col]))
	csv_writer.writerow(copy)
out.flush()

	
