#!/usr/bin/env python2.7

import sys
import re
import random
import os

def comment_remover(text):
	def replacer(match):
		s = match.group(0)
		if s.startswith('/'):
			return " " # note: a space and not an empty string
		else:
			return s
	pattern = re.compile(
		r'//.*?$|/\*.*?\*/|\'(?:\\.|[^\\\'])*\'|"(?:\\.|[^\\"])*"',
		re.DOTALL | re.MULTILINE
	)
	return re.sub(pattern, replacer, text)

def main(args):
	dirPath = args[1]
	srcFiles = [ f for f in os.listdir(dirPath) if os.path.isfile(os.path.join(dirPath, f)) ]
	srcFiles = [ "src/" + f for f in srcFiles if f != ".DS_Store" ]
	filename = srcFiles[random.randrange(len(srcFiles))]
	with open(filename, "r") as f:
		fileLines = comment_remover(f.read()).split("\n")
		fileLines = [x for x in fileLines if len(x) > 0 and not x.strip().startswith("#include")]
		print "=== %s ===" % filename
		startIndex = random.randrange(0, len(fileLines) - 5)
		print "\n".join(fileLines[startIndex:startIndex + 5])

if __name__ == '__main__':
	main(sys.argv)