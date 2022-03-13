#pragma once

#include <QObject>
#include <QQmlEngine>

class EditPopupMode
{
    Q_GADGET

public:
    enum Value
    {
        ADD_NEW,
        EDIT_EXISING
    };
    Q_ENUM(Value)
    using EditPopupModeEnum = EditPopupMode::Value;

public:
    static void registerEditPopupModeEnum()
    {
        qRegisterMetaType<EditPopupModeEnum>("EditPopupMode");
        qmlRegisterUncreatableType<EditPopupMode>(
            "app.edit_popup.mode", 1, 0, "EditPopupMode", "Can't create enum type holder");
    }

private:
    explicit EditPopupMode() = default;
};
