#!/usr/bin/env python

#   Filename: tasks.py
#   Purpose:  Build helper tools for gyp and documentation
#
#   Part of Engine2D
#
#   Copyright (C) 2014 Vbitz
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

import sys;
import os;
import subprocess;
import re;
import shutil;

PROJECT_ROOT = 0;
PROJECT_SOURCE = 1;
PROJECT_BUILD_PATH = 2;

CTAGS_PATH = "/usr/local/bin/ctags";

WINDOW_SYSTEM = "glfw";

commands = {};

def noop():
	return True;

class command(object):
	def __init__(self, requires=[], usage="TODO: ", check=noop):
		self.requires = requires;
		self.usage = usage;
		self.check = check;
	
	def __call__(self, func):
		commands[func.__name__] = func;
		commands[func.__name__].requires = self.requires;
		commands[func.__name__].usage = self.usage;
		commands[func.__name__].check = self.check;
		commands[func.__name__].run = False;
		return func;

def get_exe_name():
	if sys.platform == "darwin":
		return "engine2D";

def resolve_path(base, path=""):
	if base == PROJECT_ROOT:
		return os.path.relpath(path);
	elif base == PROJECT_SOURCE:
		return os.path.join(resolve_path(PROJECT_ROOT, "src"), path);
	elif base == PROJECT_BUILD_PATH:
		return os.path.join(resolve_path(PROJECT_ROOT, "build/Default"), path)
	else:
		raise "Invalid base";

def require_in_path(arr):
	for x in arr:
		pass;
	return True;

def shell_command(cmd, throw=True):
	print("shell : %s" % (cmd));
	if throw:
		subprocess.check_call(cmd);
	else:
		subprocess.call(cmd);

def run_engine(args):
	shell_command([resolve_path(PROJECT_BUILD_PATH, get_exe_name())] + args);

def get_git_hash():
	# check for .git folder
	return subprocess.check_output(["git", "rev-parse", "HEAD"])[:10];

@command(usage="Downloads a local copy of SVN if we can't find one")
def fetch_svn():
	try:
		require_in_path(["svn"]);
	except Exception, e:
		# check for SVN in path, if we can find it then put it in the path
		print("SVN not found: " + str(e));
		# download a standalone copy of SVN and put it in third_party/svn then put it in the path
		raise e
	# at this point we have SVN in the path and can download GYP
	pass;

@command(usage="Downloads a local copy of CMake if we can't find one")
def fetch_cmake():
	pass;

@command(requires=["fetch_svn"], usage="Downloads the latest version of GYP using SVN")
def fetch_gyp():
	# check for GYP in third_party first
	# if it's not there then download it using SVN
	pass;

@command(requires=["fetch_gyp"], usage="Builds platform project files")
def gyp():
	# run GYP
	shell_command([
			resolve_path(PROJECT_ROOT, "third_party/gyp/gyp"),
			"--depth=0",
			"-DWINDOW=" + WINDOW_SYSTEM,
			resolve_path(PROJECT_ROOT, "engine2D.gyp")
		]);

def check_depends():
	return True;

@command(usage="Fetch and install GLM")
def fetch_glm():
	# both platforms need http://sourceforge.net/projects/ogl-math/files/glm-0.9.5.1/glm-0.9.5.1.zip/download (4071873 bytes)
	# extracts to glm
	# copy entire glm/include folder to thrid_party/include
	pass;

@command(usage="Fetch a freetype binary distribution or compile one")
def fetch_freetype():
	# osx comes with freetype already installed
	if sys.platform == "win32": # windows
		# windows needs http://sourceforge.net/projects/gnuwin32/files/freetype/2.3.5-1/freetype-2.3.5-1-lib.zip/download
		# and also http://sourceforge.net/projects/gnuwin32/files/freetype/2.3.5-1/freetype-2.3.5-1-bin.zip/download
		pass;

@command(requires=["fetch_cmake"], usage="Fetch and build glfw3 using cmake")
def fetch_glfw3():
	# both platforms need https://github.com/glfw/glfw/archive/3.0.3.zip (400,137 bytes)
	# extracts to glfw-3.0.3
	# run cmake -DGLFW_BUILD_TESTS=false . in glfw-3.0.3
	# run make 
	# copy include/GLFW to third_party/include
	# copy src/libglfw3.a to third_party/lib
	pass;

@command(requires=["fetch_cmake"], usage="Fetch and build physfs using cmake")
def fetch_physfs():
	# both platforms need https://icculus.org/physfs/downloads/physfs-2.0.2.tar.gz (671,700 bytes)
	# extracts to physfs-2.0.2
	# run cmake . in physfs-2.0.2
	# run make (takes no time at all)
	# copy physfs.h to third_party/include
	# copy libphysfs.a to third_party/lib
	pass;

@command(requires=["fetch_svn"], usage="Fetch and build V8 using GYP")
def fetch_v8():
	# both platforms need https://github.com/v8/v8/archive/3.21.17.zip (16,949,393 bytes)
	# the zip file extracts to v8-3.21.17
	# change directory to third_party/depend_src/v8-3.21.17
	# svn co http://gyp.googlecode.com/svn/trunk build/gyp (pretty small)

	if sys.platform == "win32": # windows
		# svn co http://src.chromium.org/svn/trunk/deps/third_party/cygwin@231940 third_party/cygwin
		# python build\gyp_v8
		# "c:\Program Files (x86)\Microsoft Visual Studio 9.0\Common7\IDE\devenv.com" /build Release build\All.sln
		# copy the relevent files to third_party/lib and third_party/include
		pass
	elif sys.platform == "darwin": #osx
		# make native i18nsupport=off (takes bloody ages)
		pass

@command(usage="Fetch a freeimage binary distribution")
def fetch_freeimage():
	if sys.platform == "win32":
		# windows needs http://downloads.sourceforge.net/freeimage/FreeImage3154Win32.zip
		pass;
	elif sys.platform == "darwin": #osx
		# osx needs http://downloads.sourceforge.net/freeimage/FreeImage3154.zip (5,513,923 bytes)
		# extracts to FreeImage using unzip -aa to make sure patching works
		# Patch FreeImage/Makefile.gnu with third_party/patches/FreeImage_osx.patch
		# Patch FreeImage/Source/OpenEXR/IlmImf/ImfAutoArray.h with third_party/patches/FreeImage_OpenEXR_osx.patch
		# run make -p Makefile.gnu inside of FreeImage
		# Copy the files from dist to third_party/lib and third_party/include
		pass;

@command(requires=["fetch_glm", "fetch_freetype", "fetch_glfw3", "fetch_physfs", "fetch_v8", "fetch_freeimage"], usage="Fetches Build Dependancys", check=check_depends)
def fetch_build_deps():
	pass;

@command(requires=["gyp"], usage="Open's the platform specfic IDE")
def ide():
	print("Opening IDE");

@command(usage="Writes the most recent git version to the src/Build.hpp")
def add_version_info():
	f = open(resolve_path(PROJECT_SOURCE, "Build.hpp"), "w");
	f.write("// AUTOGENERATED BUILD INFOMATION FILE\n");
	f.write("#define ENGINE_VERSION \"GIT_%s\"\n" % (get_git_hash()));
	f.close();

@command(requires=["gyp", "add_version_info"], usage="Cleans object files")
def clean():
	if sys.platform == "darwin": # OSX
		shell_command(["xcodebuild", "clean"]);

@command(requires=["gyp", "add_version_info"], usage="Builds executables")
def build_bin():
	if sys.platform == "darwin": # OSX
		shell_command(["xcodebuild"]);

@command(requires=["build_bin"], usage="Copys support files to the build path")
def build_env():
	print("Copying Enviroment");

@command(requires=["build_env"], usage="Runs the engine in Development Mode")
def run():
	run_engine(["-devmode", "-debug"]);

@command(requires=["build_env"], usage="Runs the engine in Test Mode")
def test():
	run_engine(["-test"]);
	run_engine(["-test", "-Ccore.render.openGL=2.0"]);
	run_engine(["-test", "-Ccore.test.testFrames=100"]);
	#run_engine(["-test", "-Ccore.render.openGL=2.0", "-Ccore.test.testFrames=100"]); # broken due to bug in SprieSheets
	run_engine(["-test", "-Ccore.render.neoFont=true", "-Ccore.test.testFrames=100"]);

@command(usage="Build documentation using jsdoc")
def doc():
	shell_command([
			"jsdoc", "-d", "./doc/apiDocs/", "./doc/api.js", "./Readme.md"
		]);

@command(usage="Prints the commandName and usage for each command")
def help():
	for x in commands:
		print("%s | %s" % (x, commands[x].usage));

@command(usage="Prints the current building enviroment")
def env():
	print("sys.platform = %s" % (sys.platform));

@command(usage="Runs CTags on the source directory")
def tags():
	srcPath = resolve_path(PROJECT_ROOT, "src");
	srcFiles = [ f for f in os.listdir(srcPath) if os.path.isfile(os.path.join(srcPath,f)) ];
	srcFiles = [ "src/" + f for f in srcFiles if f != ".DS_Store"];
	shell_command([
			CTAGS_PATH, "--c++-kinds=+p", "--fields=+iaS", "--extra=+q"] + srcFiles);

@command(requires=["build_env"], usage="Run the engine and take a screenshot after 1 second automaticly")
def screenshot():
	output = subprocess.check_output([resolve_path(PROJECT_BUILD_PATH, get_exe_name()),
		"-devmode", "-debug", "-Ccore.test.screenshotTime=1"]);
	output = [f for f in output.split("\n") if "TestScreenshot - ####" in f];
	reg = re.search("to \[(.*)\]", output[0]);
	filename = reg.group(0);
	filename = filename[4:-1];
	shutil.copyfile(filename, resolve_path(PROJECT_ROOT, "screenshot.png"));

def run_command(cmdName):
	if not commands[cmdName].check():
		print("==== Skiping: %s" % (cmdName));
		return

	# run all required commands
	for cmd in commands[cmdName].requires:
		if not commands[cmdName].run: ## make sure the command is not run twice
			run_command(cmd);

	# finaly run the commadn the user is intrested in
	print("==== Running: %s ====" % (cmdName));
	commands[cmdName]();
	commands[cmdName].run = True;

def main(args):
	# require python version 2.7

	# get the command and run it
	if len(args) < 2:
		print("tasks.py : usage: ./tasks.py [commandName]");
	elif not args[1] in commands:
		print("tasks.py : command %s not found" % (args[1]));
	else:
		run_command(args[1]);

if __name__ == '__main__':
  main(sys.argv);
