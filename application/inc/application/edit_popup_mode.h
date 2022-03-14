#pragma once

#include <QObject>
#include <QQmlEngine>

namespace EditPopupModeNs
{
Q_NAMESPACE

enum Value
{
    ADD_NEW,
    EDIT_EXISING
};
QML_NAMED_ELEMENT(editPopupMode)

static void registerEditPopupModeEnum()
{
    qmlRegisterUncreatableMetaObject(
        EditPopupModeNs::staticMetaObject,
        "app.edit_popup.mode",
        1,
        0,
        "EditPopupMode",
        "Error: only enums");
    qRegisterMetaType<EditPopupModeNs::Value>("EditPopupMode");
}
Q_ENUM_NS(Value)

}; // namespace EditPopupModeNs
