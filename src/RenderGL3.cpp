#include "RenderGL3.hpp"

#include "GL3Buffer.hpp"
#include "TextureLoader.hpp"

#include "Application.hpp"

namespace Engine {
    namespace RenderGL3 {
        
        bool _drawOffScreen = true;
		
		Color4f _currentColor(1.0f, 1.0f, 1.0f, 1.0f);
        
        int _polygons = 0;
        int _drawCalls = 0;
        
        int _centerX = 0;
        int _centerY = 0;
        
        int _currentVerts = 0;
        
        /*
         Buffer format
         (x, y, z)      Position
         (r, g, b, a)   Color
         (u, v)         TexCourd
         */
        float _buffer[BUFFER_SIZE * 9];
        
        GLenum _currentMode = 0;
        
        std::string _currentFontName = "basic"; // ah there we are fonts fixed
        int _currentFontSize = 16;
        
        EffectParameters _currentEffect;
        
        Texture* _defaultTexture;
        
        Texture* _currentTexture = NULL;
        
        glm::mat4 _currentModelMatrix;;
        
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
        
        bool CheckGLError(const char* source) {
            GLenum err;
            bool oneError = false;
            while ((err = glGetError()) != GL_NO_ERROR) {
                Logger::begin("OpenGL", Logger::LogLevel_Error) << "GLError in " << source << " : " << GLErrorString(err) << Logger::end();
                throw new GLError(source, err, GLErrorString(err));
            }
            return oneError;
        }
        
        void ResetMatrix() {
            if (GetAppSingilton()->UsingGL3()) {
                _currentModelMatrix = glm::mat4();
            } else {
                //glPushMatrix();
                glDisable(GL_DEPTH_TEST);
                glLoadIdentity();
                Application* app = GetAppSingilton();
                glOrtho(0, app->GetScreenWidth(), app->GetScreenHeight(), 0, 1, -1);
            }
        }
        
        void SetDefinedColor(std::string name, int col) {
            Color4f::SetDefinedColor(name, col);
        }
        
        void BeginRendering(GLenum mode) {
            if (GetAppSingilton()->UsingGL3()) {
                if (_currentMode != mode || _currentMode == GL_LINE_STRIP) { // it's a hack, I really need to fix this
                    FlushAll();
                    _currentMode = mode;
                }
            } else {
                glBegin(mode);
                GLSetColor();
            }
        }
        
        void EndRendering() {
            if (GetAppSingilton()->UsingGL3()) {
                if (_currentVerts > BUFFER_SIZE - 256) {
                    FlushAll();
                    BeginRendering(_currentMode);
                }
            } else {
                glEnd();
                _drawCalls++;
            }
        }
        
        void AddVert(float x, float y, float z) {
            if (GetAppSingilton()->UsingGL3()) {
                AddVert(x, y, z, _currentColor, 0.0, 0.0);
            } else {
                glVertex3f(x - _centerX, y - _centerY, z);
                _currentVerts++;
            }
        }
        
        void AddVert(float x, float y, float z, Color4f col) {
            if (GetAppSingilton()->UsingGL3()) {
                AddVert(x, y, z, col, 0.0, 0.0);
            } else {
                glColor4f(col.r, col.g, col.b, col.a);
                AddVert(x, y, z);
            }
        }
        
        void AddVert(float x, float y, float z, float s, float t) {
            if (GetAppSingilton()->UsingGL3()) {
                AddVert(x, y, z, _currentColor, s, t);
            } else {
                glTexCoord2f(s, t);
                AddVert(x, y, z);
            }
        }
        
        void AddVert(float x, float y, float z, Color4f col, float s, float t) {
            if (GetAppSingilton()->UsingGL3()) {
                _buffer[_currentVerts * 9 + 0] = x - _centerX;
                _buffer[_currentVerts * 9 + 1] = y - _centerY;
                _buffer[_currentVerts * 9 + 2] = z;
                
                _buffer[_currentVerts * 9 + 3] = col.r;
                _buffer[_currentVerts * 9 + 4] = col.g;
                _buffer[_currentVerts * 9 + 5] = col.b;
                _buffer[_currentVerts * 9 + 6] = col.a;
                
                _buffer[_currentVerts * 9 + 7] = s;
                _buffer[_currentVerts * 9 + 8] = t;
                _currentVerts++;
            } else {
                glTexCoord2f(s, t);
                AddVert(x, y, z, col);
            }
        }
        
        void EnableTexture(Texture* tex) {
            if (!GetAppSingilton()->UsingGL3()) {
                glEnable(GL_TEXTURE_2D);
                
                tex->Begin();
            } else {
                FlushAll();
                _currentTexture = tex;
            }
        }
        
        void DisableTexture() {
            if (!GetAppSingilton()->UsingGL3()) {
                glBindTexture(GL_TEXTURE_2D, 0);
                
                glDisable(GL_TEXTURE_2D);
            } else {
                FlushAll();
                _currentTexture = _defaultTexture;
            }
        }
        
        void EnableSmooth() {
            glEnable(GL_LINE_SMOOTH);
            glEnable(GL_POLYGON_SMOOTH);
            glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
            glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
        }
        
        void DisableSmooth() {
            glDisable(GL_LINE_SMOOTH);
            glDisable(GL_POLYGON_SMOOTH);
        }
        
        void Print(float x, float y, const char* string) {
            if (GetAppSingilton()->UsingGL3()) {
                FlushAll();
                
                GLFT_Font* drawingFont = GetAppSingilton()->GetFont(_currentFontName, _currentFontSize);
                
                drawingFont->drawTextGL3(x - _centerX, y - _centerY, &_currentEffect, _currentColor.r, _currentColor.g, _currentColor.b, string);
                
                CheckGLError("Post GL3 Print");
                
            } else {
                glEnable(GL_TEXTURE_2D);
                
                GLSetColor();
                
                GLFT_Font* drawingFont = GetAppSingilton()->GetFont(_currentFontName, _currentFontSize);
                
                drawingFont->drawText(x - _centerX, y - _centerY, string);
                
                glDisable(GL_TEXTURE_2D);
                
                CheckGLError("postPrint");
                
                _polygons += strlen(string) * 4;
            }
        }
        
        float CalcStringWidth(std::string str) {
            return GetAppSingilton()->GetFont(_currentFontName, _currentFontSize)->calcStringWidth(str);
        }
        
        void SetFont(std::string name, int size) {
            _currentFontName = name;
            _currentFontSize = size;
        }
        
        void LoadFont(std::string prettyName, std::string filename) {
            GetAppSingilton()->LoadFont(prettyName, filename);
            _currentFontName = prettyName;
        }
        
        bool IsFontLoaded(std::string name) {
            return GetAppSingilton()->IsFontLoaded(name);
        }
        
        bool IsValidTextureID(int texID) {
            return glIsTexture(texID);
        }
        
        Texture* GenerateDefaultTexture() {
            return ImageReader::TextureFromBuffer(new float[4] {1.0f, 1.0f, 1.0f, 1.0f}, 1, 1);
        }
        
        void FlushAll() {
            static GL3Buffer buf(_currentEffect); // temporory
            
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
            buf.Upload(_buffer, _currentVerts * 9 * sizeof(float));
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
        
        void Init2d() {
            if (GetAppSingilton()->UsingGL3()) {
                std::string gl3Effect = Config::GetString("core.render.basicEffect");
                _currentEffect = EffectReader::GetEffectFromFile(gl3Effect);
                _currentEffect.CreateShader();
                if (_defaultTexture == NULL || !_defaultTexture->IsValid()) {
                    Logger::begin("RenderGL3", Logger::LogLevel_Verbose) << "Creating Default Texture" << Logger::end();
                    _defaultTexture = GenerateDefaultTexture();
                }
                _currentTexture = _defaultTexture;
            }
        }
		
		void Begin2d() {
            EnableSmooth();
            
            if (GetAppSingilton()->UsingGL3()) {
                _currentTexture = _defaultTexture;
                glDisable(GL_DEPTH_TEST);
            } else {
                glDisable(GL_TEXTURE_2D);
            }
            
            ResetMatrix();
            
            CheckGLError("Post Begin2D");
		}
		
		void End2d() {
            if (GetAppSingilton()->UsingGL3()) {
                FlushAll();
            } else {
                //glPopMatrix();
            }
            
            _polygons = 0;
            _drawCalls = 0;
            
            CheckGLError("Post End2D");
		}
        
        void Reset() {
            int oldVertCount = _polygons;
            
            End2d();
            Begin2d();
            
            _polygons = oldVertCount;
        }
        
        void ClearColor(Color4f col) {
            glClearColor(col.r, col.g, col.b, col.a);
        }
        
        void ClearColor(int col) {
            ClearColor(Color4f(col));
        }
        
        void ClearColor(std::string colorName) {
            ClearColor(Color4f(colorName));
        }
        
        void ClearColor(float r, float g, float b) {
            ClearColor(Color4f(r, g, b, 1.0f));
        }
        
        Color4f GetColor() {
            return _currentColor;
        }
        
        void SetColor(Color4f col) {
            _currentColor = col;
        }
        
        void SetColor(int col) {
            SetColor(Color4f(col));
        }
        
        void SetColor(std::string colorName) {
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
        
        void SetColor(float r, float g, float b) {
            SetColor(r, g, b, 1.0f);
        }
        
        void SetColor(float r, float g, float b, float a) {
            _currentColor = Color4f(r, g, b, a);
        }
        
        void GLSetColor() {
            glColor4f(_currentColor.r, _currentColor.g, _currentColor.b, _currentColor.a);
        }
        
        int GetVerts() {
            return _polygons;
        }
        
        void SetDrawOffscreen(bool drawOffscreen) {
            _drawOffScreen = drawOffscreen;
        }
        
        void SetCenter(float x, float y) {
            _centerX = x;
            _centerY = y;
        }
        
        void CameraPan(float x, float y) {
            if (GetAppSingilton()->UsingGL3()) {
                FlushAll();
                _currentModelMatrix = glm::translate(_currentModelMatrix, glm::vec3(x, y, 0.0f));
            } else {
                glTranslatef(x, y, 0.0f);
            }
        }
        
        void CameraZoom(float f) {
            if (GetAppSingilton()->UsingGL3()) {
                FlushAll();
                _currentModelMatrix = glm::scale(_currentModelMatrix, glm::vec3(f, f, 0.0f));
            } else {
                glScalef(f, f, 0.0f);
            }
        }
        
        void CameraRotate(float r) {
            if (GetAppSingilton()->UsingGL3()) {
                FlushAll();
                _currentModelMatrix = glm::rotate(_currentModelMatrix, r, glm::vec3(0, 0, 1));
            } else {
                glRotatef(r, 0.0f, 0.0f, 1.0f);
            }
        }
    }
}