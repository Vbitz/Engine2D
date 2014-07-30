#!/usr/bin/env python

import os,sys,inspect
currentdir = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))
parentdir = os.path.dirname(currentdir)
sys.path.insert(0, parentdir)

from tasks import shell_command

import uuid
import sys
import argparse

def compile(sources, outputFilename):
	if outputFilename == None:
		outputFilename = "addon.dylib"
	args = ["clang++", "-dynamiclib", "-std=gnu++11",
		"-stdlib=libc++", "-lengine2D", "-lv8", "-Lbuild/Default", "-o", outputFilename]
	args += ["-DBUILD_UUID=\"" + uuid.uuid4() + "\""]
	args += sources
	shell_command(args)
	shell_command(["install_name_tool", "-change", "/usr/local/lib/libengine2D.dylib",
		"@executable_path/libengine2D.dylib", outputFilename])

parser = argparse.ArgumentParser(description='')
parser.add_argument("sources", nargs='+', help='C++ source files to compile into a library')
parser.add_argument("--output", help='Output filename')

args = parser.parse_args()

compile(args.sources, args.output)