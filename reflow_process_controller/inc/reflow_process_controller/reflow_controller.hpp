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

    Q_PROPERTY(Reflow::Client::SystemState systemState READ getSystemState WRITE setSystemState
                   NOTIFY systemStateChanged)

    Reflow::Client::SystemState getSystemState() const;
    void setSystemState(const Reflow::Client::SystemState& systemState);
signals:
    void systemStateChanged(Reflow::Client::SystemState state);

public:
    static void registerQmlType();

private:
    class ProcessControllerImpl;
    std::unique_ptr<ProcessControllerImpl> m_pImpl;
};
}; // namespace Reflow
