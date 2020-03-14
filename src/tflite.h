#ifndef TFLITE_H
#define TFLITE_H

#include <QObject>
#include <QElapsedTimer>
#include <QDebug>
#include <QFile>

#include "tensorflow/lite/error_reporter.h"
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/model.h"
#include "tensorflow/lite/kernels/register.h"

class TFLiteBase : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString modelFile MEMBER m_modelFile NOTIFY modelFileChanged)
    Q_PROPERTY(int threads MEMBER m_threads NOTIFY threadsChanged)
    Q_PROPERTY(bool acceleration MEMBER m_acceleration NOTIFY accelerationChanged)
    Q_PROPERTY(int inferenceTime READ inferenceTime NOTIFY inferenceTimeChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)

public:
    explicit TFLiteBase(QObject *parent = nullptr);

    virtual bool customInitStep() = 0;

    void queueInit();

    int inferenceTime() const;
    void setInferenceTime(int time);

    QString errorString() const;
    void setErrorString(const QString &errorString);

Q_SIGNALS:
    void modelFileChanged(QString modelFile);
    void threadsChanged(int threads);
    void accelerationChanged(bool acceleration);
    void inferenceTimeChanged(int time);
    void errorStringChanged(QString errorString);

protected:
    bool initialize();

    std::unique_ptr<tflite::Interpreter> m_interpreter;
    std::unique_ptr<tflite::FlatBufferModel> m_model;
    tflite::ops::builtin::BuiltinOpResolver m_resolver;
    tflite::StderrReporter m_error;

private:
    Q_DISABLE_COPY(TFLiteBase)

    std::atomic<bool> m_initialized;

    QString m_modelFile;
    int m_threads;
    bool m_acceleration;
    int m_inferenceTime;
    QString m_errorString;
};

template <typename T>
class TFLite : public TFLiteBase
{
public:
    explicit TFLite(QObject *parent = nullptr) : TFLiteBase(parent) {}

    virtual bool preProcessing(const T &input) = 0;
    virtual void postProcessing() = 0;

    bool run(const T &input) {
        if (!initialize())
            return false;

        if (!preProcessing(input))
            return false;

        QElapsedTimer timer;
        timer.start();

        if (m_interpreter->Invoke() != kTfLiteOk) {
            qWarning() << "Failed to invoke interpreter";
            return false;
        }

        setInferenceTime(timer.elapsed());

        postProcessing();

        return true;
    }
};

#endif // TFLITE_H
