// Engine2D Javascript API Documentation
// Correct as of 14th Jan 2014

this.global = this; // Global object exposed as global

/** @namespace */
global.console = {};

/**
 * Low level logging function that prints obj at level
 * 
 * @param  {string} level - The level to log at can be "warning"|"verbose"|"error"|"raw"|"log"
 * @param  {...*} obj - The value to print, internaly the values will be joined with spaces
 * @internal
 */
global.console._log = function (level, obj) {};

/**
 * Hide all output on the EngineUI console
 */
global.console.clear = function () {};

/**
 * Toggle the EngineUI console
 */
global.console.toggle = function () {};

/**
 * Derived off global.console._log, prints with a user loglevel
 * @param  {...*} obj - The value to print, internaly the values will be joined with spaces
 */
global.console.log = function (obj) {};

/**
 * Derived off global.console._log, prints with a warning loglevel
 * @param  {...*} obj - The value to print, internaly the values will be joined with spaces
 */
global.console.warn = function (obj) {};

/**
 * Derived off global.console._log, prints with a error loglevel
 * @param  {...*} obj - The value to print, internaly the values will be joined with spaces
 */
global.console.error = function (obj) {};

/**
 * Derived off global.console._log, prints with a verbose loglevel
 * @param  {...*} obj - The value to print, internaly the values will be joined with spaces
 */
global.console.verbose = function (obj) {};

/**
 * Derived off global.console._log, prints without using the Logger interface directly to the system console
 * @param  {...*} obj - The value to print, internaly the values will be joined with spaces
 */
global.console.writeRaw = function (obj) {};

/** @namespace */
global.sys = {};

/**
 * Returns an array of command line arguments passed to the engine
 * @return {string[]}
 */
global.sys.argv = function () {};

/**
 * Load and execute a javascript file from the filesystem
 * Requires that core.script.autoReload == true
 * @param  {string} filename - The filename of the script to load without the .js extention
 * @param  {boolean} persists - Should this file be automaticly reloaded?
 */
global.sys.runFile = function (filename, persists) {};

/**
 * @callback DrawFunction
 */

/**
 * Calls func each frame so drawing can be performed
 * @deprecated In favor of the "draw" event
 * @param  {DrawFunction} func
 */
global.sys.drawFunc = function (func) {};

/**
 * @callback KeyboardFunction
 * @param {null} _ - Always ""
 * @param {string} char - The key pressed/released
 * @param {boolean} press - True if the key was pressed otherwise false
 */

/**
 * Calls func whenever a key is pressed
 * @deprecated In favor of the "input" event
 * @param  {KeyboardFunction} func
 */
global.sys.keyboardFunc = function (func) {};

/**
 * @callback EventCallback
 * @param {Object} args - Event Arguments
 */

/**
 * Registers func to be called when event is emited
 * @param  {string} event
 * @param  {string} id - A unique ID to assoiate with the function
 * @param  {Object} [filter] - Filter events to not execute if 1 of the values does not match the emited arguments
 * @param  {EventCallback} func
 */
global.sys.on = function (event, id, filter, func) {};

/**
 * Emits event to all listening handlers with args
 * @param  {string} event
 * @param  {Object} [args]
 */
global.sys.emit = function (event, args) {};

/**
 * Disables any event with id
 * @param  {string} id
 */
global.sys.clearEvent = function (id) {};

/**
 * Returns the number of seconds since the engine started with at least milisecond accuracy
 * @return {number}
 */
global.sys.microtime = function () {};

/**
 * @typedef {Object} HeapStats
 * @property {number} heapLimit  The maximum size of the heap
 * @property {number} heapTotalSize  The total size of the heap
 * @property {number} heapTotalExecSize  The total amount of execuatable memory in the heap
 * @property {number} heapUsed  The used amount of heap space
 */

/**
 * Returns a object containing current heap statistics from V8
 * @return {HeapStats}
 */
global.sys.heapStats = function () {};

/**
 * @typedef {Object} MemoryStats
 * @property {number} totalVirtual  The amount of virtual memory in use by the system
 * @property {number} totalVirtualFree  The amount of free virtual memory avalible for processes on the system
 * @property {number} myVirtualUsed  The amount of virtual memory in use by the Engine
 * @property {number} totalPhysical  The total amount of physical memory installed in the system
 * @property {number} totalPhysicalFree  How much free memory the system has
 * @property {number} myPhysicalUsed  How much physical memory the Engine is using
 */

/**
 * Returns a object of current system memory stats
 * @return {MemoryStats}
 */
global.sys.memoryStats = function () {};

/**
 * Dumps a stacktrace to the console, this won't relyably show which C++ function called the script
 */
global.sys.trace = function () {};

/**
 * Exits the engine after the frame ends
 */
global.sys.exit = function () {};

/**
 * @typedef {Object} OpenGLVersion
 * @property {number} major  The major version of OpenGL
 * @property {number} minor  The minor version of OpenGL
 * @property {number} rev  The revision of OpenGL
 */

/**
 * Returns a object contining the current OpenGL version
 * @return {OpenGLVersion}
 */
global.sys.getGLVersion = function () {};

/**
 * Returns true if the OpenGL extention is avalibe to current instance of the Engine
 * @param  {strirg}  extention
 * @return {Boolean}
 */
global.sys.hasExtention = function (extention) {};

/**
 * Returns a list of all OpenGL extentions supported in the current instance of the Engine
 * @return {string[]}
 */
global.sys.getExtentions = function () {};

/**
 * Returns the maximum texture size supported by the Graphics Card installed in the system
 * @return {number}
 */
global.sys.getMaxTextureSize = function () {};

/**
 * Save a screenshot at the end of the frame to path
 * requires that fs.configDir has been called beforehand
 * @param  {string} path
 */
global.sys.saveScreenshot = function (path) {};

/**
 * Resizes the current window to width by height
 * @param  {number} width
 * @param  {number} height
 */
global.sys.resizeWindow = function (width, height) {};

/**
 * Toggles the window into fullscreen mode
 * @deprecated Crashes OpenGL 3.x due to a bug in the Engine
 */
global.sys.toggleFullscreen = function () {};

/**
 * Closes and reopens the window resetting any render paramters
 * @deprecated Crashes OpenGL 3.x due to a bug in the Engine
 */
global.sys.restartRenderer = function () {};

/**
 * Returns the last timespan zone took in seconds
 * @param  {string} zone
 * @return {number}
 */
global.sys.getProfilerTime = function (zone) {};

/**
 * Returns a list of all profiler zones in this session
 * @return {string[]}
 */
global.sys.getProfilerZones = function () {};

/**
 * @callback TimingFuncion
 */

/**
 * Time how long it takes for func to execute using the profiler
 * Requires core.debug.profiler
 * @param  {string} name - A label to use for the results
 * @param  {TimingFuncion} func
 */
global.sys.perf = function () {};

/**
 * Time how long it takes for func to execute
 * @param  {string} name - A label to use for the results
 * @param  {TimingFuncion} func
 */
global.sys.time = function (name, func) {};

/**
 * Sets and Gets config paramters
 * @example <caption>Without any arguments it prints a list of all configs</caption>
 * sys.config();
 * @example <caption>Just passing a key returns the current value</caption>
 * var startingWidth = sys.config("core.window.width");
 * @example <caption>Passing a value sets key=value</caption>
 * sys.config("core.runOnIdle", true);
 * @param  {string} key
 * @param  {string|boolean|number} value
 */
global.sys.config = function (key, value) {};

/**
 * Manuly invokes the garbage collector.
 */
global.sys.gc = function () {};

/**
 * Captures profiling results for frames and then writes them in CSV format to filename, fs.configDir has be called beforehand.
 * @example
 * fs.configDir("example");
 * sys.profile(100, "profileResults.csv");
 * @param  {number} frames
 * @param  {string} filename
 */
global.sys.profile = function (frames, filename) {};

/**
 * Changes enables or disables maximum profile zone time. A warning will be printed if this time is exceaded.
 * @example
 * sys.profileSet("Frame", false); // Disable frametime performace warnings
 * @param  {string} profileZone - The zone to modify
 * @param  {number|boolean} maxTime - The maximum time in seconds that the zone can take before printing a warning, pass false to disable
 */
global.sys.profileSet = function (profileZone, maxTime) {};

/**
 * Forces lib/boot.js to be reloaded at the start of the next frame
 */
global.sys.reloadRootScript = function () {};

/**
 * Forces script to be reloaded at the start of the next frame
 * @example
 * function lose() {
 *     sys.forceReload("script/game.js"); // Restart the game when the player loses
 * }
 * @param  {string} script - The filename of the script to reload including the extention
 */
global.sys.forceReload = function (script) {};


/**
 * @typedef {object} VersionInfo
 * @property {string} openGL  The OpenGL version currently in use
 * @property {string} glew  The version of GLEW the engine was compiled with
 * @property {string} v8  The version of V8 the engine was compiled with
 * @property {string} engine  The release version identifyer for the engine 
 * @property {string} glfw  The version of GLFW compiled into the engine
 * @property {string} glsl  The version of GLSL currently in use with OpenGL
 */

/**
 * Returns a {@link VersionInfo} object specifying the versions in use currently
 * @return {VersionInfo}
 */
global.sys.version = function () {};

/**
 * Shows a platform dependent MessageBox
 * @param  {string} title
 * @param  {string} msg
 * @param  {boolean} modal - Should the code block until the box is closed
 */
global.sys.msgBox = function (title, msg, modal) {};

/**
 * Open filename with the default handler for the format
 * @example
 * sys.shell("http://vbitz.com/"); // Open vbitz.com with the default web browser
 * @param  {string} filename
 */
global.sys.shell = function (filename) {};

/**
 * The current platform the engine is running on, the value can be "Windows"|"Darwin (OSX)"|"Linux"
 * @type {String}
 */
global.sys.platform = "";

/**
 * Is the engine running in developer mode. Developer mode allows arbitiry code execution and enables the console and profiler
 * @type {Boolean}
 */
global.sys.devMode = false;

/**
 * Has the OpenGL context been created yet?
 * @type {Boolean}
 */
global.sys.preload = false;

/**
 * The number of processers installed in the system.
 * @type {Number}
 */
global.sys.numProcessers = 0;

/**
 * The username of the currently logged in user.
 * @type {String}
 */
global.sys.username = "";

/**
 * The width of the Engine viewpoint
 * @type {Number}
 */
global.sys.screenWidth = 0;

/**
 * The height of the Engine viewpoint
 * @type {Number}
 */
global.sys.screenHeight = 0;

/**
 * The number of seconds since the last frame.
 * @type {Number}
 */
global.sys.deltaTime = 0;

/** @namespace */
global.fs = {};

/**
 * Returns the content of the file mounted at path.
 * @param  {string} filename
 * @param  {boolean} [raw] - If true the function will return a raw byte array
 * @return {string|number[]}
 */
global.fs.readFile = function (filename, raw) {};

/**
 * Writes content to filename mounted on PhysFS
 * @param  {string} filename
 * @param  {string} content
 */
global.fs.writeFile = function (filename, content) {};

/**
 * Returns true if filename exists.
 * @param  {string} filename
 * @return {boolean}
 */
global.fs.fileExists = function (filename) {};

/**
 * Returns the filesize of filename in bytes
 * @param  {string} filename
 * @return {number}
 */
global.fs.fileSize = function (filename) {};

/**
 * Mounts archivePath to mountPath on PhysFS. filetypes supported by archivePath including .zip and .7z
 * @param  {string} archivePath
 * @param  {string} mountPath
 */
global.fs.mountFile = function (archivePath, mountPath) {};

/**
 * Set's up a directory under the user's local profile to write files to. This function must be called before any files can be writen.
 * @param  {string} appName - The name of the directory, this should be a unique application name
 */
global.fs.configDir = function (appName) {};

/**
 * Create's a new directory at path.
 * @param  {string} path
 */
global.fs.mkdir = function (path) {};

/** @namespace */
global.draw = {};

/**
 * Draw a rectange with the current color at x,y, w, h in size
 * @param  {number} x
 * @param  {number} y
 * @param  {number} w
 * @param  {number} h
 */
global.draw.rect = function (x, y, w, h) {};

/**
 * Draw a rectange outline with the current color at x,y, w, h in size
 * @param  {number} x
 * @param  {number} y
 * @param  {number} w
 * @param  {number} h
 */
global.draw.grid = function (x, y, w, h) {};

/**
 * Draw a rectange filled with a left/right or up/down gradient specifyed by col1 and col2 at x,y, w, h in size
 * @param  {number} x
 * @param  {number} y
 * @param  {number} w
 * @param  {number} h
 * @param  {number} col1 - Specifyed as a number with the format 0xrrggbb
 * @param  {number} col2 - Specifyed as a number with the format 0xrrggbb
 * @param  {boolean} orientation - true for up/down and false for left/right
 */
global.draw.grad = function (x, y, w, h, col1, col2, orientation) {};

/**
 * Draw circular primatives including pie slices, doughnuts and circles
 * @param  {number} x
 * @param  {number} y
 * @param  {number} radius
 * @param  {number} [innerRadius] - Set to radius to draw a regular circle, radius by default
 * @param  {number} [numberOfSides] - The number of vertex's at the edge of the circle, radius * 5 by default
 * @param  {number} [startPos] - Start persent of the circle, 0.0 by default
 * @param  {number} [endPos] - End persent of the circle, 1.0 by default
 * @param  {number} [fillStyle] - Fill the inside of the circle or draw a line strip at the edge
 */
global.draw.circle = function (x, y, radius, innerRadius, numberOfSides, startPos, endPos, fillStyle) {};

/**
 * Draw a line from x0, y0 to x1, y1
 * @param  {number} x0
 * @param  {number} y0
 * @param  {number} x1
 * @param  {number} y1
 */
global.draw.line = function (x0, y0, x1, y1) {};

/**
 * Draw a cubic benzier curve from x0, y0, to x3, y3 through x1, y1 and x2, y2
 * @param  {number} x0
 * @param  {number} y0
 * @param  {number} x1
 * @param  {number} y1
 * @param  {number} x2
 * @param  {number} y2
 * @param  {number} x3
 * @param  {number} y3
 */
global.draw.curve = function (x0, y0, x1, y1, x2, y2, x3, y3) {};

/**
 * Set's predefined colors for use in draw.setColor
 * @param  {string|Object} name
 * @param  {number} color
 * @example <caption>Used with 2 arguments draw.colorPalette accepts a string and a color</caption>
 * draw.colorPalette("blue", 0x0000ff);
 * @example <caption>Used with 1 object draw.colorPalette set's each string to a color value</caption>
 * draw.colorPalette({
 *     "red": 0xff0000,
 *     "green": 0x00ff00,
 *     "blue": 0x0000ff
 * });
 */
global.draw.colorPalette = function (name, color) {};

/**
 * Set's the current drawing color to r, g, b
 * @param {number} r - The red value between 0.0 and 1.0
 * @param {number} g - The green value between 0.0 and 1.0
 * @param {number} b - The blue value between 0.0 and 1.0
 */
global.draw.setColorF = function (r, g, b) {};

/**
 * @typedef {Object} Color
 * @property {number} r The red value between 0.0 and 1.0
 * @property {number} g The green value between 0.0 and 1.0
 * @property {number} b The blue value between 0.0 and 1.0
 */

/**
 * Set's the current drawing color
 * @param {string|number|Color} color
 * @example <caption>Passing a string set's the color to a predefined color</caption>
 * draw.setColor("red");
 * @example <caption>Passing a number in the format 0xrrggbb set's the color to that value</caption>
 * draw.setColor(0xff0000);
 * @example <caption>Passing a {@link Color} set's it to the value inside color</caption>
 * draw.setColor({r: 1.0, g: 0.0, b: 0.0})
 */
global.draw.setColor = function (color) {};

/**
 * Set's the current drawing color to r, g, b
 * @param {number} r - The red value between 0 and 255
 * @param {number} g - The green value between 0 and 255
 * @param {number} b - The blue value between 0 and 255
 */
global.draw.setColorI = function (r, g, b) {};

/**
 * Set's the background clear color
 * @param  {string|number} color - Same argument format as {@link global.draw.setColor} without support for {@link Color}
 */
global.draw.clearColor = function (color) {};

/**
 * Returns a {@link Color} from the HSV values
 * @param  {number} h - The Hue Component between 0 and 360
 * @param  {number} s - The Saturation Component between 0.0f and 1.0f
 * @param  {number} v - The Value Component between 0.0f and 1.0f
 * @return {Color}
 */
global.draw.getRGBFromHSV = function (h, s, v) {};

/**
 * Prints str at x, y using the current font
 * @param  {number} x
 * @param  {number} y
 * @param  {string} str
 */
global.draw.print = function (x, y, str) {};

/**
 * Returns the width of str in pixels
 * @param  {string} str
 * @return {number}
 */
global.draw.getStringWidth = function (str) {};

/**
 * @typedef {number} TextureID
 */

/**
 * Renders the image specifyed by texId at x, y sized at w, h
 * @param  {TextureID} texId
 * @param  {number} x
 * @param  {number} y
 * @param  {number} w
 * @param  {number} h
 */
global.draw.draw = function (texId, x, y, w, h) {};

/**
 * Renders a segment of the image specifyed by texId at x, y sized at w, h
 * @param  {TextureID} texId
 * @param  {number} x
 * @param  {number} y
 * @param  {number} w
 * @param  {number} h
 * @param  {number} xSub
 * @param  {number} ySub
 * @param  {number} wSub
 * @param  {number} hSub
 */
global.draw.drawSub = function (texId, x, y, w, h, xSub, ySub, wSub, hSub) {};

/**
 * Loads filename as a image, most file formats are supported using FreeImage
 * @param  {string} filename
 * @return {TextureID}
 */
global.draw.openImage = function (filename) {};

/**
 * @typedef {number[]} Image
 * Image is optimised for loading using {@link global.draw.createImage}
 * @property {number} width The width of the image
 * @property {number} height The height of the image
 */

/**
 * Loads filename as a image returning the raw pixel array
 * @param  {string} filename
 * @return {Image}
 */
global.draw.getImageArray = function (filename) {};

/**
 * Returns a {@link Image} with width * height pixels
 * @param  {number} w
 * @param  {number} h
 * @return {Image}
 */
global.draw.createImageArray = function (w, h) {};

/**
 * Converts a {@link Image} into a {@link TextureID}
 * @param  {Image|number[]} arr - {@link Image} is strongly prefered to number[]
 * @param  {number} w - The width of the Image to create
 * @param  {number} h - The height of the Image to create
 * @return {TextureID}
 */
global.draw.createImage = function (arr, w, h) {};

/**
 * Save texId to filename, requires fs.configDir to be called before hand
 * @param  {TextureID} texId
 * @param  {string} filename
 */
global.draw.saveImage = function (texId, filename) {};

/**
 * Delete's texId from the system's graphics memory
 * @param  {TextureID} texId
 */
global.draw.freeImage = function (texId) {};

/**
 * Returns true if texId is valid
 * @param  {TextureID}  texId
 * @return {Boolean}
 */
global.draw.isTexture = function (texId) {};

/**
 * Reset the current camera positon
 * @deprecated Not avalible on OpenGL 3.3 yet
 */
global.draw.cameraReset = function () {};

/**
 * Pans the camera by x, y
 * @deprecated Not avalible on OpenGL 3.3 yet
 * @param  {number} x
 * @param  {number} y
 */
global.draw.cameraPan = function (x, y) {};

/**
 * Zooms the camera by factor f
 * @deprecated Not avalible on OpenGL 3.3 yet
 * @param  {number} f
 */
global.draw.cameraZoom = function (f) {};

/**
 * Rotates the camera by r degrees
 * @deprecated Not avalible on OpenGL 3.3 yet
 * @param  {number} r
 */
global.draw.cameraRotate = function (r) {};

/**
 * Set's the current font to prettyName, size. The font will be loaded if it does not exist
 * @param {string} prettyName
 * @param {number} size
 */
global.draw.setFont = function (prettyName, size) {};

/**
 * Load a .ttf font file. The font is not loaded until setFont is called at least once
 * @param  {string} prettyName
 * @param  {string} filename
 */
global.draw.loadFont = function (prettyName, filename) {};

/**
 * Returns true if the font pointed to by prettyName is loaded
 * @param  {string}  prettyName
 * @return {Boolean}
 */
global.draw.isFontLoaded = function (prettyName) {};

/**
 * Returns the number of verts rendered this frame
 * @return {number}
 */
global.draw.getVerts = function () {};

/**
 * Should offscreen objects be rendered
 * @deprecated Not Implamented
 * @param {boolean} draw
 */
global.draw.setDrawOffscreen = function (draw) {};

/**
 * Set's the point to center drawing around
 * @param {number} x
 * @param {number} y
 */
global.draw.setCenter = function (x, y) {};

/** @namespace */
global.input = {};

/**
 * Returns true if key is pressed
 * @param  {string} key
 * @return {boolean}
 */
global.input.keyDown = function (key) {};

/**
 * The X postion of the mouse
 * @type {Number}
 */
global.input.mouseX = 0;

/**
 * The Y postion of the mouse
 * @type {Number}
 */
global.input.mouseY = 0;

/**
 * The Left mouse button state
 * @type {Boolean}
 */
global.input.leftMouseButton = false;

/**
 * The Middle mouse button state
 * @type {Boolean}
 */
global.input.middleMouseButton = false;

/**
 * The Right mouse button state
 * @type {Boolean}
 */
global.input.rightMouseButton = false;

/** @namespace */
global.mod = {};

global.mod.open = function () {};
global.mod.call = function () {};

/** @namespace */
global.db = {};

global.db.open = function () {};
global.db.exec = function () {};
global.db.execPrepare = function () {};

/** Currently only defined if Developer Mode is enabled
 * @namespace
*/
global.unsafe = {};

global.unsafe.getNumberAddress = function () {};
global.unsafe.getNative = function () {};
global.unsafe.call = function () {};
global.unsafe.malloc = function () {};
global.unsafe.free = function () {};
global.unsafe.addressOf = function () {};
global.unsafe.mprotect = function () {};
global.unsafe.getPageSize = function () {};
