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

public:
    static void registerQmlType();

private:
    class ProcessControllerImpl;
    std::unique_ptr<ProcessControllerImpl> m_pImpl;
};
}; // namespace Reflow
