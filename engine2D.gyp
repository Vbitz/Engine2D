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

				"src/Drawables/CubeDrawableTest.cpp",

				"src/vendor/jsoncpp.cpp",
				"src/vendor/sqlite3.c",
				"src/vendor/glew.c",

				# SOIL
				"src/vendor/soil/image_DXT.c",
				"src/vendor/soil/image_helper.c",
				"src/vendor/soil/SOIL.c",
				"src/vendor/soil/stb_image_aug.c",

				# ZLIB
				"src/vendor/zlib123/adler32.c",
				"src/vendor/zlib123/compress.c",
				"src/vendor/zlib123/crc32.c",
				"src/vendor/zlib123/deflate.c",
				"src/vendor/zlib123/gzio.c",
				"src/vendor/zlib123/infback.c",
				"src/vendor/zlib123/inffast.c",
				"src/vendor/zlib123/inflate.c",
				"src/vendor/zlib123/inftrees.c",
				"src/vendor/zlib123/trees.c",
				"src/vendor/zlib123/uncompr.c",
				"src/vendor/zlib123/zutil.c",

				# PHYSFS
				"src/vendor/physfs/physfs.c",
    			"src/vendor/physfs/physfs_byteorder.c",
    			"src/vendor/physfs/physfs_unicode.c",
    			"src/vendor/physfs/platform/posix.c",
    			"src/vendor/physfs/platform/unix.c",
    			"src/vendor/physfs/platform/macosx.c",
    			"src/vendor/physfs/platform/windows.c",
    			"src/vendor/physfs/archivers/dir.c",
    			"src/vendor/physfs/archivers/zip.c"
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
							"-lv8"
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
