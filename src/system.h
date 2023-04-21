#ifndef SYSTEM_H
#define SYSTEM_H

#include <QObject>

#include <QtQml/qqmlregistration.h>

class QQmlEngine;
class QJSEngine;

class System : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    inline static System *instance;
    static void init() { instance = new System(); }
    static System *create(QQmlEngine *, QJSEngine *) { return instance; }

    static bool checkDirs();

    Q_INVOKABLE static QString dataRoot();
    Q_INVOKABLE static QString language();
    Q_INVOKABLE static QString locale();
    Q_INVOKABLE static QStringList translations();

#ifdef Q_OS_ANDROID
    Q_INVOKABLE static void keepScreenOn(bool on);
#endif

private:
    explicit System(QObject *parent = nullptr);
    Q_DISABLE_COPY_MOVE(System)
};

#endif // SYSTEM_H
