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

    Q_INVOKABLE static QString dataRoot();
    Q_INVOKABLE static QString genericPath();
    Q_INVOKABLE static QString language();
    Q_INVOKABLE static QString locale();
    Q_INVOKABLE static QStringList translations();

private:
    explicit System(QObject *parent = nullptr);
    Q_DISABLE_COPY(System)
};

#endif // SYSTEM_H
