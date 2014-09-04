import re
import json

commentSearch = re.compile("/\*\*\*(.*?)\*/", re.DOTALL)

def getFromFile(filename):
	lines = []
	with open(filename, "r") as f:
		lines += f.readlines()
	linesStr = "\n".join(map(lambda b: b.strip(),lines))
	comments = map(lambda s: json.loads(s.strip()), commentSearch.findall(linesStr))
	return comments

print getFromFile("src/JSDraw.cpp")