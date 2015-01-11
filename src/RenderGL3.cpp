/*
   Filename: RenderGL3.cpp
   Purpose:  OpenGL 3.x Rendering Interface

   Part of Engine2D

   Copyright (C) 2014 Vbitz

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include <unordered_map>

#define GLEW_STATIC
#include "vendor/GL/glew.h"

#include "RenderGL3.hpp"

#include "GL3Buffer.hpp"
#include "TextureLoader.hpp"

#include "Config.hpp"

#include "Application.hpp"
#include "Profiler.hpp"

#define GLM_FORCE_RADIANS
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"

namespace Engine {
    std::string GLErrorString(int error) {
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
    
    std::string GLModeToString(int mode) {
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
    
	void __stdcall DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam) {
		std::string sourceStr = "[UNKNOWN source]";
		switch (source) {
			case GL_DEBUG_SOURCE_API: sourceStr = "OpenGL"; break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceStr = "GLSLCompiler"; break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM: sourceStr = "WindowSystem"; break;
			case GL_DEBUG_SOURCE_THIRD_PARTY: sourceStr = "ThirdParty"; break;
			case GL_DEBUG_SOURCE_APPLICATION: sourceStr = "Application"; break;
			case GL_DEBUG_SOURCE_OTHER: sourceStr = "Other"; break;
			default: break;
		}
		std::string typeStr = "[UNKNOWN type]";
		switch (type) {
			case GL_DEBUG_TYPE_ERROR: typeStr = "Error"; break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeStr = "DeprecatedBehavior"; break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: typeStr = "UndefinedBehavior"; break;
			case GL_DEBUG_TYPE_PERFORMANCE: typeStr = "Performance"; break;
			case GL_DEBUG_TYPE_PORTABILITY: typeStr = "Portability"; break;
			case GL_DEBUG_TYPE_MARKER: typeStr = "Marker"; break;
			case GL_DEBUG_TYPE_OTHER: typeStr = "Other"; break;
			default: break;
		}
		std::string severityStr = "[UNKNOWN severity]";
		switch (severity) {
			case GL_DEBUG_SEVERITY_HIGH: severityStr = "High Severity"; break;
			case GL_DEBUG_SEVERITY_MEDIUM: severityStr = "Medium Severity"; break;
			case GL_DEBUG_SEVERITY_LOW: severityStr = "Low Severity"; break;
			case GL_DEBUG_SEVERITY_NOTIFICATION: severityStr = "Notification"; return; // Notifys are ignored because many happen every frame 
			default: break;
		}
		Logger::begin("OpenGLDebug", Logger::LogLevel_Verbose) << sourceStr << " : " << typeStr << " : " <<
			id << " : " << severityStr << " : " << message << Logger::end();
    }
    
    ENGINE_CLASS(RenderGL3);
    
    class RenderGL3 : public RenderDriver {
    public:
        RendererType GetRendererType() override {
            return RendererType::OpenGL3;
        }
        
        OpenGLVersion GetOpenGLVersion() override {
			static OpenGLVersion version = GetAppSingilton()->GetWindow()->GetGlVersion();
            return version;
        }
        
        EffectShaderType GetBestEffectShaderType() override {
            OpenGLVersion v = this->GetOpenGLVersion();
            if (v.major == 2 && v.minor == 0)
                return EffectShaderType::GLSL_110;
            else if (v.major == 2 && v.minor == 1)
                return EffectShaderType::GLSL_120;
            else if (v.major == 3 && v.minor == 0)
                return EffectShaderType::GLSL_130;
            else if (v.major == 3 && v.minor == 1)
                return EffectShaderType::GLSL_140;
            else if (v.major == 3 && v.minor == 2)
                return EffectShaderType::GLSL_150;
            else if (v.major == 3 && v.minor == 3)
                return EffectShaderType::GLSL_330;
            else if (v.major == 4 && v.minor == 0)
                return EffectShaderType::GLSL_400;
            else if (v.major == 4 && v.minor == 1)
                return EffectShaderType::GLSL_410;
            else if (v.major == 4 && v.minor == 2)
                return EffectShaderType::GLSL_420;
            else if (v.major == 4 && v.minor == 3)
                return EffectShaderType::GLSL_430;
            else if (v.major == 4 && v.minor == 4)
                return EffectShaderType::GLSL_440;
            else
                return EffectShaderType::Unknown;
        }
        
        bool CheckError(const char* source) override {
            GLenum err;
            bool oneError = false;
            while ((err = glGetError()) != GL_NO_ERROR) {
                Logger::begin("OpenGL", Logger::LogLevel_Error) << "GLError in " << source << " : " << GLErrorString(err) << Logger::end();
                Platform::DumpStackTrace();
                throw new RenderDriverError(source, err, GLErrorString(err));
            }
            return oneError;
        }

		void EnableDefaultTexture() override {
			static TexturePtr tex = NULL;
			static Platform::MutexPtr createMutex = Platform::CreateMutex();
			if (createMutex->SafeEnter()) {
				if (tex == NULL) {
					tex = ImageReader::TextureFromBuffer(new float[4] {1.0f, 1.0f, 1.0f, 1.0f}, 1, 1);
				}
				tex->Begin();
				createMutex->Exit();
			}
		}
        
        bool HasExtention(std::string extentionName) override {
            ENGINE_PROFILER_SCOPE;
            
            bool has = glewGetExtension(extentionName.c_str());
            
            glGetError(); // A bug in GLEW always throws a GL error
            
            CheckError("RenderGL3::HasExtention::PostGlewGetExtention");
            
            return has;
        }
        
        std::vector<std::string> GetExtentions() override {
            ENGINE_PROFILER_SCOPE;
            
            std::vector<std::string> ret;
            
            int extentionCount;
            glGetIntegerv(GL_NUM_EXTENSIONS, &extentionCount);
                
            for (int i = 0; i < extentionCount; i++) {
                const GLubyte* str = glGetStringi(GL_EXTENSIONS, i);
                if (str != NULL) {
                    ret.push_back(std::string((const char*) str));
                }
            }
            
            CheckError("RenderGL3::GetExtentions");
            
            return ret;
        }
        
        void ResetMatrix() override {
            ENGINE_PROFILER_SCOPE;
            
            this->_currentModelMatrix = glm::mat4();
            if (Config::GetBoolean("core.render.halfPix")) {
                this->_currentModelMatrix = glm::translate(this->_currentModelMatrix, glm::vec3(0.5f, 0.5f, 0.0f));
            }
        }
        
        void BeginRendering(PolygonMode mode) override {
            ENGINE_PROFILER_SCOPE;
            
            if (this->_currentMode != mode ||
                this->_currentMode == PolygonMode::LineStrip) {
                // it's a hack, I really need to fix this
                this->TrackStat(RenderStatistic::PrimitiveFlush, 1);
                FlushAll();
                this->_currentMode = mode;
            }
            
            if (this->_currentTexture != this->_activeTexture && this->_currentTexture != NULL) {
                this->TrackStat(RenderStatistic::TextureFlush, 1);
                //std::cout << "TextureChange: " << (this->_currentTexture != NULL ?Platform::StringifyUUID(this->_currentTexture->GetUUID()) : "NULL") << " -> " << (this->_activeTexture != NULL ?Platform::StringifyUUID(this->_activeTexture->GetUUID()) : "NULL") << std::endl;
                FlushAll();
            }
        }
        
        void EndRendering() override {
            this->TrackStat(RenderStatistic::PrimitiveEnd, 1);
        }
        
        void EnableTexture(TexturePtr texId) override {
            this->_currentTexture = texId;
        }
        
        void DisableTexture() override {
            this->_currentTexture = NULL;
        }
        
        void EnableSmooth() override {
            ENGINE_PROFILER_SCOPE;
            
            glEnable(GL_LINE_SMOOTH);
            glEnable(GL_POLYGON_SMOOTH);
            glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
            glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
        }
        
        void DisableSmooth() override {
            ENGINE_PROFILER_SCOPE;
            
            glDisable(GL_LINE_SMOOTH);
            glDisable(GL_POLYGON_SMOOTH);
        }
        
        void SetShader(ShaderPtr shader) override {
            unsigned int id = shader->GetId();
            if (this->_currentShader != id) {
                ENGINE_PROFILER_SCOPE;
                
                glUseProgram(id);
                
                this->_currentShader = id;
            }
        }
        
        void SetLineWidth(float value) override {
            glLineWidth(value);
        }
        
        void FlushAll() override {
            ENGINE_PROFILER_SCOPE;
            
            this->CheckError("RenderGL3::FlushAll::Pre");
            
            if (this->_gl3Buffer->Update()) {
                Logger::begin("RenderGL3", Logger::LogLevel_Log) << "Render Buffer Reloaded" << Logger::end();
            }
            
            this->_gl3Buffer->Draw(this->_currentMode, this->_currentModelMatrix);
            
            this->CheckError("RenderGL3::FlushAll::PostDraw");
            
            this->_gl3Buffer->Reset();
            
            if (this->_activeTexture != this->_currentTexture) {
                this->_switchTextures();
            }
        }
		
        void Init2d() override {
            ENGINE_PROFILER_SCOPE;
            
			if (Config::GetBoolean("core.debug.debugRenderer") && glDebugMessageCallback != NULL) {
				glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
                glDebugMessageCallback((GLDEBUGPROC) DebugMessageCallback, NULL);
            }
            
            std::string gl3Effect = Config::GetString("core.render.basicEffect");
            this->_currentEffect = EffectReader::GetEffectFromFile(gl3Effect);
            this->_currentEffect->CreateShader();
            this->_gl3Buffer = new VertexBuffer(this, this->_currentEffect);
			this->_currentTexture = NULL;
			this->EnableDefaultTexture();
            
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        
        void Clear() {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        }
        
		void Begin2d() override {
            ENGINE_PROFILER_SCOPE;
            
            EnableSmooth();
            
			this->_currentTexture = NULL;
			this->EnableDefaultTexture();
            
            glDisable(GL_DEPTH_TEST);
            
            ResetMatrix();
            
            this->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
            
            CheckError("RenderGL3::Begin2d");
        }
        
        void End2d() override {
            ENGINE_PROFILER_SCOPE;
            
            this->TrackStat(RenderStatistic::EndRenderFlush, 1);
            FlushAll();
            
            CheckError("RenderGL3::End2d");
        }
        
        void Reset() override {
            ENGINE_PROFILER_SCOPE;
            
            End2d();
            Begin2d();
        }
        
        void Set(RenderStateFlag flag, bool enable) override {
            if (this->_flagStates[flag] == enable) return; // noop
            GLenum glFlag;
            switch (flag) {
                case RenderStateFlag::DepthTest: glFlag = GL_DEPTH_TEST; break;
                case RenderStateFlag::Blend: glFlag = GL_BLEND; break;
                case RenderStateFlag::Lighting: glFlag = GL_LIGHTING; break;
            }
            enable ? glEnable(glFlag) : glDisable(glFlag);
            this->_flagStates[flag] = enable;
        }
        
        void SetCenter(float x, float y) override {
            this->_center = glm::vec3(x, y, 0);
        }
        
        void CameraPan(float x, float y) override {
            this->TrackStat(RenderStatistic::CameraFlush, 1);
            FlushAll();
            this->_currentModelMatrix = glm::translate(this->_currentModelMatrix, glm::vec3(x, y, 0.0f));
        }
        
        void CameraZoom(float f) override {
            this->TrackStat(RenderStatistic::CameraFlush, 1);
            FlushAll();
            this->_currentModelMatrix =
            glm::scale(_currentModelMatrix, glm::vec3(f, f, 0.0f));
        }
        
        void CameraRotate(float r) override {
            this->TrackStat(RenderStatistic::CameraFlush, 1);
            FlushAll();
            r = glm::radians(r);
            this->_currentModelMatrix =
            glm::rotate(this->_currentModelMatrix, r, glm::vec3(0, 0, 1));
        }
    protected:
        void _clearColor(Color4f col) override {
            glClearColor(col.r, col.g, col.b, col.a);
        }
        
        void _addVert(glm::vec3 pos, Color4f col, glm::vec2 uv, glm::vec3 normal) override {
            this->_gl3Buffer->AddVert(pos - this->_center,
                                      col, uv, this->_currentTexture != NULL ? 1 : 2);
        }
        
    private:
        void _switchTextures() {
            ENGINE_PROFILER_SCOPE;
            
            this->_activeTexture = this->_currentTexture;
            
			if (this->_activeTexture != NULL) {
				try {
					this->_activeTexture->Begin();
				}
				catch (...) {
					Logger::begin("RenderGL3", Logger::LogLevel_Error) << "Error switching textures" << Logger::end();
					this->_activeTexture = NULL;
				}
			}
        }
        
        glm::vec3 _center = glm::vec3(0, 0, 0);
        
        PolygonMode _currentMode = PolygonMode::Invalid;
        
        EffectParametersPtr _currentEffect;

        TexturePtr _activeTexture = NULL;
        TexturePtr _currentTexture = NULL;
        
        VertexBufferPtr _gl3Buffer = NULL;
        
        glm::mat4 _currentModelMatrix;

		bool usingTexture = false;
        
        unsigned int _currentShader = std::numeric_limits<unsigned int>::max();
        
        // From https://stackoverflow.com/questions/9646297/c11-hash-function-for-any-enum-type
        
        std::unordered_map<RenderStateFlag, bool, enum_hash> _flagStates;
    };
    
    RenderDriverPtr CreateRenderGL3() {
        return new RenderGL3();
    }
}