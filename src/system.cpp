#include "system.h"

#include <QStandardPaths>
#include <QLocale>
#include <QQmlEngine>

System::System(QObject *parent) : QObject(parent)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

System *System::instance()
{
    static System instance;

    return &instance;
}

QObject *System::singletonProvider(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    Q_UNUSED(qmlEngine)
    Q_UNUSED(jsEngine)

    return instance();
}

QString System::dataRoot()
{
    QStringList paths = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
#ifdef Q_OS_ANDROID
    if (paths.size() > 1)
        return paths[1];
#endif
    if (paths.size() > 0)
        return paths[0];
    return "";
}

QString System::genericPath()
{
    QStringList paths = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
    if (paths.size() > 0)
#ifdef Q_OS_ANDROID
        return "file://" + paths[0];
#else
        return paths[0];
#endif
    return "";
}

QString System::language()
{
    return QLocale().name().left(2);
}

QString System::locale()
{
    return QLocale().name();
}

QStringList System::translations()
{
    QStringList translations({ "en" });
    translations.append(QString(AVAILABLE_TRANSLATIONS).split(' '));
    translations.sort();
    return translations;
}
