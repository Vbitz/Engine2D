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
- /site/ : R : Site Specific folder - Mapped to a default user-specfic folder on supported platforms
- /save/ : RW : Mapped to a user-specific or specified folder to save game data in
- /cache/ : RW : Mapped to a semi-tempory folder to save pre-processed textures and models.