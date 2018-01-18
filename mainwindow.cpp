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

    fmt.setVersion(4, 1);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setRenderableType(QSurfaceFormat::OpenGL);

    // The example rendering will need a depth buffer.
    fmt.setDepthBufferSize(16);

    m_output->clear();
    qDebug() << "Requesting surface format" << fmt;

    RenderWindow *renderWindow = new RenderWindow(fmt, this);
    if (!renderWindow->context()) {
        m_output->append(tr("Failed to create context"));
        delete renderWindow;
        m_renderWindowContainer = new QWidget;
        addRenderWindow();
        return;
    }
    m_surface = renderWindow;

    connect(renderWindow, &RenderWindow::ready, this, &MainWindow::renderWindowReady);
    connect(renderWindow, &RenderWindow::error, this, &MainWindow::renderWindowError);

    m_renderWindowContainer = QWidget::createWindowContainer(renderWindow);
    addRenderWindow();
}

void MainWindow::renderWindowReady()
{
    QOpenGLContext *context = QOpenGLContext::currentContext();
    Q_ASSERT(context);
}

void MainWindow::renderWindowError(const QString &msg)
{
    m_output->append(tr("An error has occurred:\n%1").arg(msg));
}

void MainWindow::addRenderWindow()
{
    ui->renderLayout->addWidget(m_renderWindowContainer);
}
