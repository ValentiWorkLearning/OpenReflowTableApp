#include <QQmlContext>
#include <application/application.h>
#include <application/edit_popup_mode.h>
#include <presetslistmodel.h>
#include <reflow_process_controller/reflow_controller.hpp>
#include <reflow_rest_client/reflow_client.hpp>

namespace AppSetup
{

class App::AppImpl : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(PresetsListModel* presetsModel READ getPresetsModel CONSTANT)
    Q_PROPERTY(Reflow::ProcessController* reflowController READ getReflowController CONSTANT)
    Q_INVOKABLE void refresh()
    {
        m_presetsModel->refresh();
    }

public:
    void registerTypes()
    {
        EditPopupModeNs::registerEditPopupModeEnum();
        PresetsListModel::registerQmlType();
        Reflow::ProcessController::registerQmlType();
        Reflow::Client::Stage::registerQmlType();
    }

    void addContextProperties(QQmlApplicationEngine& _qmlEngine)
    {
        m_restClient = std::make_shared<Reflow::Client::ReflowRestClient>("localhost:8086");
        m_restClient->testPingPongConnection();

        m_presetsModel = std::make_unique<PresetsListModel>(m_restClient);
        m_reflowProcessController = std::make_unique<Reflow::ProcessController>(m_restClient);

        _qmlEngine.setObjectOwnership(m_presetsModel.get(), QQmlEngine::CppOwnership);

        qmlRegisterSingletonType<App::AppImpl>(
            "app.root_model", 1, 0, "AppModel", [this](QQmlEngine* engine, QJSEngine*) -> QObject* {
                engine->setObjectOwnership(this, QQmlEngine::CppOwnership);
                return this;
            });
    }

    void initAppHandlers()
    {
    }

    void cleanupApplication()
    {
    }
signals:

    void presetsModelChanged();

private:
    PresetsListModel* getPresetsModel() const
    {
        return m_presetsModel.get();
    }
    Reflow::ProcessController* getReflowController() const

    {
        return m_reflowProcessController.get();
    }

private:
    std::unique_ptr<PresetsListModel> m_presetsModel;
    std::unique_ptr<Reflow::ProcessController> m_reflowProcessController;
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

#include "application.moc"
