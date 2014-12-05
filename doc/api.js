// Engine2D Javascript API Documentation
// Correct as of 27th Oct 2014

/** @namespace */
this.global = this; // Global object exposed as global

/**
 * Runs cb after timeout seconds
 * @param {Function} cb      The function to call once the timer expires
 * @param {Number}   timeout The number of seconds to wait before cb is called
 * @return {Number} The ID to manage the timer
 */
global.setTimeout = function (cb, timeout) {};

/**
 * Runs cb every timeout seconds
 * @param {Function} cb       The function to call each time the timer expires
 * @param {Number}   interval The number of seconds between calls to cb
 * @return {Numver} The ID to manage the timer
 */
global.setInterval = function (cb, interval) {};

/**
 * Stops timeout id from being called
 * @param  {Number} id       The timer to remove
 */
global.clearTimeout = function (id) {};

/**
 * Stops interval id from being called
 * @param  {Number} id       The interval to remove
 */
global.clearInterval = function (id) {};

/**
 * Force Quits the engine if value converts to false.
 * @param  {*} value The value to check
 * @param  {String} msg   The message to print to the console if value == false
 * @return {*}       value
 */
global.assert = function (value, msg) {};

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
 * @bind none
 */
global.console.log = function (obj) {};

/**
 * Derived off global.console._log, prints with a warning loglevel
 * @param  {...*} obj - The value to print, internaly the values will be joined with spaces
 * @bind none
 */
global.console.warn = function (obj) {};

/**
 * Derived off global.console._log, prints with a error loglevel
 * @param  {...*} obj - The value to print, internaly the values will be joined with spaces
 * @bind none
 */
global.console.error = function (obj) {};

/**
 * Derived off global.console._log, prints with a verbose loglevel
 * @param  {...*} obj - The value to print, internaly the values will be joined with spaces
 * @bind none
 */
global.console.verbose = function (obj) {};

/**
 * Derived off global.console._log, prints without using the Logger interface directly to the system console
 * @param  {...*} obj - The value to print, internaly the values will be joined with spaces
 * @bind none
 */
global.console.writeRaw = function (obj) {};

/**
 * Derived off global.console._log, prints highlighted text at the highest log level
 * @param  {...*} obj - The value to print, internaly the values will be joined with spaces
 * @bind none
 */
global.console.highlight = function (obj) {};

/**
 * Derived off global.console._log, inserts a toast into the EngineUI display which overrides the console visiblity
 * @param  {...*} obj - The value to print, internaly the values will be joined with spaces
 * @bind none
 */
global.console.toast = function (obj) {};

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
 * Called each frame for draw code to be called.
 * All draw code should be performed during this event.
 * 
 * @event draw
 */

/**
 * TODO: Fill in event
 * 
 * @event dumpProfile
 */

/**
 * Called when the mouse is clicked
 * Params:
 * 		string buttonName = enum { mouseLeft, mouseRight, mouseMiddle,
 * 			mouse4, mouse5, mouse6, mouse7, mouse8, unknown };
 * 		string action = enum { press, release };
 * 		int rawMods;
 * 		int x, y;
 * 
 * @event mouseButton
 */

/**
 * Called when the frame finishes
 * 
 * @event endOfFrame
 */

/**
 * Called when a file is run using sys.runFile(filename, params)
 * Use EM_CANCEL to override the result
 * Params:
 * 		string path;
 * 
 * @event runFile
 */

/**
 * Called when a key is pressed,
 * there's also seprate events prefixed by key_ which are called when that key is pressed
 * Params:
 * 		int rawKey;
 * 		string key;
 * 		int rawPress
 *   	string state = enum { press, release, repeat };
 *    	bool shift;
 *    	
 * @event input
 */

// TODO: Fill in all the other events

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
 * Run any pending event handers
 * @param  {string} event
 */
global.sys.pollDeferedMessages = function (event) {};

/**
 * Set defer to prevent events from executing until pollDeferedMessages is called
 * @param {string} event
 * @param {bool} defer
 */
global.sys.setDeferedEvent = function (event, defer) {};

/**
 * Disables any event with id
 * @param  {string} id
 */
global.sys.clearEvent = function (id) {};

global.sys.createTimer = function () {};

global.sys.deleteTimer = function () {};

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
 * @param  {string}  extention
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
 * Resizes the current window to width by height
 * @param  {number} width
 * @param  {number} height
 */
global.sys.resizeWindow = function (width, height) {};

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
 * Shows a platform MessageBox
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
 * Returns a newly generated UUID
 * @return {string}
 */
global.sys.uuid = function () {};

/**
 * Saves the entire log to filename, fs.configDir has be called beforehand.
 * @param  {string} filename The filename to save the log to
 */
global.sys.dumpLog = function (filename) { };

/** @namespace */
function ThreadAPI () { }

/**
 * The same as console.log but identifys the line with the thread that created it
 * @param  {...*} obj - The value to print, internaly the values will be joined with spaces
 */
ThreadAPI.prototype.log = function () { };

/**
 * Suspends the thread for secs
 * @param  {Number} secs mesured in seconds
 */
ThreadAPI.prototype.sleep = function (secs) { };

/**
 * Sends a event to functions listening for target event.
 * If the target is on another thread it will be defered to be dispatched by the event loop on the main thread
 * @param  {String} target
 * @param  {*} args
 */
ThreadAPI.prototype.emit = function (target, args) { };

/**
 * @callback WorkerFunction
 * This API uses a little black magic, internaly the source of the function is fetched and dispatched to a worker thread where
 * it's compiled and executed in a brand new V8 instance, this meens that any globals or most regular API's don't work insided.
 * To reduce confusion the Threading API is provided as a argument
 * @param {ThreadAPI} $thread
 */

/**
 * Creates a new thread executing workerFunc
 * @param  {WorkerFunction} workerFunc
 */
global.sys.createWorker = function (workerFunc) { };

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
 * Is the engine running in debug mode.
 * @type {Boolean}
 */
global.sys.debugMode = false;

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

/**
 * A read write object providing access to the config system
 * @type {Object}
 */
global.sys.runtimeConfig = {};

/**
 * A read write object providing a interface to the event system
 * @example <caption>You can bind new events</caption>
 * event.draw = function test_event (args) { draw.rect(0, 0, 100, 100); };
 * @example <caption>You can call existing events</caption>
 * event.exit();
 * event.detailProfile({filename: "test.log", frames: 100});
 * @type {Object}
 */
global.event = {};

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

/**
 * Returns a list of all filenames in path
 * @param  {string} path The path to list
 * @return {string[]}
 */
global.fs.lsdir = function (path) {};

/**
 * Returns the SHA512 hash of a file
 * @param  {string} path 
 * @return {string}
 */
global.fs.hashFile = function (path) {};

/**
 * Has the config been set yet
 * @readOnly
 * @type {Boolean}
 */
global.fs.hasSetConfigDir = false;

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
 * Draw a line from x0, y0 to x1, y1
 * @param  {number} x0
 * @param  {number} y0
 * @param  {number} x1
 * @param  {number} y1
 */
global.draw.line = function (x0, y0, x1, y1) {};

/**
 * Renders a polygon using a triangle fan with arr points
 * @param  {number} xCenter [description]
 * @param  {number} yCenter [description]
 * @param  {number[]} arr     In the format [x1, y1, x2, y2, ...]
 */
global.draw.polygon = function (xCenter, yCenter, arr) {};

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
 * @typedef {Object} Color
 * @property {number} r The red value between 0.0 and 1.0
 * @property {number} g The green value between 0.0 and 1.0
 * @property {number} b The blue value between 0.0 and 1.0
 */

/**
 * Set's the current drawing color to r, g, b
 * @param {number} r - The red value between 0.0 and 1.0
 * @param {number} g - The green value between 0.0 and 1.0
 * @param {number} b - The blue value between 0.0 and 1.0
 */
global.draw.setColorF = function (r, g, b) {};

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
 * Returns the current draw color
 * @return {Color}
 */
global.draw.getColor = function () {};

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
 * Renders a sprite using spriteSheet, this sprite can be animated in the sprite definition
 * @param  {SpriteSheet} spriteSheet The spritesheet to use
 * @param  {string} sprite           The sprite to draw
 * @param  {number} x
 * @param  {number} y
 * @param  {number} w
 * @param  {number} h
 */
global.draw.drawSprite = function (spriteSheet, sprite, x, y, w, h) {};

/**
 * Loads filename as a image, most file formats are supported using FreeImage
 * @param  {string} filename
 * @return {TextureID}
 */
global.draw.openImage = function (filename) {};

/**
 * Loads filename as a spritesheet, spritesheets are specfiyed using a JSON format
 * @param  {string} filename      The file to load
 * @return {SpriteSheet}          The loaded sprite sheet
 */
global.draw.openSpriteSheet = function (filename) {};

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
 * Returns true if texId is valid
 * @param  {TextureID}  texId
 * @return {Boolean}
 */
global.draw.isTexture = function (texId) {};

/**
 * Returns true if spriteSheet is valid
 * @param  {SpriteSheet} 
 * @return {Boolean}
 */
global.draw.isSpiteSheet = function (spriteSheet) {};

/**
 * Reset the current camera positon
 */
global.draw.cameraReset = function () {};

/**
 * Pans the camera by x, y
 * @param  {number} x
 * @param  {number} y
 */
global.draw.cameraPan = function (x, y) {};

/**
 * Zooms the camera by factor f
 * @param  {number} f
 */
global.draw.cameraZoom = function (f) {};

/**
 * Rotates the camera by r degrees
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
 * Set's the point to center drawing around
 * @param {number} x
 * @param {number} y
 */
global.draw.setCenter = function (x, y) {};

/**
 * Read/Write value for draw color
 * @type {Color}
 */
global.draw.drawColor = {r: 0, g: 0, b: 0};

/** @namespace */
global.input = {};

/**
 * Returns true if key is pressed
 * @param  {string} key
 * @return {boolean}
 */
global.input.keyDown = function (key) {};

/**
 * Enable or disable capturing the mouse to the window
 * @param {boolean} val
 */
global.input.setCaptureMouse = function (val) {};

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

/** 
 * db is powered by sqlite3 and uses SQL syntax for statements
 * @namespace
 */
global.db = {};

/**
 * Constructor for sqlite3 database
 * @constructor
 * @param {string} filename The filename to load the SQLite3 database from
 */
global.db.Database = function (filename) {};

/**
 * Run a SQL query on the database
 * @param  {string} sql The valid SQL query to execute on the database
 */
global.db.Database.prototype.exec = function (sql) {};

/**
 * Run a SQL query on the database returning the result as a array
 * @param  {string} sql The valid SQL query to execute on the database
 * @return {object[]}   return value as a list of objects
 */
global.db.Database.prototype.execPrepare = function (sql) {};

/**
 * Exposed as a dynamicly loadable lib, test.js uses
 * sys.runFile("modules/js_unsafe.dylib", false)
 * to run it
 * @namespace
*/
global.unsafe = {};

/**
 * @typedef {number} Address
 * Warning, this is really a double thanks to JavaScript so be careful about accuracy
 */

/**
 * @typedef {number[]} MallocArray
 * This array is a read/write view of the memory at the location it was created
 */

/**
 * Allocates a buffer to store number and returns the address
 * @param  {number} number
 * @return {Address}
 */
global.unsafe.getNumberAddress = function (number) {};

/**
 * Returns a {@link MallocArray} for address
 * @param  {Address} address
 * @param  {number} length
 * @return {MallocArray}
 */
global.unsafe.getNative = function (address, length) {};

/**
 * Casts address to a function pointer and calls it
 * @param  {Address} address
 */
global.unsafe.call = function (address) {};

/**
 * Allocates a {@link MallocArray} array of length
 * @param  {number} length
 * @return {MallocArray}
 */
global.unsafe.malloc = function (length) {};

/**
 * Frees the memory in a {@link MallocArray}
 * @param  {MallocArray} array
 */
global.unsafe.free = function (array) {};

/**
 * Returns the real address of a {@link MallocArray}
 * @param  {MallocArray} array
 * @return {Address}
 */
global.unsafe.addressOf = function (array) {};

/**
 * Set's execute permission on pages of memory. Returns true if it's sucessful
 * @param  {Address} address - Needs to be aligned to nearest page.
 * @param  {number} length
 * @param  {boolean} enable
 * @return {boolean}
 */
global.unsafe.mprotect = function (address, length, enable) {};

/**
 * Returns the current system pagesize, the only safe method in this namespace
 * @bind smart_noContext
 * @return {number}
 */
global.unsafe.getPageSize = function () {};

/**
 * The currrent systems's pagesize
 * @type {Number}
 */
global.unsafe.pageSize = 4096;
