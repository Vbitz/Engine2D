#!/usr/bin/env python

import re
import bindingGenerator
import json

def read_file(filename):
	with open(filename, "r") as f:
		return f.read()

javaDocRegex = re.compile("(/\*\*(([^\*]|\*(?!/))*?.*?)\*/)\n([\w.]*) = function (\w*)\((.*)\) {};")
jDocReturnFragment = re.compile("@return\s+\{([\w\.\*\[\]|]+)\}")
jDocParamFragment = re.compile("@param\s+\{([\w\.\*\[\]|]+)\}(\s+([\[\]\w]+)(\s+(.*))?)?")
jDocDeprecatedFragment = re.compile("@deprecated (.*)")

def parse_file(filename):
	f = read_file(filename)
	for match in re.finditer(javaDocRegex, f):
		fullStr, javaDoc, e1, functionName, e2, functionArgs = match.groups()
		functionPath = functionName
		functionNamespace = ""
		functionName = functionName.strip().split(".")[1:]
		if len(functionName) > 1:
			functionNamespace, functionName = functionName
		else:
			functionName = functionName[0]
		functionArgs = functionArgs.strip().split(", ")
		javaDoc = javaDoc.strip().split("\n *")
		javaDoc = map(lambda l: l.lstrip(" *"), javaDoc)[1:]

		functionParams = []
		functionReturnType = "void"

		for jDocFragment in javaDoc:
			if not jDocFragment.startswith("@"):
				continue
			if jDocFragment.startswith("@example"):
				break
			#print jDocFragment
			match = re.match(jDocReturnFragment, jDocFragment)
			if match != None:
				functionReturnType = match.groups()[0]
				continue
			match = re.match(jDocParamFragment, jDocFragment)
			if match != None:
				paramType, i1, paramName, i2, paramDoc = match.groups()
				paramType = paramType.split("|")
				if paramDoc == None:
					paramDoc = ""
				functionParams.append([paramType[0], paramName, paramDoc])
				continue
			match = re.match(jDocDeprecatedFragment, jDocFragment)
			if match != None:
				#print "isDeprecated", match.groups()
				deprecatedReason = match.groups()[0]
				continue
			if jDocFragment == "@internal":
				#print "ignore"
				continue
			raise Exception("Bad JavaDoc Fragment")

		functionSpec = {
			"path": functionPath,
			"flags": [],
			"args": functionParams
		}

		print json.dumps(functionSpec)

		print bindingGenerator.compileMethod("", functionSpec, functionName)

def main():
	parse_file("../doc/api.js")

if __name__ == '__main__':
	main()