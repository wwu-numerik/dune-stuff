#!/usr/bin/env python
# -*- coding: utf-8 -*-
from __future__ import absolute_import, division, print_function
import os
import sys
import subprocess
from collections import defaultdict

sample_interval = 2000 #ms
csv_filename = 'watch.csv'

class DataPlot(object):

  def __init__(self):
    self.data = defaultdict(list)
    args = sys.argv[1:]
    self.p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE )
    self.current_time = 0

  def run(self):
    while self.p.poll() is None:
      with open( '/proc/%d/status'%self.p.pid, 'r' ) as mem:
        for line in mem.readlines():
          el = line.split()
          if len(el) > 1 and el[0].startswith('Vm'):
            self.data[el[0][:-1]].append(str(el[1]))
      self.current_time += sample_interval
    with open(csv_filename, 'wb') as csv:
      csv.write(','.join(self.data.keys()))
      csv.write('\n')
      #max_values = ','.join([str(max(l)) for l in self.data.values()])
      #csv.write(max_values)
      #avg_values = ','.join([str(sum(l) / len(l)) for l in self.data.values()])
      #csv.write(avg_values)
      for i in range(min(len(v) for v in self.data.values())):
        csv.write(','.join([value[i] for value in self.data.values()]))
        csv.write('\n')
    return self.p.poll() or 0

def usage(args):
	print('usage:\npython %s /absolute/path/to/binary [params_for_binary]*'%args[0])
	return -1

def main(args): 
	if len(args) < 2:
		sys.exit(usage(args))
	demo = DataPlot()
	sys.exit(demo.run())

if __name__ == '__main__':
	main(sys.argv)
