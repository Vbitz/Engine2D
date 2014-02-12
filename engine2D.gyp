{
	"targets": [
		{
			"target_name": "engine2D",
			"type": "executable",
			"dependencies": [],
			"sources": [
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
				"src/GL3Buffer.cpp",
				"src/Shader.cpp",
				"src/EngineUI.cpp",
				"src/Draw2D.cpp",
				"src/Logger.cpp",
				"src/Profiler.cpp",
				"src/FramePerfMonitor.cpp",
				"src/ResourceManager.cpp",
				"src/Config.cpp",
				"src/LogGraphEvents.cpp",
				"src/Util.cpp",
				"src/PlatformOSX.cpp",
				"src/Events.cpp",
				"src/TestSuite.cpp",
				"src/Application.cpp",
				"src/PlatformTests.cpp",
				"src/EffectParameters.cpp",
				"src/SpriteSheet.cpp",
				"src/TextureLoader.cpp",
				"src/Timer.cpp",
				"src/ScriptingManager.cpp",
				"src/Window_glfw.cpp"
			],
			"conditions": [
				["OS == \"mac\"", {
					"xcode_settings": {
						"OTHER_CPLUSPLUSFLAGS": [
							"-std=gnu++11",
							"-stdlib=libc++",
							"-pthread"
						],
						"OTHER_LDFLAGS": [
							"-lfreetype "
							"-lglew",
							"-lglfw3",
							"-lphysfs",
							"-lv8",
							"-lfreeimage",
							"-pthread"
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
