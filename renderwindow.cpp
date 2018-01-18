#include "renderwindow.h"
#include <QTimer>
#include <QMatrix4x4>
#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>

#include "shader.h"

RenderWindow::RenderWindow(const QSurfaceFormat &format)
    : m_context(0),
      m_initialized(false),
      m_forceGLSL110(false),
      m_angle(0.0f)
{
    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(format);
    m_context = new QOpenGLContext(this);
    m_context->setFormat(requestedFormat());
    if (!m_context->create()) {
        delete m_context;
        m_context = 0;
    }
}

void RenderWindow::exposeEvent(QExposeEvent *)
{
    if (isExposed())
        render();
}

static const char *vertexShaderSource =
        "#version 330\n"
        "in vec4 posAttr;\n"
        "in vec4 colAttr;\n"
        "out vec4 col;\n"
        "uniform mat4 matrix;\n"
        "void main() {\n"
        "   col = colAttr;\n"
        "   gl_Position = matrix * posAttr;\n"
        "}\n";

static const char *fragmentShaderSource =
        "#version 330\n"
        "in vec4 col;\n"
        "out vec4 fragColor;\n"
        "void main() {\n"
        "   fragColor = col;\n"
        "}\n";

static GLfloat vertices[] = {
    0.0f, 0.707f,
    -0.5f, -0.5f,
    0.5f, -0.5f
};

static GLfloat colors[] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f
};

void RenderWindow::init()
{
    initializeOpenGLFunctions();
    m_program = new QOpenGLShaderProgram(this);
    m_program2 = new Shader("../QtOpenGL/plainvertex.vert", "../QtOpenGL/plainfragment.frag");

    QSurfaceFormat format = m_context->format();
    //    bool useNewStyleShader = format.profile() == QSurfaceFormat::CoreProfile;
    //    // Try to handle 3.0 & 3.1 that do not have the core/compatibility profile concept 3.2+ has.
    //    // This may still fail since version 150 (3.2) is specified in the sources but it's worth a try.
    //    if (format.renderableType() == QSurfaceFormat::OpenGL && format.majorVersion() == 3 && format.minorVersion() <= 1)
    //        useNewStyleShader = !format.testOption(QSurfaceFormat::DeprecatedFunctions);
    //    if (m_forceGLSL110)
    //        useNewStyleShader = false;

    const char *vsrc = vertexShaderSource;
    const char *fsrc = fragmentShaderSource;
    qDebug("Using version %s shader",  "330");

    if (!m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vsrc)) {
        emit error(m_program->log());
        return;
    }
    if (!m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fsrc)) {
        emit error(m_program->log());
        return;
    }
    if (!m_program->link()) {
        emit error(m_program->log());
        return;
    }

    //    m_posAttr = m_program->attributeLocation("posAttr");
    //    m_colAttr = m_program->attributeLocation("colAttr");
    //    m_matrixUniform = m_program->uniformLocation("matrix");

    //m_program2->Use();
    m_colAttr = glGetUniformLocation( m_program2->Program, "colAttr" );
    m_posAttr = glGetUniformLocation( m_program2->Program, "posAttr" );
    m_matrixUniform = glGetUniformLocation( m_program2->Program, "matrix" );

    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(vertices, sizeof(vertices) + sizeof(colors));
    m_vbo.write(sizeof(vertices), colors, sizeof(colors));
    m_vbo.release();

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    if (m_vao.isCreated()) // have VAO support, use it
        std::cout << "vao created\n";
//        setupVertexAttribs();
}

//void RenderWindow::setupVertexAttribs()
//{
//    m_vbo.bind();
//    m_program->setAttributeBuffer(m_posAttr, GL_FLOAT, 0, 2);
//    m_program->setAttributeBuffer(m_colAttr, GL_FLOAT, sizeof(vertices), 3);
//    m_program->enableAttributeArray(m_posAttr);
//    m_program->enableAttributeArray(m_colAttr);
//    m_vbo.release();
//}

void RenderWindow::render()
{
    if (!m_context->makeCurrent(this)) {
        emit error(tr("makeCurrent() failed"));
        return;
    }

    QOpenGLFunctions *f = m_context->functions();
    if (!m_initialized) {
        m_initialized = true;
        f->glEnable(GL_DEPTH_TEST);
        f->glClearColor(0.2f, 0.2f, 0.2f,1.0f);
        init();
        emit ready();
    }

    if (!m_vbo.isCreated()) // init() failed, don't bother with trying to render
        return;

    const qreal retinaScale = devicePixelRatio();
    f->glViewport(0, 0, width() * retinaScale, height() * retinaScale);
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //m_program->bind();
    //m_program2->Use();
    f->glUseProgram( m_program2->Program );
    //    QMatrix4x4 matrix;
    //    matrix.perspective(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    //    matrix.translate(0.0f, 0.0f, -2.0f);
    //    matrix.rotate(m_angle, 0.0f, 1.0f, 0.0f);

    const GLfloat modelviewmatrix[] = {
        1.0f , 0.0f , 0.0f , 0.0f,
        0.0f, 1.0f, 0.0f , 0.0f ,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    f->glUniformMatrix4fv( m_matrixUniform, 1, GL_FALSE, modelviewmatrix);
    //m_program2->s->setUniformValue(m_matrixUniform, matrix);

    if (m_vao.isCreated())
        m_vao.bind();
//    else // no VAO support, set the vertex attribute arrays now
//        setupVertexAttribs();

    f->glDrawArrays(GL_TRIANGLES, 0, 3);
    GLenum err = GL_NO_ERROR;
//    while((err = f->glGetError()) != GL_NO_ERROR)
        f->glGetError();
    {
        std::cout << "glGetError returns " << err;
    }


    m_vao.release();
    m_program->release();

    // swapInterval is 1 by default which means that swapBuffers() will (hopefully) block
    // and wait for vsync.
    m_context->swapBuffers(this);

    m_angle += 1.0f;

    // Instead of 0 wait a few more milliseconds before rendering again. This is
    // only here to make the UI widgets more responsive on slower machines. We
    // can afford it since our rendering is so lightweight.
    const int interval = 5;
    QTimer::singleShot(interval, this, &RenderWindow::render);
}

