#include "appdata.h"

#include <QDir>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QQmlEngine>
#include <QDebug>

#include "system.h"

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#include <QtAndroid>

static void checkPermissions(QStringList permissions)
{
    if (QtAndroid::androidSdkVersion() < 23)
        return;

    std::remove_if(permissions.begin(), permissions.end(), [](QString perm) {
        if (QtAndroid::checkPermission(perm) == QtAndroid::PermissionResult::Granted)
            return true;
        return false;
    });

    if (permissions.size() > 0) {
        QtAndroid::requestPermissions(
                    permissions,
                    [](QHash<QString, QtAndroid::PermissionResult> result) {
            for (const auto perm : result.keys()) {
                if (result.value(perm) == QtAndroid::PermissionResult::Granted)
                    qDebug() << "Permission" << perm << "Granted";
                else
                    qDebug() << "Permission" << perm << "Denied";
            }
        });
    }
}
#endif

AppData::AppData(QObject *parent)
    : QObject(parent)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (!checkDirs())
        qFatal("App won't work - cannot create data directory.");

#ifdef Q_OS_ANDROID
    checkPermissions({ "android.permission.READ_EXTERNAL_STORAGE" });
//    setScreenOrientation(1);
    keepScreenOn(true);
#endif
}

AppData *AppData::instance()
{
    static AppData instance;

    return &instance;
}

QObject *AppData::singletonProvider(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    Q_UNUSED(qmlEngine)
    Q_UNUSED(jsEngine)

    return instance();
}

bool AppData::checkDirs() const
{
    QDir myDir;
    QString path = System::dataRoot();

    if (!myDir.exists(path)) {
        if (!myDir.mkpath(path)) {
            qWarning() << "Cannot create" << path;
            return false;
        }
        qDebug() << "Created directory" << path;
    }

    return true;
}

#ifdef Q_OS_ANDROID
bool AppData::setScreenOrientation(int orientation)
{
    QAndroidJniObject activity = QtAndroid::androidActivity();

    if (activity.isValid()) {
        activity.callMethod<void>("setRequestedOrientation", "(I)V", orientation);
        return true;
    }

    return false;
}

void AppData::keepScreenOn(bool on)
{
    static const int FLAG_KEEP_SCREEN_ON
        = QAndroidJniObject::getStaticField<jint>("android/view/WindowManager$LayoutParams",
                                                  "FLAG_KEEP_SCREEN_ON");
    QtAndroid::runOnAndroidThread([on]() {
        QAndroidJniObject window
            = QtAndroid::androidActivity().callObjectMethod("getWindow",
                                                            "()Landroid/view/Window;");
        if (on)
            window.callMethod<void>("addFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
        else
            window.callMethod<void>("clearFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
    });
}
#endif
