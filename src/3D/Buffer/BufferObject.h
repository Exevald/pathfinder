#pragma once

#include <QOpenGLContext>
#include <QOpenGLFunctions_3_2_Core>
#include <stdexcept>

class BufferObjectBase : protected QOpenGLFunctions_3_2_Core
{
public:
    void Create()
    {
        if (m_bufferId == 0)
        {
            initializeOpenGLFunctions();
            glGenBuffers(1, &m_bufferId);
        }
    }

    BufferObjectBase(BufferObjectBase const&) = delete;
    BufferObjectBase& operator=(BufferObjectBase const&) = delete;

    explicit operator GLuint() const { return m_bufferId; }

    GLuint Detach()
    {
        const GLuint bufferId = m_bufferId;
        m_bufferId = 0;
        return bufferId;
    }

    void Delete()
    {
        if (m_bufferId != 0)
        {
            glDeleteBuffers(1, &m_bufferId);
            m_bufferId = 0;
        }
    }

protected:
    explicit BufferObjectBase(GLuint bufferId = 0) : m_bufferId(bufferId)
    {
		QOpenGLFunctions_3_2_Core::initializeOpenGLFunctions();
    }

    void BindTo(GLenum target)
    {
        glBindBuffer(target, m_bufferId);
    }

    void BindTo(GLenum target) const
    {
        const_cast<BufferObjectBase*>(this)->BindTo(target);
    }

    void SetBuffer(GLuint bufferId) { m_bufferId = bufferId; }
    [[nodiscard]] GLuint GetBufferId() const { return m_bufferId; }

private:
    GLuint m_bufferId = 0;
};

template <bool t_managed, GLenum target>
class BufferObjectImpl final : public BufferObjectBase
{
public:
    explicit BufferObjectImpl(GLuint bufferId = 0) : BufferObjectBase(bufferId) {}

    ~BufferObjectImpl() override
    {
        if (t_managed && GetBufferId() != 0)
            Delete();
    }

    void Bind() const
    {
        BindTo(target);
    }

    void Attach(GLuint bufferId)
    {
        if (t_managed && GetBufferId() != 0 && bufferId != GetBufferId())
            Delete();
        SetBuffer(bufferId);
    }

	static void* MapBuffer(GLenum access)
    {
    	QOpenGLFunctions_3_2_Core glFuncs;
    	if (!glFuncs.initializeOpenGLFunctions())
    		throw std::runtime_error("Failed to initialize OpenGL functions");
    	return glFuncs.glMapBuffer(target, access);
    }

	static bool UnmapBuffer()
    {
    	QOpenGLFunctions_3_2_Core glFuncs;
    	if (!glFuncs.initializeOpenGLFunctions())
    		throw std::runtime_error("Failed to initialize OpenGL functions");
    	return glFuncs.glUnmapBuffer(target);
    }

	static void BufferData(GLsizeiptr size, const void* data, GLenum usage)
    {
    	QOpenGLFunctions_3_2_Core glFuncs;
    	if (!glFuncs.initializeOpenGLFunctions())
    		throw std::runtime_error("Failed to initialize OpenGL functions");
    	glFuncs.glBufferData(target, size, data, usage);
    }

	static void BufferSubData(GLintptr offset, GLsizeiptr size, const void* data)
    {
    	QOpenGLFunctions_3_2_Core glFuncs;
    	if (!glFuncs.initializeOpenGLFunctions())
    		throw std::runtime_error("Failed to initialize OpenGL functions");
    	glFuncs.glBufferSubData(target, offset, size, data);
    }

    BufferObjectImpl& operator=(GLuint bufferId)
    {
        Attach(bufferId);
        return *this;
    }
};

using ArrayBuffer = BufferObjectImpl<true, GL_ARRAY_BUFFER>;
using ArrayBufferHandle = BufferObjectImpl<false, GL_ARRAY_BUFFER>;
using ElementArrayBuffer = BufferObjectImpl<true, GL_ELEMENT_ARRAY_BUFFER>;
using ElementArrayBufferHandle = BufferObjectImpl<false, GL_ELEMENT_ARRAY_BUFFER>;