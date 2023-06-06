#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QTranslator>
#include <QDebug>

#include <memory>

#include <BaseUI/core.h>

#include "settings.h"
#include "system.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("TFLite examples");

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine *engine = new QQmlApplicationEngine();

    System::init(engine);
    Settings::init(engine);

    BaseUI::init(engine);

    qDebug() << "Available translations:" << System::translations();
    std::unique_ptr<QTranslator> translator;
    QObject::connect(Settings::instance, &Settings::languageChanged,
                     [engine, &translator](QString language) {
        if (translator) {
            QCoreApplication::removeTranslator(translator.get());
            translator.reset();
        }
        if (language != "en") {
            translator = std::make_unique<QTranslator>();
            if (translator->load(QLocale(language), "tflite-examples", "_", ":/i18n"))
                QCoreApplication::installTranslator(translator.get());
        }
        engine->retranslate();
    });

    Settings::instance->readSettingsFile();

    QUrl url("qrc:/qml/main.qml");
    QObject::connect(engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine->load(url);

    QObject::connect(&app, &QGuiApplication::applicationStateChanged,
                     [](Qt::ApplicationState state) {
        if (state == Qt::ApplicationSuspended) {
            Settings::instance->writeSettingsFile();
        }
    });

    int ret = app.exec();

    delete engine;

    return ret;
}
