#ifndef APPDATA_H
#define APPDATA_H

#include <QObject>

class QQmlEngine;
class QJSEngine;

class AppData : public QObject
{
    Q_OBJECT

public:
    static AppData *instance();
    static QObject *singletonProvider(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    bool checkDirs() const;

#ifdef Q_OS_ANDROID
    bool setScreenOrientation(int orientation);
    void keepScreenOn(bool on);
#endif

private:
    explicit AppData(QObject *parent = nullptr);
    Q_DISABLE_COPY(AppData)
};

#endif // APPDATA_H
