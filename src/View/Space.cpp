#include "Space.h"
#include <QWheelEvent>
#include <QFile>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QDebug>

Space::Space(QWidget* parent)
    : QOpenGLWidget(parent)
    , m_cameraPosition(0.0f, 0.0f, 5.0f)
    , m_cameraTarget(0.0f, 0.0f, 0.0f)
    , m_zoom(1.0f)
{
    m_rotationController = std::make_unique<RotationController>(width(), height());
}

Space::~Space() = default;

bool Space::LoadModelFromOBJ(const QString& filePath)
{
    // Здесь должен быть ваш загрузчик .obj, который наполняет m_model
    // Например:
    // m_model = std::make_unique<Model>();
    // ObjLoader loader;
    // loader.LoadFile(filePath.toStdString(), *m_model);
    // Для примера вернём true
    update();
    return true;
}

void Space::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // Загрузка и компиляция шейдеров
    m_shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertex.glsl");
    m_shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragment.glsl");
    if (!m_shader.link()) {
        qCritical() << "Shader link error:" << m_shader.log();
    }
}

void Space::resizeGL(int w, int h)
{
    m_projection.setToIdentity();
    m_projection.perspective(45.0f, float(w) / float(h), 0.1f, 100.0f);
    m_rotationController->ResizeWindow(w, h);
}

void Space::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setupCamera();

    if (m_model) {
        renderModel();
    }
}

void Space::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        QPoint pos = event->pos();
        m_rotationController->OnMouse(0, 0, pos.x(), pos.y());
    }
}

void Space::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        QPoint pos = event->pos();
        m_rotationController->OnMouse(0, 1, pos.x(), pos.y());
    }
}

void Space::mouseMoveEvent(QMouseEvent* event)
{
    QPoint pos = event->pos();
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
    // Пример передачи матриц в шейдер
    m_shader.bind();
    m_shader.setUniformValue("projection", m_projection);
    m_shader.setUniformValue("view", m_view * m_rotationController->GetRotationMatrix());
    // Здесь должен быть ваш код рендера модели через VBO/VAO
    // Например:
    // for (const auto& mesh : m_model->meshes()) { mesh.draw(&m_shader); }
    m_shader.release();
}
