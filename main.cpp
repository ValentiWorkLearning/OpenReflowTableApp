#include <QApplication>
#include <QQmlApplicationEngine>
#include <application/application.h>

int main(int argc, char* argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    qputenv("QT_QUICK_CONTROLS_STYLE", QByteArray("Material"));
    qputenv("QT_QUICK_CONTROLS_MATERIAL_THEME", QByteArray("Dark"));

    QApplication app(argc, argv);

    AppSetup::App::Instance().registerTypes();

    QQmlApplicationEngine engine;

    AppSetup::App::Instance().initAppHandlers();
    AppSetup::App::Instance().addContextProperties(engine);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject* obj, const QUrl& objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    QObject::connect(&app, &QGuiApplication::aboutToQuit, [] {
        AppSetup::App::Instance().cleanupApplication();
    });

    return app.exec();
}
