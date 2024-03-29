#include <reflow_rest_client/reflow_client.hpp>

#include <qcoro/network/qcoronetworkreply.h>
#include <qcoro/qcorotask.h>

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
static constexpr std::string_view presetInfoEndpoint =
    "http://{}/api/v1/ReflowController/preset/{}";

static constexpr std::string_view addPresetStagesEndpoint =
    "http://{}/api/v1/ReflowController/preset/{}";

static constexpr std::string_view allPresetsEndpoint = "http://{}/api/v1/ReflowController/presets";

static constexpr std::string_view commandsEndpoint = "http://{}/api/v1/ReflowController/command";

static constexpr std::string_view regulatorEndpoint = "http://{}/api/v1/ReflowController/regulator";

static constexpr std::string_view telemetryEndpoint = "http://{}/api/v1/ReflowController/telemetry";

namespace Reflow::Commands
{
static constexpr std::string_view kStart = "start";
static constexpr std::string_view kStop = "stop";
static constexpr std::string_view kSelectPreset = "select-preset";
} // namespace Reflow::Commands

struct NamHolder
{

    static NamHolder& Instance()
    {
        static NamHolder holder;
        return holder;
    }

public:
    std::shared_ptr<QNetworkAccessManager> getNam()
    {
        return m_networkAccessManager;
    }

private:
    NamHolder() = default;

private:
    std::shared_ptr<QNetworkAccessManager> m_networkAccessManager =
        std::make_shared<QNetworkAccessManager>();
};

namespace Reflow::Client
{
class ReflowRestClient::ReflowRestClientImpl
{
public:
    ReflowRestClientImpl()
        : m_serverUrlBase{}, m_nam{NamHolder::Instance().getNam()}
    {
    }

public:
    QCoro::Task<> testPingPongConnection()
    {
        co_await executeGetRequest(pingPongUrlEndpoint);
    }

    QCoro::Task<std::uint64_t> createNewPreset(const QString& presetName)
    {
        nlohmann::json payload;
        payload["preset-name"] = presetName.toStdString();

        auto responseOpt = co_await executePostRequest(createPresetEndpoint, payload.dump());
        // TODO handle Erorrs
        auto jsonResult = responseOpt.value();
        std::uint64_t result{jsonResult["preset-id"].get<std::uint64_t>()};
        co_return result;
    }

    QCoro::Task<> addStagesToPreset(QString presetId, const std::vector<Stage>& stages)
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

        spdlog::info("Stages JSON payload:{}", payload.dump());

        co_await executePutRequest(addPresetStagesEndpoint, payload.dump(), presetId.toStdString());
    }

    QCoro::Task<> addStagesToPreset(QString presetId, QString stages)
    {
        co_await executePutRequest(
            addPresetStagesEndpoint, stages.toStdString(), presetId.toStdString());
    }

    QCoro::Task<QVector<Preset>> getAvailablePresets()
    {
        QVector<Preset> result;
        auto parsedData = co_await executeGetRequest(allPresetsEndpoint);
        
        if (!parsedData.has_value())
            co_return result;

        const auto& resultArray = parsedData.value().get_ref<const nlohmann::json::array_t&>();
        for (auto preset : resultArray)
        {
            result.push_back(
                {QString::number(preset["preset-id"].get<std::uint64_t>()),
                 QString::fromStdString(preset["preset-name"].get<std::string>())});
        }

        co_return result;
    }

    QCoro::Task<> selectActivePreset(QString presetId)
    {
        nlohmann::json presetPayload;
        presetPayload["preset-id"] = presetId.toLongLong();
        co_await sendCommandToServer(Reflow::Commands::kSelectPreset, presetPayload);
    }

    QCoro::Task<> setRegulatorParams(const RegulatorParams& regulatorParams)
    {
        nlohmann::json regulator;
        regulator["hysteresis"] = regulatorParams.hysteresis;
        regulator["k"] = regulatorParams.k;

        co_await executePostRequest(regulatorEndpoint, regulator.dump());
    }

    QCoro::Task<RegulatorParams> getRegulatorParams()
    {
        RegulatorParams params{};

        std::vector<Preset> result;
        auto parsedData = co_await executeGetRequest(regulatorEndpoint);
        if (!parsedData.has_value())
            co_return params;

        params.hysteresis = parsedData.value()["hysteresis"].get<std::uint32_t>();
        params.k = parsedData.value()["k"].get<float>();
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

        co_await executePostRequest(commandsEndpoint, payload.dump());
    }

    QCoro::Task<QVector<Stage>> getPresetStages(QString presetId)
    {
        QVector<Stage> stagesResult;
        auto presetData = co_await executeGetRequest(presetInfoEndpoint, presetId.toStdString());

        if (!presetData)
            co_return stagesResult;

        const auto& stagesArray = presetData.value()["stages"].get_ref<const nlohmann::json::array_t&>();
        for (const auto& stageItem : stagesArray)
        {
            stagesResult.push_back(
                {stageItem["temperature"].get<std::uint32_t>(),
                 static_cast<std::uint32_t>(
                     std::stoll(stageItem["stage-duration"].get<std::string>()))});
        }
        co_return stagesResult;
    }

    QCoro::Task<std::optional<SystemState>> getSystemState()
    {
        SystemState state{};
        auto telemetryDataOpt{ co_await executeGetRequest(telemetryEndpoint) };

        if (!telemetryDataOpt)
            co_return std::nullopt;
        const auto& telemetryData{ telemetryDataOpt.value() };

        state.currentTemperature = telemetryData["temperature-data"].get<std::int32_t>();
        state.surroundingTemperature = telemetryData["surrounding-temperature"].get<std::int32_t>();

        state.systemTime = telemetryData["system-time"].get<std::uint32_t>();
        state.isRunning = telemetryData["is-reflow-running"].get<bool>();

        auto presetIdOpt = telemetryData["active-preset-id"];

        if (presetIdOpt.is_null())
            co_return state;

        state.activePresetId = QString::number(presetIdOpt.get<std::uint64_t>());

        co_return state;
    }

    [[nodiscard]] QString getDeviceAddress() const
    {
        return QString::fromStdString(m_serverUrlBase);
    }

    void setDeviceAddress(const QString& deviceAddress)
    {
        m_serverUrlBase = deviceAddress.toStdString();
    }

private:
    template <typename... ExtraArgs>
    QCoro::Task<std::optional<nlohmann::json>> executeGetRequest(
        std::string_view endpoint,
        ExtraArgs&&... extraArgs)
    {
        constexpr int kRequestTimeoutMs{ 400 };

        auto request{ QNetworkRequest{getFormattedUrl(endpoint, extraArgs...)} };
        request.setTransferTimeout(kRequestTimeoutMs);

        auto* pReply =
            co_await m_nam->get(request);
        auto replyData = pReply->readAll();

        pReply->deleteLater();

        if (replyData.isEmpty())
        {
            co_return std::nullopt;
        }

        auto parsedData =
            nlohmann::json::parse(std::string_view(replyData.constData(), replyData.length()));

        co_return parsedData;
    }

    QCoro::Task<std::optional<nlohmann::json>> executePostRequest(
        std::string_view endpoint,
        std::string_view requestPayload,
        std::optional<std::string> endpointFormatExtra = std::nullopt)
    {

        auto* pReply = co_await m_nam->post(
            buildRequest(endpoint, requestPayload, endpointFormatExtra),
            QByteArray{requestPayload.data()});

        auto responseBody = pReply->readAll();
        pReply->deleteLater();
        co_return handleRequestResponse(responseBody);
    }

    QCoro::Task<std::optional<nlohmann::json>> executePutRequest(
        std::string_view endpoint,
        std::string_view requestPayload,
        std::optional<std::string> endpointFormatExtra = std::nullopt)
    {

        auto* pReply = co_await m_nam->put(
            buildRequest(endpoint, requestPayload, endpointFormatExtra),
            QByteArray{requestPayload.data()});

        auto responseBody = pReply->readAll();
        pReply->deleteLater();
        co_return handleRequestResponse(responseBody);
    }

    QNetworkRequest buildRequest(
        std::string_view endpoint,
        std::string_view requestPayload,
        std::optional<std::string> endpointFormatExtra = std::nullopt)
    {
        QNetworkRequest request{
            endpointFormatExtra
                ? QNetworkRequest{getFormattedUrl(endpoint, endpointFormatExtra.value())}
                : QNetworkRequest{getFormattedUrl(endpoint)}};

        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        return request;
    }

    std::optional<nlohmann::json> handleRequestResponse(const QByteArray& responseBody)
    {
        if (!responseBody.isEmpty())
        {
            auto parsedData = nlohmann::json::parse(
                std::string_view(responseBody.constData(), responseBody.length()));
            return parsedData;
        }
        return std::nullopt;
    }

private:
    template <typename... ExtraArgs>
    QUrl getFormattedUrl(std::string_view baseUrl, ExtraArgs&&... extraFormatArgs)
    {
        return QString::fromStdString(
            fmt::format(fmt::runtime(baseUrl), m_serverUrlBase, extraFormatArgs...));
    }

private:
    std::string m_serverUrlBase;
    std::shared_ptr<QNetworkAccessManager> m_nam;
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
    QString presetId,
    const std::vector<Stage>& stages)
{
    co_await m_pImpl->addStagesToPreset(presetId, stages);
}

QCoro::Task<> ReflowRestClient::addStagesToPreset(QString presetId, QString stages)
{
    co_await m_pImpl->addStagesToPreset(presetId, stages);
}

QCoro::Task<QVector<Stage>> ReflowRestClient::getPresetStages(QString presetId)
{
    auto presetStages = co_await m_pImpl->getPresetStages(presetId);
    co_return presetStages;
}

QCoro::Task<QVector<Preset>> ReflowRestClient::getAvailablePresets()
{
    auto presets = co_await m_pImpl->getAvailablePresets();
    co_return presets;
}

QCoro::Task<> ReflowRestClient::selectActivePreset(QString presetId)
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

QCoro::Task<std::optional<SystemState>> ReflowRestClient::getSystemState()
{
    auto systemState = co_await m_pImpl->getSystemState();
    co_return systemState;
}

[[nodiscard]] QString ReflowRestClient::getDeviceAddress() const
{
    return m_pImpl->getDeviceAddress();
}

void ReflowRestClient::setDeviceAddress(const QString& deviceAddress)
{
    return m_pImpl->setDeviceAddress(deviceAddress);
}


ReflowRestClient::ReflowRestClient()
    : m_pImpl{std::make_unique<ReflowRestClientImpl>()}
{
}

ReflowRestClient::~ReflowRestClient() = default;

} // namespace Reflow::Client
