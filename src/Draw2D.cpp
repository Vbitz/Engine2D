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
        
        std::map<std::string, int> _predefinedColors = {
            {"pink", 0xFFC0CB},
            {"lightPink", 0xFFB6C1},
            {"hotPink", 0xFF69B4},
            {"deepPink", 0xFF1493},
            {"paleVioletRed", 0xDB7093},
            {"mediumVioletRed", 0xC71585},
            {"lightSalmon", 0xFFA07A},
            {"salmon", 0xFA8072},
            {"darkSalmon", 0xE9967A},
            {"lightCoral", 0xF08080},
            {"indianRed", 0xCD5C5C},
            {"crimson", 0xDC143C},
            {"fireBrick", 0xB22222},
            {"darkRed", 0x8B0000},
            {"red", 0xFF0000},
            {"orangeRed", 0xFF4500},
            {"tomato", 0xFF6347},
            {"coral", 0xFF7F50},
            {"darkOrange", 0xFF8C00},
            {"orange", 0xFFA500},
            {"gold", 0xFFD700},
            {"yellow", 0xFFFF00},
            {"lightYellow", 0xFFFFE0},
            {"lemonChiffon", 0xFFFACD},
            {"lightGoldenrodYellow", 0xFAFAD2},
            {"papayaWhip", 0xFFEFD5},
            {"moccasin", 0xFFE4B5},
            {"peachPuff", 0xFFDAB9},
            {"paleGoldenrod", 0xEEE8AA},
            {"khaki", 0xF0E68C},
            {"darkKhaki", 0xBDB76B},
            {"cornsilk", 0xFFF8DC},
            {"blanchedAlmond", 0xFFEBCD},
            {"bisque", 0xFFE4C4},
            {"navajoWhite", 0xFFDEAD},
            {"wheat", 0xF5DEB3},
            {"burlyWood", 0xDEB887},
            {"tan", 0xD2B48C},
            {"rosyBrown", 0xBC8F8F},
            {"sandyBrown", 0xF4A460},
            {"goldenrod", 0xDAA520},
            {"darkGoldenrod", 0xB8860B},
            {"peru", 0xCD853F},
            {"chocolate", 0xD2691E},
            {"saddleBrown", 0x8B4513},
            {"sienna", 0xA0522D},
            {"brown", 0xA52A2A},
            {"maroon", 0x800000},
            {"darkOliveGreen", 0x556B2F},
            {"olive", 0x808000},
            {"oliveDrab", 0x6B8E23},
            {"yellowGreen", 0x9ACD32},
            {"limeGreen", 0x32CD32},
            {"lime", 0x00FF00},
            {"lawnGreen", 0x7CFC00},
            {"chartreuse", 0x7FFF00},
            {"greenYellow", 0xADFF2F},
            {"springGreen", 0x00FF7F},
            {"nediumSpringGreen", 0x00FA9A},
            {"lightGreen", 0x90EE90},
            {"paleGreen", 0x98FB98},
            {"darkSeaGreen", 0x8FBC8F},
            {"mediumSeaGreen", 0x3CB371},
            {"seaGreen", 0x2E8B57},
            {"forestGreen", 0x228B22},
            {"green", 0x008000},
            {"darkGreen", 0x006400},
            {"mediumAquamarine", 0x66CDAA},
            {"aqua", 0x00FFFF},
            {"cyan", 0x00FFFF},
            {"lightCyan", 0xE0FFFF},
            {"paleTurquoise", 0xAFEEEE},
            {"aquamarine", 0x7FFFD4},
            {"turquoise", 0x40E0D0},
            {"mediumTurquoise", 0x48D1CC},
            {"darkTurquoise", 0x00CED1},
            {"lightSeaGreen", 0x20B2AA},
            {"cadetBlue", 0x5F9EA0},
            {"darkCyan", 0x008B8B},
            {"teal", 0x008080},
            {"lightSteelBlue", 0xB0C4DE},
            {"powderBlue", 0xB0E0E6},
            {"lightBlue", 0xADD8E6},
            {"skyBlue", 0x87CEEB},
            {"lightSkyBlue", 0x87CEFA},
            {"deepSkyBlue", 0x00BFFF},
            {"dodgerBlue", 0x1E90FF},
            {"dornflowerBlue", 0x6495ED},
            {"steelBlue", 0x4682B4},
            {"royalBlue", 0x4169E1},
            {"blue", 0x0000FF},
            {"mediumBlue", 0x0000CD},
            {"darkBlue", 0x00008B},
            {"navy", 0x000080},
            {"midnightBlue", 0x191970},
            {"lavender", 0xE6E6FA},
            {"thistle", 0xD8BFD8},
            {"plum", 0xDDA0DD},
            {"violet", 0xEE82EE},
            {"orchid", 0xDA70D6},
            {"fuchsia", 0xFF00FF},
            {"magenta", 0xFF00FF},
            {"mediumOrchid", 0xBA55D3},
            {"mediumPurple", 0x9370DB},
            {"blueViolet", 0x8A2BE2},
            {"darkViolet", 0x9400D3},
            {"darkOrchid", 0x9932CC},
            {"darkMagenta", 0x8B008B},
            {"purple", 0x800080},
            {"indigo", 0x4B0082},
            {"darkSlateBlue", 0x483D8B},
            {"slateBlue", 0x6A5ACD},
            {"mediumSlateBlue", 0x7B68EE},
            {"white", 0xFFFFFF},
            {"snow", 0xFFFAFA},
            {"honeydew", 0xF0FFF0},
            {"mintCream", 0xF5FFFA},
            {"azure", 0xF0FFFF},
            {"aliceBlue", 0xF0F8FF},
            {"ghostWhite", 0xF8F8FF},
            {"whiteSmoke", 0xF5F5F5},
            {"seashell", 0xFFF5EE},
            {"beige", 0xF5F5DC},
            {"oldLace", 0xFDF5E6},
            {"floralWhite", 0xFFFAF0},
            {"ivory", 0xFFFFF0},
            {"antiqueWhite", 0xFAEBD7},
            {"linen", 0xFAF0E6},
            {"lavenderBlush", 0xFFF0F5},
            {"mistyRose", 0xFFE4E1},
            {"gainsboro", 0xDCDCDC},
            {"lightGray", 0xD3D3D3},
            {"silver", 0xC0C0C0},
            {"darkGrey", 0xA9A9A9},
            {"grey", 0x808080},
            {"dimGrey", 0x696969},
            {"lightSlateGrey", 0x778899},
            {"slateGrey", 0x708090},
            {"darkSlateGrey", 0x2F4F4F},
            {"black", 0x000000}
        };
        
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
                _currentVerts++;
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
            EnableSmooth();
            
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
        
        
        void ClearColor(int col) {
            glClearColor((float)((col & 0xff0000) >> 16 )   / 255,
                         (float)((col & 0x00ff00) >> 8  )   / 255,
                         (float)((col & 0x0000ff)       )   / 255, 1.0f);
        }
        
        void ClearColor(std::string colorName) {
            ClearColor(_predefinedColors[colorName]);
        }
        
        void SetColor(int col) {
            SetColor((float)((col & 0xff0000) >> 16 )   / 255,
                     (float)((col & 0x00ff00) >> 8  )   / 255,
                     (float)((col & 0x0000ff)       )   / 255);
        }
        
        void SetColor(std::string colorName) {
            SetColor(_predefinedColors[colorName]);
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
            BeginRendering(GL_LINE_LOOP);
                AddVert(x, y, 0);
                AddVert(x + w, y, 0);
                AddVert(x + w, y + h, 0);
                AddVert(x, y + h, 0);
                AddVert(x, y, 0);
            EndRendering();
        }
        
        void Grad(float x, float y, float w, float h, int col1, int col2, bool vert) {
            if (col1 > 256 * 256 * 256 || col2 > 256 * 256 * 256) {
                return;
			}
            
            float col1R = (float)((col1 & 0xff0000) >> 16) / 255;
            float col1G = (float)((col1 & 0x00ff00) >> 8) / 255;
            float col1B = (float)(col1 & 0x0000ff) / 255;
            
            float col2R = (float)((col2 & 0xff0000) >> 16) / 255;
            float col2G = (float)((col2 & 0x00ff00) >> 8) / 255;
            float col2B = (float)(col2 & 0x0000ff) / 255;
            
            Draw2D::BeginRendering(GL_QUADS);
            if (vert) {
                Draw2D::AddVert(x, y, 0, col1R, col1G, col1B, 1.0f);
                Draw2D::AddVert(x + w, y, 0, col1R, col1G, col1B, 1.0f);
                Draw2D::AddVert(x + w, y + h, 0, col2R, col2G, col2B, 1.0f);
                Draw2D::AddVert(x, y + h, 0, col2R, col2G, col2B, 1.0f);
            } else {
                Draw2D::AddVert(x, y, 0, col1R, col1G, col1B, 1.0f);
                Draw2D::AddVert(x + w, y, 0, col2R, col2G, col2B, 1.0f);
                Draw2D::AddVert(x + w, y + h, 0, col2R, col2G, col2B, 1.0f);
                Draw2D::AddVert(x, y + h, 0, col1R, col1G, col1B, 1.0f);
            }
            Draw2D::EndRendering();
        }
        
        void Lines(float* points, unsigned int count) {
            Draw2D::BeginRendering(GL_LINE_STRIP);
            for (int i = 0; i < count * 2; i += 2) {
                AddVert(points[i], points[i + 1], 0);
            }
            EndRendering();
        }
        
        void LineGraph(float xOff, float yOff, float xScale, float yScale, float* points, unsigned int count) {
            float* newPoints = (float*) std::malloc(count * 2 * sizeof(float)); // twice the size of points
            for (int x = 0; x < count; x++) {
                newPoints[x * 2] = xOff + (x * xScale);
                newPoints[x * 2 + 1] = yOff + ((-points[x]) * yScale);
            }
            Lines(newPoints, count);
            std::free(newPoints);
        }
        
        void Circle(float xCenter, float yCenter, float radius) {
            Circle(xCenter, yCenter, radius, radius * 5, 0.0f, 1.0f, true);
        }
        
        void Circle(float xCenter, float yCenter, float radius, bool fill) {
            Circle(xCenter, yCenter, radius, radius * 5, 0.0f, 1.0f, fill);
        }
        
        void Circle(float xCenter, float yCenter, float radius, int segments, bool fill) {
            Circle(xCenter, yCenter, radius, segments, 0.0f, 1.0f, fill);
        }
        
        void Circle(float xCenter, float yCenter, float radius, int segments, float start, float end, bool fill) {
            Circle(xCenter, yCenter, radius, radius, segments, start, end, fill);
        }
        
        void Circle(float xCenter, float yCenter, float radius, float innerRadius,
                    int segments, float start, float end, bool fill) {
            static double pi2 = 2 * 3.14159265358979323846;
            float rStart = pi2 * start;
            float rEnd = pi2 * end;
            float res = pi2 / segments;
            BeginRendering(fill ? (innerRadius != radius ? GL_TRIANGLE_STRIP // doughnut
                                   : GL_TRIANGLE_FAN) // filled circle
                           : (end == 1.0f ? GL_LINE_LOOP // circle outline
                              : GL_LINE_STRIP // circle outline segment
                              ));
            if (fill) {
                if (innerRadius == radius) {
                    AddVert(xCenter, yCenter, 0.0f);
                } else {
                    AddVert(xCenter + radius * glm::cos(rStart), yCenter + radius * glm::sin(rStart), 0.0f);
                    AddVert(xCenter + innerRadius * glm::cos(rStart), yCenter + innerRadius * glm::sin(rStart), 0.0f);
                }
            }
            for (float i = rStart; i < rEnd; i += res) {
                                                    AddVert(xCenter + radius * glm::cos(i),
                                                            yCenter + radius * glm::sin(i), 0.0f);
                if (innerRadius != radius && fill)  AddVert(xCenter + innerRadius * glm::cos(i),
                                                            yCenter + innerRadius * glm::sin(i), 0.0f);
            }
            if (fill && end == 1.0f && start == 0.0f) {
                if (innerRadius == radius) {
                    AddVert(xCenter + radius * glm::cos(rStart), yCenter + radius * glm::sin(rStart), 0.0f);
                } else {
                    AddVert(xCenter + radius * glm::cos(rStart), yCenter + radius * glm::sin(rStart), 0.0f);
                    AddVert(xCenter + innerRadius * glm::cos(rStart), yCenter + innerRadius * glm::sin(rStart), 0.0f);
                }
            }
            EndRendering();
        }
        
        
        // from http://devmag.org.za/2011/04/05/bzier-curves-a-tutorial/
        glm::vec3 _calculateBezierPoint(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
            float u = 1.0f - t;
            float tt = t*t;
            float uu = u*u;
            float uuu = uu * u;
            float ttt = tt * t;
            
            glm::vec3 p = uuu * p0; //first term
            p += 3 * uu * t * p1;   //second term
            p += 3 * u * tt * p2;   //third term
            p += ttt * p3;          //fourth term
            
            return p;
        }
        
        void BezierCurve(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
            glm::vec3 vec1 = glm::vec3(x1, y1, 0.0f);
            glm::vec3 vec2 = glm::vec3(x2, y2, 0.0f);
            glm::vec3 vec3 = glm::vec3(x3, y3, 0.0f);
            glm::vec3 vec4 = glm::vec3(x4, y4, 0.0f);
            
            float totalDistince = glm::distance(vec1, vec2) + glm::distance(vec2, vec3) + glm::distance(vec3, vec4);
            
            BezierCurve(vec1, vec2, vec3, vec4, glm::floor(totalDistince / 25));
        }
        
        void BezierCurve(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int segments) {
            glm::vec3 vec1 = glm::vec3(x1, y1, 0.0f);
            glm::vec3 vec2 = glm::vec3(x2, y2, 0.0f);
            glm::vec3 vec3 = glm::vec3(x3, y3, 0.0f);
            glm::vec3 vec4 = glm::vec3(x4, y4, 0.0f);
            BezierCurve(vec1, vec2, vec3, vec4, segments);
        }
        
        void BezierCurve(glm::vec3 vec1, glm::vec3 vec2, glm::vec3 vec3, glm::vec3 vec4, int segments) {
            glm::vec3 q0 = _calculateBezierPoint(0, vec1, vec2, vec3, vec4);
            
            BeginRendering(GL_LINE_STRIP);
            
            AddVert(q0.x, q0.y, q0.z);
            
            for (int i = 1; i <= segments; i++) {
                float t = (1 / (float) segments) * i;
                q0 = _calculateBezierPoint(t, vec1, vec2, vec3, vec4);
                AddVert(q0.x, q0.y, q0.z);
            }
            EndRendering();
        }
        
        void Line(float x1, float y1, float x2, float y2) {
            BeginRendering(GL_LINES);
                AddVert(x1, y1, 0.0f);
                AddVert(x2, y2, 0.0f);
            EndRendering();
        }
    }
}