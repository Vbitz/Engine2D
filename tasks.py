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

import sys
import os
import subprocess
import re
import shutil
import hashlib
import ConfigParser
import multiprocessing
import glob
import json

import inspect
currentdir = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))
sys.path.insert(0, currentdir + "/tools")

import buildAddon
import bindingGenerator

PROJECT_ROOT = 0
PROJECT_SOURCE = 1
PROJECT_BUILD_PATH = 2
PROJECT_TMP_PATH = 3

CTAGS_PATH = "/usr/local/bin/ctags"

WINDOW_SYSTEM = os.getenv("ENGINE_WINDOW_SYSTEM", "glfw")
ENABLE_GPROFTOOLS = os.getenv("ENGINE_GPROFTOOLS", "off")

commands = {}

class bcolors:
    HEADER = '\x1b[0;42m\x1b[1;91m'
    HIGHLIGHT = '\x1b[0;44m\x1b[1;91m'
    ENDC = '\033[0m'

def noop():
	return True

class command(object):
	def __init__(self, requires=[], usage="TODO: ", check=noop):
		self.requires = requires
		self.usage = usage
		self.check = check
	
	def __call__(self, func):
		commands[func.__name__] = func
		commands[func.__name__].requires = self.requires
		commands[func.__name__].usage = self.usage
		commands[func.__name__].check = self.check
		commands[func.__name__].run = False
		return func

def get_exe_name():
	if sys.platform == "darwin" or sys.platform == "linux2":
		return "engine2D"

def get_lib_name():
	if sys.platform == "darwin":
		return "libengine2D.dylib"
	elif sys.platform == "linux2":
		return "libengine2D.so"

def resolve_path(base, path=""):
	if base == PROJECT_ROOT:
		return os.path.relpath(path)
	elif base == PROJECT_SOURCE:
		return os.path.join(resolve_path(PROJECT_ROOT, "src"), path)
	elif base == PROJECT_BUILD_PATH:
		if sys.platform == "darwin":
			return os.path.join(resolve_path(PROJECT_ROOT, "build/Default"), path)
		elif sys.platform == "linux2":
			return os.path.join(resolve_path(PROJECT_ROOT, "0/build/Default"), path)
	elif base == PROJECT_TMP_PATH:
		return os.path.join(resolve_path(PROJECT_ROOT, "tmp"), path)
	else:
		raise "Invalid base"

def require_in_path(arr):
	for x in arr:
		pass
	return True

def shell_command(cmd, throw=True):
	print(bcolors.HEADER + "shell in %s : %s" % (os.getcwd(), cmd) + bcolors.ENDC)
	if throw:
		subprocess.check_call(cmd)
	else:
		subprocess.call(cmd)

def run_engine(args):
	shell_command([resolve_path(PROJECT_BUILD_PATH, get_exe_name())] + args)

def run_engine_test(args, onlyHighlight=True):
	cmdArgs = [resolve_path(PROJECT_BUILD_PATH, get_exe_name()), "-test"]
	if onlyHighlight:
		cmdArgs += ["-Ccore.log.levels.onlyHighlight=true"]
	shell_command(cmdArgs + args)

def get_git_hash():
	# check for .git folder
	return subprocess.check_output(["git", "rev-parse", "HEAD"])[:10]

def get_hash(inStr):
	m = hashlib.md5()
	m.update(inStr)
	return m.hexdigest()

def get_filename_hash(filename):
	with open(filename, "r") as f:
		return get_hash(f.read())

def ensure_dir(path):
	if not os.path.exists(path):
		os.makedirs(path)

def get_arch():
	return "x64"

def get_max_jobs():
	return multiprocessing.cpu_count()

config = ConfigParser.RawConfigParser()
try:
	config.read(resolve_path(PROJECT_TMP_PATH, "tasks.cfg"))
except Exception, e:
	print("Could not load config file")

def write_config():
	with open(resolve_path(PROJECT_TMP_PATH, "tasks.cfg"), "wb") as config_file:
		config.write(config_file)

def ensure_file_hash(filenames):
	if not config.has_section("StoredHashes"):
		return False

	for filename in filenames:
		if not config.has_option("StoredHashes", filename):
			return False
		else:
			if not config.get("StoredHashes", filename) == get_filename_hash(filename):
				return False

	return True

def store_file_hash(filenames):
	if not config.has_section("StoredHashes"):
		config.add_section("StoredHashes")

	for filename in filenames:
		config.set("StoredHashes", filename, get_filename_hash(filename))

def ensure_str_hash(inStrs):
	if not config.has_section("StoredHashes"):
		return False

	for inStr in inStrs:
		if not config.has_option("StoredHashes", inStr):
			return False
		else:
			if not config.get("StoredHashes", inStr) == get_filename_hash(inStr):
				return False

	return True

def store_str_hash(inStrs):
	if not config.has_section("StoredHashes"):
		config.add_section("StoredHashes")

	for inStr in inStrs:
		config.set("StoredHashes", filename, get_hash(inStr))

def read_json(filename):
	with open(filename, "r") as f:
		return json.loads(f.read())

def is_travis():
	return False

@command(usage="Downloads a local copy of CMake if we can't find one")
def fetch_cmake(args):
	if sys.platform == "linux2":
		# Some of these commands need sudo
		# just run "sudo apt-get install -y cmake"
		pass

@command(requires=["fetch_cmake"], usage="Fetch and build glfw3 using cmake")
def build_glfw3(args):
	os.chdir(resolve_path(PROJECT_ROOT, "third_party/glfw"))
	
	shell_command([
			"cmake",
			"-Wno-dev",
			"-DGLFW_BUILD_EXAMPLES:BOOL=OFF",
			"-DGLFW_BUILD_TESTS:BOOL=OFF",
			"-DBUILD_SHARED_LIBS:BOOL=ON",
			"."
		])
	
	shell_command(["make"])

	glfw_glob = ""
	if sys.platform == "linux2":
		glfw_glob = "src/libglfw.so*"
	elif sys.platform == "darwin":
		glfw_glob = "src/libglfw.*dylib"

	for f in glob.glob(glfw_glob):
		print "Copying " + f + " to " + "../lib"
		shutil.copy(f, "../lib")
	
	os.chdir("../..")

@command(usage="Fetch and build V8 using GYP")
def build_v8(args):
	if sys.platform == "win32": # windows
		# svn co http://src.chromium.org/svn/trunk/deps/third_party/cygwin@231940 third_party/cygwin
		# python build\gyp_v8 -Dv8_enable_i18n_support=0
		# msbuild /p:Configuration=Release build\All.sln
		# copy the relevent files to third_party/lib and third_party/include
		pass
	elif sys.platform == "linux2" or sys.platform == "darwin": # linux
		os.chdir(resolve_path(PROJECT_ROOT, "third_party/v8"))
		if not os.path.exists("build/gyp"):
			shutil.copytree("../gyp", "build/gyp")
		oldPythonPath = os.getenv("PYTHONPATH", "")
		os.environ["PYTHONPATH"] = os.getcwd() + "/tools/generate_shim_headers:" + os.getcwd() + "/build"
		os.environ["GYP_GENERATORS"] = "make"
		shell_command([
				"build/gyp/gyp",
				"--generator-output=out",
				"src/d8.gyp",
	            "-Ibuild/standalone.gypi",
	            "--depth=.",
	            "-S.native",
	            "-Dcomponent=shared_library",
	            "-Dv8_enable_backtrace=1",
	            "-Dv8_enable_i18n_support=0",
	            "-Darm_fpu=default",
	            "-Darm_float_abi=default"
			])
		os.environ["GYP_GENERATORS"] = ""
		os.environ["PYTHONPATH"] = "PYTHONPATH"
		os.chdir("out");
		shell_command([
				"make",
				"-fMakefile.native",
				"BUILDTYPE=Release",
				"all",
				"-j" + str(get_max_jobs())
			])
		os.chdir("..");
		if sys.platform == "linux2":
			print "Copying out/out/Release/lib.target/libv8.so to ../lib"
			shutil.copy("out/out/Release/lib.target/libv8.so", "../lib/libv8.so")
		elif sys.platform == "darwin":
			print "Copying out/out/Release/libv8.dylib to ../lib"
			shutil.copy("out/out/Release/libv8.dylib", "../lib/libv8.dylib")
		os.chdir("../..")

@command(requires=["build_glfw3", "build_v8"], usage="Fetches Build Dependancys")
def build_deps(args):
	pass

@command(requires=["build_deps"], usage="Builds platform project files")
def gyp(args):
	# run GYP
	shell_command([
			resolve_path(PROJECT_ROOT, "third_party/gyp/gyp"),
			"--depth=0",
			"-DWINDOW=" + WINDOW_SYSTEM,
			"-DGPERFTOOLS=" + ENABLE_GPROFTOOLS,
			resolve_path(PROJECT_ROOT, "engine2D.gyp")
		])

@command(requires=["gyp"], usage="Open's the platform specfic IDE")
def ide(args):
	print("Opening IDE")

@command(usage="Generates Script Bindings")
def gen_bindings(args):
	ensure_dir("src/gen")
	srcPath = "src/spec"
	srcFiles = [ f for f in os.listdir(srcPath) if os.path.isfile(os.path.join(srcPath, f)) ]
	for f in srcFiles:
		filePath = os.path.join(srcPath, f)
		if not ensure_file_hash([filePath]):
			store_file_hash([filePath])
			bindingGenerator.parseFile(filePath, f)

@command(usage="Generates Build.hpp")
def gen_build_hpp(args):
	if not config.has_section("StoredState"):
		config.add_section("StoredState")
	
	git_hash = get_git_hash()
	if (not config.has_option("StoredState", "LastGitHash")) or \
			(not config.get("StoredState", "LastGitHash") == git_hash):
		config.set("StoredState", "LastGitHash", git_hash)
		f = open(resolve_path(PROJECT_SOURCE, "Build.hpp"), "w")
		f.write("// AUTOGENERATED BUILD INFOMATION FILE\n")
		f.write("#define ENGINE_VERSION \"GIT_%s\"\n" % (get_git_hash()))
		f.close()

@command(requires=["gen_bindings", "gen_build_hpp"], usage="Builds dynamicly generated source files")
def gen_source(args):
	pass

@command(requires=["gyp", "gen_source"], usage="Cleans object files")
def clean(args):
	if sys.platform == "darwin": # OSX
		shell_command(["xcodebuild", "clean"])
	elif sys.platform == "linux2": # Linux
		pass

def _build_bin(output=True, analyze=False):
	if sys.platform == "darwin": # OSX
		shutil.copy("third_party/lib/libglfw.dylib", resolve_path(PROJECT_BUILD_PATH, "libglfw.dylib"))
		shutil.copy("third_party/lib/libv8.dylib", resolve_path(PROJECT_BUILD_PATH, "libv8.dylib"))
		xcodeArgs = ["xcodebuild", "-jobs", str(get_max_jobs())]
		if analyze:
			xcodeArgs += ["RUN_CLANG_STATIC_ANALYZER=YES"]
		shell_command(xcodeArgs)
		shell_command(["install_name_tool", "-change",
			"/usr/local/lib/libengine2D.dylib",
			"@executable_path/libengine2D.dylib",
			resolve_path(PROJECT_BUILD_PATH, get_exe_name())]) # libengine2D.dylib
		shell_command(["install_name_tool", "-change",
			"/usr/local/lib/libglfw.dylib",
			"@executable_path/libglfw.dylib",
			resolve_path(PROJECT_BUILD_PATH, get_lib_name())]) # libglfw3.dylib
		shell_command(["install_name_tool", "-change",
			"/usr/local/lib/libv8.dylib",
			"@executable_path/libv8.dylib",
			resolve_path(PROJECT_BUILD_PATH, get_lib_name())]) # libv8.dylib 
	elif sys.platform == "linux2":
		os.chdir(resolve_path(PROJECT_ROOT, "0"))
		os.environ["CC"] = "clang"
		os.environ["CXX"] = "clang++"
		os.environ["CXXFLAGS"] = "-std=gnu++11"
		shell_command("make")
		if analyze:
			os.environ["CXXFLAGS"] = "-std=gnu++11 --analyze -Wall"
			os.environ["LD_LIBRARY_PATH"] = "../third_party/lib/" # use resolve_path
			shell_command("make", throw=False)
		os.chdir("..")


@command(requires=["gyp", "gen_source"], usage="Builds executables")
def build_bin(args):
	_build_bin()

@command(requires=["gyp", "gen_source"], usage="Runs static analyzer")
def analyze(args):
	_build_bin(analyze=True)

@command(requires=[], usage="Builds addons")
def build_addons(args):
	if sys.platform == "darwin":
		if not ensure_file_hash(["src/Modules/JSUnsafe.cpp"]):
			store_file_hash(["src/Modules/JSUnsafe.cpp"])
			buildAddon.compile(["src/Modules/JSUnsafe.cpp"], "res/modules/js_unsafe.dylib", link_v8=True)

@command(requires=["build_bin", "build_addons"], usage="Copys support files to the build path")
def build_env(args):
	print("Copying Enviroment")

@command(requires=["build_env"], usage="Runs the engine in Development Mode")
def run(args):
	run_engine(["-devmode", "-debug"])

@command(requires=["build_env"], usage="Runs the engine in Test Mode")
def test(args):
	run_engine_test([])
	run_engine_test(["-Ccore.render.openGL=2.0"])
	run_engine_test(["-Ccore.test.testFrames=100"])
	run_engine_test(["-Ccore.render.openGL=2.0", "-Ccore.test.testFrames=100"])

@command(requires=["build_env"], usage="Runs 1 test of the engine in Test Mode")
def test_once(args):
	run_engine_test([], onlyHighlight=False)

@command(requires=["gen_source", "fetch_gyp"], usage="Tests both GLFW and SDL")
def test_full(args):
	print("=== Starting Full Test Suite ===")
	
	print("=== Building GLFW Window ===")
	clean()
	WINDOW_SYSTEM = "glfw"
	gyp()
	build_bin()
	print("=== Testing GLFW Window ===")
	test()

	print("=== Build SDL Window ===")
	clean()
	WINDOW_SYSTEM = "sdl"
	gyp()
	build_bin()
	print("=== Testing SDL Window ===")
	test()

@command(usage="Build documentation using jsdoc")
def doc(args):
	shell_command([
			"jsdoc", "-d", "./doc/apiDocs/", "./doc/api.js", "./Readme.md"
		])

@command(usage="Prints the commandName and usage for each command")
def help(args):
	for x in commands:
		print("%s | %s" % (x, commands[x].usage))

@command(usage="Prints the current building enviroment")
def env(args):
	print("sys.platform = %s" % (sys.platform))
	print("sys.version = %s" % (sys.version))
	print("sys.version_info = %s" % (sys.version_info))
	print("resolve_path(PROJECT_ROOT, \"tasks.py\") = %s" % (resolve_path(PROJECT_ROOT, "tasks.py")))

@command(usage="Runs CTags on the source directory")
def tags(args):
	srcPath = resolve_path(PROJECT_ROOT, "src")
	srcFiles = [ f for f in os.listdir(srcPath) if os.path.isfile(os.path.join(srcPath,f)) ]
	srcFiles = [ "src/" + f for f in srcFiles if f != ".DS_Store"]
	shell_command([
			CTAGS_PATH, "--c++-kinds=+p", "--fields=+iaS", "--extra=+q"] + srcFiles)

@command(requires=["build_env"], usage="Run the engine and take a screenshot after 1 second automaticly")
def screenshot(args):
	output = subprocess.check_output([resolve_path(PROJECT_BUILD_PATH, get_exe_name()),
		"-devmode", "-debug", "-Ccore.test.screenshotTime=1"])
	output = [f for f in output.split("\n") if "TestScreenshot - ####" in f]
	reg = re.search("to \[(.*)\]", output[0])
	filename = reg.group(0)
	filename = filename[4:-1]
	shutil.copyfile(filename, resolve_path(PROJECT_ROOT, "screenshot.bmp"))

@command(requires=["build_env"], usage="Builds a release package")
def release(args):
	deployFile = read_json(resolve_path(PROJECT_ROOT, "deploy.json"))
	basePath = resolve_path(PROJECT_ROOT, deployFile["resourceRoot"])
	outputName = "%s_%s_v%s" % (deployFile["appName"], sys.platform, get_git_hash())
	outputPath = os.path.join(resolve_path(PROJECT_ROOT, "release"), outputName) 

	# check current operating system is in the [os] array
	currentPlatform = sys.platform
	foundOs = False
	for osName in deployFile["os"]:
		if osName == currentPlatform:
			foundOs = True
	if not foundOs:
		raise Exception("the deploy.json file does not support your current operating system")

	# build a list of files to copy to release
	fileList = []
	fileList += [os.path.join(basePath, deployFile["configFile"])]
	for fName in deployFile["files"]:
		fileList += [f for f in glob.glob(os.path.join(basePath, fName))]

	ensure_dir(outputPath)
	ensure_dir(os.path.join(outputPath, "bin"))

	binPath = resolve_path(PROJECT_BUILD_PATH)
	binList = [os.path.relpath(f, binPath) for f in glob.glob(os.path.join(binPath, "*"))]
	for fName in binList:
		shutil.copyfile(os.path.join(binPath, fName), os.path.join(outputPath, "bin", fName))
		if fName == get_exe_name():
			os.chmod(os.path.join(outputPath, "bin", fName), 0o755)

	for fName in fileList:
		ensure_dir(os.path.dirname(os.path.join(outputPath, fName)))
		shutil.copyfile(fName, os.path.join(outputPath, fName))

	shutil.make_archive(outputName, "gztar", root_dir=resolve_path(PROJECT_ROOT, "release"))

	if is_travis():
		pass

def run_command(cmdName, rawArgs):
	if not commands[cmdName].check():
		print("==== Skiping: %s" % (cmdName))
		return

	# run all required commands
	for cmd in commands[cmdName].requires:
		if not commands[cmdName].run: ## make sure the command is not run twice
			run_command(cmd, rawArgs)

	# finaly run the commadn the user is intrested in
	print(bcolors.HIGHLIGHT + "==== Running: %s ====" % (cmdName) + bcolors.ENDC)
	commands[cmdName](rawArgs)
	commands[cmdName].run = True

def main(args):
	# require python version 2.7
	if sys.version_info < (2, 7) or sys.version_info > (3, 0):
		print("tasks.py requires Python 2.7 or Greater")
		return

	# get the command and run it
	if len(args) < 2:
		print("tasks.py : usage: ./tasks.py [commandName]")
	elif not args[1] in commands:
		print("tasks.py : command %s not found" % (args[1]))
	else:
		run_command(args[1], args)

	write_config()

if __name__ == '__main__':
  main(sys.argv)
