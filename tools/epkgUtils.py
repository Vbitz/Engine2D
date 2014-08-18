#!/usr/bin/env python

from ctypes import *
import argparse
import uuid

def engine_uuid_to_python(euuid):
	ruuid = (euuid[0] << 64) + euuid[1]
	return uuid.UUID(int=ruuid)

class PackageDiskHeader(Structure):
	_pack_ = 0
	_fields_ = [("magic", c_char * 4),
				("version", c_ushort),
				("firstFileOffset", c_uint),
				("thisUUID", c_ulong * 2),
				("patchUUID", c_ulong * 2),
				("firstIndexOffset", c_uint),
				("numOfFiles", c_ushort),
				("nextRegionOffset", c_uint),
				("nextFileHeaderOffset", c_uint)]

	def __str__(self):
		return "PackageDiskHeader(magic=%s,\n\tversion=%s,\n\tfirstFileOffset=%s, \
			\n\tthisUUID=%s,\n\tpatchUUID=%s,\n\tfirstIndexOffset=%s,\n\tnumOfFiles=%s, \
			\n\tnextRegionOffset=%s,\n\tnextFileHeaderOffset=%s)" % \
			(self.magic, self.version, self.firstFileOffset,
				engine_uuid_to_python(self.thisUUID), engine_uuid_to_python(self.patchUUID), self.firstIndexOffset,
				self.numOfFiles, self.nextRegionOffset, self.nextFileHeaderOffset)

class PackageDiskFile(Structure):
	_pack_ = 0
	_fields_ = [("magic", c_uint),
				("offset", c_uint),
				("size", c_uint),
				("decompressedSize", c_uint),
				("compression", c_ubyte),
				("encryption", c_ubyte),
				("padding1", c_ubyte * 2),
				("latestRevisonOffset", c_uint),
				("nextFileOffset", c_uint),
				("padding2", c_ubyte * 4),
				("name", c_char * 96)]

def main():
	parser = argparse.ArgumentParser(description='Read a .epkg file and output infomation')
	parser.add_argument("filename", nargs=1, help="the .epkg file to read")
	
	args = parser.parse_args()

	with open(args.filename[0], "r") as f:
		# read package header
		header = PackageDiskHeader()
		f.readinto(header)
		print header
		pdf = PackageDiskFile()
		f.seek(header.firstFileOffset)
		f.readinto(pdf)
		print pdf.name + " : " + str(pdf.offset) + " : " + str(pdf.size)
		while pdf.nextFileOffset != 0x00:
			f.seek(pdf.nextFileOffset)
			f.readinto(pdf)
			print pdf.name + " : " + str(pdf.offset) + " : " + str(pdf.size)

if __name__ == '__main__':
	main()