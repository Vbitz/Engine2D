JavaScript | C++
--- | ---
`console._log(level, obj)` | `Logger::begin("Script", level) << obj << Logger::end();`
`console.clear()` | `EngineUI::ClearConsole();`
`console.toggle()` | `EngineUI::ToggleConsole();`
`console.log(obj)` | `Logger::begin("Script", Logger::LogLevel_Script) << obj << Logger::end();`
`console.warn(obj)` | `Logger::begin("Script", Logger::LogLevel_Warning) << obj << Logger::end();`
`console.error(obj)` | `Logger::begin("Script", Logger::LogLevel_Error) << obj << Logger::end();`
`console.verbose(obj)` | `Logger::begin("Script", Logger::LogLevel_Verbose) << obj << Logger::end();`
`console.writeRaw(obj)` | `std::cout << obj << std::endl;`
`sys.argv()` | `GetAppSingilton()->GetCommandLineArgs()`
`sys.runFile(filename, persists)` | **JavaScript Specfic**
`sys.drawFunc(func)` | `Events::Clear("sys.drawFunc"); Events::On("draw", "sys.drawFunc", func);`
`sys.keyboardFunc(func)` | **Bascily the same as above**
`sys.on(event, id, filter, func)` | `Events::On(event, id, filter, func);`
`sys.emit(event, args)` | `Events::Emit(event, args)`
`sys.clearEvent(id)` | `Events::Clear(id)`
`sys.microtime()` | `Platform::GetTime()`
`sys.heapStats()` | **JavaScript Specfic**, if you really want it this calls `v8::Isolate::GetCurrent()->GetHeapStatistics(&v8::HeapStatistics)`
`sys.memoryStats()` | `Platfrom::GetMemoryInfo()`
`sys.trace()` | `Platform::DumpStackTrace()`
`sys.exit()` | `GetAppSingilton()->Exit()`
`sys.getGLVersion()` | `GetAppSingilton()->GetOpenGLVersion()`
`sys.hasExtention(extention)` | `glewGetExtension(extention)`
`sys.getExtentions()` | Quite compilcated but it boils down to `glGetString(GL_EXTENSIONS)` and the GL Legasy version
`sys.getMaxTextureSize()` | `GLint result = 0; glGetIntegerv(GL_MAX_TEXTURE_SIZE, &result);`
`sys.saveScreenshot(path)` | `GetAppSingilton()->SaveScreenshot(path)`
`sys.resizeWindow(width, height)` | `GetAppSingilton()->SetScreenSize(width, height)`
`sys.toggleFullscreen()` | `GetAppSingilton()->ToggleFullscreen()`
`sys.restartRenderer()` | `GetAppSingilton()->RestartRenderer()`
`sys.getProfilerTime(zone)` | `Profiler::GetTime(zone)`
`sys.getProfilerZones()` | `Profiler::GetZones()`
`sys.perf(zone, func)` | `Profiler::Begin(zone); func(); Profiler::End(zone);`
`sys.time(name, func)` | Basicly just `Platform::GetTime()`
`sys.config(key, value)` | `Config::GetAll()`, `Config::Get*()` and `Config::Set*()`
`sys.gc()` | **JavaScript Specfic**, `v8::V8::IdleNotification(200)`
`sys.profile(frames, filename)` | `GetAppSingilton()->DetailProfile(frames, filename`
`sys.profileSet(profileZone, maxTime)` | `Profiler::DisableMaxTime(profileZone)` and `Profiler::EnableMaxTime(profileZone, maxTime)`
`sys.reloadRootScript()` | `GetAppSingilton()->InvalidateScript(Config::GetString("core.script.entryPoint"))`
`sys.forceReload(script)` | `GetAppSingilton()->InvalidateScript(script)`
`sys.version()` | **JavaScript Specfic**
`sys.msgBox(title, msg, modal)` | `Platform::ShowMessageBox(title, msg, modal)`
`sys.shell(filename)` | `Platform::ShellExecute(filename)`
`sys.platform` | `_PLATFORM`
`sys.devMode` | Set to a private property inside of Application, `_developerMode`
`sys.preload` | **JavaScript Specfic**
`sys.numProcessers` | `Platform::GetProcesserCount()`
`sys.username` | `Platform::GetUsername()`
`sys.screenWidth` | `GetAppSingilton()->GetScreenWidth()`
`sys.screenHeight` | `GetAppSingilton()->GetScreenHeight()`
`sys.deltaTime` | `FramePerfMonitor::GetFrameTime()`
`fs.readFile(filename, raw)` | `Filesystem::GetFileContent`,
`fs.writeFile(filename, content)` | `Filesystem::WriteFile(filename, content, contentLength)`
`fs.fileExists(filename)` | `Filesystem::FileExists(filename)`
`fs.fileSize(filename)` | `Filesystem::FileSize(filename)`
`fs.mountFile(archivePath, mountPath)` | `Filesystem::Mount(archivePath, mountPath)`
`fs.configDir(appName)` | `Filesystem::SetupUserDir(appName)`
`fs.mkdir(path)` | `Filesystem::Mkdir(path)`
`draw.rect(x, y, w, h)` | The same method in Draw2D
`draw.grid(x, y, w, h)` | The same method in Draw2D
`draw.grad(x, y, w, h, col1, col2, orientation)` | The same method in Draw2D
`draw.circle(x, y, radius, innerRadius, numberOfSides, startPos, endPos, fillStyle)` | The same method in Draw2D
`draw.line(x0, y0, x1, y1)` | The same method in Draw2D
`draw.curve(x0, y0, x1, y1, x2, y2, x3, y3)` | The same method in Draw2D
`draw.colorPalette(name, color)` | ``
`draw.setColorF(r, g, b)` | ``
`draw.setColor(color)` | ``
`draw.setColorI(r, g, b)` | ``
`draw.clearColor(color)` | ``
`draw.getRGBFromHSV(h, s, v)` | ``
`draw.print(x, y, str)` | ``
`draw.getStringWidth(str)` | ``
`draw.draw(texId, x, y, w, h)` | ``
`draw.drawSub(texId, x, y, w, h, xSub, ySub, wSub, hSub)` | ``
`draw.drawSprite(spriteSheet, sprite, x, y, w, h)` | ``
`draw.openImage(filename)` | ``
`draw.openSpriteSheet(filename)` | ``
`draw.getImageArray(filename)` | ``
`draw.createImageArray(w, h)` | ``
`draw.createImage(arr, w, h)` | ``
`draw.saveImage(texId, filename)` | ``
`draw.freeImage(texId)` | ``
`draw.isTexture(texId)` | ``
`draw.isSpiteSheet(spriteSheet)` | ``
`draw.cameraReset()` | ``
`draw.cameraPan(x, y)` | ``
`draw.cameraZoom(f)` | ``
`draw.cameraRotate(r)` | ``
`draw.setFont(prettyName, size)` | ``
`draw.loadFont(prettyName, filename)` | ``
`draw.isFontLoaded(prettyName)` | ``
`draw.getVerts()` | ``
`draw.setDrawOffscreen(draw)` | ``
`draw.setCenter(x, y)` | ``
`input.keyDown(key)` | ``
`input.mouseX` | ``
`input.mouseY` | ``
`input.leftMouseButton` | ``
`input.middleMouseButton` | ``
`input.rightMouseButton` | ``
`mod.open(filename)` | ``
`mod.call(id, method)` | ``
`db.open(filename)` | ``
`db.exec(statement)` | ``
`db.execPrepare(statement)` | ``
`unsafe.getNumberAddress(number)` | ``
`unsafe.getNative(address, length)` | ``
`unsafe.call(address)` | ``
`unsafe.malloc(length)` | ``
`unsafe.free(array)` | ``
`unsafe.addressOf(array)` | ``
`unsafe.mprotect(address, length, enable)` | ``
`unsafe.getPageSize()` | ``