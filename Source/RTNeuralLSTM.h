#pragma once

#include <RTNeural/RTNeural.h>

class RT_LSTM
{
public:
    RT_LSTM() = default;

    void reset();
    //void load_json(const char* filename);
    void load_json3(const char* filename);

    //void process(const float* inData, float* outData, int numSamples);
    void process(const float* inData, float param1, fload param2, float* outData, int numSamples);

    int input_size = 3;
    
private:
    RTNeural::ModelT<float, 3, 1,
        RTNeural::LSTMLayerT<float, 3, 20>,
        RTNeural::DenseT<float, 20, 1>> model_cond2;

    float inArray[2] = { 0.0, 0.0, 0.0 };
};
