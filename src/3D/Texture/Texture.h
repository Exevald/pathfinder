#pragma once

#include <QOpenGLTexture>
#include <memory>
#include <cassert>

class BaseTexture
{
public:
    BaseTexture()
        : m_texture(nullptr)
    {}

    explicit BaseTexture(QOpenGLTexture::Target target)
        : m_texture(std::make_unique<QOpenGLTexture>(target))
    {}

    // Генерируем текстурный объект
    void Create(QOpenGLTexture::Target target)
    {
        assert(!m_texture);
        m_texture = std::make_unique<QOpenGLTexture>(target);
    }

    // Удаляем текстурный объект
    void Delete()
    {
        m_texture.reset();
    }

    // Отвязываемся от текстурного объекта и возвращаем указатель на него
    std::unique_ptr<QOpenGLTexture> Detach()
    {
        return std::move(m_texture);
    }

    // Получаем указатель на текстурный объект
    QOpenGLTexture* Get() const
    {
        return m_texture.get();
    }

    // Делаем объект активным (биндим)
    void Bind() const
    {
        assert(m_texture);
        m_texture->bind();
    }

protected:
    void SetTexture(std::unique_ptr<QOpenGLTexture> texture)
    {
        m_texture = std::move(texture);
    }

private:
    std::unique_ptr<QOpenGLTexture> m_texture;
};

// Управляемая/неуправляемая обертка
template <bool t_managed, class TBase>
class TextureImpl : public TBase
{
public:
    explicit TextureImpl(QOpenGLTexture::Target target = QOpenGLTexture::Target2D)
        : TBase(target)
    {}

    ~TextureImpl()
    {
        if (t_managed && this->Get())
            this->Delete();
    }

    void Attach(std::unique_ptr<QOpenGLTexture> texture)
    {
        if (t_managed && this->Get() && texture.get() != this->Get())
            this->Delete();
        this->SetTexture(std::move(texture));
    }
};

class Texture2DImpl : public BaseTexture
{
public:
	Texture2DImpl()
		: BaseTexture(QOpenGLTexture::Target2D)
	{}

	explicit Texture2DImpl(QOpenGLTexture::Target target)
		: BaseTexture(target)
	{}

	void Bind() const
	{
		BaseTexture::Bind();
	}
};

class Texture1DImpl : public BaseTexture
{
public:
	Texture1DImpl()
		: BaseTexture(QOpenGLTexture::Target1D)
	{}

	explicit Texture1DImpl(QOpenGLTexture::Target target)
		: BaseTexture(target)
	{}

	void Bind() const
	{
		BaseTexture::Bind();
	}
};

// typedef-ы для удобства
using Texture        = TextureImpl<true,  BaseTexture>;
using TextureHandle  = TextureImpl<false, BaseTexture>;
using Texture2D      = TextureImpl<true,  Texture2DImpl>;
using Texture2DHandle= TextureImpl<false, Texture2DImpl>;
using Texture1D      = TextureImpl<true,  Texture1DImpl>;
using Texture1DHandle= TextureImpl<false, Texture1DImpl>;
