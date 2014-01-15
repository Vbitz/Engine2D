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

global.fs.readFile = function () {};
global.fs.writeFile = function () {};
global.fs.fileExists = function () {};
global.fs.fileSize = function () {};
global.fs.mountFile = function () {};
global.fs.configDir = function () {};
global.fs.mkdir = function () {};

/** @namespace */
global.draw = {};

global.draw.rect = function () {};
global.draw.grid = function () {};
global.draw.grad = function () {};
global.draw.circle = function () {};
global.draw.curve = function () {};

global.draw.colorPalette = function () {};
global.draw.setColorF = function () {};
global.draw.setColor = function () {};
global.draw.setColorI = function () {};
global.draw.clearColor = function () {};
global.draw.getRGBFromHSV = function () {};

global.draw.print = function () {};
global.draw.getStringWidth = function () {};

global.draw.draw = function () {};
global.draw.drawSub = function () {};
global.draw.openImage = function () {};
global.draw.getImageArray = function () {};
global.draw.createImageArray = function () {};
global.draw.createImage = function () {};
global.draw.saveImage = function () {};
global.draw.freeImage = function () {};
global.draw.isTexture = function () {};

global.draw.cameraReset = function () {};
global.draw.cameraPan = function () {};
global.draw.cameraZoom = function () {};
global.draw.cameraRotate = function () {};

global.draw.setFont = function () {};
global.draw.loadFont = function () {};
global.draw.isFontLoaded = function () {};

global.draw.getVerts = function () {};
global.draw.setDrawOffscreen = function () {};
global.draw.setCenter = function () {};

/** @namespace */
global.input = {};

global.input.keyDown = function () {};

global.input.mouseX = 0;
global.input.mouseY = 0;
global.input.leftMouseButton = false;
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