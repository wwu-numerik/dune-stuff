#!/usr/bin/env pvbatch
from paraview.simple import *
import argparse
import os 

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
print args

servermanager.LoadState(args.state)

AnimationScene1 = GetAnimationScene()

writer = servermanager.vtkSMAnimationSceneImageWriter()
writer.SetFileName(args.filename)
writer.SetFrameRate(1)
writer.SetAnimationScene(AnimationScene1.SMProxy)
if not writer.Save():
	raise exceptions.RuntimeError, "Saving of animation failed!"
