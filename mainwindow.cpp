#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "renderwindow.h"
#include <QSurfaceFormat>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QTextEdit>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_output = new QTextEdit;
    m_output->setReadOnly(true);
    start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::start()
{
    QSurfaceFormat fmt;

//    int idx = m_version->currentIndex();
//    if (idx < 0)
//        return;
    fmt.setVersion(4, 1);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setRenderableType(QSurfaceFormat::OpenGL);

    // The example rendering will need a depth buffer.
    fmt.setDepthBufferSize(16);

    m_output->clear();
//    m_extensions->clear();
    qDebug() << "Requesting surface format" << fmt;

    RenderWindow *renderWindow = new RenderWindow(fmt);
    if (!renderWindow->context()) {
        m_output->append(tr("Failed to create context"));
        delete renderWindow;
        m_renderWindowContainer = new QWidget;
        addRenderWindow();
        return;
    }
    m_surface = renderWindow;

    renderWindow->setForceGLSL110(false);
    connect(renderWindow, &RenderWindow::ready, this, &MainWindow::renderWindowReady);
    connect(renderWindow, &RenderWindow::error, this, &MainWindow::renderWindowError);

    m_renderWindowContainer = QWidget::createWindowContainer(renderWindow);
    addRenderWindow();
}

void MainWindow::renderWindowReady()
{
    QOpenGLContext *context = QOpenGLContext::currentContext();
    Q_ASSERT(context);

//    QString vendor, renderer, version, glslVersion;
//    const GLubyte *p;
//    QOpenGLFunctions *f = context->functions();
//    if ((p = f->glGetString(GL_VENDOR)))
//        vendor = QString::fromLatin1(reinterpret_cast<const char *>(p));
//    if ((p = f->glGetString(GL_RENDERER)))
//        renderer = QString::fromLatin1(reinterpret_cast<const char *>(p));
//    if ((p = f->glGetString(GL_VERSION)))
//        version = QString::fromLatin1(reinterpret_cast<const char *>(p));
//    if ((p = f->glGetString(GL_SHADING_LANGUAGE_VERSION)))
//        glslVersion = QString::fromLatin1(reinterpret_cast<const char *>(p));

//    m_output->append(tr("*** Context information ***"));
//    m_output->append(tr("Vendor: %1").arg(vendor));
//    m_output->append(tr("Renderer: %1").arg(renderer));
//    m_output->append(tr("OpenGL version: %1").arg(version));
//    m_output->append(tr("GLSL version: %1").arg(glslVersion));

//    m_output->append(tr("\n*** QSurfaceFormat from context ***"));
//    printFormat(context->format());

//    m_output->append(tr("\n*** QSurfaceFormat from window surface ***"));
//    printFormat(m_surface->format());

//    m_output->append(tr("\n*** Qt build information ***"));
//    const char *gltype[] = { "Desktop", "GLES 2", "GLES 1" };
//    m_output->append(tr("Qt OpenGL configuration: %1")
//                     .arg(QString::fromLatin1(gltype[QOpenGLContext::openGLModuleType()])));
//    m_output->append(tr("Qt OpenGL library handle: %1")
//                     .arg(QString::number(qintptr(QOpenGLContext::openGLModuleHandle()), 16)));

//    QList<QByteArray> extensionList = context->extensions().toList();
//    std::sort(extensionList.begin(), extensionList.end());
//    m_extensions->append(tr("Found %1 extensions:").arg(extensionList.count()));
//    Q_FOREACH (const QByteArray &ext, extensionList)
//        m_extensions->append(QString::fromLatin1(ext));

//    m_output->moveCursor(QTextCursor::Start);
//    m_extensions->moveCursor(QTextCursor::Start);
}

void MainWindow::renderWindowError(const QString &msg)
{
    m_output->append(tr("An error has occurred:\n%1").arg(msg));
}

void MainWindow::addRenderWindow()
{
    ui->renderLayout->addWidget(m_renderWindowContainer);
}
