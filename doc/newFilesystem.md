New Filesystem
==============================
I've been using PhysFS since Engine2D started development pretty much and over time I've found it's lacked in some features.

- Sensitivity to current directory
- Only 1 write directory
- Lacking effective caching
- No nested reading
- No memory mapped IO
- My current auto-update detection code is a strain on the disk, I'd rather some OS specific more effective solution.

My best solution for this right now is to write my own FS layer in C++ which fixes these issues.

Filesystem Layout
------------------------------
Like PhysFS/Unix I prefer the common root design for operating systems.

- / : Root - Unmappable
- /resource/ : R : Content folder - Mapped by default to res/ in the current working directory and App.app/Contents/Resources in OSX
- /site/ : R : Site Specific folder - Mapped to a default user-specific folder on supported platforms.
- /save/ : RW : Mapped to a user-specific or specified folder to save game data in.
- /cache/ : RW : Mapped to a semi-temporary folder to save pre-processed textures and models.

Packages
------------------------------
The new FS layer transparently handles packages without much user interaction, any writes to package files are automatically passed on to the package system.

**e.g.**
```javascript
fData = fs.readFile("/resource/maps/testing.epkg/data/map.json", "json");
```

In that example a bit of map data is being read from inside a package file, although packages themselves don't have any concept of directory's / is not a special character so files may be indexed as if directory's don't exist.

Writing to packages is a bit more complex since the user may want to encrypt content or compress content, certain file formats are automatically compressed and others are compressed in the offline baking process. Encryption/Decryption is handled by KeyProviders, if a file is associated with a key then it's automatically encrypted.