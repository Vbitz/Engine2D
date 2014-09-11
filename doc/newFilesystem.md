New File-system
==============================

I've been using PhysFS since Engine2D started development pretty much and over time I've found it's lacked in some features.

- Sensitivity to current directory
- Only 1 write directory
- Lacking effective caching
- No nested reading
- No memory mapped IO
- My current auto-update detection code is a strain on the disk, I'd rather some OS specific more effective solution.

My best solution for this right now is to write my own FS layer in C++ which fixes these issues.