#include "COpenGLWidget.h"

COpenGLWidget::COpenGLWidget(QWidget* parent)
	: QOpenGLWidget(parent)
	, m_cameraPosition(0.0f, 0.0f, 5.0f)
	, m_cameraDirection(0.0f, 0.0f, -1.0f)
{
}

COpenGLWidget::~COpenGLWidget()
	= default;

void COpenGLWidget::initializeGL()
{
	initializeOpenGLFunctions();

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat lightPosition[] = { 1.0f, 1.0f, 3.0f, 0.0f };
	GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat lightDiffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat lightSpecular[] = { 0.3f, 0.3f, 0.3f, 1.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	GLfloat specularColor[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specularColor);
	glMaterialf(GL_FRONT, GL_SHININESS, 20.0f);
}

void COpenGLWidget::resizeGL(int w, int h)
{
	m_projection.setToIdentity();
	m_projection.perspective(45.0f, GLfloat(w) / float(h), 0.1f, 100.0f);
}

void COpenGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_view.setToIdentity();
	m_view.lookAt(m_cameraPosition, m_cameraPosition + m_cameraDirection, QVector3D(0.0f, 1.0f, 0.0f));

	drawParallelepiped();
}

void COpenGLWidget::drawParallelepiped()
{
}

void COpenGLWidget::keyPressEvent(QKeyEvent* event)
{
	switch (event->key())
	{
	case Qt::Key_W:
		m_wPressed = true;
		break;
	case Qt::Key_A:
		m_aPressed = true;
		break;
	case Qt::Key_S:
		m_sPressed = true;
		break;
	case Qt::Key_D:
		m_dPressed = true;
		break;
	default:
		break;
	}
	updateCameraPosition();
}

void COpenGLWidget::keyReleaseEvent(QKeyEvent* event)
{
	switch (event->key())
	{
	case Qt::Key_W:
		m_wPressed = false;
		break;
	case Qt::Key_A:
		m_aPressed = false;
		break;
	case Qt::Key_S:
		m_sPressed = false;
		break;
	case Qt::Key_D:
		m_dPressed = false;
		break;
	default:
		break;
	}
	updateCameraPosition();
}

void COpenGLWidget::updateCameraPosition()
{
	const float speed = 0.1f;

	if (m_wPressed)
		m_cameraPosition += m_cameraDirection * speed;
	if (m_sPressed)
		m_cameraPosition -= m_cameraDirection * speed;
	if (m_aPressed)
		m_cameraPosition -= QVector3D::crossProduct(m_cameraDirection, QVector3D(0.0f, 1.0f, 0.0f)).normalized() * speed;
	if (m_dPressed)
		m_cameraPosition += QVector3D::crossProduct(m_cameraDirection, QVector3D(0.0f, 1.0f, 0.0f)).normalized() * speed;

	update();
}