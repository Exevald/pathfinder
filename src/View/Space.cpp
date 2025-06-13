#include "Space.h"

#include "Model/ModelRenderer.h"
#include <QDebug>
#include <QFile>
#include <QOpenGLBuffer>
#include <QWheelEvent>

Space::Space(QWidget* parent)
	: QOpenGLWidget(parent)
	, m_cameraPosition(0.0f, 0.0f, 5.0f)
	, m_cameraTarget(0.0f, 0.0f, 0.0f)
{
	m_rotationController = std::make_unique<RotationController>(width(), height());
}

Space::~Space() = default;

void Space::SetModel(std::shared_ptr<Model> model)
{
	m_model = std::move(model);
}

std::shared_ptr<Model> Space::GetModel() const
{
	return m_model;
}

void Space::initializeGL()
{
	initializeOpenGLFunctions();

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	m_shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertex.glsl");
	m_shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragment.glsl");
	if (!m_shader.link())
	{
		qCritical() << "Shader link error:" << m_shader.log();
	}

	m_glInitialized = true;
	emit glInitialized();
}

void Space::resizeGL(int w, int h)
{
	m_projection.setToIdentity();
	m_projection.perspective(45.0f, static_cast<float>(w) / static_cast<float>(h), 0.1f, 100.0f);
	m_rotationController->ResizeWindow(w, h);
}

void Space::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setupCamera();

	if (m_model)
	{
		renderModel();
	}
}

void Space::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		const QPoint pos = event->pos();
		m_rotationController->OnMouse(0, 0, pos.x(), pos.y());
	}
}

void Space::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		const QPoint pos = event->pos();
		m_rotationController->OnMouse(0, 1, pos.x(), pos.y());
	}
}

void Space::mouseMoveEvent(QMouseEvent* event)
{
	const QPoint pos = event->pos();
	m_rotationController->OnMotion(pos.x(), pos.y());
	update();
}

void Space::wheelEvent(QWheelEvent* event)
{
	m_zoom += event->angleDelta().y() / 1200.0f;
	m_zoom = qBound(0.1f, m_zoom, 10.0f);
	update();
}

void Space::setupCamera()
{
	m_view.setToIdentity();
	m_view.translate(0.0f, 0.0f, -5.0f * m_zoom);
	m_view.lookAt(m_cameraPosition, m_cameraTarget, QVector3D(0.0f, 1.0f, 0.0f));
}

void Space::renderModel()
{
	if (!m_model)
	{
		return;
	}

	m_shader.bind();
	m_shader.setUniformValue("projection", m_projection);
	m_shader.setUniformValue("view", m_view * m_rotationController->GetRotationMatrix());

	const ModelRenderer renderer(this, &m_shader);
	renderer.RenderModel(*m_model);

	m_shader.release();
}
