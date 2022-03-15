#pragma once

#include <qcoro/task.h>

#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <string_view>

#include <QMetaType>
#include <QString>

namespace Reflow::Client
{

struct Stage
{
private:
    Q_GADGET
public:
    Q_PROPERTY(std::uint32_t temperature MEMBER temperature)
    Q_PROPERTY(std::uint32_t duration MEMBER duration)

    static void registerQmlType()
    {
        qRegisterMetaType<QVector<Stage>>();
    }

public:
    std::uint32_t temperature;
    std::uint32_t duration;
};

struct Preset
{
private:
    Q_GADGET
public:
    Q_PROPERTY(QString presetId MEMBER presetId)
    Q_PROPERTY(QString presetName MEMBER presetName)

public:
    QString presetId;
    QString presetName;
};

struct RegulatorParams
{
private:
    Q_GADGET
public:
    Q_PROPERTY(std::uint32_t hysteresis MEMBER hysteresis)
    Q_PROPERTY(float k MEMBER k)
public:
    std::uint32_t hysteresis;
    float k;
};

struct SystemState
{
private:
    Q_GADGET
public:
    Q_PROPERTY(std::int32_t currentTemperature MEMBER currentTemperature)
    Q_PROPERTY(QString reflowState MEMBER reflowState)
    Q_PROPERTY(QString activePresetId MEMBER activePresetId)
    Q_PROPERTY(std::int32_t systemTime MEMBER systemTime)
    Q_PROPERTY(bool isReflowRunning MEMBER isRunning)

    std::uint32_t currentTemperature;
    std::uint32_t systemTime;
    QString reflowState;
    QString activePresetId;
    bool isRunning;
};

class ReflowRestClient
{

public:
    ReflowRestClient(const std::string& serverUrlBase);
    ~ReflowRestClient();

public:
    QCoro::Task<> testPingPongConnection();
    QCoro::Task<std::uint64_t> createNewPreset(const QString& presetName);
    QCoro::Task<> addStagesToPreset(QString presetId, const std::vector<Stage> stages);
    QCoro::Task<QVector<Stage>> getPresetStages(QString presetId);
    QCoro::Task<QVector<Preset>> getAvailablePresets();
    QCoro::Task<> selectActivePreset(QString presetId);
    QCoro::Task<> setRegulatorParams(const RegulatorParams& regulatorParams);
    QCoro::Task<RegulatorParams> getRegulatorParams();
    QCoro::Task<SystemState> getSystemState();
    QCoro::Task<> startReflow();
    QCoro::Task<> stopReflow();

private:
    class ReflowRestClientImpl;
    std::unique_ptr<ReflowRestClientImpl> m_pImpl;
};
} // namespace Reflow::Client

Q_DECLARE_METATYPE(Reflow::Client::Stage)
Q_DECLARE_METATYPE(Reflow::Client::Preset)
Q_DECLARE_METATYPE(Reflow::Client::RegulatorParams)
Q_DECLARE_METATYPE(Reflow::Client::SystemState)
