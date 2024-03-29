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
        , m_regulatorParams{}
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

    void setRegulatorParams(const Reflow::Client::RegulatorParams& regulatorParams)
    {
        m_regulatorParams = regulatorParams;
        emit m_pThis->regulatorParamsChanged(m_regulatorParams.value());
    }

    QCoro::Task<> setRegulatorParams(QString k, QString hysteresis)
    {
        auto params = Reflow::Client::RegulatorParams{
            .hysteresis = static_cast<std::uint32_t>(hysteresis.toUInt()), .k = k.toFloat()};

        co_await m_pRestClient->setRegulatorParams(params);
        co_await requestRegulatorParams();
    }

    Reflow::Client::RegulatorParams getRegulatorParams()
    {
        if (!m_regulatorParams)
        {
            requestRegulatorParams();
            return {};
        }
        else
        {
            auto value = std::move(m_regulatorParams.value());
            return value;
        }
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

private:
    QCoro::Task<> requestRegulatorParams()
    {
        auto regulatorParams = co_await m_pRestClient->getRegulatorParams();
        setRegulatorParams(regulatorParams);
    }
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
    std::optional<Reflow::Client::RegulatorParams> m_regulatorParams;
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

Reflow::Client::RegulatorParams ProcessController::getRegulatorParams() const
{
    return m_pImpl->getRegulatorParams();
}

void ProcessController::setSystemState(const Reflow::Client::SystemState& systemState)
{
    return m_pImpl->setSystemState(systemState);
}

void ProcessController::setRegulatorParams(const Reflow::Client::RegulatorParams& regulatorParams)
{
    return m_pImpl->setRegulatorParams(regulatorParams);
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

} // namespace Reflow
