#pragma once
#include <QMatrix4x4>

class RotationController
{
public:
	RotationController(int windowWidth, int windowHeight);
	~RotationController() = default;

	[[nodiscard]] bool LeftButtonIsPressed() const;
	void ResizeWindow(int windowWidth, int windowHeight);
	void OnMouse(int button, int state, int x, int y);
	void OnMotion(int x, int y);
	[[nodiscard]] QMatrix4x4 GetRotationMatrix() const;

private:
	void NormalizeModelViewMatrix();
	void RotateCamera(float rotateX, float rotateY);

	bool m_leftButtonPressed = false;
	int m_mouseX = 0;
	int m_mouseY = 0;
	int m_windowWidth;
	int m_windowHeight;
	QMatrix4x4 m_rotationMatrix;
};