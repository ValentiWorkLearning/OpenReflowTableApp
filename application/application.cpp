#include <application/application.h>

#include <QQmlContext>

namespace AppSetup
{

class App::AppImpl
{

public:
    void registerTypes()
    {
    }

    void addContextProperties(QQmlApplicationEngine& _qmlEngine)
    {
        // _qmlEngine.rootContext()->setContextProperty(
        //     "testingDocumentsModel", m_testingDocumentsModel.get());
        // _qmlEngine.rootContext()->setContextProperty("logsDataModel", m_logsDataModel.get());
    }

    void initAppHandlers()
    {
    }

    void cleanupApplication()
    {
    }

private:
private:
};

App::App() : m_pAppImpl{std::make_unique<App::AppImpl>()}
{
}

App::~App() = default;

App& App::Instance()
{
    static App appInstance{};
    return appInstance;
}

void App::registerTypes()
{
    m_pAppImpl->registerTypes();
}

void App::addContextProperties(QQmlApplicationEngine& _qmlEngine)
{
    m_pAppImpl->addContextProperties(_qmlEngine);
}

void App::initAppHandlers()
{
    m_pAppImpl->initAppHandlers();
}

void App::cleanupApplication()
{
    m_pAppImpl->cleanupApplication();
}

} // namespace AppSetup
