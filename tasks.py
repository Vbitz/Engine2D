#!/usr/bin/env python

# Engine2D Task Scripts
# Built for Python 2.7

import sys;

PROJECT_ROOT = 0;

commands = {};

class command(object):
	def __init__(self, requires=[], usage="TODO: "):
		self.requires = requires;
		self.usage = usage;
	
	def __call__(self, func):
		commands[func.__name__] = func;
		commands[func.__name__].requires = self.requires;
		commands[func.__name__].usage = self.usage;
		commands[func.__name__].run = False;
		return func;

def resolve_path(base, path):
	return path;

def require_in_path(arr):
	for x in arr:
		pass;
	return True;

def shell_command(cmd, throw=False):
	print("shell : %s" % (cmd));

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

@command(requires=["fetch_svn"], usage="Downloads the latest version of GYP using SVN")
def fetch_gyp():
	# check for GYP in third_party first
	# if it's not there then download it using SVN
	pass;

@command(requires=["fetch_gyp"], usage="Builds platform project files")
def gyp():
	# run GYP
	shell_command(" ".join([
			resolve_path(PROJECT_ROOT, "third_party/gyp/gyp"),
			"--depth 0",
			resolve_path(PROJECT_ROOT, "engine2D.gyp")
		]), True);

@command(requires=["gyp"], usage="Open's the platform specfic IDE")
def ide():
	print("Opening IDE");

@command(requires=["gyp"], usage="Builds executables")
def buildBinary():
	print("Running Build");

@command(requires=["buildBinary"], usage="Copys support files to the build path")
def buildEnviroment():
	print("Copying Enviroment");

@command(requires=["buildEnviroment"], usage="Runs the engine in Development Mode")
def run():
	print("Running Run");

@command(usage="Prints the commandName and usage for each command")
def help():
	for x in commands:
		print("%s | %s" % (x, commands[x].usage));

def run_command(cmdName):
	# run all required commands
	for cmd in commands[cmdName].requires:
		if not commands[cmdName].run: ## make sure the command is not run twice
			run_command(cmd);

	# finaly run the commadn the user is intrested in
	commands[cmdName]();
	commands[cmdName].run = True;

def main(args):
	# require python version 2.7

	# check for GYP

	# get the command and run it
	if len(args) < 2:
		print("tasks.py : usage: ./tasks.py [commandName]");
	elif not args[1] in commands:
		print("tasks.py : command %s not found" % (args[1]));
	else:
		run_command(args[1]);

if __name__ == '__main__':
  main(sys.argv);