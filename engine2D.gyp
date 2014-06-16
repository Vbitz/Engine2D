{
	"variables": {
		"WINDOW%": "glfw"
	},
	"targets": [
		{
			"target_name": "engine2D",
			"type": "executable",
			"dependencies": [],
			"sources": [
				"src/stdlib.cpp",
				"src/main.cpp",
				"src/JSInput.cpp",
				"src/JSDraw.cpp",
				"src/JSSys.cpp",
				"src/JSFS.cpp",
				"src/JSDatabase.cpp",
				"src/JSMod.cpp",
				"src/JSUnsafe.cpp",
				"src/JSMathHelper.cpp",
				"src/Database.cpp",
				"src/Filesystem.cpp",
				"src/vendor/jsoncpp.cpp",
				"src/vendor/sqlite3.c",
				"src/vendor/glew.c",
				"src/GL3Buffer.cpp",
				"src/Shader.cpp",
				"src/EngineUI.cpp",
				"src/Draw2D.cpp",
				"src/RenderDriver.cpp",
				"src/RenderGL2.cpp",
				"src/RenderGL3.cpp",
				"src/Logger.cpp",
				"src/Profiler.cpp",
				"src/FramePerfMonitor.cpp",
				"src/ResourceManager.cpp",
				"src/Config.cpp",
				"src/Util.cpp",
				"src/Platform_mac.cpp",
				"src/Platform_win.cpp",
				"src/Events.cpp",
				"src/TestSuite.cpp",
				"src/Application.cpp",
				"src/PlatformTests.cpp",
				"src/CoreTests.cpp",
				"src/ScriptingTests.cpp",
				"src/StdLibTests.cpp",
				"src/EffectParameters.cpp",
				"src/SpriteSheet.cpp",
				"src/FontSheet.cpp",
				"src/TextureLoader.cpp",
				"src/Timer.cpp",
				"src/ScriptingManager.cpp",

				"src/WorkerThreadPool.cpp",

				"src/Drawables/CubeDrawableTest.cpp"
			],
			"conditions": [
				['OS != "win"', {
					'sources!': [
						# Windows-only; exclude on other platforms.
						"src/Platform_win.cpp",
					]
				}],
				['OS != "mac"', {
					'sources!': [
						# Mac-only; exclude on other platforms.
						"src/Platform_mac.cpp",
					]
				}],
				["OS == \"mac\"", {
					"xcode_settings": {
						"OTHER_CPLUSPLUSFLAGS": [
							"-D_FORTIFY_SOURCE=2",
							"-std=gnu++11",
							"-stdlib=libc++",
							"-pthread"
						],
						"OTHER_LDFLAGS": [
							"-lphysfs",
							"-lv8",
							"-lfreeimage"
						]
					},
					"include_dirs": [
						"/usr/local/include",
					],
					"library_dirs": [
						"/usr/local/lib/"
					],
					"link_settings": {
						"libraries": [
							"$(SDKROOT)/System/Library/Frameworks/Cocoa.framework",
							"$(SDKROOT)/System/Library/Frameworks/OpenGL.framework",
							"$(SDKROOT)/System/Library/Frameworks/IOKit.framework"
						]
					}
				}],
				['("<(WINDOW)" == "glfw") & (OS == "mac")', {
					'sources': [
						"src/Window_glfw.cpp"
					],
					"xcode_settings": {
						"OTHER_LDFLAGS": [
							"-lglfw3"
						]
					}
				}],
				['("<(WINDOW)" == "sdl") & (OS == "mac")', {
					'sources': [
						"src/Window_sdl.cpp"
					],
					"xcode_settings": {
						"OTHER_CPLUSPLUSFLAGS": [
							"-D_THREAD_SAFE"
						],
						"OTHER_LDFLAGS": [
							"-lSDL2"
						]
					},
					"include_dirs": [
						"/usr/local/include/SDL2"
					]
				}],
			]
		}
	]
}
