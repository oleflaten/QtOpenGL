#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include <QWindow>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions_4_1_Core>

class QOpenGLContext;
class QOpenGLShaderProgram;
class Shader;

class RenderWindow : public QWindow, protected QOpenGLFunctions_4_1_Core
{
    Q_OBJECT
public:
    RenderWindow(const QSurfaceFormat &format);
    QOpenGLContext *context() { return m_context; }
    void exposeEvent(QExposeEvent *) override;

signals:
    void ready();
    void error(const QString &msg);

private slots:
    void render();

private:
    void init();
    //void setupVertexAttribs();

    QOpenGLContext *m_context;
    bool m_initialized;
    QOpenGLShaderProgram *m_program;
    Shader *m_program2;
    GLint m_posAttr, m_colAttr, m_matrixUniform;

    GLuint VBO;
    GLuint colorbuffer;

//    QOpenGLVertexArrayObject m_vao;
//    QOpenGLBuffer m_vbo;
    float m_angle;
};

#endif // RENDERWINDOW_H
