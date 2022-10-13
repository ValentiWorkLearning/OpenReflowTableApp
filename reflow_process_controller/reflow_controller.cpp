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
        : m_pRestClient{pRestClient}
        , m_pThis{pThis}
        , m_systemState{}
        , m_hasConnection{false}
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

    QCoro::Task<> setRegulatorParams(QString k, QString hysteresis)
    {
        auto params = Reflow::Client::RegulatorParams{
            .hysteresis = static_cast<std::uint32_t>(hysteresis.toUInt()), .k = k.toFloat()};

        co_await m_pRestClient->setRegulatorParams(params);
    }

    void startWork()
    {
        requestSystemStateWork();
    }

    bool hasConnection() const
    {
        return m_hasConnection;
    }

    [[nodiscard]] QString getDeviceAddress() const
    {
        return m_pRestClient->getDeviceAddress();
    }
    void setDeviceAddress(const QString& deviceAddress)
    {
        return m_pRestClient->setDeviceAddress(deviceAddress);
    }

    QCoro::QmlTask requestRegulatorParams()
    {
        return m_pRestClient->getRegulatorParams();
    }

private:
 
    QCoro::Task<> requestSystemStateWork()
    {
        using namespace std::chrono_literals;
        QTimer stateTimer;
        stateTimer.setInterval(400ms);
        stateTimer.start();
        while (true)
        {
            co_await stateTimer;
            auto systemState = co_await m_pRestClient->getSystemState();
            if (!systemState.has_value())
            {
                setConnectionUnavailable();
            }
            else
            {
                setConnectionAvailable();
                setSystemState(systemState.value());
            }
        }
    }

    void setConnectionUnavailable()
    {
        if (m_hasConnection)
        {
            m_hasConnection = false;
            emit m_pThis->connectionStatusChanged();
        }
    }

    void setConnectionAvailable()
    {
        if (!m_hasConnection)
        {
            m_hasConnection = true;
            emit m_pThis->connectionStatusChanged();
        }
    }

private:
    std::shared_ptr<Reflow::Client::ReflowRestClient> m_pRestClient;
    Reflow::Client::SystemState m_systemState;
    ProcessController* m_pThis;
    bool m_hasConnection;
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

void ProcessController::setRegulatorParams(QString k, QString hysteresis)
{
    m_pImpl->setRegulatorParams(k, hysteresis);
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

bool ProcessController::getConnectionState() const
{
    return m_pImpl->hasConnection();
}

QString ProcessController::getDeviceAddress() const
{
    return m_pImpl->getDeviceAddress();
}

void ProcessController::setDeviceAddress(const QString& deviceAddress)
{
    return m_pImpl->setDeviceAddress(deviceAddress);
}

QCoro::QmlTask ProcessController::requestRegulatorParams()
{
    return m_pImpl->requestRegulatorParams();
}

} // namespace Reflow
