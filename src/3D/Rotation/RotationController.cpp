#include "RotationController.h"
#include "Vector3.h"

RotationController::RotationController(int windowWidth, int windowHeight)
	: m_leftButtonPressed(false)
	, m_windowWidth(windowWidth)
	, m_windowHeight(windowHeight)
{
}

void RotationController::ResizeWindow(int windowWidth, int windowHeight)
{
	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;
}

void RotationController::OnMouse(int button, int state, int x, int y)
{
}

void RotationController::OnMotion(int x, int y)
{
	if (m_leftButtonPressed)
	{
		int dx = x - m_mouseX;
		int dy = y - m_mouseY;

		float rotateX = static_cast<float>(dy) * 180.0f / m_windowHeight;
		float rotateY = static_cast<float>(dx) * 180.0f / m_windowWidth;

		RotateCamera(rotateX, rotateY);

		m_mouseX = x;
		m_mouseY = y;
	}
}

QMatrix4x4 RotationController::GetRotationMatrix() const
{
	return m_rotationMatrix;
}

// Вращаем камеру вокруг начала кординат на заданный угол
void RotationController::RotateCamera(float rotateX, float rotateY)
{
	// Извлекаем направления осей из матрицы вращения
	QVector3D xAxis = m_rotationMatrix.column(0).toVector3D().normalized();
	QVector3D yAxis = m_rotationMatrix.column(1).toVector3D().normalized();

	// Создаём матрицы вращения
	QMatrix4x4 rotation;
	rotation.rotate(rotateY, yAxis);
	rotation.rotate(rotateX, xAxis);

	// Применяем вращение к текущей матрице
	m_rotationMatrix = rotation * m_rotationMatrix;

	// Ортонормируем матрицу
	NormalizeModelViewMatrix();
}

void RotationController::NormalizeModelViewMatrix()
{
	// Извлекаем векторы осей
	QVector3D xAxis = m_rotationMatrix.column(0).toVector3D().normalized();
	QVector3D yAxis = m_rotationMatrix.column(1).toVector3D().normalized();

	// Пересчитываем оси
	QVector3D zAxis = QVector3D::crossProduct(xAxis, yAxis).normalized();
	xAxis = QVector3D::crossProduct(yAxis, zAxis).normalized();
	yAxis = QVector3D::crossProduct(zAxis, xAxis).normalized();

	// Обновляем матрицу
	m_rotationMatrix.setColumn(0, QVector4D(xAxis, 0.0f));
	m_rotationMatrix.setColumn(1, QVector4D(yAxis, 0.0f));
	m_rotationMatrix.setColumn(2, QVector4D(zAxis, 0.0f));
}

bool RotationController::LeftButtonIsPressed() const
{
	return m_leftButtonPressed;
}