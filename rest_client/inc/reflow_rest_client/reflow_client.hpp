#pragma once

#include <qcoro/task.h>

#include <memory>
#include <optional>

namespace Reflow::Client
{
class ReflowRestClient
{

public:
    ReflowRestClient();
    ~ReflowRestClient();

public:
    QCoro::Task<> testPingPongConnection();
    QCoro::Task<> createNewPreset(const QString& presetName);
private:

    class ReflowRestClientImpl;
    std::unique_ptr<ReflowRestClientImpl> m_pImpl;
};
}
