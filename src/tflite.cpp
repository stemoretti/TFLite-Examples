#include "tflite.h"

#include <QThread>

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
    if (m_initialized.exchange(true))
        return true;

    if (m_modelFile.trimmed().isEmpty()) {
        setErrorString("TensorFlow model filename empty");
        qWarning() << errorString();
        m_initialized = false;
        return false;
    }

    m_model = FlatBufferModel::BuildFromFile(m_modelFile.toUtf8(), &m_error);

    if (m_model == nullptr) {
        setErrorString("TensorFlow model loading: ERROR");
        qWarning() << errorString();
        m_initialized = false;
        return false;
    }

    InterpreterBuilder builder(*m_model, m_resolver);

    if (builder(&m_interpreter) != kTfLiteOk) {
        setErrorString("Interpreter: ERROR");
        qWarning() << errorString();
        m_initialized = false;
        return false;
    }

    m_interpreter->UseNNAPI(m_acceleration);
    qDebug() << "NNAPI:" << m_acceleration;

    if (m_threads > 1)
        m_interpreter->SetNumThreads(m_threads);
    qDebug() << "Num. Threads:" << m_threads;

    if (m_interpreter->AllocateTensors() != kTfLiteOk) {
        setErrorString("Allocate tensors: ERROR");
        qWarning() << errorString();
        m_initialized = false;
        return false;
    }

    if (!customInitStep()) {
        setErrorString("Custom init step failed");
        qWarning() << errorString();
        m_initialized = false;
        return false;
    }

    setErrorString(QString());

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
    emit inferenceTimeChanged(m_inferenceTime);
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
    emit errorStringChanged(m_errorString);
}
