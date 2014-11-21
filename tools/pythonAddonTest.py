#!/usr/bin/env python

import ctypes

StartupFunction = ctypes.CFUNCTYPE(None)
ShutdownFunction = ctypes.CFUNCTYPE(None)

class AddonSpec(ctypes.Structure):
	_fields_ = [("uuid", ctypes.c_char_p),
				("name", ctypes.c_char_p),
				("author", ctypes.c_char_p),
				("startup", StartupFunction),
				("shutdown", ShutdownFunction),
				("load", ctypes.c_short)]

def addonInit():
	print "Hello, World from a addon"

startup = StartupFunction(addonInit)
shutdown = ShutdownFunction(0)

def main():
	edll = ctypes.cdll.LoadLibrary("build/Default/libengine2D.dylib")
	edll.RegisterCAddon(AddonSpec("131231", "Hello From Python", "Vbitz", startup, shutdown, 400))
	argvArray = ctypes.c_char_p * 3
	argv = argvArray("build/Default/engine2D", "-devmode", "-debug")
	edll.EngineMain(3, argv)

if __name__ == '__main__':
	main()