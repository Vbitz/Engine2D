#include "RenderGL3.hpp"

#include "GL3Buffer.hpp"
#include "TextureLoader.hpp"

#include "Application.hpp"

namespace Engine {
    namespace RenderGL3 {
        
        bool _drawOffScreen = true;
		
		Color4f _currentColor = {
            1.0f, 1.0f, 1.0f, 1.0f
        };
        
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
        
		// fuuuu visual studio
        
		std::unordered_map<std::string, int> _predefinedColors;
        
		bool _predefFilled = false;
        
		void _initPredefinedColors() {
			_predefinedColors["pink"] = 0xFFC0CB;
            _predefinedColors["lightPink"] = 0xFFB6C1;
            _predefinedColors["hotPink"] = 0xFF69B4;
            _predefinedColors["deepPink"] = 0xFF1493;
            _predefinedColors["paleVioletRed"] = 0xDB7093;
            _predefinedColors["mediumVioletRed"] = 0xC71585;
            _predefinedColors["lightSalmon"] = 0xFFA07A;
            _predefinedColors["salmon"] = 0xFA8072;
            _predefinedColors["darkSalmon"] = 0xE9967A;
            _predefinedColors["lightCoral"] = 0xF08080;
            _predefinedColors["indianRed"] = 0xCD5C5C;
            _predefinedColors["crimson"] = 0xDC143C;
            _predefinedColors["fireBrick"] = 0xB22222;
            _predefinedColors["darkRed"] = 0x8B0000;
            _predefinedColors["red"] = 0xFF0000;
            _predefinedColors["orangeRed"] = 0xFF4500;
            _predefinedColors["tomato"] = 0xFF6347;
            _predefinedColors["coral"] = 0xFF7F50;
            _predefinedColors["darkOrange"] = 0xFF8C00;
            _predefinedColors["orange"] = 0xFFA500;
            _predefinedColors["gold"] = 0xFFD700;
            _predefinedColors["yellow"] = 0xFFFF00;
            _predefinedColors["lightYellow"] = 0xFFFFE0;
            _predefinedColors["lemonChiffon"] = 0xFFFACD;
            _predefinedColors["lightGoldenrodYellow"] = 0xFAFAD2;
            _predefinedColors["papayaWhip"] = 0xFFEFD5;
            _predefinedColors["moccasin"] = 0xFFE4B5;
            _predefinedColors["peachPuff"] = 0xFFDAB9;
            _predefinedColors["paleGoldenrod"] = 0xEEE8AA;
            _predefinedColors["khaki"] = 0xF0E68C;
            _predefinedColors["darkKhaki"] = 0xBDB76B;
            _predefinedColors["cornsilk"] = 0xFFF8DC;
            _predefinedColors["blanchedAlmond"] = 0xFFEBCD;
            _predefinedColors["bisque"] = 0xFFE4C4;
            _predefinedColors["navajoWhite"] = 0xFFDEAD;
            _predefinedColors["wheat"] = 0xF5DEB3;
            _predefinedColors["burlyWood"] = 0xDEB887;
            _predefinedColors["tan"] = 0xD2B48C;
            _predefinedColors["rosyBrown"] = 0xBC8F8F;
            _predefinedColors["sandyBrown"] = 0xF4A460;
            _predefinedColors["goldenrod"] = 0xDAA520;
            _predefinedColors["darkGoldenrod"] = 0xB8860B;
            _predefinedColors["peru"] = 0xCD853F;
            _predefinedColors["chocolate"] = 0xD2691E;
            _predefinedColors["saddleBrown"] = 0x8B4513;
            _predefinedColors["sienna"] = 0xA0522D;
            _predefinedColors["brown"] = 0xA52A2A;
            _predefinedColors["maroon"] = 0x800000;
            _predefinedColors["darkOliveGreen"] = 0x556B2F;
            _predefinedColors["olive"] = 0x808000;
            _predefinedColors["oliveDrab"] = 0x6B8E23;
            _predefinedColors["yellowGreen"] = 0x9ACD32;
            _predefinedColors["limeGreen"] = 0x32CD32;
            _predefinedColors["lime"] = 0x00FF00;
            _predefinedColors["lawnGreen"] = 0x7CFC00;
            _predefinedColors["chartreuse"] = 0x7FFF00;
            _predefinedColors["greenYellow"] = 0xADFF2F;
            _predefinedColors["springGreen"] = 0x00FF7F;
            _predefinedColors["nediumSpringGreen"] = 0x00FA9A;
            _predefinedColors["lightGreen"] = 0x90EE90;
            _predefinedColors["paleGreen"] = 0x98FB98;
            _predefinedColors["darkSeaGreen"] = 0x8FBC8F;
            _predefinedColors["mediumSeaGreen"] = 0x3CB371;
            _predefinedColors["seaGreen"] = 0x2E8B57;
            _predefinedColors["forestGreen"] = 0x228B22;
            _predefinedColors["green"] = 0x008000;
            _predefinedColors["darkGreen"] = 0x006400;
            _predefinedColors["mediumAquamarine"] = 0x66CDAA;
            _predefinedColors["aqua"] = 0x00FFFF;
            _predefinedColors["cyan"] = 0x00FFFF;
            _predefinedColors["lightCyan"] = 0xE0FFFF;
            _predefinedColors["paleTurquoise"] = 0xAFEEEE;
            _predefinedColors["aquamarine"] = 0x7FFFD4;
            _predefinedColors["turquoise"] = 0x40E0D0;
            _predefinedColors["mediumTurquoise"] = 0x48D1CC;
            _predefinedColors["darkTurquoise"] = 0x00CED1;
            _predefinedColors["lightSeaGreen"] = 0x20B2AA;
            _predefinedColors["cadetBlue"] = 0x5F9EA0;
            _predefinedColors["darkCyan"] = 0x008B8B;
            _predefinedColors["teal"] = 0x008080;
            _predefinedColors["lightSteelBlue"] = 0xB0C4DE;
            _predefinedColors["powderBlue"] = 0xB0E0E6;
            _predefinedColors["lightBlue"] = 0xADD8E6;
            _predefinedColors["skyBlue"] = 0x87CEEB;
            _predefinedColors["lightSkyBlue"] = 0x87CEFA;
            _predefinedColors["deepSkyBlue"] = 0x00BFFF;
            _predefinedColors["dodgerBlue"] = 0x1E90FF;
            _predefinedColors["dornflowerBlue"] = 0x6495ED;
            _predefinedColors["steelBlue"] = 0x4682B4;
            _predefinedColors["royalBlue"] = 0x4169E1;
            _predefinedColors["blue"] = 0x0000FF;
            _predefinedColors["mediumBlue"] = 0x0000CD;
            _predefinedColors["darkBlue"] = 0x00008B;
            _predefinedColors["navy"] = 0x000080;
            _predefinedColors["midnightBlue"] = 0x191970;
            _predefinedColors["lavender"] = 0xE6E6FA;
            _predefinedColors["thistle"] = 0xD8BFD8;
            _predefinedColors["plum"] = 0xDDA0DD;
            _predefinedColors["violet"] = 0xEE82EE;
            _predefinedColors["orchid"] = 0xDA70D6;
            _predefinedColors["fuchsia"] = 0xFF00FF;
            _predefinedColors["magenta"] = 0xFF00FF;
            _predefinedColors["mediumOrchid"] = 0xBA55D3;
            _predefinedColors["mediumPurple"] = 0x9370DB;
            _predefinedColors["blueViolet"] = 0x8A2BE2;
            _predefinedColors["darkViolet"] = 0x9400D3;
            _predefinedColors["darkOrchid"] = 0x9932CC;
            _predefinedColors["darkMagenta"] = 0x8B008B;
            _predefinedColors["purple"] = 0x800080;
            _predefinedColors["indigo"] = 0x4B0082;
            _predefinedColors["darkSlateBlue"] = 0x483D8B;
            _predefinedColors["slateBlue"] = 0x6A5ACD;
            _predefinedColors["mediumSlateBlue"] = 0x7B68EE;
            _predefinedColors["white"] = 0xFFFFFF;
            _predefinedColors["snow"] = 0xFFFAFA;
            _predefinedColors["honeydew"] = 0xF0FFF0;
            _predefinedColors["mintCream"] = 0xF5FFFA;
            _predefinedColors["azure"] = 0xF0FFFF;
            _predefinedColors["aliceBlue"] = 0xF0F8FF;
            _predefinedColors["ghostWhite"] = 0xF8F8FF;
            _predefinedColors["whiteSmoke"] = 0xF5F5F5;
            _predefinedColors["seashell"] = 0xFFF5EE;
            _predefinedColors["beige"] = 0xF5F5DC;
            _predefinedColors["oldLace"] = 0xFDF5E6;
            _predefinedColors["floralWhite"] = 0xFFFAF0;
            _predefinedColors["ivory"] = 0xFFFFF0;
            _predefinedColors["antiqueWhite"] = 0xFAEBD7;
            _predefinedColors["linen"] = 0xFAF0E6;
            _predefinedColors["lavenderBlush"] = 0xFFF0F5;
            _predefinedColors["mistyRose"] = 0xFFE4E1;
            _predefinedColors["gainsboro"] = 0xDCDCDC;
            _predefinedColors["lightGrey"] = 0xD3D3D3;
            _predefinedColors["silver"] = 0xC0C0C0;
            _predefinedColors["darkGrey"] = 0xA9A9A9;
            _predefinedColors["grey"] = 0x808080;
            _predefinedColors["dimGrey"] = 0x696969;
            _predefinedColors["lightSlateGrey"] = 0x778899;
            _predefinedColors["slateGrey"] = 0x708090;
            _predefinedColors["darkSlateGrey"] = 0x2F4F4F;
            _predefinedColors["black"] = 0x000000;
		}
        
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
            _predefinedColors[name] = col;
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
        
        Color4f IntToColor(int col) {
            return {
                .r = (float)((col & 0xff0000) >> 16) / 255,
                .g = (float)((col & 0x00ff00) >> 8) / 255,
                .b = (float)( col & 0x0000ff) / 255,
                .a = 1.0f
            };
        }
        
        void ClearColor(Color4f col) {
            glClearColor(col.r, col.g, col.b, col.a);
        }
        
        void ClearColor(int col) {
            ClearColor(IntToColor(col));
        }
        
        void ClearColor(std::string colorName) {
            ClearColor(_predefinedColors[colorName]);
        }
        
        void ClearColor(float r, float g, float b) {
            ClearColor({
                .r = r,
                .g = g,
                .b = b,
                .a = 1.0f
            });
        }
        
        Color4f GetColor() {
            return _currentColor;
        }
        
        void SetColor(int col) {
            SetColor(IntToColor(col));
        }
        
        void SetColor(std::string colorName) {
			if (!_predefFilled) {
				_initPredefinedColors();
				_predefFilled = true;
			}
            
            if (colorName[0] == '#') {
                unsigned int tempCol;
                std::stringstream ss;
                ss << std::hex << colorName.substr(1);
                ss >> tempCol;
                SetColor(tempCol);
            } else {
                SetColor(_predefinedColors[colorName]);
            }
        }
        
        void SetColor(float r, float g, float b) {
            SetColor(r, g, b, 1.0f);
        }
        
        void SetColor(float r, float g, float b, float a) {
            _currentColor = {
                .r = r,
                .g = g,
                .b = b,
                .a = a
            };
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