#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os,sys, time, subprocess, datetime
from PyQt4 import Qt
import PyQt4.Qwt5 as Qwt
from PyQt4.Qwt5.anynumpy import *

sample_interval = 500 #ms
csv_filename = 'watch.csv'

class DataPlot(Qwt.QwtPlot):

	def __init__(self, *args):
		Qwt.QwtPlot.__init__(self, *args)

		self.setCanvasBackground(Qt.Qt.white)
		self.alignScales()

		# Initialize data
		self.x = arange(0.0, 100.1, 0.5)
		self.y = zeros(len(self.x), Float)

		self.setTitle("Running Memory consumption plot")
		self.insertLegend(Qwt.QwtLegend(), Qwt.QwtPlot.BottomLegend);

		self.curveR = Qwt.QwtPlotCurve("Data Moving Right")
		self.curveR.attach(self)
		self.curveR.setPen(Qt.QPen(Qt.Qt.red))

		mY = Qwt.QwtPlotMarker()
		mY.setLabelAlignment(Qt.Qt.AlignRight | Qt.Qt.AlignTop)
		mY.setLineStyle(Qwt.QwtPlotMarker.HLine)
		mY.setYValue(0.0)
		mY.attach(self)

		self.setAxisTitle(Qwt.QwtPlot.xBottom, "Time (# last  )")
		self.setAxisTitle(Qwt.QwtPlot.yLeft, "Memory (kB)")
	
		self.csv = open(csv_filename, 'wb')
		args = sys.argv[1:]
		self.p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE )
		self.startTimer(sample_interval)
		self.running = True #I see no way of killing the timer...
		self.current_time = -sample_interval

	def alignScales(self):
		self.canvas().setFrameStyle(Qt.QFrame.Box | Qt.QFrame.Plain)
		self.canvas().setLineWidth(1)
		for i in range(Qwt.QwtPlot.axisCnt):
			scaleWidget = self.axisWidget(i)
			if scaleWidget:
				scaleWidget.setMargin(0)
			scaleDraw = self.axisScaleDraw(i)
			if scaleDraw:
				scaleDraw.enableComponent(
					Qwt.QwtAbstractScaleDraw.Backbone, False)
	
	def timerEvent(self, e):
		if not self.running:
			return
		if self.p.poll() != None:
			self.killTimer(sample_interval)
			step_size = self.current_time / 400.0
			self.x = arange(0.0, self.current_time  + 0.1, step_size)
			self.csv.close()
			self.csv = open( csv_filename, 'r' )
			self.y = [ int(u.split()[1]) for u in self.csv.readlines() ][1:]
			Qt.QMessageBox.information( self, "done", "Child process completed, now plotting full timeline" )
			self.setTitle("Total Memory consumption plot")
			self.running = False
		else:
			self.current_time += sample_interval
			# y moves from left to right:
			# shift y array right and assign new value y[0]
			self.y = concatenate((self.y[:1], self.y[:-1]), 1)
			with open( '/proc/%d/status'%self.p.pid, 'r' ) as mem:
					for line in mem.readlines():
						el = line.split()
						if el[0] == 'VmSize:':
							self.y[0] = int(el[1])
							self.csv.write( '%d\t%s\n'%(self.current_time, el[1]) )

		self.curveR.setData(self.x, self.y)
		self.replot()

def usage(args):
	print 'usage:\npython %s /absolute/path/to/binary [params_for_binary]*'%args[0]
	return -1

def main(args): 
	if len(args) < 2:
		sys.exit(usage(args))
	app = Qt.QApplication(args)
	demo = DataPlot()
	demo.resize(800, 600)
	demo.show()
	sys.exit(app.exec_())

if __name__ == '__main__':
	main(sys.argv)
