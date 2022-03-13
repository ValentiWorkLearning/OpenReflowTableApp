#pragma once

#include <qcoro/task.h>

#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <string_view>

#include <QString>

namespace Reflow::Client
{

struct Stage
{
    std::uint32_t temperature;
    std::uint32_t duration;
};

struct Preset
{
    std::uint64_t presetId;
    QString presetName;
};

struct RegulatorParams
{
    std::uint32_t hysteresis;
    float k;
};

class ReflowRestClient
{

public:
    ReflowRestClient(const std::string& serverUrlBase);
    ~ReflowRestClient();

public:
    QCoro::Task<> testPingPongConnection();
    QCoro::Task<std::uint64_t> createNewPreset(const QString& presetName);
    QCoro::Task<> addStagesToPreset(std::uint64_t presetId,const std::vector<Stage> stages);
    QCoro::Task<std::vector<Preset>> getAvailablePresets();
    QCoro::Task<> selectActivePreset(const std::uint64_t presetId);
    QCoro::Task<> setRegulatorParams(const RegulatorParams& regulatorParams);
    QCoro::Task<RegulatorParams> getRegulatorParams();
    QCoro::Task<> startReflow();
    QCoro::Task<> stopReflow();
private:
    class ReflowRestClientImpl;
    std::unique_ptr<ReflowRestClientImpl> m_pImpl;
};
} // namespace Reflow::Client
