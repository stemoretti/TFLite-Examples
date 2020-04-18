#include "system.h"

#include <QDir>
#include <QStandardPaths>
#include <QLocale>
#include <QQmlEngine>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

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

System::System(QObject *parent) : QObject(parent)
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

bool System::checkDirs() const
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

#ifdef Q_OS_ANDROID
bool System::setScreenOrientation(int orientation)
{
    QAndroidJniObject activity = QtAndroid::androidActivity();

    if (activity.isValid()) {
        activity.callMethod<void>("setRequestedOrientation", "(I)V", orientation);
        return true;
    }

    return false;
}

void System::keepScreenOn(bool on)
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
