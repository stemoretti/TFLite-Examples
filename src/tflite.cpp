#include "tflite.h"

#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QThread>

#ifndef Q_OS_ANDROID
#include <QUrl>
#endif

#ifdef Q_OS_ANDROID
#include "tensorflow/lite/delegates/nnapi/nnapi_delegate.h"
#endif

using namespace tflite;

TFLiteBase::TFLiteBase(QObject *parent)
    : QObject(parent)
    , m_initialized(false)
    , m_threads(QThread::idealThreadCount())
    , m_acceleration(false)
    , m_inferenceTime(0)
{
    connect(this, &TFLiteBase::modelFileChanged, this, &TFLiteBase::queueInit);
    connect(this, &TFLiteBase::threadsChanged, this, &TFLiteBase::queueInit);
    connect(this, &TFLiteBase::accelerationChanged, this, &TFLiteBase::queueInit);
}

void TFLiteBase::queueInit()
{
    m_initialized = false;
}

bool TFLiteBase::initialize()
{
    if (m_initialized)
        return true;

    if (m_modelFile.trimmed().isEmpty()) {
        setErrorString("TensorFlow model filename empty");
        return false;
    }

    QFile file;
#ifdef Q_OS_ANDROID
    file.setFileName(m_modelFile);
#else
    file.setFileName(QUrl(m_modelFile).toLocalFile());
#endif
    if (!file.exists()) {
        qWarning() << "Model file doesn't exist:" << m_modelFile;
        return false;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file:" << m_modelFile;
        return false;
    }
    m_modelBuffer = file.readAll();
    m_model = FlatBufferModel::BuildFromBuffer(m_modelBuffer, m_modelBuffer.size(), &m_error);

    if (m_model == nullptr) {
        setErrorString("TensorFlow model not valid");
        return false;
    }

    InterpreterBuilder builder(*m_model, m_resolver);

    if (builder(&m_interpreter) != kTfLiteOk) {
        setErrorString("Interpreter builder failed");
        return false;
    }

#ifdef Q_OS_ANDROID
    m_interpreter->ModifyGraphWithDelegate(std::make_unique<StatefulNnApiDelegate>());
    qDebug() << "NNAPI:" << m_acceleration;
#endif

    if (m_threads > 1)
        m_interpreter->SetNumThreads(m_threads);
    qDebug() << "Num. Threads:" << m_threads;

    if (m_interpreter->AllocateTensors() != kTfLiteOk) {
        setErrorString("Allocate tensors failed");
        return false;
    }

    if (!customInitStep())
        return false;

    setErrorString(QString());

    m_initialized = true;

    qDebug() << "Tensorflow initialization: OK";

    return true;
}

int TFLiteBase::inferenceTime() const
{
    return m_inferenceTime;
}

void TFLiteBase::setInferenceTime(int time)
{
    if (m_inferenceTime == time)
        return;

    m_inferenceTime = time;
    Q_EMIT inferenceTimeChanged(time);
}

QString TFLiteBase::errorString() const
{
    return m_errorString;
}

void TFLiteBase::setErrorString(const QString &errorString)
{
    if (m_errorString == errorString)
        return;

    m_errorString = errorString;
    Q_EMIT errorStringChanged(errorString);
    if (!m_errorString.isEmpty())
        qWarning() << m_errorString;
}
