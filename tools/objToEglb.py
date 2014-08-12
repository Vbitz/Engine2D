#!/usr/bin/env python
# convert a .obj text file to .egbl binary format

import argparse
import sys
import struct
import random

def doConvert(inputFilename, outputFilename, scale):
	vertexs = []
	texcoords = []
	normals = []
	faces = []
	indexes = []
	# parse input file
	with open(inputFilename, "r") as fInput:
		for line in [l.strip() for l in fInput]:
			if len(line) == 0 or line[0] == '#':
				continue
			tokens = line.split(" ")
			if tokens[0] == "v":
				vertexs += [[float(tokens[1]) * scale, float(tokens[2]) * scale, float(tokens[3]) * scale]]
			elif tokens[0] == "vt":
				texcoords += [[float(tokens[1]), float(tokens[2])]]
			elif tokens[0] == "vn":
				normals += [[float(tokens[1]), float(tokens[2]), float(tokens[3])]]
			elif tokens[0] == "vp":
				# ignored
				pass
			elif tokens[0] == "f":
				newFace = []
				for t in tokens[1:]:
					newVertex = []
					faceTokens = t.split("/")
					if len(faceTokens) == 1:
						# vertex
						newVertex += [vertexs[int(faceTokens[0]) - 1]]
						newVertex += [[0, 0]]
					elif len(faceTokens) == 2:
						# vertex + texcoord
						newVertex += [vertexs[int(faceTokens[0]) - 1]]
						newVertex += [texcoords[int(faceTokens[1]) - 1]]
					elif len(faceTokens) == 3:
						# vertex + texcoord? + normal
						newVertex += [vertexs[int(faceTokens[0]) - 1]]
						if len(faceTokens[0]) > 0:
							newVertex += [texcoords[int(faceTokens[1]) - 1]]
						else:
							newVertex += [[0, 0]]
					else:
						raise Exception("Unknown count of faceTokens on line %s" % (line))
					newFace += [newVertex]
				faces += [newFace]
			else:
				raise Exception("Unknown type in .obj file %s on line %s" % (tokens[0], line))

	vertexCount = 0
	with open(outputFilename, "w") as fOutput:
		# write header
		fOutput.write("EGLB")
		# jump foward 4 ints
		fOutput.seek(16, 1)
		# write vertexs
		for f in faces:
			for vertex in f:
				fOutput.write(struct.pack("<fffffffff",
					vertex[0][0], vertex[0][1], vertex[0][2],	# position	
					random.random(), random.random(), random.random(), 1,									# color (hardcoded to white)
					vertex[1][0], vertex[1][1]					# uv
					))
				vertexCount += 1
		indexOffset = fOutput.tell()
		# write indexes
		for i in range(0, vertexCount):
			fOutput.write(struct.pack("<H", i))
		# jump back to the start to write the header
		fOutput.seek(4, 0)
		fOutput.write(struct.pack("<IIII",
			4 + 16, # vertexOffset
			vertexCount, # vertexCount
			indexOffset, # indexOffset
			vertexCount  # indexCount
			))

def main(args):
	parser = argparse.ArgumentParser(description='Convert an .obj text file to .egbl binary format')
	parser.add_argument("--scale", default="1", help="factor to scale the input by")
	parser.add_argument("input", nargs=1, help="input file in .obj format")
	parser.add_argument("output", nargs=1, help="output file in .obj format")
	
	args = parser.parse_args()

	doConvert(args.input[0], args.output[0], float(args.scale))

if __name__ == '__main__':
	main(sys.argv)