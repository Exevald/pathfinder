#pragma once

#include "../3D/Model/Model.h"
#include "../3D/Rotation/RotationController.h"
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QVector3D>
#include <memory>

class Space final : public QOpenGLWidget
	, protected QOpenGLFunctions_3_2_Core
{
	Q_OBJECT

public:
	explicit Space(QWidget* parent = nullptr);
	~Space() override;
	void SetModel(std::shared_ptr<Model> model);
	[[nodiscard]] std::shared_ptr<Model> GetModel() const;
	[[nodiscard]] bool isGLInitialized() const { return m_glInitialized; }

signals:
	void glInitialized();

protected:
	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;

	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;

private:
	void setupCamera();
	void renderModel();

	std::shared_ptr<Model> m_model;
	std::unique_ptr<RotationController> m_rotationController;

	QOpenGLShaderProgram m_shader;
	QMatrix4x4 m_projection;
	QMatrix4x4 m_view;
	QVector3D m_cameraPosition;
	QVector3D m_cameraTarget;
	float m_zoom = 1.0f;

	bool m_glInitialized = false;
};
