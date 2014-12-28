#!/usr/bin/env python

import uuid
import sys
import argparse
import os
import subprocess

import tasks

def compile(sources, outputFilename, link_v8=False, addedArgs=[]):
	if outputFilename == None:
		outputFilename = "addon." + tasks.get_lib_postfix()
	if addedArgs == None:
		addedArgs = []
	args = ["clang++", "-dynamiclib", "-std=gnu++11", "-D_FORTIFY_SOURCE=2", "-pthread", "-fPIC",
		"-stdlib=libc++", "-lengine2D", "-L" + tasks.resolve_path(tasks.PROJECT_BUILD_PATH, ""), "-o", outputFilename]
	args += ["-D_BUILD_UUID=\"" + str(uuid.uuid4()) + "\""]
	if link_v8:
		args += ["-lv8", "-Ithird_party/v8"]
	args += addedArgs
	args += sources
	tasks.shell_command(args)
	if tasks.is_osx():
		tasks.shell_command(["install_name_tool", "-change", "/usr/local/lib/libengine2D.dylib",
			"@executable_path/libengine2D.dylib", outputFilename])
		if link_v8:
			tasks.shell_command(["install_name_tool", "-change", "/usr/local/lib/libv8.dylib",
				"@executable_path/libv8.dylib", outputFilename])

def main(args):
	parser = argparse.ArgumentParser(description='Build script for Engine2D addons')
	parser.add_argument("sources", nargs='+', help='C++ source files to compile into a library')
	parser.add_argument("-o", help='Output filename', metavar='filename')
	parser.add_argument("-a", nargs='+', help='Add additional arguments to the build command-line', metavar='arg')
	parser.add_argument("--link-v8", default=False, help='Links V8', action='store_true')

	args = parser.parse_args()

	compile(args.sources, args.o, args.link_v8, args.a)

if __name__ == '__main__':
  main(sys.argv);