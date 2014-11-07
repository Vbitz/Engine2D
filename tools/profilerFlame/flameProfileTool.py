#!/usr/bin/env python

import sys
import json

MAX_WIDTH = 1000

def drawRect(x, y, width, height, title):
	color = "%s, %s, %s" % (y, y, y)
	print ("<rect x=\"%s\" y=\"%s\" width=\"%s\" height=\"%s\"" + \
		" fill=\"rgb(%s)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\"><title>%s</title></rect>") % (x, y, width, height, color, title)

def parseProfile(profile, scale, x, y):
	if (scale == 0):
		scale = MAX_WIDTH / profile["total"]
	drawRect(x, y, profile["total"] * scale, 20, profile["name"].replace("<", " ").replace(">", " ").replace("&", " "))
	currentPos = x
	for child in profile["children"]:
		parseProfile(profile["children"][child], scale, currentPos, y + 25)
		currentPos += profile["children"][child]["total"] * scale

def main(args):
	with open(args[1], "r") as f:
		profileJson = json.loads(f.read())
		profileJson = profileJson["children"]["FrameScope"]["children"]["DrawScope"]
		print "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" height=\"300\" width=\"%s\">" % (MAX_WIDTH + 50)
		parseProfile(profileJson, 0, 10, 10)
		print "</svg>"

if __name__ == '__main__':
	main(sys.argv)