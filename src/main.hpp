#pragma once

#include "common.hpp"

#include "Filesystem.hpp"

#include "JSSys.hpp"
#include "JSDraw.hpp"
#include "JSInput.hpp"
#include "JSFS.hpp"
#include "JSDatabase.hpp"

#include "EngineUI.hpp"

#include "extern/GLFT_Font.hpp"

namespace Engine {

	GLFT_Font* 					getFont(std::string fontName, int fontSize);
    void                        loadFont(std::string prettyName, std::string filename);
    bool                        isFontLoaded(std::string fontName);
    
    v8::Persistent<v8::Context> getGlobalContext();
    
    int                         getCommandLineArgCount();
    
    const char**                getCommandLineArgs();

	int 						getScreenWidth();
	int 						getScreenHeight();

	void 						setDrawFunction(v8::Persistent<v8::Function> func);
    void                        setKeyFunction(v8::Persistent<v8::Function> func);
    void                        setPostLoadFunction(v8::Persistent<v8::Function> func);
    
	void						runFile(std::string path, bool persist);
    
    void                        exit();
    void                        toggleFullscreen();
    void                        saveScreenshot(std::string filename);
    void                        dumpProfile();
    void                        upgradeGL3();
    
    bool                        usingGL3();
    
    void                        runCommand(std::string str);
    
    GLFWwindow*                 getGLFWWindow();
}