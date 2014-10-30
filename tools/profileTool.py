#!/usr/bin/env python

import sys
import json

def parseProfile(profile, ws):
	print (ws + profile["name"]).ljust(125, " ") + " : " + "{0:.20f}".format(profile["self"])
	for child in profile["children"]:
		parseProfile(profile["children"][child], ws + "  ")

def main(args):
	with open(args[1], "r") as f:
		profileJson = json.loads(f.read())
		profile = profileJson[0]
		parseProfile(profile, "")

if __name__ == '__main__':
	main(sys.argv)