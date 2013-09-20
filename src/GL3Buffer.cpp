#include "GL3Buffer.hpp"

namespace Engine {
    GL3Buffer::GL3Buffer(Shader shader) : _currentShader(shader), _shaderBound(false) {
        glGenVertexArrays(1, &this->_vertexArrayPointer);
        glGenBuffers(1, &this->_vertexBufferPointer);
    }
    
    GL3Buffer::~GL3Buffer() {
        glDeleteBuffers(1, &this->_vertexBufferPointer);
        glDeleteVertexArrays(1, &this->_vertexArrayPointer);
    }
    
    void GL3Buffer::Upload(float *buffer, int count) {
        glBindVertexArray(this->_vertexArrayPointer);
        glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBufferPointer);
        
        glBufferData(GL_ARRAY_BUFFER, count, buffer, GL_STATIC_DRAW);
        
        Draw2D::CheckGLError("GL3Buffer::Upload::PostUploadBufferData");
        
        if (!this->_shaderBound) {
            this->bindShader();
            this->_shaderBound = true;
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    
    void GL3Buffer::Draw(GLenum mode, int vertexCount) {
        glBindVertexArray(this->_vertexArrayPointer);
        glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBufferPointer);
        
        this->_currentShader.Begin();
        
        Draw2D::CheckGLError("GL3Buffer::Draw::PostBindShader");
        
        glm::mat4 proj = glm::ortho(0.0f, (float) getScreenWidth(), (float) getScreenHeight(), 0.0f, 1.0f, -1.0f);
        
        this->_currentShader.UploadUniform("projection", proj);
        
        Draw2D::CheckGLError("GL3Buffer::Draw::PostUploadUniform");
        
        glDrawArrays(mode, 0, vertexCount);
        
        Draw2D::CheckGLError("GL3Buffer::Draw::PostDraw");
        
        this->_currentShader.End();
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    
    void GL3Buffer::bindShader() {
        this->_currentShader.Begin();
        
        Draw2D::CheckGLError("GL3Buffer::Upload::PostBeginShader");
        
        this->_currentShader.BindUniform("projection");
        
        Draw2D::CheckGLError("GL3Buffer::Upload::PostBindViewpointSize");
        
        this->_currentShader.BindVertexAttrib("vertex", 3, 9, 0);
        this->_currentShader.BindVertexAttrib("color", 4, 9, 3);
        this->_currentShader.BindVertexAttrib("texCoard", 2, 9, 7);
        
        Draw2D::CheckGLError("GL3Buffer::Upload::PostBindVertexAttributes");
        
        this->_currentShader.End();
    }
}