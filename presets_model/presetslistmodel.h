#ifndef PRESETSLISTMODEL_H
#define PRESETSLISTMODEL_H

#include <QAbstractListModel>

#include <QVector>
#include <QVariant>

#include<memory>
#include <reflow_rest_client/reflow_client.hpp>

class PresetsListModel : public QAbstractListModel
{
    Q_OBJECT

public:

    enum ChecklistModelRoles
       {
           PresetId = Qt::UserRole,
           PresetName
       };

    explicit PresetsListModel(std::shared_ptr<Reflow::Client::ReflowRestClient> restClient, QObject *parent = nullptr);
    ~PresetsListModel();
public:

    Q_INVOKABLE void addReflowPreset(QString presetTitle);
    Q_INVOKABLE Reflow::Client::Preset at(int index);
    Q_INVOKABLE void sheduleGetPresetStages(QString presetId);
    Q_INVOKABLE int itemsCount()const;
    Q_INVOKABLE void updatePresetStages(QString presetId,QString stagesList);
    void refresh();

public:
signals:
    void presetStagesReady(QVector<Reflow::Client::Stage> stages);

public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    static void registerQmlType();

    using TRoleNames = QHash<int, QByteArray>;
    TRoleNames roleNames() const override;

private:
    class PresetsListModelPrivate;
    std::unique_ptr<PresetsListModelPrivate> m_pImpl;
};



#endif // PRESETSLISTMODEL_H
