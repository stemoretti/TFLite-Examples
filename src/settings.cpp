#include "settings.h"

#include <QDebug>
#include <QStringList>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>
#include <QQmlEngine>

#include "system.h"

Settings::Settings(QObject *parent)
    : QObject(parent)
    , m_settingsFilePath(System::dataRoot() + "/settings.json")
    , m_darkTheme(false)
    , m_primaryColor("#607D8B") // Material.BlueGrey
    , m_accentColor("#FF9800") // Material.Orange
    , m_language("en")
    , m_country("en_US")
    , m_resolution("640x480")
    , m_showTime(true)
    , m_nnapi(false)
    , m_threads(QThread::idealThreadCount())
    , m_confidence(0.6)
    , m_threshold(0.001)
    , m_score(0.5)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

Settings::~Settings()
{
    writeSettingsFile();
}

Settings *Settings::instance()
{
    static Settings instance;

    return &instance;
}

QObject *Settings::singletonProvider(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    Q_UNUSED(qmlEngine)
    Q_UNUSED(jsEngine)

    return instance();
}

void Settings::readSettingsFile()
{
    qDebug() << "Read the settings file";

    QFile readFile(m_settingsFilePath);

    if (!readFile.exists()) {
        qWarning() << "Cannot find the settings file:" << m_settingsFilePath;
        qDebug() << "Using default settings values";
        setCountry(System::locale());
        if (System::translations().contains(System::language()))
            setLanguage(System::language());
        return;
    }
    if (!readFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file:" << m_settingsFilePath;
        return;
    }
    auto jdoc = QJsonDocument::fromJson(readFile.readAll());
    readFile.close();
    if (!jdoc.isObject()) {
        qWarning() << "Cannot read JSON file:" << m_settingsFilePath;
        return;
    }
    QJsonObject jobj = jdoc.object();
    setDarkTheme(jobj["darkTheme"].toBool());
    setPrimaryColor(jobj["primaryColor"].toString());
    setAccentColor(jobj["accentColor"].toString());
    setLanguage(jobj["language"].toString());
    setCountry(jobj["country"].toString());
    setResolution(jobj["resolution"].toString());
    setShowTime(jobj["showTime"].toBool());
    setNnapi(jobj["nnapi"].toBool());
    setThreads(jobj["threads"].toInt());
    setObjectsModel(jobj["objectsModel"].toString());
    setObjectsLabels(jobj["objectsLabels"].toString());
    setConfidence(jobj["confidence"].toString().toFloat());
    setClassifierModel(jobj["classifierModel"].toString());
    setClassifierLabels(jobj["classifierLabels"].toString());
    setThreshold(jobj["threshold"].toString().toFloat());
    setPoseModel(jobj["poseModel"].toString());
    setScore(jobj["score"].toString().toFloat());

    qDebug() << "Settings file read";
}

void Settings::writeSettingsFile() const
{
    qDebug() << "Write the settings file";

    QFile writeFile(m_settingsFilePath);

    if (!writeFile.open(QIODevice::WriteOnly)) {
        qWarning() << "Cannot open file for writing:" << m_settingsFilePath;
        return;
    }
    QJsonObject jobj;
    jobj["darkTheme"] = m_darkTheme;
    jobj["primaryColor"] = m_primaryColor.name(QColor::HexRgb);
    jobj["accentColor"] = m_accentColor.name(QColor::HexRgb);
    jobj["language"] = m_language;
    jobj["country"] = m_country;
    jobj["resolution"] = m_resolution;
    jobj["showTime"] = m_showTime;
    jobj["nnapi"] = m_nnapi;
    jobj["threads"] = m_threads;
    jobj["objectsModel"] = m_objectsModel;
    jobj["objectsLabels"] = m_objectsLabels;
    jobj["confidence"] = QString::number(m_confidence, 'f', 2);
    jobj["classifierModel"] = m_classifierModel;
    jobj["classifierLabels"] = m_classifierLabels;
    jobj["threshold"] = QString::number(m_threshold, 'f', 3);
    jobj["poseModel"] = m_poseModel;
    jobj["score"] = QString::number(m_score, 'f', 2);
    writeFile.write(QJsonDocument(jobj).toJson());
    writeFile.close();

    qDebug() << "Settings file saved";
}

//{{{ Properties getters/setters definitions

bool Settings::darkTheme() const
{
    return m_darkTheme;
}

void Settings::setDarkTheme(bool darkTheme)
{
    if (m_darkTheme == darkTheme)
        return;

    m_darkTheme = darkTheme;
    emit darkThemeChanged(m_darkTheme);
}

QColor Settings::primaryColor() const
{
    return m_primaryColor;
}

void Settings::setPrimaryColor(const QColor &primaryColor)
{
    if (m_primaryColor == primaryColor)
        return;

    m_primaryColor = primaryColor;
    emit primaryColorChanged(m_primaryColor);
}

QColor Settings::accentColor() const
{
    return m_accentColor;
}

void Settings::setAccentColor(const QColor &accentColor)
{
    if (m_accentColor == accentColor)
        return;

    m_accentColor = accentColor;
    emit accentColorChanged(m_accentColor);
}

QString Settings::language() const
{
    return m_language;
}

void Settings::setLanguage(const QString &language)
{
    if (m_language == language)
        return;

    m_language = language;
    emit languageChanged(m_language);
}

QString Settings::country() const
{
    return m_country;
}

void Settings::setCountry(const QString &country)
{
    if (m_country == country)
        return;

    m_country = country;
    emit countryChanged(m_country);
}

QString Settings::resolution() const
{
    return m_resolution;
}

void Settings::setResolution(const QString &resolution)
{
    if (m_resolution == resolution)
        return;

    m_resolution = resolution;
    emit resolutionChanged(m_resolution);
}

bool Settings::showTime() const
{
    return m_showTime;
}

void Settings::setShowTime(bool showTime)
{
    if (m_showTime == showTime)
        return;

    m_showTime = showTime;
    emit showTimeChanged(m_showTime);
}

bool Settings::nnapi() const
{
    return m_nnapi;
}

void Settings::setNnapi(bool nnapi)
{
    if (m_nnapi == nnapi)
        return;

    m_nnapi = nnapi;
    emit nnapiChanged(m_nnapi);
}

int Settings::threads() const
{
    return m_threads;
}

void Settings::setThreads(int threads)
{
    if (m_threads == threads)
        return;

    m_threads = threads;
    emit threadsChanged(m_threads);
}

QString Settings::objectsModel() const
{
    return m_objectsModel;
}

void Settings::setObjectsModel(const QString &objectsModel)
{
    if (m_objectsModel == objectsModel)
        return;

    m_objectsModel = objectsModel;
    emit objectsModelChanged(m_objectsModel);
}

QString Settings::objectsLabels() const
{
    return m_objectsLabels;
}

void Settings::setObjectsLabels(const QString &objectsLabels)
{
    if (m_objectsLabels == objectsLabels)
        return;

    m_objectsLabels = objectsLabels;
    emit objectsLabelsChanged(m_objectsLabels);
}

float Settings::confidence() const
{
    return m_confidence;
}

void Settings::setConfidence(float confidence)
{
    if (qFuzzyCompare(m_confidence, confidence))
        return;

    m_confidence = confidence;
    emit confidenceChanged(m_confidence);
}

QString Settings::classifierModel() const
{
    return m_classifierModel;
}

void Settings::setClassifierModel(const QString &classifierModel)
{
    if (m_classifierModel == classifierModel)
        return;

    m_classifierModel = classifierModel;
    emit classifierModelChanged(m_classifierModel);
}

QString Settings::classifierLabels() const
{
    return m_classifierLabels;
}

void Settings::setClassifierLabels(const QString &classifierLabels)
{
    if (m_classifierLabels == classifierLabels)
        return;

    m_classifierLabels = classifierLabels;
    emit classifierLabelsChanged(m_classifierLabels);
}

float Settings::threshold() const
{
    return m_threshold;
}

void Settings::setThreshold(float threshold)
{
    if (qFuzzyCompare(m_threshold, threshold))
        return;

    m_threshold = threshold;
    emit thresholdChanged(m_threshold);
}

QString Settings::poseModel() const
{
    return m_poseModel;
}

void Settings::setPoseModel(const QString &poseModel)
{
    if (m_poseModel == poseModel)
        return;

    m_poseModel = poseModel;
    emit poseModelChanged(m_poseModel);
}

float Settings::score() const
{
    return m_score;
}

void Settings::setScore(float score)
{
    if (qFuzzyCompare(m_score, score))
        return;

    m_score = score;
    emit scoreChanged(m_score);
}

//}}} Properties getters/setters definitions
