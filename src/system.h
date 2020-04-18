#ifndef SYSTEM_H
#define SYSTEM_H

#include <QObject>

class QQmlEngine;
class QJSEngine;

class System : public QObject
{
    Q_OBJECT

public:
    static System *instance();
    static QObject *singletonProvider(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    bool checkDirs() const;

    Q_INVOKABLE static QString dataRoot();
    Q_INVOKABLE static QString genericPath();
    Q_INVOKABLE static QString language();
    Q_INVOKABLE static QString locale();
    Q_INVOKABLE static QStringList translations();

#ifdef Q_OS_ANDROID
    Q_INVOKABLE bool setScreenOrientation(int orientation);
    Q_INVOKABLE void keepScreenOn(bool on);
#endif

private:
    explicit System(QObject *parent = nullptr);
    Q_DISABLE_COPY(System)
};

#endif // SYSTEM_H
