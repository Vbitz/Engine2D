#include "RenderGL3.hpp"

#include "GL3Buffer.hpp"
#include "TextureLoader.hpp"

#include "Application.hpp"

namespace Engine {
    std::string GLErrorString(GLenum error) {
        switch (error) {
            case GL_INVALID_ENUM:       return "Invalid Enum";
            case GL_INVALID_VALUE:      return "Invalid Value";
            case GL_INVALID_OPERATION:  return "Invalid Operation";
            case GL_OUT_OF_MEMORY:      return "Out of Memory";
            case GL_STACK_OVERFLOW:     return "Stack Overflow";
            case GL_STACK_UNDERFLOW:    return "Stack Underflow";
            default:                    return "Unknown Error: " + std::to_string(error);
        }
    }
    
    std::string GLModeToString(GLenum mode) {
        switch (mode) {
            case GL_POINTS:         return "GL_POINTS";
            case GL_LINE_STRIP:     return "GL_LINE_STRIP";
            case GL_LINE_LOOP:      return "GL_LINE_LOOP";
            case GL_LINES:          return "GL_LINES";
            case GL_TRIANGLE_STRIP: return "GL_TRIANGLE_STRIP";
            case GL_TRIANGLE_FAN:   return "GL_TRIANGLE_FAN";
            case GL_TRIANGLES:      return "GL_TRIANGLES";
            case GL_QUAD_STRIP:     return "GL_QUAD_STRIP";
            case GL_QUADS:          return "GL_QUADS";
            case GL_POLYGON:        return "GL_POLYGON";
            default:                return "Unknown";
        }
    }
    
    bool RenderGL3::CheckGLError(const char* source) {
        GLenum err;
        bool oneError = false;
        while ((err = glGetError()) != GL_NO_ERROR) {
            Logger::begin("OpenGL", Logger::LogLevel_Error) << "GLError in " << source << " : " << GLErrorString(err) << Logger::end();
            throw new GLError(source, err, GLErrorString(err));
        }
        return oneError;
    }
    
    void RenderGL3::ResetMatrix() {
        if (GetAppSingilton()->UsingGL3()) {
            this->_currentModelMatrix = glm::mat4();
        } else {
            //glPushMatrix();
            glDisable(GL_DEPTH_TEST);
            glLoadIdentity();
            Application* app = GetAppSingilton();
            glOrtho(0, app->GetScreenWidth(), app->GetScreenHeight(), 0, 1, -1);
        }
    }
    
    void RenderGL3::SetDefinedColor(std::string name, int col) {
        Color4f::SetDefinedColor(name, col);
    }
    
    void RenderGL3::BeginRendering(GLenum mode) {
        if (GetAppSingilton()->UsingGL3()) {
            if (this->_currentMode != mode ||
                    this->_currentMode == GL_LINE_STRIP) {
                    // it's a hack, I really need to fix this
                FlushAll();
                this->_currentMode = mode;
            }
        } else {
            glBegin(mode);
            GLSetColor();
        }
    }
    
    void RenderGL3::EndRendering() {
        if (GetAppSingilton()->UsingGL3()) {
            if (this->_currentVerts > BUFFER_SIZE - 256) {
                FlushAll();
                BeginRendering(_currentMode);
            }
        } else {
            glEnd();
            _drawCalls++;
        }
    }
    
    void RenderGL3::AddVert(float x, float y, float z) {
        if (GetAppSingilton()->UsingGL3()) {
            AddVert(x, y, z, _currentColor, 0.0, 0.0);
        } else {
            glVertex3f(x - _centerX, y - _centerY, z);
            _currentVerts++;
        }
    }
    
    void RenderGL3::AddVert(float x, float y, float z, Color4f col) {
        if (GetAppSingilton()->UsingGL3()) {
            AddVert(x, y, z, col, 0.0, 0.0);
        } else {
            glColor4f(col.r, col.g, col.b, col.a);
            AddVert(x, y, z);
        }
    }
    
    void RenderGL3::AddVert(float x, float y, float z, float s, float t) {
        if (GetAppSingilton()->UsingGL3()) {
            AddVert(x, y, z, _currentColor, s, t);
        } else {
            glTexCoord2f(s, t);
            AddVert(x, y, z);
        }
    }
    
    void RenderGL3::AddVert(float x, float y, float z, Color4f col, float s, float t) {
        if (GetAppSingilton()->UsingGL3()) {
            _buffer[_currentVerts].pos = glm::vec3(x - _centerX, y - _centerY, z);
            _buffer[_currentVerts].col = col;
            _buffer[_currentVerts].uv = glm::vec2(s, t);
            _currentVerts++;
        } else {
            glTexCoord2f(s, t);
            AddVert(x, y, z, col);
        }
    }
    
    void RenderGL3::EnableTexture(Texture* tex) {
        if (!GetAppSingilton()->UsingGL3()) {
            glEnable(GL_TEXTURE_2D);
            
            tex->Begin();
        } else {
            FlushAll();
            this->_currentTexture = tex;
        }
    }
    
    void RenderGL3::DisableTexture() {
        if (!GetAppSingilton()->UsingGL3()) {
            glBindTexture(GL_TEXTURE_2D, 0);
            
            glDisable(GL_TEXTURE_2D);
        } else {
            FlushAll();
            _currentTexture = _defaultTexture;
        }
    }
    
    void RenderGL3::EnableSmooth() {
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_POLYGON_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    }
    
    void RenderGL3::DisableSmooth() {
        glDisable(GL_LINE_SMOOTH);
        glDisable(GL_POLYGON_SMOOTH);
    }
    
    void RenderGL3::Print(float x, float y, const char* string) {
        if (GetAppSingilton()->UsingGL3()) {
            FlushAll();
            
            GLFT_Font* drawingFont = GetAppSingilton()->GetFont(
                this->_currentFontName, this->_currentFontSize);
            
            drawingFont->drawTextGL3(x - this->_centerX, y - this->_centerY,
                &this->_currentEffect,
                this->_currentColor.r, this->_currentColor.g, this->_currentColor.b, string);
            
            CheckGLError("Post GL3 Print");
            
        } else {
            glEnable(GL_TEXTURE_2D);
            
            GLSetColor();
            
            GLFT_Font* drawingFont = GetAppSingilton()->GetFont(_currentFontName, _currentFontSize);
            
            drawingFont->drawText(x - _centerX, y - _centerY, string);
            
            glDisable(GL_TEXTURE_2D);
            
            CheckGLError("postPrint");
            
            this->_polygons += strlen(string) * 4;
        }
    }
    
    float RenderGL3::CalcStringWidth(std::string str) {
        return GetAppSingilton()->GetFont(
            this->_currentFontName, this->_currentFontSize)->calcStringWidth(str);
    }
    
    void RenderGL3::SetFont(std::string name, int size) {
        this->_currentFontName = name;
        this->_currentFontSize = size;
    }
    
    void RenderGL3::LoadFont(std::string prettyName, std::string filename) {
        GetAppSingilton()->LoadFont(prettyName, filename);
        this->_currentFontName = prettyName;
    }
    
    bool RenderGL3::IsFontLoaded(std::string name) {
        return GetAppSingilton()->IsFontLoaded(name);
    }
    
    Texture* GenerateDefaultTexture() {
        return ImageReader::TextureFromBuffer(new float[4] {1.0f, 1.0f, 1.0f, 1.0f}, 1, 1);
    }
    
    void RenderGL3::FlushAll() {
        static GL3Buffer buf(this->_currentEffect); // temporory
        
        if (!GetAppSingilton()->UsingGL3()) {
            throw "That's a bug";
        }
        
        if (_currentVerts == 0) {
            return; // nothing to draw
        }
        
        if (buf.Update()) {
            Logger::begin("RenderGL3", Logger::LogLevel_Log) << "Render Buffer Reloaded" << Logger::end();
        }
        
        try {
            _currentTexture->Begin();
        } catch (...) {
            _currentTexture->End();
            _currentTexture = _defaultTexture;
            _currentTexture->Begin();
        }
        buf.Upload((float*) _buffer, _currentVerts * sizeof(BufferFormat));
        //std::cout << "Drawing Using: " << GLModeToString(_currentMode) << std::endl;
        buf.Draw(_currentMode, _currentModelMatrix, glm::mat4(), _currentVerts);
        
        _currentTexture->End();
        
        /* I think this line here needs a story. What happens when you forget it?
         Well as it turns out it will eat though the buffer growing ever bigger until
         it starts ruining C++ like a fucking pacman. I got 2 lovely crashes because I
         forgot this line. The first was caused when any std::unordered_map tries to
         rehash somehing, the second was the slow and painful death as a std::vector
         exceptions it's self to death over the span of 3 frames. This was my best line
         that day.
         */
        _currentVerts = 0;
        
        // maybe zero the buffer
    }
    
    void RenderGL3::Init2d() {
        if (GetAppSingilton()->UsingGL3()) {
            std::string gl3Effect = Config::GetString("core.render.basicEffect");
            this->_currentEffect = EffectReader::GetEffectFromFile(gl3Effect);
            this->_currentEffect.CreateShader();
            if (this->_defaultTexture == NULL || !this->_defaultTexture->IsValid()) {
                Logger::begin("RenderGL3", Logger::LogLevel_Verbose) << "Creating Default Texture" << Logger::end();
                this->_defaultTexture = GenerateDefaultTexture();
            }
            this->_currentTexture = this->_defaultTexture;
        }
    }
    
    void RenderGL3::Begin2d() {
        EnableSmooth();
        
        if (GetAppSingilton()->UsingGL3()) {
            this->_currentTexture = this->_defaultTexture;
            glDisable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_TEXTURE_2D);
        }
        
        ResetMatrix();
        
        CheckGLError("Post Begin2D");
    }
    
    void RenderGL3::End2d() {
        if (GetAppSingilton()->UsingGL3()) {
            FlushAll();
        } else {
            //glPopMatrix();
        }
        
        this->_polygons = 0;
        this->_drawCalls = 0;
        
        CheckGLError("Post End2D");
    }
    
    void RenderGL3::Reset() {
        int oldVertCount = this->_polygons;
        
        End2d();
        Begin2d();
        
        this->_polygons = oldVertCount;
    }
    
    void RenderGL3::ClearColor(Color4f col) {
        glClearColor(col.r, col.g, col.b, col.a);
    }
    
    void RenderGL3::ClearColor(int col) {
        ClearColor(Color4f(col));
    }
    
    void RenderGL3::ClearColor(std::string colorName) {
        ClearColor(Color4f(colorName));
    }
    
    void RenderGL3::ClearColor(float r, float g, float b) {
        ClearColor(Color4f(r, g, b, 1.0f));
    }
    
    Color4f RenderGL3::GetColor() {
        return this->_currentColor;
    }
    
    void RenderGL3::SetColor(Color4f col) {
        this->_currentColor = col;
    }
    
    void RenderGL3::SetColor(int col) {
        SetColor(Color4f(col));
    }
    
    void RenderGL3::SetColor(std::string colorName) {
        if (colorName[0] == '#') {
            unsigned int tempCol;
            std::stringstream ss;
            ss << std::hex << colorName.substr(1);
            ss >> tempCol;
            SetColor(tempCol);
        } else {
            SetColor(Color4f(colorName));
        }
    }
    
    void RenderGL3::SetColor(float r, float g, float b) {
        SetColor(r, g, b, 1.0f);
    }
    
    void RenderGL3::SetColor(float r, float g, float b, float a) {
        this->_currentColor = Color4f(r, g, b, a);
    }
    
    void RenderGL3::GLSetColor() {
        glColor4f(this->_currentColor.r,
                  this->_currentColor.g,
                  this->_currentColor.b,
                  this->_currentColor.a);
    }
    
    int RenderGL3::GetVerts() {
        return this->_polygons;
    }
    
    void RenderGL3::SetCenter(float x, float y) {
        this->_centerX = x;
        this->_centerY = y;
    }
    
    void RenderGL3::CameraPan(float x, float y) {
        if (GetAppSingilton()->UsingGL3()) {
            FlushAll();
            this->_currentModelMatrix =
                glm::translate(this->_currentModelMatrix, glm::vec3(x, y, 0.0f));
        } else {
            glTranslatef(x, y, 0.0f);
        }
    }
    
    void RenderGL3::CameraZoom(float f) {
        if (GetAppSingilton()->UsingGL3()) {
            FlushAll();
            this->_currentModelMatrix =
                glm::scale(_currentModelMatrix, glm::vec3(f, f, 0.0f));
        } else {
            glScalef(f, f, 0.0f);
        }
    }
    
    void RenderGL3::CameraRotate(float r) {
        if (GetAppSingilton()->UsingGL3()) {
            FlushAll();
            this->_currentModelMatrix =
                glm::rotate(this->_currentModelMatrix, r, glm::vec3(0, 0, 1));
        } else {
            glRotatef(r, 0.0f, 0.0f, 1.0f);
        }
    }
    
    RenderGL3* _renderGl3Singilton = NULL;
    
    RenderGL3* GetRenderGL() {
        if (_renderGl3Singilton == NULL) {
            _renderGl3Singilton = new RenderGL3();
        }
        return _renderGl3Singilton;
    }
}