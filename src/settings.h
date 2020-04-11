#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QColor>
#include <QString>

class QQmlEngine;
class QJSEngine;

class Settings : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool darkTheme READ darkTheme WRITE setDarkTheme NOTIFY darkThemeChanged)
    Q_PROPERTY(QColor primaryColor READ primaryColor WRITE setPrimaryColor NOTIFY primaryColorChanged)
    Q_PROPERTY(QColor accentColor READ accentColor WRITE setAccentColor NOTIFY accentColorChanged)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(QString country READ country WRITE setCountry NOTIFY countryChanged)
    Q_PROPERTY(QString resolution READ resolution WRITE setResolution NOTIFY resolutionChanged)
    Q_PROPERTY(bool showTime READ showTime WRITE setShowTime NOTIFY showTimeChanged)
    Q_PROPERTY(bool nnapi READ nnapi WRITE setNnapi NOTIFY nnapiChanged)
    Q_PROPERTY(int threads READ threads WRITE setThreads NOTIFY threadsChanged)
    Q_PROPERTY(QString objectsModel READ objectsModel WRITE setObjectsModel NOTIFY objectsModelChanged)
    Q_PROPERTY(QString objectsLabels READ objectsLabels WRITE setObjectsLabels NOTIFY objectsLabelsChanged)
    Q_PROPERTY(float confidence READ confidence WRITE setConfidence NOTIFY confidenceChanged)
    Q_PROPERTY(QString classifierModel READ classifierModel WRITE setClassifierModel NOTIFY classifierModelChanged)
    Q_PROPERTY(QString classifierLabels READ classifierLabels WRITE setClassifierLabels NOTIFY classifierLabelsChanged)
    Q_PROPERTY(float threshold READ threshold WRITE setThreshold NOTIFY thresholdChanged)
    Q_PROPERTY(QString poseModel READ poseModel WRITE setPoseModel NOTIFY poseModelChanged)
    Q_PROPERTY(float score READ score WRITE setScore NOTIFY scoreChanged)

public:
    ~Settings();

    static Settings *instance();
    static QObject *singletonProvider(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    void readSettingsFile();
    void writeSettingsFile() const;

    //{{{ Properties getters/setters declarations

    bool darkTheme() const;
    void setDarkTheme(bool darkTheme);

    QColor primaryColor() const;
    void setPrimaryColor(const QColor &primaryColor);

    QColor accentColor() const;
    void setAccentColor(const QColor &accentColor);

    QString language() const;
    void setLanguage(const QString &language);

    QString country() const;
    void setCountry(const QString &country);

    QString resolution() const;
    void setResolution(const QString &resolution);

    bool showTime() const;
    void setShowTime(bool showTime);

    bool nnapi() const;
    void setNnapi(bool nnapi);

    int threads() const;
    void setThreads(int threads);

    QString objectsModel() const;
    void setObjectsModel(const QString &objectsModel);

    QString objectsLabels() const;
    void setObjectsLabels(const QString &objectsLabels);

    float confidence() const;
    void setConfidence(float confidence);

    QString classifierModel() const;
    void setClassifierModel(const QString &classifierModel);

    QString classifierLabels() const;
    void setClassifierLabels(const QString &classifierLabels);

    float threshold() const;
    void setThreshold(float threshold);

    QString poseModel() const;
    void setPoseModel(const QString &poseModel);

    float score() const;
    void setScore(float score);

    //}}} Properties getters/setters declarations

Q_SIGNALS:
    //{{{ Properties signals

    void darkThemeChanged(bool darkTheme);
    void primaryColorChanged(QColor primaryColor);
    void accentColorChanged(QColor accentColor);
    void languageChanged(QString language);
    void countryChanged(QString country);
    void resolutionChanged(QString resolution);
    void showTimeChanged(bool showTime);
    void nnapiChanged(bool nnapi);
    void threadsChanged(int threads);
    void objectsModelChanged(QString objectsModel);
    void objectsLabelsChanged(QString objectsLabels);
    void confidenceChanged(float confidence);
    void classifierModelChanged(QString classifierModel);
    void classifierLabelsChanged(QString classifierLabels);
    void thresholdChanged(float threshold);
    void poseModelChanged(QString poseModel);
    void scoreChanged(float threshold);

    //}}} Properties signals

private:
    explicit Settings(QObject *parent = nullptr);
    Q_DISABLE_COPY(Settings)

    QString m_settingsFilePath;

    //{{{ Properties declarations

    bool m_darkTheme;
    QColor m_primaryColor;
    QColor m_accentColor;
    QString m_language;
    QString m_country;
    QString m_resolution;
    bool m_showTime;
    bool m_nnapi;
    int m_threads;
    QString m_objectsModel;
    QString m_objectsLabels;
    float m_confidence;
    QString m_classifierModel;
    QString m_classifierLabels;
    float m_threshold;
    QString m_poseModel;
    float m_score;

    //}}} Properties declarations
};

#endif // SETTINGS_H
