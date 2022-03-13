#include <reflow_rest_client/reflow_client.hpp>

#include <qcoro/network/qcoronetworkreply.h>
#include <qcoro/task.h>

#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

static constexpr std::string_view pingPongUrlEndpoint =
    "http://{}/api/v1/ReflowController/ping-pong";
static constexpr std::string_view createPresetEndpoint = "http://{}/api/v1/ReflowController/preset";
static constexpr std::string_view addPresetStagesEndpoint =
    "http://{1}/api/v1/ReflowController/preset/{2}";

static constexpr std::string_view allPresetsEndpoint = "http://{1}/api/v1/ReflowController/presets";

static constexpr std::string_view commandsEndpoint = "http://{1}/api/v1/ReflowController/command";

static constexpr std::string_view regulatorEndpoint =
    "http://{1}/api/v1/ReflowController/regulator";

namespace Reflow::Commands
{
static constexpr std::string_view kStart = "start";
static constexpr std::string_view kStop = "stop";
static constexpr std::string_view kSelectPreset = "select-preset";
} // namespace Reflow::Commands

namespace Reflow::Client
{
class ReflowRestClient::ReflowRestClientImpl
{
public:
    ReflowRestClientImpl(const std::string& serverUrlBase) : m_serverUrlBase{serverUrlBase}
    {
    }

public:
    QCoro::Task<> testPingPongConnection()
    {
        auto* pReply = co_await m_nam.get(QNetworkRequest{getFormattedUrl(pingPongUrlEndpoint)});
        pReply->deleteLater();
    }

    QCoro::Task<std::uint64_t> createNewPreset(const QString& presetName)
    {
        nlohmann::json payload;
        payload["preset-name"] = presetName.toStdString();
        std::string resultJson = payload.dump();
        auto* pReply = co_await m_nam.post(
            QNetworkRequest{getFormattedUrl(createPresetEndpoint)}, QByteArray{resultJson.c_str()});

        auto presetId = pReply->readAll();
        auto jsonResult =
            nlohmann::json::parse(std::string_view(presetId.constData(), presetId.length()));
        pReply->deleteLater();

        std::uint64_t result{jsonResult["preset-id"].get<std::uint64_t>()};
        co_return result;
    }

    QCoro::Task<> addStagesToPreset(std::uint64_t presetId, const std::vector<Stage> stages)
    {
        nlohmann::json payload;
        nlohmann::json stagesArray;
        for (const auto& stage : stages)
        {
            nlohmann::json presetObj;
            presetObj["temperature"] = stage.temperature;
            presetObj["duration"] = stage.duration;
            stagesArray.push_back(presetObj);
        }
        payload["stages"] = stagesArray;

        auto payloadData = payload.dump();

        auto* pReply = co_await m_nam.post(
            QNetworkRequest{getFormattedUrl(addPresetStagesEndpoint, presetId)},
            QByteArray{payloadData.c_str()});
        pReply->deleteLater();
    }
    QCoro::Task<std::vector<Preset>> getAvailablePresets()
    {
        auto* pReply = co_await m_nam.get(QNetworkRequest{getFormattedUrl(allPresetsEndpoint)});
        auto replyData = pReply->readAll();

        std::vector<Preset> result;
        auto parsedData =
            nlohmann::json::parse(std::string_view(replyData.constData(), replyData.length()));

        const auto& resultArray = parsedData.get_ref<const nlohmann::json::array_t&>();
        for (auto preset : resultArray)
        {
            result.push_back(
                {preset["preset-id"].get<std::uint64_t>(),
                 QString::fromStdString(preset["preset-name"].get<std::string>())});
        }
        pReply->deleteLater();

        co_return result;
    }

    QCoro::Task<> selectActivePreset(const std::uint64_t presetId)
    {
        nlohmann::json presetPayload;
        presetPayload["preset-id"] = presetId;
        co_await sendCommandToServer(Reflow::Commands::kSelectPreset, presetPayload);
    }

    QCoro::Task<> setRegulatorParams(const RegulatorParams& regulatorParams)
    {
        nlohmann::json regulator;
        regulator["hysteresis"] = regulatorParams.hysteresis;
        regulator["k"] = regulatorParams.k;
        auto* pReply = co_await m_nam.post(
            QNetworkRequest{getFormattedUrl(regulatorEndpoint)},
            QByteArray{regulator.dump().c_str()});
        pReply->deleteLater();
    }

    QCoro::Task<RegulatorParams> getRegulatorParams()
    {
        RegulatorParams params{};

        auto* pReply = co_await m_nam.get(QNetworkRequest{getFormattedUrl(regulatorEndpoint)});
        pReply->deleteLater();

        auto replyData = pReply->readAll();

        std::vector<Preset> result;
        auto parsedData =
            nlohmann::json::parse(std::string_view(replyData.constData(), replyData.length()));

        params.hysteresis = parsedData["hysteresis"].get<std::uint32_t>();
        params.k = parsedData["k"].get<float>();
        co_return params;
    }

    QCoro::Task<> sendCommandToServer(
        std::string_view command,
        std::optional<nlohmann::json> commandPayload = std::nullopt)
    {
        nlohmann::json payload;
        payload["command"] = command;

        if (commandPayload.has_value())
            payload["payload"] = commandPayload.value();

        auto payloadData = payload.dump();
        auto* pReply = co_await m_nam.post(
            QNetworkRequest{getFormattedUrl(commandsEndpoint)}, QByteArray{payloadData.c_str()});

        pReply->deleteLater();
    }

private:
    template <typename... ExtraArgs>
    QUrl getFormattedUrl(std::string_view baseUrl, ExtraArgs&&... extraFormatArgs)
    {
        return QString::fromStdString(fmt::format(baseUrl, m_serverUrlBase, extraFormatArgs...));
    }

private:
    std::string m_serverUrlBase;
    QNetworkAccessManager m_nam;
};

QCoro::Task<> ReflowRestClient::testPingPongConnection()
{
    co_await m_pImpl->testPingPongConnection();
}

QCoro::Task<std::uint64_t> ReflowRestClient::createNewPreset(const QString& presetName)
{
    auto result = co_await m_pImpl->createNewPreset(presetName);
    co_return result;
}

QCoro::Task<> ReflowRestClient::addStagesToPreset(
    std::uint64_t presetId,
    const std::vector<Stage> stages)
{
    co_await m_pImpl->addStagesToPreset(presetId, stages);
}
QCoro::Task<std::vector<Preset>> ReflowRestClient::getAvailablePresets()
{
    auto presets = co_await m_pImpl->getAvailablePresets();
    co_return presets;
}
QCoro::Task<> ReflowRestClient::selectActivePreset(const std::uint64_t presetId)
{
    co_await m_pImpl->selectActivePreset(presetId);
}
QCoro::Task<> ReflowRestClient::setRegulatorParams(const RegulatorParams& regulatorParams)
{
    co_await m_pImpl->setRegulatorParams(regulatorParams);
}
QCoro::Task<RegulatorParams> ReflowRestClient::getRegulatorParams()
{
    auto regulatorParams = co_await m_pImpl->getRegulatorParams();
    co_return regulatorParams;
}

QCoro::Task<> ReflowRestClient::startReflow()
{
    co_await m_pImpl->sendCommandToServer(Reflow::Commands::kStart);
}
QCoro::Task<> ReflowRestClient::stopReflow()
{
    co_await m_pImpl->sendCommandToServer(Reflow::Commands::kStop);
}

ReflowRestClient::ReflowRestClient(const std::string& serverUrlBase)
    : m_pImpl{std::make_unique<ReflowRestClientImpl>(serverUrlBase)}
{
}
ReflowRestClient::~ReflowRestClient() = default;

} // namespace Reflow::Client
