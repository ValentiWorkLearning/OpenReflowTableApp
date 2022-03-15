#include "reflow_process_controller/reflow_controller.hpp"
#include <QQmlEngine>
#include <QTimer>
#include <qcoro/core/qcorotimer.h>

namespace Reflow
{
class ProcessController::ProcessControllerImpl
{
public:
    ProcessControllerImpl(
        std::shared_ptr<Reflow::Client::ReflowRestClient> pRestClient,
        ProcessController* pThis)
        : m_pRestClient{pRestClient}, m_pThis{pThis}
    {
    }

    QCoro::Task<> startReflow()
    {
        co_await m_pRestClient->startReflow();
    }
    QCoro::Task<> stopReflow()
    {
        co_await m_pRestClient->stopReflow();
    }

    QCoro::Task<> selectPreset(QString presetId)
    {
        co_await m_pRestClient->selectActivePreset(presetId);
    }

public:
    Reflow::Client::SystemState getSystemState() const
    {
        return m_systemState;
    }

    void setSystemState(const Reflow::Client::SystemState& systemState)
    {
        m_systemState = systemState;
        emit m_pThis->systemStateChanged(m_systemState);
    }

    void startWork()
    {
        requestSystemStateWork();
    }

private:
    QCoro::Task<> requestSystemStateWork()
    {
        using namespace std::chrono_literals;
        QTimer stateTimer;
        stateTimer.setInterval(1s);
        stateTimer.start();
        while (true)
        {
            co_await stateTimer;
            auto systemState = co_await m_pRestClient->getSystemState();
            setSystemState(systemState);
        }
    }

private:
    std::shared_ptr<Reflow::Client::ReflowRestClient> m_pRestClient;
    Reflow::Client::SystemState m_systemState;
    ProcessController* m_pThis;
};

ProcessController::ProcessController(
    std::shared_ptr<Reflow::Client::ReflowRestClient> pRestClient,
    QObject* parent)
    : QObject{parent}, m_pImpl{std::make_unique<ProcessControllerImpl>(pRestClient, this)}
{
    m_pImpl->startWork();
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

void ProcessController::selectPreset(QString presetId)
{
    m_pImpl->selectPreset(presetId);
}

void ProcessController::registerQmlType()
{
    qmlRegisterUncreatableType<ProcessController>(
        "app.reflow_controller", 1, 0, "ProcessController", "interface");
    qRegisterMetaType<ProcessController*>("ProcessController*");
}

Reflow::Client::SystemState ProcessController::getSystemState() const
{
    return m_pImpl->getSystemState();
}

void ProcessController::setSystemState(const Reflow::Client::SystemState& systemState)
{
    return m_pImpl->setSystemState(systemState);
}

} // namespace Reflow
