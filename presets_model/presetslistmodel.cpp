#include "presetslistmodel.h"
#include <QQmlEngine>
#include <reflow_rest_client/reflow_client.hpp>

class PresetsListModel::PresetsListModelPrivate

{
public:
    PresetsListModelPrivate(
        PresetsListModel* model,
        std::shared_ptr<Reflow::Client::ReflowRestClient> restClient)
        : q_ptr{model}, m_restClient{restClient}, m_isLoaded{false}
    {
    }

public:
    int getRowsCount() const
    {
        return m_presetsStorage.size();
    }

    bool isLoaded() const
    {
        return m_isLoaded;
    }

    QCoro::Task<> refresh()
    {
        co_await loadPresetsData();
    }
    QCoro::Task<> loadPresetsData()
    {
        q_ptr->beginResetModel();
        m_presetsStorage = co_await m_restClient->getAvailablePresets();
        m_isLoaded = true;
        q_ptr->endResetModel();
    }

    QCoro::Task<> addReflowPreset(QString presetTitle)
    {
        co_await m_restClient->createNewPreset(presetTitle);
        co_await loadPresetsData();
    }

    QCoro::Task<> getPresetStages(QString presetId)
    {
        auto presetItems = co_await m_restClient->getPresetStages(presetId);
        emit q_ptr->presetStagesReady(presetItems);
    }

    std::optional<Reflow::Client::Preset> getPresetByIndex(int presetIndex)
    {
        return presetIndex >= m_presetsStorage.size()
                   ? std::nullopt
                   : std::make_optional(m_presetsStorage.at(presetIndex));
    }

private:
    bool m_isLoaded;
    PresetsListModel* const q_ptr;
    std::shared_ptr<Reflow::Client::ReflowRestClient> m_restClient;
    QVector<Reflow::Client::Preset> m_presetsStorage;
};

PresetsListModel::PresetsListModel(
    std::shared_ptr<Reflow::Client::ReflowRestClient> restClient,
    QObject* parent)
    : QAbstractListModel(parent)
    , m_pImpl{std::make_unique<PresetsListModelPrivate>(this, restClient)}
{
}

PresetsListModel::~PresetsListModel() = default;

void PresetsListModel::addReflowPreset(QString presetTitle)
{
    m_pImpl->addReflowPreset(presetTitle);
}

int PresetsListModel::rowCount(const QModelIndex& parent) const
{
    return itemsCount();
}

void PresetsListModel::registerQmlType()
{
    qmlRegisterUncreatableType<PresetsListModel>(
        "app.presets_model", 1, 0, "PresetsModel", "interface");
    qRegisterMetaType<PresetsListModel*>("PresetsListModel*");
}

QVariant PresetsListModel::data(const QModelIndex& index, int role) const
{
    const bool isIndexInvalid = !index.isValid() || index.row() < 0 || index.row() >= rowCount();

    if (isIndexInvalid)
        return QVariant();

    const auto& item = m_pImpl->getPresetByIndex(index.row());
    if (!item.has_value())
        return QVariant();

    switch (role)
    {
    case PresetId:
        return item->presetId;
        break;
    case PresetName:
        return item->presetName;
        break;

    default:
        return QVariant{};
        break;
    }

    return QVariant{};
}

Reflow::Client::Preset PresetsListModel::at(int index)
{
    const bool isIndexValid = index >= 0 && index < m_pImpl->getRowsCount();
    if (!isIndexValid)
        return {};

    return m_pImpl->getPresetByIndex(index).value();
}

void PresetsListModel::refresh()
{
    m_pImpl->refresh();
}

void PresetsListModel::sheduleGetPresetStages(QString presetId)
{
    m_pImpl->getPresetStages(presetId);
}
PresetsListModel::TRoleNames PresetsListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[PresetId] = "presetId";
    roles[PresetName] = "presetName";
    return roles;
}

int PresetsListModel::itemsCount() const
{
    if (!m_pImpl->isLoaded())
    {
        m_pImpl->loadPresetsData();
        return 0;
    }

    return m_pImpl->getRowsCount();
}
