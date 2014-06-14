#!/usr/bin/env python

# this scripts reads every #include in in C / C++ files and outputs it to stdout as a .dot file

import sys;
import re;
from os import walk, getcwd;
from os.path import isfile, join, realpath, relpath;

ignoreFiles = [".DS_Store"];
searchExts = ["cpp", "hpp", "c", "h"];

includeString = "#include";
basePath = getcwd();

def comment_remover(text):
    def replacer(match):
        s = match.group(0)
        if s.startswith('/'):
            return " " # note: a space and not an empty string
        else:
            return s
    pattern = re.compile(
        r'//.*?$|/\*.*?\*/|\'(?:\\.|[^\\\'])*\'|"(?:\\.|[^\\"])*"',
        re.DOTALL | re.MULTILINE
    )
    return re.sub(pattern, replacer, text)

def readFileIncludes(dirpath, filename):
	with open(filename) as f:
		for l in [l for l in comment_remover(f.read()).splitlines() if l.strip().startswith("#include")]:
			l = l[len(includeString):].strip();
			lFile = l[1:-1];
			if l.startswith("<"):
				print("\"" + filename + "\" -> \"<" + lFile + ">\"");
				pass
			elif l.startswith("\""):
				print("\"" + filename + "\" -> \"" + relpath(realpath(join(dirpath, lFile)), basePath) + "\"");


def walkDirectory(dirPath):
	for (dirpath, dirnames, filenames) in walk(dirPath):
		for f in [join(dirpath, f) for f in filenames if (f not in ignoreFiles) and (f.split(".")[-1] in searchExts)]:
			readFileIncludes(dirpath, f);

def main(args):
	print("digraph depends {");
	walkDirectory("src");
	print("}");

if __name__ == '__main__':
	main(sys.argv);