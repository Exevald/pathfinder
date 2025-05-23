#pragma once
#include "Buffer.h"

#include <GL/gl.h>

class BufferObjectBase
{
public:
	void Create()
	{
		if (m_bufferId == 0)
		{
			glGenBuffers(1, &m_bufferId);
		}
	}

	BufferObjectBase(BufferObjectBase const&) = delete;
	BufferObjectBase& operator=(BufferObjectBase const&) = delete;

	explicit operator GLuint() const
	{
		return m_bufferId;
	}

	GLuint Detach()
	{
		GLuint bufferId = m_bufferId;
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
	explicit BufferObjectBase(GLuint bufferId)
		: m_bufferId(bufferId)
	{
	}

	void BindTo(GLenum target) const
	{
		glBindBuffer(target, m_bufferId);
	}

	void SetBuffer(GLuint bufferId)
	{
		m_bufferId = bufferId;
	}

	GLuint GetBufferId()
	{
		return m_bufferId;
	}

private:
	GLuint m_bufferId;
};

template <bool t_managed, GLenum target>
class BufferObjectImpl : public BufferObjectBase
{
public:
	explicit BufferObjectImpl(GLuint bufferId = 0)
		: BufferObjectBase(bufferId)
	{
	}

	int GetBufferId()
	{
		return BufferObjectBase::GetBufferId();
	}

	~BufferObjectImpl()
	{
		if (t_managed && (GetBufferId() != 0))
		{
			Delete();
		}
	}

	void Bind() const
	{
		BindTo(target);
	}

	void Attach(GLuint bufferId)
	{
		if (t_managed && (GetBufferId() != 0) && (bufferId != GetBufferId()))
		{
			Delete();
		}
		SetBuffer(bufferId);
	}

	static GLvoid* MapBuffer(GLenum access)
	{
		return glMapBuffer(target, access);
	}

	static GLboolean UnmapBuffer()
	{
		return glUnmapBuffer(target);
	}

	static void BufferData(GLsizeiptr size, GLvoid const* data, GLenum usage)
	{
		glBufferData(target, size, data, usage);
	}

	static void BufferSubData(GLintptr offset, GLsizeiptr size, const GLvoid* data)
	{
		glBufferSubData(target, offset, size, data);
	}

	BufferObjectImpl& operator=(GLuint bufferId)
	{
		Attach(bufferId);
		return *this;
	}
};

typedef BufferObjectImpl<true, GL_ARRAY_BUFFER> ArrayBuffer;
typedef BufferObjectImpl<false, GL_ARRAY_BUFFER> ArrayBufferHandle;
typedef BufferObjectImpl<true, GL_ELEMENT_ARRAY_BUFFER> ElementArrayBuffer;
typedef BufferObjectImpl<false, GL_ELEMENT_ARRAY_BUFFER> ElementArrayBufferHandle;