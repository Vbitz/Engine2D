#!/usr/bin/env python

import re
import bindingGenerator
import json
import sys

def read_file(filename):
	with open(filename, "r") as f:
		return f.read()

javaDocRegex = re.compile("(/\*\*(([^\*]|\*(?!/))*?.*?)\*/)\n([\w.]*) = function (\w*)\((.*)\) {};")
jDocReturnFragment = re.compile("@return\s+\{([\w\.\*\[\]|]+)\}")
jDocParamFragment = re.compile("@param\s+\{([\w\.\*\[\]|]+)\}(\s+([\[\]\w]+)(\s+(.*))?)?")
jDocBindingFragment = re.compile("@bind (.*)")
jDocDeprecatedFragment = re.compile("@deprecated (.*)")

def parse_file(filename, signaturesOnly):
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

		# Valid binding types are:
		# 	"smart": Automaticly create a method signature based on the paramters
		# 	"smart_noContext": Automaticly create a method signture without a arguments wrapper
		# 	"none": Don't generate a binding
		# 	anything else: Generate a method signature with a specfiyed name and namespace
		bindingType = "smart"

		for jDocFragment in javaDoc:
			if not jDocFragment.startswith("@"):
				continue

			if jDocFragment.startswith("@example"):
				break

			if jDocFragment == "@internal":
				#print "ignore"
				continue
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
				if paramName == None:
					continue # WTF
				paramName = paramName.strip("[]")
				functionParams.append([paramType[0], paramName, paramDoc])
				continue

			match = re.match(jDocDeprecatedFragment, jDocFragment)
			if match != None:
				#print "isDeprecated", match.groups()
				deprecatedReason = match.groups()[0]
				continue

			match = re.match(jDocBindingFragment, jDocFragment)
			if match != None:
				bindingType = match.groups()[0]
				continue

			raise Exception("Bad JavaDoc Fragment")

		functionSpec = {
			"path": functionPath,
			"bindingType": bindingType,
			"returns": functionReturnType,
			"flags": [],
			"args": functionParams,
			"signaturesOnly": signaturesOnly
		}

		print bindingGenerator.compileMethod("", functionSpec, functionName)

def main():
	parse_file("../doc/api.js", (len(sys.argv) > 1) and (sys.argv[1] == "signaturesOnly"))

if __name__ == '__main__':
	main()