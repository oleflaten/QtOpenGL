#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include <QWindow>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

class QOpenGLContext;
class QOpenGLShaderProgram;

class RenderWindow : public QWindow
{
    Q_OBJECT
public:
    RenderWindow(const QSurfaceFormat &format);
    QOpenGLContext *context() { return m_context; }
    void exposeEvent(QExposeEvent *) override;
    void setForceGLSL110(bool enable) { m_forceGLSL110 = enable; }

signals:
    void ready();
    void error(const QString &msg);

private slots:
    void render();

private:
    void init();
    void setupVertexAttribs();

    QOpenGLContext *m_context;
    bool m_initialized;
    bool m_forceGLSL110;
    QOpenGLShaderProgram *m_program;
    int m_posAttr, m_colAttr, m_matrixUniform;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo;
    float m_angle;
};

#endif // RENDERWINDOW_H
