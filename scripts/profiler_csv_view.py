#!/usr/bin/env python
'''
Created on Nov 29, 2012

@author: Rene Milk
'''
from __future__ import division
import pylab as pl
import pprint
import hashlib
import colorsys

def do_stuff(values):
    '''
    '''
    dataB = sorted([(a.replace('_sum', '').lower(), b) for a,b in values if a != 'run' and '_avg' not in a], reverse=True)
    values = [float(a) for _,a in dataB]
    keys = [b for b,_ in dataB]
    max_components = max([len(x.split('.')) for x in keys])

    def str_to_rgb(s):
        cp = s.split('.')
        inc = 1 / max_components
        hs = hashlib.md5(cp[0]).hexdigest()
        h = int(hs[:2], 16) / 255
        v = int(hs[2:4], 16) / 255
        s = 1 - (len(cp) * inc)
        return colorsys.hsv_to_rgb(h,s,v)
    colors = [str_to_rgb(a) for a,_ in dataB]
    pprint.pprint(colors)
    data_count = len(values)
    pprint.pprint(values)
    pl.barh(pl.arange(data_count)+0.6, values, color=colors, height=0.2, )
    pl.yticks(pl.arange(data_count) + 0.4, keys, x=0.02, ha='left')
    pl.show()

if __name__ == '__main__':
    import csv
    import sys
    delimiter = ','
    fn = sys.argv[1]
    csv_reader = csv.DictReader( open(fn,'rb'), delimiter=delimiter )
    values = [row for row in csv_reader]
    fieldnames = csv_reader.fieldnames
    do_stuff(values[0].items())
    