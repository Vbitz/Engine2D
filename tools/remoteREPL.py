#!/usr/bin/env python

import sys;
import socket;
import json;
import time;
import atexit;

currentSeq = 0;

def connectToV8(host, port):
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM);
	s.connect((host, port));
	return s

def sendRequest(s, cmd, args=None):
	global currentSeq;
	obj = {
		"seq": currentSeq,
		"type": "request",
		"command": cmd
		};
	if (args != None):
		obj["arguments"] = args;
	data = json.dumps(obj);
	s.sendall("Content-Length: " + str(len(data)) + "\r\n\r\n" + data);
	currentSeq = currentSeq + 1;
	time.sleep(0.25);
	res = s.recv(4096);
	res = res[(res.find("\r\n") + 4):];
	res = json.loads(res);
	return res;

def evaluate(s, code):
	res = sendRequest(s, "evaluate", {"expression": code, "global": True});
	if (res["success"]):
		print(res["body"]["text"]);
	else:
		print("Error: " + res["message"]);

@atexit.register
def disconnect():
	global s;
	sendRequest(s, "disconnect");
	s.close();

def main(args):
	global s;
	s = connectToV8("localhost", 5858);
	time.sleep(0.25);
	s.recv(4096);
	time.sleep(0.25);
	while (True):
		evaluate(s, raw_input("> "));

if __name__ == '__main__':
  main(sys.argv);