#include <reflow_rest_client/reflow_client.hpp>

#include <qcoro/network/qcoronetworkreply.h>
#include <qcoro/task.h>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

static const QUrl pingPongUrlEndpoint =
    QUrl{QStringLiteral("http://localhost:8086/api/v1/ReflowController/ping-pong")};
static const QUrl createPresetEndpoint =
    QUrl{QStringLiteral("http://localhost:8086/api/v1/ReflowController/preset")};

namespace Reflow::Client
{
class ReflowRestClient::ReflowRestClientImpl
{
public:
    QCoro::Task<> testPingPongConnection()
    {
        auto* pReply = co_await m_nam.get(QNetworkRequest{pingPongUrlEndpoint});
        pReply->deleteLater();
    }

    QCoro::Task<> createNewPreset(const QString& presetName)
    {
        nlohmann::json payload;
        payload["preset-name"] = presetName.toStdString();
        std::string resultJson = payload.dump();
        auto* pReply = co_await m_nam.post(
            QNetworkRequest{createPresetEndpoint}, QByteArray{resultJson.c_str()});

        auto presetId = pReply->readAll();
        spdlog::info("Got preset id: {}", presetId);
        pReply->deleteLater();
    }

private:
    QNetworkAccessManager m_nam;
};

QCoro::Task<> ReflowRestClient::testPingPongConnection()
{
    co_await m_pImpl->testPingPongConnection();
}

QCoro::Task<> ReflowRestClient::createNewPreset(const QString& presetName)
{
    co_await m_pImpl->createNewPreset(presetName);
}

ReflowRestClient::ReflowRestClient() : m_pImpl{std::make_unique<ReflowRestClientImpl>()}
{
}
ReflowRestClient::~ReflowRestClient() = default;

} // namespace Reflow::Client
