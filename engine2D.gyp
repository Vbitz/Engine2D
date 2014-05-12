{
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
				"src/vendor/GLFT_Font.cpp",
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
				"src/TextureLoader.cpp",
				"src/Timer.cpp",
				"src/ScriptingManager.cpp",
				"src/Window_glfw.cpp",
				"src/WorkerThreadPool.cpp"
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
							"-lfreetype "
							"-lglfw3",
							"-lphysfs",
							"-lv8",
							"-lfreeimage"
						]
					},
					"include_dirs": [
						"/usr/local/include",
						"/usr/local/include/freetype2"
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
				}]
			]
		}
	]
}
