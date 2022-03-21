#pragma once
#include <QObject>
#include <memory>
#include <reflow_rest_client/reflow_client.hpp>
namespace Reflow
{

class ProcessController : public QObject
{
    Q_OBJECT
public:
    ProcessController(
        std::shared_ptr<Reflow::Client::ReflowRestClient> pRestClient,
        QObject* parent = nullptr);
    ~ProcessController();

public:
    Q_INVOKABLE void startReflow();
    Q_INVOKABLE void stopReflow();
    Q_INVOKABLE void selectPreset(QString presetId);

    Q_INVOKABLE void setRegulatorParams(QString k, QString hysteresis);

    Q_PROPERTY(Reflow::Client::SystemState systemState READ getSystemState WRITE setSystemState
                   NOTIFY systemStateChanged)
    Q_PROPERTY(Reflow::Client::RegulatorParams regulatorParams READ getRegulatorParams WRITE
                   setRegulatorParams NOTIFY regulatorParamsChanged)

    Reflow::Client::SystemState getSystemState() const;
    Reflow::Client::RegulatorParams getRegulatorParams() const;

    void setSystemState(const Reflow::Client::SystemState& systemState);
    void setRegulatorParams(const Reflow::Client::RegulatorParams& regulatorParams);
signals:
    void systemStateChanged(Reflow::Client::SystemState state);
    void regulatorParamsChanged(Reflow::Client::RegulatorParams state);

public:
    static void registerQmlType();

private:
    class ProcessControllerImpl;
    std::unique_ptr<ProcessControllerImpl> m_pImpl;
};
}; // namespace Reflow
