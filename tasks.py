#!/usr/bin/env python

# Engine2D Task Scripts
# Built for Python 2.7

import sys;

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

@command(usage="Builds platform project files")
def gyp():
	print("Running GYP");

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
		if not commands[cmdName].run:
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