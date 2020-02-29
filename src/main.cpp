#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QLibraryInfo>
#include <QTranslator>
#include <QDebug>
#include <QFontDatabase>

#include <memory>

#include "appdata.h"
#include "settings.h"
#include "system.h"
#include "iconprovider.h"

#include "videofilter.h"
#include "tflite.h"
#include "objectdetection.h"
#include "imageclassification.h"

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#ifdef QT_DEBUG
    qputenv("QML_DISABLE_DISK_CACHE", "true");
#endif
    QGuiApplication app(argc, argv);

    if (QFontDatabase::addApplicationFont(":/icons/MaterialIcons-Regular.ttf") == -1)
        qWarning() << "Failed to load font Material";

    QQuickStyle::setStyle("Material");

    QQmlApplicationEngine engine;

    engine.addImageProvider("icon", new IconProvider("Material Icons", ":/icons/codepoints.json"));

    qDebug() << "Available translations:" << System::translations();
    std::unique_ptr<QTranslator> translator;
    QObject::connect(Settings::instance(), &Settings::languageChanged,
                     [&engine, &translator](QString language) {
        if (translator) {
            QCoreApplication::removeTranslator(translator.get());
            translator.reset();
        }
        if (language != "en") {
            translator = std::make_unique<QTranslator>();
            if (translator->load(QLocale(language), "tflite-examples", "_", ":/translations"))
                QCoreApplication::installTranslator(translator.get());
        }
        engine.retranslate();
    });

    Settings::instance()->readSettingsFile();

    qmlRegisterSingletonType<AppData>("AppData", 1, 0, "AppData", AppData::singletonProvider);
    qmlRegisterSingletonType<Settings>("Settings", 1, 0, "Settings", Settings::singletonProvider);
    qmlRegisterSingletonType<System>("System", 1, 0, "System", System::singletonProvider);

    qmlRegisterType<VideoFilter>("VideoFilter", 1, 0, "VideoFilter");
    qmlRegisterType<TFLiteBase>();
    qmlRegisterType<ObjectDetection>("ObjectDetection", 1, 0, "ObjectDetection");
    qmlRegisterType<ImageClassification>("ImageClassification", 1, 0, "ImageClassification");

    // FIXME: Instantiate the class to run the constructor
    AppData::instance();

    engine.load(QUrl("qrc:/qml/main.qml"));

    QObject::connect(&app, &QGuiApplication::applicationStateChanged,
                     [](Qt::ApplicationState state) {
        if (state == Qt::ApplicationSuspended) {
            Settings::instance()->writeSettingsFile();
        }
    });

    return app.exec();
}
