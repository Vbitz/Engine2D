Current System
====================

The current config system used by the Engine is inspired by the common place use of cVars in the industry but they have disadvantages which I've started running into. For sake of completeness there are also some advantages worth preserving.

Advantages
-----------------------
- Easy to program with
- Standard layout compared to the game development industry

Disadvantages
-----------------------
- No support for access controls, important for integrity verification
- The naming right now is borderline meaningless (not a issue with the code more with how I am using it)
- Very limited typing support (Just string, bool and number)
- The loading implementation is slow, right now it's the main reason for the JS bootloader.
- Outside of gamedev most of these issues are solved.
- Messy support for overlaying configs on top of each other.

New System
====================

The Engine is headed towards modularity in the future and fixing the config system now will have payoffs as the code base becomes more complex.

From a API level it stay the same though the C++ side could do with a update to match a more modern C++ API.

```
// get
Config::Get("core.window.width")->Int32Value();
// set
Config::Set("core.renderer.textureMipmaps", false);
```
The rational behind having get return a pointer rather then a value is to avoid a lookup in a map each time get is called.

The JS side will be identical for now.

I don't see any reason to stop storing the configs in JSON although research into a more readable format like YAML could be more useful for this file.

As for access controls a system inspired off that of Window's Access Control Lists will likely be a good solution, where you can control whether or not a key can be written to using JavaScript or other user means like the -C command line arg.
