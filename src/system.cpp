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
#include <QCoreApplication>
#include <QJniObject>
#endif

System::System(QObject *parent) : QObject(parent)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

#ifdef Q_OS_ANDROID
    // QJniObject activity = QNativeInterface::QAndroidApplication::context();
    // activity.callMethod<void>("checkPermissions", "()V");
    keepScreenOn(true);
#endif
}

bool System::checkDirs()
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
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
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
    QDir translationsDir(":/i18n");
    QStringList languages({ "en" });

    if (translationsDir.exists()) {
        QStringList translations = translationsDir.entryList({ "*.qm" });
        translations.replaceInStrings(QRegularExpression("[^_]+_(\\w+)\\.qm"), "\\1");
        languages.append(translations);
        languages.sort();
    }

    return languages;
}

#ifdef Q_OS_ANDROID
void System::keepScreenOn(bool on)
{
    static const int FLAG_KEEP_SCREEN_ON
        = QJniObject::getStaticField<jint>("android/view/WindowManager$LayoutParams",
                                           "FLAG_KEEP_SCREEN_ON");

    QNativeInterface::QAndroidApplication::runOnAndroidMainThread([on]() {
        QJniObject activity = QNativeInterface::QAndroidApplication::context();
        QJniObject window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");
        if (on)
            window.callMethod<void>("addFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
        else
            window.callMethod<void>("clearFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
    }).waitForFinished();
}
#endif
