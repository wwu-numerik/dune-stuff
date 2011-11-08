#!/usr/bin/env pvbatch
from paraview.simple import *
import os 

try:
	import argparse
	def make_abs(input_path):
		abspath = os.path.abspath(input_path)
		if not os.path.exists( abspath ):
			raise Exception('required absolute path %s not found'%abspath)
		return abspath
	parser = argparse.ArgumentParser(description='output animation')
	parser.add_argument('state', metavar='state', type=make_abs, nargs='?',
			   help='paraview state file')
	parser.add_argument('-o','--output', type=str, nargs='?',default='animation.png',
			   help='output file template', metavar='filename')
	args = parser.parse_args()
	state = args.state
	outfile = args.filename
except ImportError:
	import sys
	try:
		state = sys.argv[1]
		outfile = sys.argv[2]
	except IndexError, e:
		print('python < 2.7 and too few input args')
		sys.exit(-1)	
	
servermanager.LoadState(state)

AnimationScene1 = GetAnimationScene()

writer = servermanager.vtkSMAnimationSceneImageWriter()
writer.SetFileName(outfile)
writer.SetFrameRate(1)
writer.SetAnimationScene(AnimationScene1.SMProxy)
if not writer.Save():
	raise exceptions.RuntimeError, "Saving of animation failed!"
