#include "RTNeuralLSTM.h"

using Vec2d = std::vector<std::vector<float>>;

Vec2d transpose(const Vec2d& x)
{
    auto outer_size = x.size();
    auto inner_size = x[0].size();
    Vec2d y(inner_size, std::vector<float>(outer_size, 0.0f));

    for (size_t i = 0; i < outer_size; ++i)
    {
        for (size_t j = 0; j < inner_size; ++j)
            y[j][i] = x[i][j];
    }

    return y;
}


void RT_LSTM::load_json3(const char* filename)
{
    auto& lstm = model_cond2.get<0>();
    auto& dense = model_cond2.get<1>();

    // read a JSON file
    std::ifstream i2(filename);
    nlohmann::json weights_json;
    i2 >> weights_json;

    Vec2d lstm_weights_ih = weights_json["/state_dict/rec.weight_ih_l0"_json_pointer];
    lstm.setWVals(transpose(lstm_weights_ih));

    Vec2d lstm_weights_hh = weights_json["/state_dict/rec.weight_hh_l0"_json_pointer];
    lstm.setUVals(transpose(lstm_weights_hh));

    std::vector<float> lstm_bias_ih = weights_json["/state_dict/rec.bias_ih_l0"_json_pointer];
    std::vector<float> lstm_bias_hh = weights_json["/state_dict/rec.bias_hh_l0"_json_pointer];
    for (int i = 0; i < 80; ++i)
        lstm_bias_hh[i] += lstm_bias_ih[i];
    lstm.setBVals(lstm_bias_hh);

    Vec2d dense_weights = weights_json["/state_dict/lin.weight"_json_pointer];
    dense.setWeights(dense_weights);

    std::vector<float> dense_bias = weights_json["/state_dict/lin.bias"_json_pointer];
    dense.setBias(dense_bias.data());
}

void RT_LSTM::reset()
{
    model_cond2.reset();
}

void RT_LSTM::process(const float* inData, float param1, float param2, float* outData, int numSamples)
{
    for (int i = 0; i < numSamples; ++i) {
        inArray[0] = inData[i];
        inArray[1] = param1;
	inArray[2] = param2;
        outData[i] = model_cond1.forward(inArray) + inData[i];
    }
}
