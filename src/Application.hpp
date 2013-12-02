#pragma once

#include "common.hpp"

#include "extern/GLFT_Font.hpp"

namespace Engine {
    
    typedef struct _openGLVersion {
        int major, minor, revision;
    } OpenGLVersion;
    
	GLFT_Font* 					getFont(std::string fontName, int fontSize);
    bool                        loadFont(std::string prettyName, std::string filename);
    bool                        isFontLoaded(std::string fontName);
    
    std::vector<std::string>    getCommandLineArgs();
    
	int 						getScreenWidth();
	int 						getScreenHeight();
    void                        setScreenSize(int width, int height);
    
	bool						runFile(std::string path, bool persist);
    
    void                        exit();
    void                        toggleFullscreen();
    void                        saveScreenshot(std::string filename);
    void                        dumpProfile();
    void                        restartRenderer();
    
    bool                        usingGL3();
    
    void                        runCommand(std::string str);
    
    void                        detailProfile(int frames, std::string filename);
    
    void                        invalidateScript(std::string filename);
    
    bool                        getKeyPressed(int key);

    OpenGLVersion               getOpenGLVersion();

    int                         main(int argc, char const *argv[]);
}