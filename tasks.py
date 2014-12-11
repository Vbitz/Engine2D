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

# TODO: Split off useful functions like buildAddon into a separate library directory
# TODO: Improve support for changing build time parameters like PROFILER

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
import struct

import inspect
currentdir = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))
sys.path.insert(0, currentdir + "/tools")

import buildAddon
import bindingGenerator
import objToEglb

PROJECT_ROOT = 0
PROJECT_SOURCE = 1
PROJECT_BUILD_PATH = 2
PROJECT_TMP_PATH = 3

CTAGS_PATH = "/usr/local/bin/ctags" # installed with "brew install ctags"
GCOV_PATH = "/usr/local/bin/gcov-4.2" # installed with "brew install gcc"

WINDOW_SYSTEM = os.getenv("ENGINE_WINDOW_SYSTEM", "glfw")
ENABLE_GPROFTOOLS = os.getenv("ENGINE_GPROFTOOLS", "off")
PROFILER = os.getenv("ENGINE_PROFILER", "off")

commands = {}

class bcolors:
	LOGSRC = '\x1b[0;42m\x1b[1;91m'
	HEADER = '\x1b[0;44m\x1b[1;91m'
	SHELL = '\x1b[0;32m'
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

def log(*args, **kwargs):
	method_name = kwargs["method_name"] if "method_name" in kwargs else True
	newArgs = []
	for s in args:
		newArgs += [str(s)]
	if method_name:
		parent = inspect.getouterframes(inspect.currentframe())[1][3]
		print("%s %s" % (bcolors.LOGSRC + "[" + parent + "]" + bcolors.ENDC, " ".join(newArgs)))
	else:
		print(" ".join(newArgs))

def is_linux():
	return sys.platform == "linux2"

def is_osx():
	return sys.platform == "darwin"

def is_windows():
	return sys.platform == "win32"

def get_exe_name():
	if is_linux() or is_osx():
		return "engine2D"

def get_lib_name():
	if is_osx():
		return "libengine2D.dylib"
	elif is_linux():
		return "libengine2D.so"

def resolve_path(base, path=""):
	if base == PROJECT_ROOT:
		return os.path.relpath(path)
	elif base == PROJECT_SOURCE:
		return os.path.join(resolve_path(PROJECT_ROOT, "src"), path)
	elif base == PROJECT_BUILD_PATH:
		if is_osx():
			return os.path.join(resolve_path(PROJECT_ROOT, "build/Default"), path)
		elif is_linux():
			return os.path.join(resolve_path(PROJECT_ROOT, "0/out/Default"), path)
	elif base == PROJECT_TMP_PATH:
		return os.path.join(resolve_path(PROJECT_ROOT, "tmp"), path)
	else:
		raise "Invalid base"

def require_in_path(arr):
	for x in arr:
		pass
	return True

def shell_command(cmd, throw=True, output=False):
	log("in " + bcolors.SHELL + os.getcwd() + bcolors.ENDC + (" : %s" % (" ".join(cmd))))
	if throw:
		if output:
			return subprocess.check_output(cmd)
		else:
			subprocess.check_call(cmd)
	else:
		subprocess.call(cmd)

def run_engine(args):
	if is_linux():
		os.environ["LD_LIBRARY_PATH"] = "third_party/lib/" # TODO: use resolve_path
	shell_command([resolve_path(PROJECT_BUILD_PATH, get_exe_name())] + args)

def run_engine_test(args, onlyHighlight=True):
	cmdArgs = ["-test"]
	if onlyHighlight:
		cmdArgs += ["-Ccore.log.levels.onlyHighlight=true"]
	run_engine(cmdArgs + args)

def get_git_hash():
	# check for .git folder
	return subprocess.check_output(["git", "rev-parse", "HEAD"])[:10]

def get_hash(inStr):
	m = hashlib.md5()
	m.update(inStr)
	return m.hexdigest()

def get_file_content(filename):
	with open(filename, "r") as f:
		return f.read()

def get_filename_hash(filename):
	return get_hash(get_file_content(filename))

def ensure_dir(path):
	if not os.path.exists(path):
		os.makedirs(path)

def get_arch():
	# TODO: Handle ARM
	if 8 * struct.calcsize("P") == 64:
		return "x64"
	else:
		return "x86"

def get_max_jobs():
	return multiprocessing.cpu_count()

config = ConfigParser.RawConfigParser()
try:
	config.read(resolve_path(PROJECT_TMP_PATH, "tasks.cfg"))
except Exception, e:
	log("Could not load config file")

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
	return json.loads(get_file_content(filename))

def is_travis():
	return os.getenv("TRAVIS", "false") == "true"

def get_size(start_path = '.'):
	total_size = 0
	for dirpath, dirnames, filenames in os.walk(start_path):
		for f in filenames:
			fp = os.path.join(dirpath, f)
			total_size += os.path.getsize(fp)
	return total_size

def sizeof_fmt(num):
	for x in ['bytes','KB','MB','GB']:
		if num < 1024.0:
			return "%3.1f%s" % (num, x)
		num /= 1024.0
	return "%3.1f%s" % (num, 'TB')

@command(usage="Downloads a local copy of CMake if we can't find one")
def fetch_cmake(args):
	if is_linux():
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
	if is_linux():
		glfw_glob = "src/libglfw.so*"
	elif is_osx():
		glfw_glob = "src/libglfw.*dylib"

	for f in glob.glob(glfw_glob):
		log("copying %s to ../lib" % (f))
		shutil.copy(f, "../lib")
	
	os.chdir("../..")


# TODO: Migrate to the new v8 git repo
@command(usage="Fetch and build V8 using GYP")
def build_v8(args):
	if is_windows(): # windows
		# svn co http://src.chromium.org/svn/trunk/deps/third_party/cygwin@231940 third_party/cygwin
		# python build\gyp_v8 -Dv8_enable_i18n_support=0
		# msbuild /p:Configuration=Release build\All.sln
		# copy the relevent files to third_party/lib and third_party/include
		pass
	elif is_linux() or is_osx(): # linux/osx
		os.chdir(resolve_path(PROJECT_ROOT, "third_party"))
		if not os.path.exists("v8/build/gyp"):
			shutil.copytree("gyp", "v8/build/gyp")
		if not os.path.exists("v8/buildtools"):
			shell_command([
				"git", "clone",
				"https://chromium.googlesource.com/chromium/buildtools.git",
				"v8/buildtools"])
		if not os.path.exists("v8/tools/clang"):
			shell_command([
				"git", "clone",
				"https://chromium.googlesource.com/chromium/src/tools/clang.git",
				"v8/tools/clang"])
		shell_command([
			"python", "v8/tools/clang/scripts/update.py", "--if-needed"
			])
		os.chdir("v8")
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
		if is_linux():
			log("copying out/out/Release/lib.target/libv8.so to ../lib")
			shutil.copy("out/out/Release/lib.target/libv8.so", "../lib/libv8.so")
		elif is_osx():
			log("copying out/out/Release/libv8.dylib to ../lib")
			shutil.copy("out/out/Release/libv8.dylib", "../lib/libv8.dylib")
		os.chdir("../..")

@command(requires=["build_glfw3", "build_v8"], usage="Fetches Build Dependences")
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
			"-DPROFILER=" + PROFILER,
			resolve_path(PROJECT_ROOT, "engine2D.gyp")
		])

@command(requires=["gyp"], usage="Open the platform specific IDE")
def ide(args):
	log("Opening IDE")

@command(usage="Generates Script Bindings")
def gen_bindings(args):
	ensure_dir("src/gen")
	srcPath = "src/spec"
	srcFiles = [ f for f in os.listdir(srcPath) if os.path.isfile(os.path.join(srcPath, f)) ]
	for f in srcFiles:
		filePath = os.path.join(srcPath, f)
		if not ensure_file_hash([filePath]):
			log("building %s" % (filePath))
			store_file_hash([filePath])
			bindingGenerator.parseFile(filePath, f)
		else:
			log("skipping %s" % (filePath))

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

@command(requires=["gen_bindings", "gen_build_hpp"], usage="Builds dynamically generated source files")
def gen_source(args):
	pass

@command(requires=["gyp", "gen_source"], usage="Cleans object files")
def clean(args):
	if is_osx(): # OSX
		shell_command(["xcodebuild", "clean"])
	elif is_linux(): # Linux
		pass

def _build_bin(output=True, analyze=False, coverage=False):
	if is_osx(): # OSX
		shutil.copy("third_party/lib/libglfw.dylib", resolve_path(PROJECT_BUILD_PATH, "libglfw.dylib"))
		shutil.copy("third_party/lib/libv8.dylib", resolve_path(PROJECT_BUILD_PATH, "libv8.dylib"))
		xcodeArgs = ["xcodebuild", "-jobs", str(get_max_jobs())]
		if analyze:
			xcodeArgs += ["RUN_CLANG_STATIC_ANALYZER=YES"]
		if coverage:
			xcodeArgs += ["GCC_GENERATE_TEST_COVERAGE_FILES=YES",
				"GCC_INSTRUMENT_PROGRAM_FLOW_ARCS=YES"]
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
	elif is_linux():
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

ccovExpr = re.compile("File '(.*)'\nLines executed:(.*) of (.*)\n")

def parseCcovLine(m):
	filename = m.group(1)
	linesP = m.group(2)
	totalLines = m.group(3)
	return "[ccov] '%s' : executed %s of %s lines" % (filename, linesP, totalLines)

@command(requires=["gyp", "gen_source"], usage="Runs code coverage")
def ccov(args):
	if not is_osx(): # This won't work on linux yet
		return

	log("_build_bin(coverage=True)")
	_build_bin(coverage=True)

	log("run_engine([\"-test\"])")
	run_engine(["-test"])

	log("running gcov")
	oldDir = os.getcwd()
	os.chdir("build/engine2D.build/Default/libengine2D.build/Objects-normal/x86_64")
	objectFiles = glob.glob("*.o")
	out = shell_command([GCOV_PATH, "--no-output"] + objectFiles, output=True)
	os.chdir(oldDir)

	log("output")
	log(re.sub(ccovExpr, parseCcovLine, out), method_name=False)

@command(requires=[], usage="Compile a addon by name")
def build_addon(args):
	if len(args) < 4:
		log("usage: ./tasks.py build_addon <cFile> <outputFilename>")
	if is_osx():
		buildAddon.compile([args[2]], args[3], link_v8=True)

@command(requires=[], usage="Builds addons")
def build_addons(args):
	if is_osx():
		if not ensure_file_hash(["src/Modules/JSUnsafe.cpp"]):
			log("building res/modules/js_unsafe.dylib")
			store_file_hash(["src/Modules/JSUnsafe.cpp"])
			buildAddon.compile(["src/Modules/JSUnsafe.cpp"], "res/modules/js_unsafe.dylib", link_v8=True)
		else:
			log("skipping res/modules/js_unsafe.dylib")

@command(requires=["build_bin", "build_addons", "build_content"], usage="Copy's support files to the build path")
def build_env(args):
	log("Copying Environment")

@command(requires=["build_env"], usage="Runs the engine in Development Mode")
def run(args):
	run_engine(["-devmode", "-debug"])

@command(requires=["build_env"], usage="Runs the engine in Development Mode with garbage collector tracing enabled")
def run_gc_trace(args):
	run_engine(["-devmode", "-debug", "-v8trace-gc"])

@command(requires=["build_env"], usage="Runs the engine in Test Mode")
def test(args):
	run_engine_test([])
	run_engine_test(["-Ccore.render.openGL=2.0"])
	run_engine_test(["-Ccore.test.testFrames=100"])
	run_engine_test(["-Ccore.render.openGL=2.0", "-Ccore.test.testFrames=100"])


@command(requires=["build_env"], usage="Runs the engine in Headless Mode")
def headless_test(args):
	run_engine(["-devmode", "-debug", "-headless", "script/headlessTest"])
	run_engine(["-debug", "-headless", "script/headlessTest"])
	run_engine(["-devmode", "-headless", "script/headlessTest"])
	run_engine(["-headless", "script/headlessTest"])

@command(requires=["build_env"], usage="Runs the engine in Headless Mode with Valgrind")
def valgrind(args):
	if is_linux():
		os.environ["LD_LIBRARY_PATH"] = "third_party/lib/" # TODO: use resolve_path
		shell_command(["valgrind", "--leak-check=full", "--show-reachable=yes",
			"--suppressions=tools/valgrind.supp",
			resolve_path(PROJECT_BUILD_PATH, get_exe_name()), "-devmode", "-debug", "-headless",
			"script/headlessTest"])

@command(requires=["build_env"], usage="Runs 1 test of the engine in Test Mode")
def test_once(args):
	run_engine_test([], onlyHighlight=False)

@command(requires=["gen_source", "fetch_gyp"], usage="Tests both GLFW and SDL")
def test_full(args):
	log("Starting Full Test Suite")
	
	log("Building GLFW Window")
	clean()
	WINDOW_SYSTEM = "glfw"
	gyp()
	build_bin()
	log("Testing GLFW Window")
	test()

	log("Build SDL Window")
	clean()
	WINDOW_SYSTEM = "sdl"
	gyp()
	build_bin()
	log("Testing SDL Window")
	test()

@command(usage="Build documentation using jsdoc")
def doc(args):
	shell_command([
			"jsdoc", "-d", "./doc/apiDocs/", "./doc/api.js", "./Readme.md"
		])

@command(usage="Prints the commandName and usage for each command")
def help(args):
	for x in commands:
		log("%s | %s" % (x, commands[x].usage))

@command(usage="Prints the current building environment")
def env(args):
	log("sys.platform = \"%s\"" % (sys.platform))
	log("sys.version = \"%s\"" % (sys.version.replace("\n", "")))
	log("sys.version_info = %s" % (sys.version_info))
	log("get_arch() = \"%s\"" % (get_arch()))
	log("get_max_jobs() = %s" % (get_max_jobs()))
	log("resolve_path(PROJECT_ROOT, \"tasks.py\") = \"%s\"" % (resolve_path(PROJECT_ROOT, "tasks.py")))

@command(usage="Runs CTags on the source directory")
def tags(args):
	srcPath = resolve_path(PROJECT_ROOT, "src")
	srcFiles = [ f for f in os.listdir(srcPath) if os.path.isfile(os.path.join(srcPath,f)) ]
	srcFiles = [ "src/" + f for f in srcFiles if f != ".DS_Store"]
	shell_command([
			CTAGS_PATH, "--c++-kinds=+p", "--fields=+iaS", "--extra=+q"] + srcFiles)

@command(requires=["build_env"], usage="Run the engine and take a screenshot after 1 second automatically")
def screenshot(args):
	log([resolve_path(PROJECT_BUILD_PATH, get_exe_name()),
		"-devmode", "-debug", "-Ccore.test.screenshotTime=1"])
	output = subprocess.check_output([resolve_path(PROJECT_BUILD_PATH, get_exe_name()),
		"-devmode", "-debug", "-Ccore.test.screenshotTime=1"])
	print output
	output = [f for f in output.split("\n") if "TestScreenshot - ####" in f]
	reg = re.search("to \[(.*)\]", output[0])
	filename = reg.group(0)
	filename = filename[4:-1]
	shutil.copyfile(filename, resolve_path(PROJECT_ROOT, "screenshot.bmp"))

def build_eglb(filename):
	if not ensure_file_hash([filename]):
		log("building %s" % (filename))
		store_file_hash([filename])
		rawFilename, rawFileExtension = os.path.splitext(filename)
		objToEglb.doConvert(filename, rawFilename + ".eglb", 100)
	else:
		log("skipping %s" % (filename))

contentBuilders = {
	".obj": build_eglb
}

def build_content_file(filename, cType):
	if cType not in contentBuilders:
		raise Exception("No contentBuilder for " + cType)
	contentBuilders[cType](filename)

@command(requires=[], usage="Builds content to release format")
def build_content(args):
	# Build each piece of content in the manifest using different drivers
	# I know this method is very similar to release but it's designed for a different stage
	deployFilename	= resolve_path(PROJECT_ROOT, "deploy.json")
	deployFile		= read_json(deployFilename)
	basePath		= resolve_path(PROJECT_ROOT, deployFile["resourceRoot"])
	contentFiles	= deployFile["contentFiles"]
	contentFileList	= []

	for fileGlob in contentFiles:
		contentFileList += [f for f in glob.glob(os.path.join(basePath, fileGlob))]
	
	for contentFile in contentFileList:
		fileName, fileExtension = os.path.splitext(contentFile)
		build_content_file(contentFile, fileExtension)

@command(requires=["build_env"], usage="Builds a release package")
def release(args):
	deployFilename	= resolve_path(PROJECT_ROOT, "deploy.json")
	deployFile		= read_json(deployFilename)
	basePath		= resolve_path(PROJECT_ROOT, deployFile["resourceRoot"])
	outputName		= "%s_%s_v%s" % (deployFile["appName"], sys.platform, get_git_hash())
	outputPath		= os.path.join(resolve_path(PROJECT_ROOT, "release"), outputName)
	binPath			= resolve_path(PROJECT_BUILD_PATH)
	currentPlatform = sys.platform

	log("=== release config ===")
	log("deployFilename = \"%s\"" % (deployFilename))
	log("basePath = \"%s\"" % (basePath))
	log("outputName = \"%s\"" % (outputName))
	log("outputPath = \"%s\"" % (outputPath))
	log("binPath = \"%s\"" % (binPath))
	log("currentPlatform = \"%s\"" % (currentPlatform))
	log("platformList = %s" % (deployFile["os"]))
	log("is_travis = %s" % (is_travis()))

	log("beginning release build for %s on %s" % (deployFile["appName"], currentPlatform))

	# check current operating system is in the [os] array
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

	if is_linux():
		ensure_dir(os.path.join(outputPath, "lib"))
		libPath = os.path.join(resolve_path(PROJECT_ROOT, "third_party"), "lib")
		for fName in [os.path.join(libPath, "libv8.so"), os.path.join(libPath, "libglfw.so")]:
			shutil.copyfile(fName, os.path.join(outputPath, "lib", os.path.relpath(fName, libPath)))
			log("copying %s to %s" % (fName, os.path.join(outputPath, "lib", os.path.relpath(fName, libPath))))

		for fName in ["engine2D", "libengine2D.so"]:
			shutil.copyfile(os.path.join(binPath, fName), os.path.join(outputPath, "bin", fName))
			log("copying %s to %s" % (os.path.join(binPath, fName), os.path.join(outputPath, "bin", fName)))
			if fName == get_exe_name():
				os.chmod(os.path.join(outputPath, "bin", fName), 0o755)
	else:
		binList = [os.path.relpath(f, binPath) for f in glob.glob(os.path.join(binPath, "*"))]
		for fName in binList:
			shutil.copyfile(os.path.join(binPath, fName), os.path.join(outputPath, "bin", fName))
			log("copying %s to %s" % (os.path.join(binPath, fName), os.path.join(outputPath, "bin", fName)))
			if fName == get_exe_name():
				os.chmod(os.path.join(outputPath, "bin", fName), 0o755)

	for fName in fileList:
		ensure_dir(os.path.dirname(os.path.join(outputPath, fName)))
		log("copying %s to %s" % (fName, os.path.join(outputPath, fName)))
		shutil.copyfile(fName, os.path.join(outputPath, fName))

	log("building %s" % (outputName + ".tar.gz"))
	shutil.make_archive(outputName, "gztar", root_dir=resolve_path(PROJECT_ROOT, "release"))

	uncompressedSize = get_size(start_path=resolve_path(PROJECT_ROOT, "release"))
	compressedSize = os.path.getsize(outputName + ".tar.gz")

	log("built %s with output size %s compressed from %s (%s%%)" % (outputName + ".tar.gz",
		sizeof_fmt(compressedSize), sizeof_fmt(uncompressedSize), "%.2f" % ((1.0 * compressedSize / uncompressedSize) * 100.0)))

	if not is_travis():
		# get the release key from ENV
		# post the compressed release using configs in ENV
		return

	log("detected travis ci")

def run_command(cmdName, rawArgs):
	if not commands[cmdName].check():
		print("==== Skipping: %s" % (cmdName))
		return

	# run all required commands
	for cmd in commands[cmdName].requires:
		if not commands[cmdName].run: ## make sure the command is not run twice
			run_command(cmd, rawArgs)

	# finally run the command the user is interested in
	print(bcolors.HEADER + "==== Running: %s ====" % (cmdName) + bcolors.ENDC)
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
