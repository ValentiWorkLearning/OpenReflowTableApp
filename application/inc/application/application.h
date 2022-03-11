#ifndef APPLICATION_H
#define APPLICATION_H

#include <QQmlApplicationEngine>
#include <memory>
#include <optional>
#include <tl/expected.hpp>
#include <variant>

namespace AppSetup
{

class App
{

public:
    static App& Instance();

public:
    void registerTypes();

    void addContextProperties(QQmlApplicationEngine& _qmlEngine);

    void initAppHandlers();

    void cleanupApplication();

private:
    App();
    ~App();

private:
    class AppImpl;
    std::unique_ptr<AppImpl> m_pAppImpl;
};

} // namespace AppSetup
#endif // APPLICATION_H
