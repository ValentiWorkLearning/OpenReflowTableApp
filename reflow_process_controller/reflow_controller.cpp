#include "reflow_process_controller/reflow_controller.hpp"
#include <QQmlEngine>
namespace Reflow
{
class ProcessController::ProcessControllerImpl
{
public:
    ProcessControllerImpl(std::shared_ptr<Reflow::Client::ReflowRestClient> pRestClient)
        : m_pRestClient{pRestClient}
    {
    }

    void startReflow()
    {
    }
    void stopReflow()
    {
    }

private:
    std::shared_ptr<Reflow::Client::ReflowRestClient> m_pRestClient;
};

ProcessController::ProcessController(
    std::shared_ptr<Reflow::Client::ReflowRestClient> pRestClient,
    QObject* parent)
    : QObject{parent}, m_pImpl{std::make_unique<ProcessControllerImpl>(pRestClient)}
{
}

ProcessController::~ProcessController() = default;

void ProcessController::startReflow()
{
    m_pImpl->startReflow();
}
void ProcessController::stopReflow()
{
    m_pImpl->stopReflow();
}

void ProcessController::registerQmlType()
{
    qmlRegisterUncreatableType<ProcessController>(
        "app.reflow_controller", 1, 0, "ProcessController", "interface");
    qRegisterMetaType<ProcessController*>("ProcessController*");
}

} // namespace Reflow
