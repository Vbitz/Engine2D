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
import hashlib;
import ConfigParser;

import inspect;
currentdir = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())));
sys.path.insert(0, currentdir + "/tools");

import buildAddon;

PROJECT_ROOT = 0;
PROJECT_SOURCE = 1;
PROJECT_BUILD_PATH = 2;
PROJECT_TMP_PATH = 3;

CTAGS_PATH = "/usr/local/bin/ctags";

WINDOW_SYSTEM = os.getenv("ENGINE_WINDOW_SYSTEM", "glfw");
ENABLE_GPROFTOOLS = os.getenv("ENGINE_GPROFTOOLS", "off");

commands = {};

class bcolors:
    HEADER = '\x1b[0;42m\x1b[1;91m';
    HIGHLIGHT = '\x1b[0;44m\x1b[1;91m';
    ENDC = '\033[0m';

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
		return os.path.join(resolve_path(PROJECT_ROOT, "build/Default"), path);
	elif base == PROJECT_TMP_PATH:
		return os.path.join(resolve_path(PROJECT_ROOT, "tmp"), path);
	else:
		raise "Invalid base";

def require_in_path(arr):
	for x in arr:
		pass;
	return True;

def shell_command(cmd, throw=True):
	print(bcolors.HEADER + "shell : %s" % (cmd) + bcolors.ENDC);
	if throw:
		subprocess.check_call(cmd);
	else:
		subprocess.call(cmd);

def run_engine(args):
	shell_command([resolve_path(PROJECT_BUILD_PATH, get_exe_name())] + args);

def run_engine_test(args, onlyHighlight=True):
	cmdArgs = [resolve_path(PROJECT_BUILD_PATH, get_exe_name()), "-test"];
	if onlyHighlight:
		cmdArgs += "-Ccore.log.levels.onlyHighlight=true";
	shell_command(cmdArgs + args);

def get_git_hash():
	# check for .git folder
	return subprocess.check_output(["git", "rev-parse", "HEAD"])[:10];

def get_filename_hash(filename):
	m = hashlib.md5();
	with open(filename, "r") as f:
		m.update(f.read());
	return m.hexdigest();

def ensure_dir(path):
	if not os.path.exists(path):
		os.makedirs(path);

config = ConfigParser.RawConfigParser();
try:
	config.read(resolve_path(PROJECT_TMP_PATH, "tasks.cfg"));
except Exception, e:
	print("Could not load config file");

def write_config():
	with open(resolve_path(PROJECT_TMP_PATH, "tasks.cfg"), "wb") as config_file:
		config.write(config_file);

def ensure_file_hash(filenames):
	if not config.has_section("StoredHashes"):
		return False;

	for filename in filenames:
		if not config.has_option("StoredHashes", filename):
			return False;
		else:
			if not config.get("StoredHashes", filename) == get_filename_hash(filename):
				return False;

	return True;

def store_file_hash(filenames):
	if not config.has_section("StoredHashes"):
		config.add_section("StoredHashes");

	for filename in filenames:
		config.set("StoredHashes", filename, get_filename_hash(filename));

@command(usage="Downloads a local copy of SVN if we can't find one")
def fetch_svn(args):
	try:
		require_in_path(["svn"]);
	except Exception, e:
		# check for SVN in path, if we can find it then put it in the path
		print(bcolors.FAIL + "SVN not found: " + str(e));
		# download a standalone copy of SVN and put it in third_party/svn then put it in the path
		raise e
	# at this point we have SVN in the path and can download GYP
	pass;

@command(usage="Downloads a local copy of CMake if we can't find one")
def fetch_cmake(args):
	pass;

@command(requires=["fetch_svn"], usage="Downloads the latest version of GYP using SVN")
def fetch_gyp(args):
	# check for GYP in third_party first
	# if it's not there then download it using SVN
	pass;

@command(requires=["fetch_gyp"], usage="Builds platform project files")
def gyp(args):
	# run GYP
	shell_command([
			resolve_path(PROJECT_ROOT, "third_party/gyp/gyp"),
			"--depth=0",
			"-DWINDOW=" + WINDOW_SYSTEM,
			"-DGPERFTOOLS=" + ENABLE_GPROFTOOLS,
			resolve_path(PROJECT_ROOT, "engine2D.gyp")
		]);

def check_depends():
	return True;

@command(requires=["fetch_cmake"], usage="Fetch and build glfw3 using cmake")
def fetch_glfw3(args):
	# both platforms need https://github.com/glfw/glfw/archive/3.0.3.zip (400,137 bytes)
	# extracts to glfw-3.0.3
	# run cmake -DGLFW_BUILD_TESTS=false . in glfw-3.0.3
	# run make 
	# copy include/GLFW to third_party/include
	# copy src/libglfw3.a to third_party/lib
	pass;

@command(requires=["fetch_svn"], usage="Fetch and build V8 using GYP")
def fetch_v8(args):
	# both platforms need https://github.com/v8/v8/archive/3.21.17.zip (16,949,393 bytes)
	# the zip file extracts to v8-3.21.17
	# change directory to third_party/depend_src/v8-3.21.17
	# svn co http://gyp.googlecode.com/svn/trunk build/gyp (pretty small)

	if sys.platform == "win32": # windows
		# svn co http://src.chromium.org/svn/trunk/deps/third_party/cygwin@231940 third_party/cygwin
		# python build\gyp_v8 -Dv8_enable_i18n_support=0
		# msbuild /p:Configuration=Release build\All.sln
		# copy the relevent files to third_party/lib and third_party/include
		pass
	elif sys.platform == "darwin": #osx
		# make native i18nsupport=off (takes bloody ages)
		pass

@command(requires=["fetch_glfw3", "fetch_physfs", "fetch_v8"], usage="Fetches Build Dependancys", check=check_depends)
def fetch_build_deps(args):
	pass;

@command(requires=["gyp"], usage="Open's the platform specfic IDE")
def ide(args):
	print("Opening IDE");

@command(usage="Writes the most recent git version to the src/Build.hpp")
def add_version_info(args):
	f = open(resolve_path(PROJECT_SOURCE, "Build.hpp"), "w");
	f.write("// AUTOGENERATED BUILD INFOMATION FILE\n");
	f.write("#define ENGINE_VERSION \"GIT_%s\"\n" % (get_git_hash()));
	f.close();

@command(requires=["gyp", "add_version_info"], usage="Cleans object files")
def clean(args):
	if sys.platform == "darwin": # OSX
		shell_command(["xcodebuild", "clean"]);

def _build_bin(output=True):
	if sys.platform == "darwin": # OSX
		shell_command(["xcodebuild"]);
		shell_command(["install_name_tool", "-change",
			"/usr/local/lib/libengine2D.dylib",
			"@executable_path/libengine2D.dylib",
			resolve_path(PROJECT_BUILD_PATH, get_exe_name())]);


@command(requires=["gyp", "add_version_info"], usage="Builds executables")
def build_bin(args):
	_build_bin();

@command(requires=[], usage="Builds addons")
def build_addons(args):
	if not ensure_file_hash(["src/Modules/JSUnsafe.cpp"]):
		store_file_hash(["src/Modules/JSUnsafe.cpp"]);
		buildAddon.compile(["src/Modules/JSUnsafe.cpp"], "res/modules/js_unsafe.dylib", link_v8=True);

@command(requires=["build_bin", "build_addons"], usage="Copys support files to the build path")
def build_env(args):
	print("Copying Enviroment");

@command(requires=["build_env"], usage="Runs the engine in Development Mode")
def run(args):
	run_engine(["-devmode", "-debug"]);

@command(requires=["build_env"], usage="Runs the engine in Test Mode")
def test(args):
	run_engine_test([]);
	run_engine_test(["-Ccore.render.openGL=2.0"]);
	run_engine_test(["-Ccore.test.testFrames=100"]);
	run_engine_test(["-Ccore.render.openGL=2.0", "-Ccore.test.testFrames=100"]);

@command(requires=["build_env"], usage="Runs 1 test of the engine in Test Mode")
def test_once(args):
	run_engine_test([], onlyHighlight=False);

@command(requires=["add_version_info", "fetch_gyp"], usage="Tests both GLFW and SDL")
def test_full(args):
	print("=== Starting Full Test Suite ===");
	
	print("=== Building GLFW Window ===");
	clean();
	WINDOW_SYSTEM = "glfw";
	gyp();
	build_bin();
	print("=== Testing GLFW Window ===");
	test();

	print("=== Build SDL Window ===");
	clean();
	WINDOW_SYSTEM = "sdl";
	gyp();
	build_bin();
	print("=== Testing SDL Window ===");
	test();

@command(usage="Build documentation using jsdoc")
def doc(args):
	shell_command([
			"jsdoc", "-d", "./doc/apiDocs/", "./doc/api.js", "./Readme.md"
		]);

@command(usage="Prints the commandName and usage for each command")
def help(args):
	for x in commands:
		print("%s | %s" % (x, commands[x].usage));

@command(usage="Prints the current building enviroment")
def env(args):
	print("sys.platform = %s" % (sys.platform));
	print("sys.version = %s" % (sys.version));
	print("resolve_path(PROJECT_ROOT, \"tasks.py\") = %s" % (resolve_path(PROJECT_ROOT, "tasks.py")));

@command(usage="Runs CTags on the source directory")
def tags(args):
	srcPath = resolve_path(PROJECT_ROOT, "src");
	srcFiles = [ f for f in os.listdir(srcPath) if os.path.isfile(os.path.join(srcPath,f)) ];
	srcFiles = [ "src/" + f for f in srcFiles if f != ".DS_Store"];
	shell_command([
			CTAGS_PATH, "--c++-kinds=+p", "--fields=+iaS", "--extra=+q"] + srcFiles);

@command(requires=["build_env"], usage="Run the engine and take a screenshot after 1 second automaticly")
def screenshot(args):
	output = subprocess.check_output([resolve_path(PROJECT_BUILD_PATH, get_exe_name()),
		"-devmode", "-debug", "-Ccore.test.screenshotTime=1"]);
	output = [f for f in output.split("\n") if "TestScreenshot - ####" in f];
	reg = re.search("to \[(.*)\]", output[0]);
	filename = reg.group(0);
	filename = filename[4:-1];
	shutil.copyfile(filename, resolve_path(PROJECT_ROOT, "screenshot.bmp"));

def run_command(cmdName, rawArgs):
	if not commands[cmdName].check():
		print("==== Skiping: %s" % (cmdName));
		return

	# run all required commands
	for cmd in commands[cmdName].requires:
		if not commands[cmdName].run: ## make sure the command is not run twice
			run_command(cmd, rawArgs);

	# finaly run the commadn the user is intrested in
	print(bcolors.HIGHLIGHT + "==== Running: %s ====" % (cmdName) + bcolors.ENDC);
	commands[cmdName](rawArgs);
	commands[cmdName].run = True;

def main(args):
	# require python version 2.7
	if sys.version_info < (2, 7) and sys.version_info < (3):
		print("tasks.py requires Python 2.7 or Greater");

	# get the command and run it
	if len(args) < 2:
		print("tasks.py : usage: ./tasks.py [commandName]");
	elif not args[1] in commands:
		print("tasks.py : command %s not found" % (args[1]));
	else:
		run_command(args[1], args);

	write_config();

if __name__ == '__main__':
  main(sys.argv);
