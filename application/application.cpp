#include <QQmlContext>
#include <application/application.h>
#include <application/edit_popup_mode.h>
#include <reflow_rest_client/reflow_client.hpp>

namespace AppSetup
{

class App::AppImpl
{

public:
    void registerTypes()
    {
        EditPopupMode::registerEditPopupModeEnum();
    }

    void addContextProperties(QQmlApplicationEngine& _qmlEngine)
    {
        m_restClient = std::make_shared<Reflow::Client::ReflowRestClient>("localhost:8086");
        m_restClient->testPingPongConnection();
        m_restClient->createNewPreset("SMT9603005-38");
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
    std::shared_ptr<Reflow::Client::ReflowRestClient> m_restClient;
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
