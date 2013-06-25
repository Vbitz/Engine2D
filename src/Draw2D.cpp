#include "Draw2D.hpp"

namespace Engine {
    namespace Draw2D {
        bool _drawOffScreen = true;
		
		double _currentColorR = 1.0f;
		double _currentColorG = 1.0f;
		double _currentColorB = 1.0f;
        double _currentColorA = 1.0f;
        
        int _polygons = 0;
        int _drawCalls = 0;
        
        int _centerX = 0;
        int _centerY = 0;
        
        int _currentVerts = 0;
        
        float _vertexBuffer[BUFFER_SIZE * 3];
        float _colorBuffer[BUFFER_SIZE * 4];
        float _texCoardBuffer[BUFFER_SIZE * 2];
        
        std::string _currentFontName = "basic16px";
        int _currentFontSize = 16;
        
        std::string GLErrorString(GLenum error) {
            switch (error) {
                case GL_INVALID_ENUM:       return "Invalid Enum";
                case GL_INVALID_VALUE:      return "Invalid Value";
                case GL_INVALID_OPERATION:  return "Invalid Operation";
                case GL_OUT_OF_MEMORY:      return "Out of Memory";
                default:                    return "Unknown Error: " + std::to_string(error);
            }
        };
        
        bool CheckGLError(std::string source) {
            GLenum err;
            bool oneError = false;
            while ((err = glGetError()) != GL_NO_ERROR) {
                oneError = true;
                Logger::begin("OpenGL", Logger::LogLevel_Error) << "GLError in " << source << " : " << GLErrorString(err) << Logger::end();
            }
            return oneError;
        }
        
        void ResetMatrix() {
            glPushMatrix();
			glDisable(GL_DEPTH_TEST);
			glLoadIdentity();
			glOrtho(0, getScreenWidth(), getScreenHeight(), 0, 1, -1);
        }
        
        bool IsOffscreen(int x, int y, int w, int h) {
            return false;
        }
        
        void BeginRendering(GLenum mode) {
            if (usingGL3()) {
                _currentVerts = 0;
            } else {
                glBegin(mode);
                glColor4f(_currentColorR, _currentColorG, _currentColorB, _currentColorA);
            }
        }
        
        void EndRendering() {
            if (usingGL3()) {
                
            } else {
                glEnd();
                _drawCalls++;
            }
        }
        
        void AddVert(float x, float y, float z) {
            if (usingGL3()) {
                AddVert(x, y, z, _currentColorR, _currentColorG, _currentColorB, _currentColorA, 0.0, 0.0);
            } else {
                glVertex3f(x - _centerX, y - _centerY, z);
                _polygons++;
            }
        }
        
        void AddVert(float x, float y, float z, double r, double g, double b, double a) {
            if (usingGL3()) {
                AddVert(x, y, z, r, g, b, a, 0.0, 0.0);
            } else {
                glColor3f(r, g, b);
                AddVert(x, y, z);
            }
        }
        
        void AddVert(float x, float y, float z, float s, float t) {
            if (usingGL3()) {
                AddVert(x, y, z, _currentColorR, _currentColorG, _currentColorB, _currentColorA, s, t);
            } else {
                glTexCoord2f(s, t);
                AddVert(x, y, z);
            }
        }
        
        void AddVert(float x, float y, float z, double r, double g, double b, double a, float s, float t) {
            if (usingGL3()) {
                _vertexBuffer[_currentVerts * 3 + 0] = x;
                _vertexBuffer[_currentVerts * 3 + 1] = y;
                _vertexBuffer[_currentVerts * 3 + 2] = z;
                _colorBuffer[_currentVerts * 4 + 0] = r;
                _colorBuffer[_currentVerts * 4 + 1] = g;
                _colorBuffer[_currentVerts * 4 + 2] = b;
                _colorBuffer[_currentVerts * 4 + 3] = 1.0f;
                _texCoardBuffer[_currentVerts * 2 + 0] = s;
                _texCoardBuffer[_currentVerts * 2 + 0] = t;
                _currentVerts++;
            } else {
                glTexCoord2f(s, t);
                AddVert(x, y, z, r, g, b, a);
            }
        }
        
        void EnableTexture(int texId) {
            glEnable(GL_TEXTURE_2D);
            
            glBindTexture(GL_TEXTURE_2D, texId);
        }
        
        void DisableTexture() {
            glBindTexture(GL_TEXTURE_2D, 0);
            
            glDisable(GL_TEXTURE_2D);
        }
        
        void Print(float x, float y, const char* string) {
            glEnable(GL_TEXTURE_2D);
            
            GLSetColor();
            
            GLFT_Font* drawingFont = getFont(_currentFontName, _currentFontSize);
            
			drawingFont->drawText(x - _centerX, y - _centerY, string);
            
			glDisable(GL_TEXTURE_2D);
            
            CheckGLError("postPrint");
            
            _polygons += strlen(string) * 4;
        }
        
        float CalcStringWidth(std::string str) {
            return getFont(_currentFontName, _currentFontSize)->calcStringWidth(str);
        }
        
        void SetFont(std::string name, int size) {
            _currentFontName = name;
            _currentFontSize = size;
        }
        
        void LoadFont(std::string prettyName, std::string filename) {
            loadFont(prettyName, filename);
            _currentFontName = prettyName;
        }
        
        bool IsFontLoaded(std::string name) {
            return isFontLoaded(name);
        }
        
        bool IsValidTextureID(int texID) {
            return glIsTexture(texID);
        }
        
        void FlushAll() {
            
        }
		
		void Begin2d() {
            if (!usingGL3()) {
                ResetMatrix();
            }
		}
		
		void End2d() {
			glEnable(GL_DEPTH_TEST);
            
            if (usingGL3()) {
                FlushAll();
            } else {
                glPopMatrix();
            }
            
            _polygons = 0;
            _drawCalls = 0;
		}
        
        void Reset() {
            int oldVertCount = _polygons;
            
            End2d();
            Begin2d();
            
            _polygons = oldVertCount;
        }
        
        void SetColor(float r, float g, float b) {
            SetColor(r, g, b, 1.0f);
        }
        
        void SetColor(float r, float g, float b, float a) {
            _currentColorR = r;
            _currentColorG = g;
            _currentColorB = b;
            _currentColorA = a;
        }
        
        void GLSetColor() {
            glColor4f(_currentColorR, _currentColorG, _currentColorB, _currentColorA);
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
        
        void Rect(float x, float y, float w, float h) {
            BeginRendering(GL_QUADS);
                AddVert(x, y, 0);
                AddVert(x + w, y, 0);
                AddVert(x + w, y + h, 0);
                AddVert(x, y + h, 0);
            EndRendering();
        }
        
        void Grid(float x, float y, float w, float h) {
            Draw2D::BeginRendering(GL_LINE_LOOP);
                AddVert(x, y, 0);
                AddVert(x + w, y, 0);
                AddVert(x + w, y + h, 0);
                AddVert(x, y + h, 0);
                AddVert(x, y, 0);
            EndRendering();
        }
    }
}