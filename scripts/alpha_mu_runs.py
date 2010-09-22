#!/usr/bin/env python

import math,sys,os,subprocess,datetime

power = 6
binary = sys.argv[1]
param = sys.argv[2]
#null = sys.stdout
null = open(os.devnull, 'w')
datadir = filter( lambda f: f.startswith( 'fem.io.datadir' ), open(param).readlines() )[0].split(':')[1].strip()

for alpha_power in range(-power,power+1): 
	for mu_power in range(-power,power+1):
		cmd = [ './%s'%binary, 'paramfile:%s'%param, 'viscosity:%f'%math.pow(10,mu_power), 'alpha:%f'%math.pow(10,alpha_power) ]
		print datetime.datetime.now(),cmd
		subprocess.call(cmd, bufsize=4096, stdout=null, stderr=null)
		ko = open('%s/all.tex'%datadir, 'a' )
		ko.write( open('%s/eoc-file_body.tex'%datadir).read() )
		ko.close()
