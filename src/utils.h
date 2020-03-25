#ifndef UTILS_H
#define UTILS_H

#include <QImage>
#include <QStringList>
#include <QString>

#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/builtin_op_data.h"

namespace Utils
{

QImage rotateImage(QImage img, double rotation);
QStringList readLabels(const QString &filename);

// https://github.com/tensorflow/tensorflow/blob/master/tensorflow/lite/examples/label_image/bitmap_helpers_impl.h
template <class T>
void resize(T* out, QImage image,
            int wanted_height, int wanted_width, int wanted_channels,
            bool input_floating) {
    float input_mean = 127.5f;
    float input_std  = 127.5f;

    std::unique_ptr<tflite::Interpreter> interpreter(new tflite::Interpreter);

    int base_index = 0;

    // two inputs: input and new_sizes
    interpreter->AddTensors(2, &base_index);
    // one output
    interpreter->AddTensors(1, &base_index);
    // set input and output tensors
    interpreter->SetInputs({0, 1});
    interpreter->SetOutputs({2});

    if (image.format() != QImage::Format_RGB888)
        image = image.convertToFormat(QImage::Format_RGB888);

    // set parameters of tensors
    TfLiteQuantizationParams quant;
    interpreter->SetTensorParametersReadWrite(
                0, kTfLiteFloat32, "input",
                {1, image.height(), image.width(), 3}, quant);
    interpreter->SetTensorParametersReadWrite(1, kTfLiteInt32, "new_size", {2},
                                              quant);

    interpreter->SetTensorParametersReadWrite(
                2, kTfLiteFloat32, "output",
                {1, wanted_height, wanted_width, wanted_channels}, quant);

    tflite::ops::builtin::BuiltinOpResolver resolver;
    const TfLiteRegistration *resize_op =
            resolver.FindOp(tflite::BuiltinOperator_RESIZE_BILINEAR, 1);
    auto* params = reinterpret_cast<TfLiteResizeBilinearParams*>(
                malloc(sizeof(TfLiteResizeBilinearParams)));
    params->align_corners = false;
    interpreter->AddNodeWithParameters({0, 1}, {2}, nullptr, 0, params, resize_op,
                                       nullptr);

    interpreter->AllocateTensors();

    // We must copy lines one by one because QImage may add
    // padding bytes for alignment
    float *input = interpreter->typed_tensor<float>(0);
    for (int i = 0; i < image.height(); i++) {
        const uchar *line = image.constScanLine(i);
        for (int j = 0; j < image.width() * 3; j++)
            *input++ = line[j];
    }

    // fill new_sizes
    interpreter->typed_tensor<int>(1)[0] = wanted_height;
    interpreter->typed_tensor<int>(1)[1] = wanted_width;

    interpreter->Invoke();

    auto output = interpreter->typed_tensor<float>(2);
    auto output_number_of_pixels = wanted_height * wanted_height * wanted_channels;

    for (int i = 0; i < output_number_of_pixels; i++) {
        if (input_floating)
            out[i] = (output[i] - input_mean) / input_std;
        else
            out[i] = (uint8_t)output[i];
    }
//    QImage img(wanted_width, wanted_height, QImage::Format_RGB888);
//    uint8_t iOut[output_number_of_pixels];
//    for (int i = 0; i < output_number_of_pixels; i++)
//        iOut[i] = (uint8_t)output[i];
//    memcpy(img.bits(), iOut, output_number_of_pixels);
//    img.save(System::dataRoot() + "/test.jpg");
}

};

#endif // UTILS_H
