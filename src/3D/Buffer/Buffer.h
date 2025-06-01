#pragma once

#include "BufferObject.h"

#include <QOpenGLFunctions>
#include <QOpenGLFunctions_3_2_Core>
#include <stdexcept>
#include <vector>

template <GLenum target>
class BufferImpl final : protected QOpenGLFunctions_3_2_Core
{
public:
	explicit BufferImpl(bool useVBO = true)
		: m_hasData(false)
		, m_useVBO(useVBO)
		, m_created(false)
	{
	}

	BufferImpl(BufferImpl const&) = delete;
	BufferImpl& operator=(BufferImpl const&) = delete;

	void Create()
	{
		if (!m_created)
		{
			if (m_useVBO && m_buffer.GetBufferId() == 0)
				m_buffer.Create();
			m_created = true;
		}
	}

	void Bind() const
	{
		if (!m_created)
			throw std::logic_error("Buffer not created");

		if (m_useVBO)
			m_buffer.Bind();
		else
			QOpenGLContext::currentContext()->functions()->glBindBuffer(target, 0);

		UpdateCurrentBufferPointer(false, this);
	}

	static void Unbind()
	{
		QOpenGLContext::currentContext()->functions()->glBindBuffer(target, 0);
		UpdateCurrentBufferPointer(false, nullptr);
	}

	void BufferData(GLsizeiptr size, const void* data, GLenum usage)
	{
		UpdateCurrentBufferPointer(true, this);

		if (m_useVBO)
		{
			m_buffer.BufferData(size, data, usage);
		}
		else if (data)
		{
			m_data.assign(static_cast<const uint8_t*>(data),
				static_cast<const uint8_t*>(data) + size);
		}
		else
		{
			m_data.clear();
			m_data.shrink_to_fit();
		}

		m_hasData = (data != nullptr);
	}

	[[nodiscard]] const void* GetBufferPointer() const
	{
		if (!m_hasData)
			throw std::logic_error("Buffer contains no data");

		UpdateCurrentBufferPointer(true, this);
		return m_useVBO ? nullptr : m_data.data();
	}

	~BufferImpl()
	{
		UpdateCurrentBufferPointer(false, nullptr);
	}

private:
	static void UpdateCurrentBufferPointer(bool callBind, const BufferImpl* thisPtr)
	{
		static const BufferImpl* currentBuffer = nullptr;

		if (currentBuffer != thisPtr)
		{
			currentBuffer = thisPtr;
			if (callBind && thisPtr)
				thisPtr->Bind();
		}
	}

	BufferObjectImpl<true, target> m_buffer;
	std::vector<uint8_t> m_data;
	bool m_hasData;
	bool m_useVBO;
	bool m_created;
};

using VertexBuffer = BufferImpl<GL_ARRAY_BUFFER>;
using IndexBuffer = BufferImpl<GL_ELEMENT_ARRAY_BUFFER>;