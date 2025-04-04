#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QKeyEvent>
#include <QVector3D>

class COpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	explicit COpenGLWidget(QWidget* parent = nullptr);
	~COpenGLWidget() override;

protected:
	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;

	void keyPressEvent(QKeyEvent* event) override;
	void keyReleaseEvent(QKeyEvent* event) override;

private:
	void drawParallelepiped();

	QMatrix4x4 m_projection;
	QMatrix4x4 m_view;
	QVector3D m_cameraPosition;
	QVector3D m_cameraDirection;

	bool m_wPressed = false;
	bool m_aPressed = false;
	bool m_sPressed = false;
	bool m_dPressed = false;

	void updateCameraPosition();
};