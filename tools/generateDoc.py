#!/usr/bin/env python2.7
import re
import sys

regexStr = "\n([ \t]+)((([\w\\*\&\<\>\:]+) )+)?([\w:~]+)\((.*)\);?"

matchMethods = re.compile(regexStr)

def buildMethodDoc(match):
	orig = match.group(0)
	whitespace, _, sig, typeName, methodName, args = match.groups()
	methodName = methodName.split("::")
	args = args.split(", ") if args != None else []
	outputStrArr = []
	outputStrArr += ["<TODO: Write method brief>"]
	for arg in args:
		if len(arg) == 0:
			continue
		arg = arg.split(" ")[-1:][0]
		outputStrArr += ["@param " + arg + " <TODO: Write arg brief>"]
	outputStrArr += ["@return <TODO: Write return brief>"]
	return "\n" + whitespace + "/**\n" + "\n".join(map(lambda s: whitespace + " * " + s, outputStrArr)) + "\n" + whitespace + " */" + orig

def getMethodList(string):
	return matchMethods.sub(buildMethodDoc, string)

if __name__ == '__main__':
	with open(sys.argv[1], "r") as f:
		fileContent = f.read()
		print getMethodList(fileContent)
