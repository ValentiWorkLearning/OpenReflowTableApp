#pragma once
#include <QObject>
#include <memory>
#include <reflow_rest_client/reflow_client.hpp>
#include <qcoro/qml/qcoroqmltask.h>

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

    Q_PROPERTY(bool connectionStatus READ getConnectionState NOTIFY connectionStatusChanged)
    Q_PROPERTY(QString deviceAddress READ getDeviceAddress WRITE setDeviceAddress)

    Q_INVOKABLE void setRegulatorParams(QString k, QString hysteresis);

    Q_PROPERTY(Reflow::Client::SystemState systemState READ getSystemState WRITE setSystemState
                   NOTIFY systemStateChanged)

    Q_INVOKABLE QCoro::QmlTask requestRegulatorParams();

    [[nodiscard]] Reflow::Client::SystemState getSystemState() const;

    void setSystemState(const Reflow::Client::SystemState& systemState);
    void setRegulatorParams(const Reflow::Client::RegulatorParams& regulatorParams);
    bool getConnectionState() const;

    [[nodiscard]] QString getDeviceAddress() const;
    void setDeviceAddress(const QString& deviceAddress);

signals:
    void systemStateChanged(Reflow::Client::SystemState state);
    void connectionStatusChanged();

public:
    static void registerQmlType();

private:
    class ProcessControllerImpl;
    std::unique_ptr<ProcessControllerImpl> m_pImpl;
};
}; // namespace Reflow
