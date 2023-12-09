// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include <vector>

// clang-format off
#ifdef ${BACKEND_NAME}_FLOAT_TOLERANCE_BITS
#define DEFAULT_FLOAT_TOLERANCE_BITS ${BACKEND_NAME}_FLOAT_TOLERANCE_BITS
#endif
#ifdef ${BACKEND_NAME}_DOUBLE_TOLERANCE_BITS
#define DEFAULT_DOUBLE_TOLERANCE_BITS ${BACKEND_NAME}_DOUBLE_TOLERANCE_BITS
#endif
// clang-format on

#include "common_test_utils/file_utils.hpp"
#include "default_opset.hpp"
#include "common_test_utils/test_case.hpp"
#include "onnx_import/onnx.hpp"
#include "onnx_utils.hpp"
#include "ngraph/file_util.hpp"
#include "common_test_utils/test_control.hpp"

OPENVINO_SUPPRESS_DEPRECATED_START

using namespace ngraph;

static std::string s_manifest = ngraph::file_util::path_join(ov::test::utils::getExecutableDirectory(), "${MANIFEST}");
static std::string s_device = backend_name_to_device("${BACKEND_NAME}");

OPENVINO_TEST(${BACKEND_NAME}, onnx_model_bias_gelu) {
    const auto function = onnx_import::import_onnx_model(file_util::path_join(ov::test::utils::getExecutableDirectory(),
                                                                              SERIALIZED_ZOO,
                                                                              "onnx/com.microsoft/bias_gelu.onnx"));

    auto test_case = ov::test::TestCase(function, s_device);
    test_case.add_input<float>({0.5488135f,
                                0.71518934f,
                                0.60276335f,
                                0.5448832f,
                                0.4236548f,
                                0.6458941f,
                                0.4375872f,
                                0.891773f,
                                0.96366274f,
                                0.3834415f});
    test_case.add_input<float>({0.79172504f, 0.5288949f, 0.56804454f, 0.92559665f, 0.07103606f});
    test_case.add_expected_output<float>({1.2198428f,
                                          1.1112978f,
                                          1.0293297f,
                                          1.366493f,
                                          0.3411342f,
                                          1.329408f,
                                          0.8051748f,
                                          1.354462f,
                                          1.8336612f,
                                          0.3068893f});
    test_case.run_with_tolerance_as_fp(1e-6f);
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_model_skip_layer_normalization_with_gamma_beta_bias) {
    const auto function = onnx_import::import_onnx_model(
        file_util::path_join(ov::test::utils::getExecutableDirectory(),
                             SERIALIZED_ZOO,
                             "onnx/com.microsoft/skip_layer_normalization_with_gamma_beta_bias.onnx"));

    std::vector<float> input = {
        0.54881352f, 0.71518934f, 0.60276335f, 0.54488319f, 0.42365479f, 0.64589411f, 0.43758720f, 0.89177299f,
        0.96366274f, 0.38344151f, 0.79172504f, 0.52889490f, 0.56804454f, 0.92559665f, 0.07103606f, 0.08712930f,
        0.02021840f, 0.83261985f, 0.77815676f, 0.87001216f, 0.97861832f, 0.79915857f, 0.46147937f, 0.78052920f,
    };
    std::vector<float> skip = {
        0.11827443f, 0.63992101f, 0.14335328f, 0.94466889f, 0.52184832f, 0.41466194f, 0.26455560f, 0.77423370f,
        0.45615032f, 0.56843394f, 0.01878980f, 0.61763549f, 0.61209571f, 0.61693400f, 0.94374806f, 0.68182027f,
        0.35950789f, 0.43703195f, 0.69763118f, 0.06022547f, 0.66676670f, 0.67063785f, 0.21038257f, 0.12892629f,
    };
    std::vector<float> expected = {
        -0.19721794f, -0.42944565f, 0.18620640f, 0.61282152f,  -0.11097327f, -0.59518522f, 0.13393641f,  0.66901535f,
        0.04256713f,  -0.71902490f, 0.23107991f, 0.17300847f,  -0.04390603f, -0.31109563f, 0.51021838f,  -0.66914201f,
        -0.20009395f, -0.43313017f, 0.67281967f, -0.01712347f, 0.09767530f,  -0.43024653f, -0.01836969f, -0.29238200f,
    };
    auto test_case = ov::test::TestCase(function, s_device);
    test_case.add_input<float>(input);
    test_case.add_input<float>(skip);
    test_case.add_expected_output<float>(expected);
    test_case.run_with_tolerance_as_fp();
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_model_skip_layer_normalization_with_gamma_beta) {
    const auto function = onnx_import::import_onnx_model(
        file_util::path_join(ov::test::utils::getExecutableDirectory(),
                             SERIALIZED_ZOO,
                             "onnx/com.microsoft/skip_layer_normalization_with_gamma_beta.onnx"));

    std::vector<float> input = {
        0.54881352f, 0.71518934f, 0.60276335f, 0.54488319f, 0.42365479f, 0.64589411f, 0.43758720f, 0.89177299f,
        0.96366274f, 0.38344151f, 0.79172504f, 0.52889490f, 0.56804454f, 0.92559665f, 0.07103606f, 0.08712930f,
        0.02021840f, 0.83261985f, 0.77815676f, 0.87001216f, 0.97861832f, 0.79915857f, 0.46147937f, 0.78052920f,
    };
    std::vector<float> skip = {
        0.11827443f, 0.63992101f, 0.14335328f, 0.94466889f, 0.52184832f, 0.41466194f, 0.26455560f, 0.77423370f,
        0.45615032f, 0.56843394f, 0.01878980f, 0.61763549f, 0.61209571f, 0.61693400f, 0.94374806f, 0.68182027f,
        0.35950789f, 0.43703195f, 0.69763118f, 0.06022547f, 0.66676670f, 0.67063785f, 0.21038257f, 0.12892629f,
    };
    std::vector<float> expected = {
        -0.17974678f, -0.23946194f, -0.04376268f, 0.46959469f,  -0.11171167f, -0.41859278f, -0.11082965f, 0.64513868f,
        0.07773457f,  -0.51403606f, -0.13661698f, 0.11262375f,  -0.05096011f, -0.10416907f, 0.10070466f,  -0.50876135f,
        -0.22290939f, -0.27663514f, 0.55416691f,  -0.08064821f, 0.04857478f,  -0.25121087f, -0.15912610f, -0.26637587f,
    };
    auto test_case = ov::test::TestCase(function, s_device);
    test_case.add_input<float>(input);
    test_case.add_input<float>(skip);
    test_case.add_expected_output<float>(expected);
    test_case.run_with_tolerance_as_fp();
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_model_skip_layer_normalization_with_gamma) {
    const auto function = onnx_import::import_onnx_model(
        file_util::path_join(ov::test::utils::getExecutableDirectory(),
                             SERIALIZED_ZOO,
                             "onnx/com.microsoft/skip_layer_normalization_with_gamma.onnx"));

    std::vector<float> input = {
        0.54881352f, 0.71518934f, 0.60276335f, 0.54488319f, 0.42365479f, 0.64589411f, 0.43758720f, 0.89177299f,
        0.96366274f, 0.38344151f, 0.79172504f, 0.52889490f, 0.56804454f, 0.92559665f, 0.07103606f, 0.08712930f,
        0.02021840f, 0.83261985f, 0.77815676f, 0.87001216f, 0.97861832f, 0.79915857f, 0.46147937f, 0.78052920f,
    };
    std::vector<float> skip = {
        0.11827443f, 0.63992101f, 0.14335328f, 0.94466889f, 0.52184832f, 0.41466194f, 0.26455560f, 0.77423370f,
        0.45615032f, 0.56843394f, 0.01878980f, 0.61763549f, 0.61209571f, 0.61693400f, 0.94374806f, 0.68182027f,
        0.35950789f, 0.43703195f, 0.69763118f, 0.06022547f, 0.66676670f, 0.67063785f, 0.21038257f, 0.12892629f,
    };
    std::vector<float> expected = {
        -0.10974677f, 0.16053806f,  -0.26376268f, 0.46959469f,  -0.04171166f, -0.01859277f, -0.33082965f, 0.64513868f,
        0.14773457f,  -0.11403608f, -0.35661697f, 0.11262375f,  0.01903989f,  0.29583094f,  -0.11929534f, -0.50876135f,
        -0.15290938f, 0.12336487f,  0.33416691f,  -0.08064821f, 0.11857478f,  0.14878914f,  -0.37912610f, -0.26637587f,
    };
    auto test_case = ov::test::TestCase(function, s_device);
    test_case.add_input<float>(input);
    test_case.add_input<float>(skip);
    test_case.add_expected_output<float>(expected);
    test_case.run_with_tolerance_as_fp();
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_model_skip_layer_normalization_dynamic_shapes) {
    const auto function = onnx_import::import_onnx_model(
        file_util::path_join(ov::test::utils::getExecutableDirectory(),
                             SERIALIZED_ZOO,
                             "onnx/com.microsoft/skip_layer_normalization_dynamic_shapes.onnx"));

    std::vector<float> input = {
        0.54881352f, 0.71518934f, 0.60276335f, 0.54488319f, 0.42365479f, 0.64589411f, 0.43758720f, 0.89177299f,
        0.96366274f, 0.38344151f, 0.79172504f, 0.52889490f, 0.56804454f, 0.92559665f, 0.07103606f, 0.08712930f,
        0.02021840f, 0.83261985f, 0.77815676f, 0.87001216f, 0.97861832f, 0.79915857f, 0.46147937f, 0.78052920f,
    };
    std::vector<float> skip = {
        0.11827443f, 0.63992101f, 0.14335328f, 0.94466889f, 0.52184832f, 0.41466194f, 0.26455560f, 0.77423370f,
        0.45615032f, 0.56843394f, 0.01878980f, 0.61763549f, 0.61209571f, 0.61693400f, 0.94374806f, 0.68182027f,
        0.35950789f, 0.43703195f, 0.69763118f, 0.06022547f, 0.66676670f, 0.67063785f, 0.21038257f, 0.12892629f,
    };
    std::vector<float> gamma = {
        0.31542835f,
        0.36371076f,
        0.57019675f,
        0.43860152f,
    };
    std::vector<float> beta = {
        0.98837382f,
        0.10204481f,
        0.20887676f,
        0.16130951f,
    };
    std::vector<float> bias = {
        0.65310830f,
        0.25329161f,
        0.46631077f,
        0.24442559f,
    };
    std::vector<float> expected = {
        0.76600611f, 0.34308332f,  -0.48470584f, 0.71335256f,  1.10028172f, -0.13354334f, -0.45232186f, 0.79840088f,
        1.52454257f, -0.19450217f, -0.13759643f, 0.03988872f,  1.27861762f, 0.39529073f,  0.12247884f,  -0.52944231f,
        0.64228040f, 0.21059875f,  1.05966032f,  -0.14278713f, 1.46366918f, 0.21215858f,  -0.31640187f, -0.22832340f,
    };

    auto test_case = ov::test::TestCase(function, s_device);
    test_case.add_input<float>(Shape{3, 2, 4}, input);
    test_case.add_input<float>(Shape{3, 2, 4}, skip);
    test_case.add_input<float>(Shape{4}, gamma);
    test_case.add_input<float>(Shape{4}, beta);
    test_case.add_input<float>(Shape{4}, bias);
    test_case.add_expected_output<float>(Shape{3, 2, 4}, expected);
    test_case.run_with_tolerance_as_fp();
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_model_embed_layer_normalization) {
    const auto function =
        onnx_import::import_onnx_model(file_util::path_join(ov::test::utils::getExecutableDirectory(),
                                                            SERIALIZED_ZOO,
                                                            "onnx/com.microsoft/embed_layer_normalization.onnx"));

    std::vector<int> input_ids = {
        8, 1, 5, 9, 8, 9, 4, 3, 0, 3, 5, 0, 2, 3, 8, 1, 3, 3, 3, 7, 0, 1, 9, 9,
    };
    std::vector<float> expected_output = {
        -0.06615843f, -0.18040463f, 0.02199928f,  0.01868065f,  0.05397778f,  -0.11761580f, -0.09138932f, -0.02506775f,
        -0.02368510f, -0.10373901f, -0.05551499f, -0.20972314f, 0.01365213f,  0.01132561f,  -0.08603337f, -0.08906764f,
        0.09692993f,  -0.04444099f, -0.02037602f, -0.03453060f, -0.10214549f, -0.13331436f, -0.02665862f, -0.01228805f,
        -0.14232540f, -0.07032782f, 0.05511986f,  -0.00120272f, -0.04875736f, -0.13051267f, -0.05709254f, 0.17854357f,
        -0.01759873f, -0.01819968f, 0.07573269f,  0.00557164f,  0.06232717f,  0.00530490f,  -0.01565807f, -0.14841977f,
        -0.02299280f, 0.02038561f,  -0.00049481f, 0.02575402f,  0.10081697f,  -0.12517214f, -0.09316762f, -0.00974943f,
        -0.03093284f, -0.06309240f, -0.05551499f, -0.20972314f, 0.01365213f,  0.01132561f,  -0.08603337f, -0.06176658f,
        0.08304203f,  -0.05025182f, 0.00383657f,  -0.02288112f, -0.11407227f, -0.01386134f, -0.04411830f, -0.00537948f,
        0.00164397f,  -0.03739140f, 0.09941526f,  0.00333974f,  -0.04251949f, -0.12992151f, -0.09509478f, -0.11811313f,
        -0.03307065f, -0.00866115f, -0.15162414f, 0.01106802f,  0.06037656f,  0.00035292f,  -0.00223284f, -0.11215645f,
        -0.01390734f, 0.07064321f,  0.04028325f,  -0.00290875f, 0.12875907f,  -0.12517214f, -0.09316762f, -0.00974943f,
        -0.03093284f, -0.06309240f, -0.08723789f, 0.03130914f,  0.03131931f,  -0.01526242f, 0.20811458f,  -0.05696163f,
        0.16304255f,  -0.02407495f, -0.02955675f, -0.03086288f, -0.08130091f, -0.05001551f, -0.04875683f, 0.00143666f,
        -0.12153473f, -0.00018507f, 0.10957482f,  -0.00416618f, -0.01612359f, -0.11605026f, -0.08593204f, 0.09055272f,
        -0.03054028f, -0.03603891f, -0.08479506f, -0.00034568f, 0.03713699f,  0.00163411f,  -0.01738501f, -0.18267182f,
    };

    auto test_case = ov::test::TestCase(function, s_device);
    test_case.add_input<int>(input_ids);
    test_case.add_expected_output<float>(expected_output);
    test_case.run_with_tolerance_as_fp(1e-7f);
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_model_embed_layer_normalization_with_segment_embedding) {
    const auto function = onnx_import::import_onnx_model(
        file_util::path_join(ov::test::utils::getExecutableDirectory(),
                             SERIALIZED_ZOO,
                             "onnx/com.microsoft/embed_layer_normalization_with_segment_embedding.onnx"));

    std::vector<int> input_ids = {
        8, 1, 5, 9, 8, 9, 4, 3, 0, 3, 5, 0, 2, 3, 8, 1, 3, 3, 3, 7, 0, 1, 9, 9,
    };
    std::vector<int> segment_ids = {
        0, 2, 0, 2, 2, 0, 2, 0, 0, 0, 1, 1, 2, 0, 0, 1, 0, 1, 2, 2, 0, 1, 1, 1,
    };
    std::vector<float> expected_output = {
        -0.06044213f, -0.14845914f, 0.02457689f,  0.02091519f,  0.09514004f,  -0.10280035f, -0.02087995f, -0.03323204f,
        -0.02967127f, -0.13447416f, -0.05191760f, -0.16518904f, 0.02340531f,  0.02176395f,  0.04972410f,  -0.07360736f,
        0.12192874f,  -0.04081530f, -0.02338044f, -0.05671440f, -0.09475864f, -0.08944942f, -0.03362993f, -0.01683486f,
        -0.16770349f, -0.07382569f, 0.06230322f,  0.02215859f,  -0.05212611f, -0.03934773f, -0.04748865f, 0.18134241f,
        -0.01965741f, -0.02202452f, 0.01973994f,  0.01575558f,  0.04300199f,  0.01436110f,  -0.00198062f, -0.09065692f,
        -0.02923042f, -0.00748686f, 0.00717049f,  0.02638642f,  0.12174864f,  -0.12973398f, -0.11872391f, -0.00549398f,
        -0.02386289f, -0.02210563f, -0.03590920f, -0.13728066f, -0.01337939f, 0.01538021f,  -0.14687485f, -0.05033565f,
        0.03818212f,  -0.04939338f, 0.00961064f,  -0.07407621f, -0.09624685f, 0.05594898f,  -0.04948713f, -0.01305631f,
        -0.03779668f, -0.01469170f, 0.12346989f,  0.02082030f,  -0.03449103f, -0.06029151f, -0.09300473f, -0.16308543f,
        -0.02370042f, 0.01066893f,  -0.06523034f, 0.00497636f,  0.01933458f,  -0.00900802f, 0.00430878f,  -0.13999483f,
        -0.02377289f, 0.01760014f,  0.03896973f,  0.00831112f,  0.15634246f,  -0.11109130f, -0.11997811f, -0.02304414f,
        -0.01989413f, -0.12763791f, -0.05698400f, 0.17125534f,  0.00499324f,  -0.02953288f, 0.09178342f,  -0.05001877f,
        0.16157132f,  -0.02312993f, -0.02932195f, -0.04914058f, -0.07994118f, -0.07199102f, -0.04517454f, 0.01249476f,
        -0.07525793f, -0.00207180f, 0.03993115f,  -0.01676321f, -0.00214832f, -0.16074482f, -0.05012497f, -0.00552153f,
        -0.04302063f, -0.00549224f, -0.18399858f, -0.00767871f, -0.02209404f, -0.01383207f, -0.00082931f, -0.19533031f,
    };

    std::vector<int> expected_mask_index = {
        0,
        0,
        0,
    };

    auto test_case = ov::test::TestCase(function, s_device);
    test_case.add_input<int>(input_ids);
    test_case.add_input<int>(segment_ids);
    test_case.add_expected_output<float>(expected_output);
    test_case.add_expected_output<int>(expected_mask_index);
    test_case.run_with_tolerance_as_fp(1e-7f);
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_model_embed_layer_normalization_with_segment_embedding_and_mask) {
    const auto function = onnx_import::import_onnx_model(
        file_util::path_join(ov::test::utils::getExecutableDirectory(),
                             SERIALIZED_ZOO,
                             "onnx/com.microsoft/embed_layer_normalization_with_segment_embedding_and_mask.onnx"));

    std::vector<int> input_ids = {
        8, 1, 5, 9, 8, 9, 4, 3, 0, 3, 5, 0, 2, 3, 8, 1, 3, 3, 3, 7, 0, 1, 9, 9,
    };
    std::vector<int> segment_ids = {
        0, 2, 0, 2, 2, 0, 2, 0, 0, 0, 1, 1, 2, 0, 0, 1, 0, 1, 2, 2, 0, 1, 1, 1,
    };
    std::vector<int> mask = {
        1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1,
    };
    std::vector<float> expected_output = {
        -0.06044213f, -0.14845914f, 0.02457689f,  0.02091519f,  0.09514004f,  -0.10280035f, -0.02087995f, -0.03323204f,
        -0.02967127f, -0.13447416f, -0.05191760f, -0.16518904f, 0.02340531f,  0.02176395f,  0.04972410f,  -0.07360736f,
        0.12192874f,  -0.04081530f, -0.02338044f, -0.05671440f, -0.09475864f, -0.08944942f, -0.03362993f, -0.01683486f,
        -0.16770349f, -0.07382569f, 0.06230322f,  0.02215859f,  -0.05212611f, -0.03934773f, -0.04748865f, 0.18134241f,
        -0.01965741f, -0.02202452f, 0.01973994f,  0.01575558f,  0.04300199f,  0.01436110f,  -0.00198062f, -0.09065692f,
        -0.02923042f, -0.00748686f, 0.00717049f,  0.02638642f,  0.12174864f,  -0.12973398f, -0.11872391f, -0.00549398f,
        -0.02386289f, -0.02210563f, -0.03590920f, -0.13728066f, -0.01337939f, 0.01538021f,  -0.14687485f, -0.05033565f,
        0.03818212f,  -0.04939338f, 0.00961064f,  -0.07407621f, -0.09624685f, 0.05594898f,  -0.04948713f, -0.01305631f,
        -0.03779668f, -0.01469170f, 0.12346989f,  0.02082030f,  -0.03449103f, -0.06029151f, -0.09300473f, -0.16308543f,
        -0.02370042f, 0.01066893f,  -0.06523034f, 0.00497636f,  0.01933458f,  -0.00900802f, 0.00430878f,  -0.13999483f,
        -0.02377289f, 0.01760014f,  0.03896973f,  0.00831112f,  0.15634246f,  -0.11109130f, -0.11997811f, -0.02304414f,
        -0.01989413f, -0.12763791f, -0.05698400f, 0.17125534f,  0.00499324f,  -0.02953288f, 0.09178342f,  -0.05001877f,
        0.16157132f,  -0.02312993f, -0.02932195f, -0.04914058f, -0.07994118f, -0.07199102f, -0.04517454f, 0.01249476f,
        -0.07525793f, -0.00207180f, 0.03993115f,  -0.01676321f, -0.00214832f, -0.16074482f, -0.05012497f, -0.00552153f,
        -0.04302063f, -0.00549224f, -0.18399858f, -0.00767871f, -0.02209404f, -0.01383207f, -0.00082931f, -0.19533031f,
    };
    std::vector<int> expected_mask_index = {
        5,
        3,
        4,
    };

    auto test_case = ov::test::TestCase(function, s_device);
    test_case.add_input<int>(input_ids);
    test_case.add_input<int>(segment_ids);
    test_case.add_input<int>(mask);
    test_case.add_expected_output<float>(expected_output);
    test_case.add_expected_output<int>(expected_mask_index);
    test_case.run_with_tolerance_as_fp(1e-7f);
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_model_embed_layer_normalization_dynamic_shapes) {
    const auto function = onnx_import::import_onnx_model(
        file_util::path_join(ov::test::utils::getExecutableDirectory(),
                             SERIALIZED_ZOO,
                             "onnx/com.microsoft/embed_layer_normalization_dynamic_shapes.onnx"));

    std::vector<int> input_ids = {
        8, 1, 5, 9, 8, 9, 4, 3, 0, 3, 5, 0, 2, 3, 8, 1, 3, 3, 3, 7, 0, 1, 9, 9,
    };
    std::vector<int> segment_ids = {
        0, 2, 0, 2, 2, 0, 2, 0, 0, 0, 1, 1, 2, 0, 0, 1, 0, 1, 2, 2, 0, 1, 1, 1,
    };
    std::vector<float> word_embeddings = {
        0.96980906f, 0.65314001f, 0.17090958f, 0.35815218f, 0.75068617f, 0.60783064f, 0.32504722f, 0.03842543f,
        0.63427407f, 0.95894927f, 0.65279031f, 0.63505888f, 0.99529958f, 0.58185035f, 0.41436860f, 0.47469750f,
        0.62351012f, 0.33800763f, 0.67475230f, 0.31720173f, 0.77834547f, 0.94957107f, 0.66252685f, 0.01357164f,
        0.62284607f, 0.67365962f, 0.97194499f, 0.87819350f, 0.50962436f, 0.05571469f, 0.45115921f, 0.01998767f,
        0.44171092f, 0.97958672f, 0.35944447f, 0.48089352f, 0.68866116f, 0.88047588f, 0.91823548f, 0.21682213f,
        0.56518888f, 0.86510259f, 0.50896895f, 0.91672295f, 0.92115760f, 0.08311249f, 0.27771857f, 0.00935670f,
        0.84234208f, 0.64717412f,
    };
    std::vector<float> position_embeddings = {
        0.84138614f, 0.26473016f, 0.39782074f, 0.55282146f, 0.16494046f, 0.36980811f, 0.14644176f, 0.56961840f,
        0.70373726f, 0.28847644f, 0.43328807f, 0.75610667f, 0.39609829f, 0.89603841f, 0.63892108f, 0.89155442f,
        0.68005556f, 0.44919774f, 0.97857094f, 0.11620191f, 0.76702368f, 0.41182014f, 0.67543906f, 0.24979627f,
        0.31321833f, 0.96541619f, 0.58846509f, 0.65966839f, 0.53320622f, 0.23053302f, 0.39486930f, 0.61880857f,
        0.47486752f, 0.47013220f, 0.71607453f, 0.28799102f, 0.38346222f, 0.74916983f, 0.87845218f, 0.10286336f,
    };
    std::vector<float> segment_embeddings = {
        0.09237389f,
        0.35404667f,
        0.55181628f,
        0.03362509f,
        0.96896178f,
        0.32099724f,
        0.22126268f,
        0.14126390f,
        0.09725992f,
        0.98404223f,
        0.26034093f,
        0.53702253f,
        0.44792616f,
        0.09956909f,
        0.35231167f,
    };
    std::vector<float> gamma = {
        0.46924916f,
        0.84114015f,
        0.90464777f,
        0.03755938f,
        0.50831544f,
    };
    std::vector<float> beta = {
        0.16684751f,
        0.77905101f,
        0.86493331f,
        0.41139671f,
        0.13997258f,
    };
    std::vector<int> mask = {
        1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
    };
    std::vector<float> expected_output = {
        -0.04089922f, 0.35108989f,  0.30442458f,  0.39546335f,  1.15422225f,  0.10419128f,  -0.19301927f, 0.01070970f,
        0.43977541f,  0.89119899f,  -0.51436460f, 1.99256825f,  1.41077507f,  0.38642293f,  0.17583044f,  0.03320138f,
        1.16508031f,  -0.24356931f, 0.47440714f,  -0.17844005f, 0.20463173f,  1.90038323f,  1.14138567f,  0.34504607f,
        0.16403235f,  -0.24976699f, 0.29362509f,  0.34502214f,  0.41751838f,  1.09390712f,  0.12354189f,  1.83025289f,
        1.05569196f,  0.34413773f,  0.35469764f,  -0.69760042f, 0.76338542f,  1.75443077f,  0.44126555f,  0.18181801f,
        0.73277575f,  0.45443264f,  0.17068321f,  0.36591727f,  0.72869974f,  -0.56090516f, 0.14415455f,  1.47314119f,
        0.42908576f,  0.73084539f,  -0.22373237f, 2.26550221f,  0.05606699f,  0.39417523f,  0.35234636f,  0.78569502f,
        0.77521765f,  -0.65131050f, 0.40168875f,  0.45527256f,  0.38715565f,  0.98521245f,  2.21446753f,  0.36345237f,
        -0.33269632f, 0.36558092f,  1.36846578f,  1.37523413f,  0.33698002f,  0.28889543f,  -0.40639281f, 1.01643157f,
        0.59668219f,  0.39197800f,  1.03101778f,  0.02551098f,  -0.03612846f, -0.01371557f, 0.43444607f,  0.96746695f,
        0.60583955f,  -0.10362893f, 0.40574494f,  0.38046724f,  0.87445319f,  -0.00880148f, -0.15437943f, 0.08118075f,
        0.44650543f,  0.85956848f,  -0.27865338f, 2.10837507f,  0.04798460f,  0.43948367f,  -0.10185169f, 0.19978794f,
        1.32323360f,  1.20525467f,  0.44288942f,  -0.84200430f, 0.52563053f,  0.69949460f,  0.73987913f,  0.34668452f,
        0.74545687f,  0.57696682f,  0.22452033f,  -0.27099937f, 0.39649010f,  0.87083614f,  -0.18965788f, 0.58206403f,
        -0.08108193f, 0.42067638f,  1.05117214f,  -0.34287399f, 0.20424896f,  0.27994895f,  0.46011117f,  0.70890665f,
    };
    std::vector<int> expected_mask_index = {
        6,
        5,
        5,
    };

    auto test_case = ov::test::TestCase(function, s_device);
    test_case.add_input<int>(Shape{3, 8}, input_ids);
    test_case.add_input<int>(Shape{3, 8}, segment_ids);
    test_case.add_input<float>(Shape{10, 5}, word_embeddings);
    test_case.add_input<float>(Shape{8, 5}, position_embeddings);
    test_case.add_input<float>(Shape{3, 5}, segment_embeddings);
    test_case.add_input<float>(Shape{5}, gamma);
    test_case.add_input<float>(Shape{5}, beta);
    test_case.add_input<int>(Shape{3, 8}, mask);
    test_case.add_expected_output<float>(Shape{3, 8, 5}, expected_output);
    test_case.add_expected_output<int>(Shape{3}, expected_mask_index);
    test_case.run_with_tolerance_as_fp(1e-6f);
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_model_embed_layer_normalization_diff_seq_len_pos_embed_len) {
    const auto function = onnx_import::import_onnx_model(
        file_util::path_join(ov::test::utils::getExecutableDirectory(),
                             SERIALIZED_ZOO,
                             "onnx/com.microsoft/embed_layer_normalization_dynamic_shapes.onnx"));

    std::vector<int> input_ids = {
        8, 1, 5, 9, 8, 9, 4, 3, 0, 3, 5, 0, 2, 3, 8, 1, 3, 3, 3, 7, 0, 1, 9, 9,
    };
    std::vector<int> segment_ids = {
        0, 2, 0, 2, 2, 0, 2, 0, 0, 0, 1, 1, 2, 0, 0, 1, 0, 1, 2, 2, 0, 1, 1, 1,
    };
    std::vector<float> word_embeddings = {
        0.96980906f, 0.65314001f, 0.17090958f, 0.35815218f, 0.75068617f, 0.60783064f, 0.32504722f, 0.03842543f,
        0.63427407f, 0.95894927f, 0.65279031f, 0.63505888f, 0.99529958f, 0.58185035f, 0.41436860f, 0.47469750f,
        0.62351012f, 0.33800763f, 0.67475230f, 0.31720173f, 0.77834547f, 0.94957107f, 0.66252685f, 0.01357164f,
        0.62284607f, 0.67365962f, 0.97194499f, 0.87819350f, 0.50962436f, 0.05571469f, 0.45115921f, 0.01998767f,
        0.44171092f, 0.97958672f, 0.35944447f, 0.48089352f, 0.68866116f, 0.88047588f, 0.91823548f, 0.21682213f,
        0.56518888f, 0.86510259f, 0.50896895f, 0.91672295f, 0.92115760f, 0.08311249f, 0.27771857f, 0.00935670f,
        0.84234208f, 0.64717412f,
    };
    std::vector<float> position_embeddings = {
        0.84138614f, 0.26473016f, 0.39782074f, 0.55282146f, 0.16494046f, 0.36980811f, 0.14644176f, 0.56961840f,
        0.70373726f, 0.28847644f, 0.43328807f, 0.75610667f, 0.39609829f, 0.89603841f, 0.63892108f, 0.89155442f,
        0.68005556f, 0.44919774f, 0.97857094f, 0.11620191f, 0.76702368f, 0.41182014f, 0.67543906f, 0.24979627f,
        0.31321833f, 0.96541619f, 0.58846509f, 0.65966839f, 0.53320622f, 0.23053302f, 0.39486930f, 0.61880857f,
        0.47486752f, 0.47013220f, 0.71607453f, 0.28799102f, 0.38346222f, 0.74916983f, 0.87845218f, 0.10286336f,
        0.84138614f, 0.26473016f, 0.39782074f, 0.55282146f, 0.16494046f,
    };
    std::vector<float> segment_embeddings = {
        0.09237389f,
        0.35404667f,
        0.55181628f,
        0.03362509f,
        0.96896178f,
        0.32099724f,
        0.22126268f,
        0.14126390f,
        0.09725992f,
        0.98404223f,
        0.26034093f,
        0.53702253f,
        0.44792616f,
        0.09956909f,
        0.35231167f,
    };
    std::vector<float> gamma = {
        0.46924916f,
        0.84114015f,
        0.90464777f,
        0.03755938f,
        0.50831544f,
    };
    std::vector<float> beta = {
        0.16684751f,
        0.77905101f,
        0.86493331f,
        0.41139671f,
        0.13997258f,
    };
    std::vector<int> mask = {
        1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
    };
    std::vector<float> expected_output = {
        -0.04089922f, 0.35108989f,  0.30442458f,  0.39546335f,  1.15422225f,  0.10419128f,  -0.19301927f, 0.01070970f,
        0.43977541f,  0.89119899f,  -0.51436460f, 1.99256825f,  1.41077507f,  0.38642293f,  0.17583044f,  0.03320138f,
        1.16508031f,  -0.24356931f, 0.47440714f,  -0.17844005f, 0.20463173f,  1.90038323f,  1.14138567f,  0.34504607f,
        0.16403235f,  -0.24976699f, 0.29362509f,  0.34502214f,  0.41751838f,  1.09390712f,  0.12354189f,  1.83025289f,
        1.05569196f,  0.34413773f,  0.35469764f,  -0.69760042f, 0.76338542f,  1.75443077f,  0.44126555f,  0.18181801f,
        0.73277575f,  0.45443264f,  0.17068321f,  0.36591727f,  0.72869974f,  -0.56090516f, 0.14415455f,  1.47314119f,
        0.42908576f,  0.73084539f,  -0.22373237f, 2.26550221f,  0.05606699f,  0.39417523f,  0.35234636f,  0.78569502f,
        0.77521765f,  -0.65131050f, 0.40168875f,  0.45527256f,  0.38715565f,  0.98521245f,  2.21446753f,  0.36345237f,
        -0.33269632f, 0.36558092f,  1.36846578f,  1.37523413f,  0.33698002f,  0.28889543f,  -0.40639281f, 1.01643157f,
        0.59668219f,  0.39197800f,  1.03101778f,  0.02551098f,  -0.03612846f, -0.01371557f, 0.43444607f,  0.96746695f,
        0.60583955f,  -0.10362893f, 0.40574494f,  0.38046724f,  0.87445319f,  -0.00880148f, -0.15437943f, 0.08118075f,
        0.44650543f,  0.85956848f,  -0.27865338f, 2.10837507f,  0.04798460f,  0.43948367f,  -0.10185169f, 0.19978794f,
        1.32323360f,  1.20525467f,  0.44288942f,  -0.84200430f, 0.52563053f,  0.69949460f,  0.73987913f,  0.34668452f,
        0.74545687f,  0.57696682f,  0.22452033f,  -0.27099937f, 0.39649010f,  0.87083614f,  -0.18965788f, 0.58206403f,
        -0.08108193f, 0.42067638f,  1.05117214f,  -0.34287399f, 0.20424896f,  0.27994895f,  0.46011117f,  0.70890665f,
    };
    std::vector<int> expected_mask_index = {
        6,
        5,
        5,
    };

    auto test_case = ov::test::TestCase(function, s_device);
    test_case.add_input<int>(Shape{3, 8}, input_ids);
    test_case.add_input<int>(Shape{3, 8}, segment_ids);
    test_case.add_input<float>(Shape{10, 5}, word_embeddings);
    test_case.add_input<float>(Shape{9, 5}, position_embeddings);
    test_case.add_input<float>(Shape{3, 5}, segment_embeddings);
    test_case.add_input<float>(Shape{5}, gamma);
    test_case.add_input<float>(Shape{5}, beta);
    test_case.add_input<int>(Shape{3, 8}, mask);
    test_case.add_expected_output<float>(Shape{3, 8, 5}, expected_output);
    test_case.add_expected_output<int>(Shape{3}, expected_mask_index);
    test_case.run_with_tolerance_as_fp(1e-6f);
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_model_embed_layer_normalization_with_position_ids) {
    const auto function = onnx_import::import_onnx_model(
        file_util::path_join(ov::test::utils::getExecutableDirectory(),
                             SERIALIZED_ZOO,
                             "onnx/com.microsoft/embed_layer_normalization_dynamic_shapes_with_position_ids.onnx"));

    std::vector<int> input_ids = {
        8, 1, 5, 9, 8, 9, 4, 3, 0, 3, 5, 0, 2, 3, 8, 1, 3, 3, 3, 7, 0, 1, 9, 9,
    };
    std::vector<int> segment_ids = {
        0, 2, 0, 2, 2, 0, 2, 0, 0, 0, 1, 1, 2, 0, 0, 1, 0, 1, 2, 2, 0, 1, 1, 1,
    };
    std::vector<float> word_embeddings = {
        0.96980906f, 0.65314001f, 0.17090958f, 0.35815218f, 0.75068617f, 0.60783064f, 0.32504722f, 0.03842543f,
        0.63427407f, 0.95894927f, 0.65279031f, 0.63505888f, 0.99529958f, 0.58185035f, 0.41436860f, 0.47469750f,
        0.62351012f, 0.33800763f, 0.67475230f, 0.31720173f, 0.77834547f, 0.94957107f, 0.66252685f, 0.01357164f,
        0.62284607f, 0.67365962f, 0.97194499f, 0.87819350f, 0.50962436f, 0.05571469f, 0.45115921f, 0.01998767f,
        0.44171092f, 0.97958672f, 0.35944447f, 0.48089352f, 0.68866116f, 0.88047588f, 0.91823548f, 0.21682213f,
        0.56518888f, 0.86510259f, 0.50896895f, 0.91672295f, 0.92115760f, 0.08311249f, 0.27771857f, 0.00935670f,
        0.84234208f, 0.64717412f,
    };
    std::vector<float> position_embeddings = {
        0.84138614f, 0.26473016f, 0.39782074f, 0.55282146f, 0.16494046f, 0.43328807f, 0.75610667f, 0.39609829f,
        0.89603841f, 0.63892108f, 0.36980811f, 0.14644176f, 0.56961840f, 0.70373726f, 0.28847644f, 0.89155442f,
        0.68005556f, 0.44919774f, 0.97857094f, 0.11620191f, 0.76702368f, 0.41182014f, 0.67543906f, 0.24979627f,
        0.31321833f, 0.28799102f, 0.38346222f, 0.74916983f, 0.87845218f, 0.10286336f, 0.96541619f, 0.58846509f,
        0.65966839f, 0.53320622f, 0.23053302f, 0.39486930f, 0.61880857f, 0.47486752f, 0.47013220f, 0.71607453f,
        0.84138614f, 0.26473016f, 0.39782074f, 0.55282146f, 0.16494046f,
    };
    std::vector<float> segment_embeddings = {
        0.09237389f,
        0.35404667f,
        0.55181628f,
        0.03362509f,
        0.96896178f,
        0.32099724f,
        0.22126268f,
        0.14126390f,
        0.09725992f,
        0.98404223f,
        0.26034093f,
        0.53702253f,
        0.44792616f,
        0.09956909f,
        0.35231167f,
    };
    std::vector<float> gamma = {
        0.46924916f,
        0.84114015f,
        0.90464777f,
        0.03755938f,
        0.50831544f,
    };
    std::vector<float> beta = {
        0.16684751f,
        0.77905101f,
        0.86493331f,
        0.41139671f,
        0.13997258f,
    };
    std::vector<int> mask = {
        1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
    };
    std::vector<int> position_ids = {
        0, 2, 1, 3, 4, 6, 7, 5, 8, 2, 1, 3, 4, 6, 7, 5, 0, 2, 1, 3, 4, 6, 7, 5,
    };
    std::vector<float> expected_output = {
        -0.04089922f, 0.35108989f,  0.30442458f,  0.39546335f,  1.15422225f,  0.10419128f,  -0.19301927f, 0.01070970f,
        0.43977541f,  0.89119899f,  -0.51436460f, 1.99256825f,  1.41077507f,  0.38642293f,  0.17583044f,  0.03320138f,
        1.16508031f,  -0.24356931f, 0.47440714f,  -0.17844005f, 0.20463173f,  1.90038323f,  1.14138567f,  0.34504607f,
        0.16403235f,  -0.24976699f, 0.29362509f,  0.34502214f,  0.41751838f,  1.09390712f,  0.12354189f,  1.83025289f,
        1.05569196f,  0.34413773f,  0.35469764f,  -0.69760042f, 0.76338542f,  1.75443077f,  0.44126555f,  0.18181801f,
        0.73277575f,  0.45443264f,  0.17068321f,  0.36591727f,  0.72869974f,  -0.56090516f, 0.14415455f,  1.47314119f,
        0.42908576f,  0.73084539f,  -0.22373237f, 2.26550221f,  0.05606699f,  0.39417523f,  0.35234636f,  0.78569502f,
        0.77521765f,  -0.65131050f, 0.40168875f,  0.45527256f,  0.38715565f,  0.98521245f,  2.21446753f,  0.36345237f,
        -0.33269632f, 0.36558092f,  1.36846578f,  1.37523413f,  0.33698002f,  0.28889543f,  -0.40639281f, 1.01643157f,
        0.59668219f,  0.39197800f,  1.03101778f,  0.02551098f,  -0.03612846f, -0.01371557f, 0.43444607f,  0.96746695f,
        0.60583955f,  -0.10362893f, 0.40574494f,  0.38046724f,  0.87445319f,  -0.00880148f, -0.15437943f, 0.08118075f,
        0.44650543f,  0.85956848f,  -0.27865338f, 2.10837507f,  0.04798460f,  0.43948367f,  -0.10185169f, 0.19978794f,
        1.32323360f,  1.20525467f,  0.44288942f,  -0.84200430f, 0.52563053f,  0.69949460f,  0.73987913f,  0.34668452f,
        0.74545687f,  0.57696682f,  0.22452033f,  -0.27099937f, 0.39649010f,  0.87083614f,  -0.18965788f, 0.58206403f,
        -0.08108193f, 0.42067638f,  1.05117214f,  -0.34287399f, 0.20424896f,  0.27994895f,  0.46011117f,  0.70890665f,
    };
    std::vector<int> expected_mask_index = {
        6,
        5,
        5,
    };

    auto test_case = ov::test::TestCase(function, s_device);
    test_case.add_input<int>(Shape{3, 8}, input_ids);
    test_case.add_input<int>(Shape{3, 8}, segment_ids);
    test_case.add_input<float>(Shape{10, 5}, word_embeddings);
    test_case.add_input<float>(Shape{9, 5}, position_embeddings);
    test_case.add_input<float>(Shape{3, 5}, segment_embeddings);
    test_case.add_input<float>(Shape{5}, gamma);
    test_case.add_input<float>(Shape{5}, beta);
    test_case.add_input<int>(Shape{3, 8}, mask);
    test_case.add_input<int>(Shape{3, 8}, position_ids);
    test_case.add_expected_output<float>(Shape{3, 8, 5}, expected_output);
    test_case.add_expected_output<int>(Shape{3}, expected_mask_index);
    test_case.run_with_tolerance_as_fp(1e-6f);
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_model_attention) {
    const auto function = onnx_import::import_onnx_model(file_util::path_join(ov::test::utils::getExecutableDirectory(),
                                                                              SERIALIZED_ZOO,
                                                                              "onnx/com.microsoft/attention.onnx"));
    auto test_case = ov::test::TestCase(function, s_device);

    std::vector<float> input = {
        0.91475844f, 0.91523546f, 0.82536930f, 0.37491974f, 0.22384071f, 0.05941105f, 0.01902100f, 0.70131350f,
        0.09603709f, 0.44200060f, 0.53106076f, 0.79464376f, 0.35469049f, 0.25225943f, 0.25179818f, 0.29592562f,
        0.24836586f, 0.65088797f, 0.93126643f, 0.67980725f, 0.85708112f, 0.59808528f, 0.46321425f, 0.19301885f,
    };
    std::vector<float> output = {
        0.07966283f, 0.10783536f, -0.19424979f, 0.54514766f, 0.07965867f, 0.10783093f, -0.19424866f, 0.54510003f,
        0.07965846f, 0.10783067f, -0.19424550f, 0.54509139f, 0.07966217f, 0.10783640f, -0.19424903f, 0.54512268f,
        0.06940663f, 0.10962760f, -0.19698445f, 0.53492010f, 0.06940675f, 0.10962828f, -0.19698484f, 0.53492326f,
        0.06940714f, 0.10963022f, -0.19698712f, 0.53494006f, 0.06940673f, 0.10962812f, -0.19698519f, 0.53492481f,
    };

    test_case.add_input<float>(input);
    test_case.add_expected_output<float>(output);
    test_case.run_with_tolerance_as_fp(1e-7f);
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_model_attention_qkv_hidden_sizes) {
    const auto function =
        onnx_import::import_onnx_model(file_util::path_join(ov::test::utils::getExecutableDirectory(),
                                                            SERIALIZED_ZOO,
                                                            "onnx/com.microsoft/attention_qkv_hidden_sizes.onnx"));
    auto test_case = ov::test::TestCase(function, s_device);

    std::vector<float> input = {
        0.56477863f, 0.60309958f, 0.35158035f, 0.03123519f, 0.81918180f, 0.76905495f, 0.47219241f, 0.72016627f,
        0.59377003f, 0.91380632f, 0.56797302f, 0.34846428f, 0.83839595f, 0.16394103f, 0.34676281f, 0.09161621f,
        0.45562279f, 0.23317528f, 0.37197968f, 0.06727808f, 0.08500192f, 0.84915495f, 0.68266946f, 0.00227691f,
    };
    std::vector<float> output = {
        -0.59370947f, -0.30300471f, 0.12048547f, -0.09029539f, 0.08041390f, 0.10250041f, -0.19381392f, 0.55126983f,
        -0.59370828f, -0.30301332f, 0.12049319f, -0.09029691f, 0.08041921f, 0.10250521f, -0.19381438f, 0.55127531f,
        -0.59370869f, -0.30301058f, 0.12049074f, -0.09029643f, 0.08041564f, 0.10250199f, -0.19381410f, 0.55127168f,
        -0.59370929f, -0.30300608f, 0.12048667f, -0.09029562f, 0.08041184f, 0.10249855f, -0.19381374f, 0.55126774f,
        -0.59681994f, -0.26327702f, 0.07638434f, -0.06311120f, 0.06671587f, 0.10916986f, -0.19412412f, 0.51977092f,
        -0.59682053f, -0.26328400f, 0.07638102f, -0.06311222f, 0.06671817f, 0.10917170f, -0.19412397f, 0.51977223f,
        -0.59682077f, -0.26328647f, 0.07637984f, -0.06311259f, 0.06671739f, 0.10917108f, -0.19412403f, 0.51977175f,
        -0.59682101f, -0.26328778f, 0.07637922f, -0.06311278f, 0.06671065f, 0.10916568f, -0.19412443f, 0.51976782f,
    };

    test_case.add_input<float>(input);
    test_case.add_expected_output<float>(output);
    test_case.run_with_tolerance_as_fp(1e-4f);
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_model_attention_unidirectional) {
    const auto function =
        onnx_import::import_onnx_model(file_util::path_join(ov::test::utils::getExecutableDirectory(),
                                                            SERIALIZED_ZOO,
                                                            "onnx/com.microsoft/attention_unidirectional.onnx"));
    auto test_case = ov::test::TestCase(function, s_device);

    std::vector<float> input = {
        0.89578921f, 0.42421508f, 0.35630688f, 0.77461642f, 0.65753633f, 0.09723099f, 0.62597734f, 0.72117692f,
        0.57636845f, 0.17104276f, 0.13245547f, 0.59879875f, 0.15624641f, 0.44903454f, 0.50483286f, 0.92975074f,
        0.36934483f, 0.29919949f, 0.57185954f, 0.83036488f, 0.08384345f, 0.20378476f, 0.74684393f, 0.46716982f,
    };
    std::vector<float> output = {
        0.05604819f, 0.09000472f, -0.19437021f, 0.52487367f, 0.06211422f, 0.08740954f, -0.19139624f, 0.52762908f,
        0.06708897f, 0.08992603f, -0.19214047f, 0.53631783f, 0.06896879f, 0.10248676f, -0.19485690f, 0.53477794f,
        0.08577005f, 0.12807365f, -0.19762954f, 0.54432857f, 0.06929274f, 0.10893210f, -0.19599904f, 0.53184807f,
        0.07348281f, 0.10215081f, -0.19280069f, 0.53552240f, 0.07861833f, 0.10517240f, -0.19285706f, 0.54126489f,
    };
    std::vector<float> present = {
        -0.60427380f, -0.25958878f, -0.59609234f, -0.24055196f, -0.59613681f, -0.30088067f, -0.59633607f, -0.33270463f,
        0.06899665f,  -0.09284544f, 0.08059876f,  -0.06146053f, 0.11841078f,  -0.10019838f, 0.10605468f,  -0.09273906f,
        -0.59036821f, -0.32410735f, -0.60532302f, -0.25127757f, -0.58926487f, -0.25271094f, -0.58640373f, -0.31730092f,
        0.12509561f,  -0.07968873f, 0.06005794f,  -0.08937149f, 0.10523240f,  -0.05083811f, 0.14162725f,  -0.07438751f,
        0.05604819f,  0.09000472f,  0.06819826f,  0.08480665f,  0.07700446f,  0.09494394f,  0.07459175f,  0.14003153f,
        -0.19437021f, 0.52487367f,  -0.18843602f, 0.53037173f,  -0.19362189f, 0.55360907f,  -0.20299932f, 0.53020388f,
        0.08577005f,  0.12807365f,  0.05276009f,  0.08972625f,  0.08190014f,  0.08852972f,  0.09400313f,  0.11423884f,
        -0.19762954f, 0.54432857f,  -0.19435294f, 0.51924801f,  -0.18643703f, 0.54280555f,  -0.19302703f, 0.55837619f,
    };

    test_case.add_input<float>(input);
    test_case.add_expected_output<float>(output);
    test_case.add_expected_output<float>(present);
    test_case.run_with_tolerance_as_fp(1e-7f);
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_model_attention_mask_index_1f) {
    const auto function =
        onnx_import::import_onnx_model(file_util::path_join(ov::test::utils::getExecutableDirectory(),
                                                            SERIALIZED_ZOO,
                                                            "onnx/com.microsoft/attention_mask_index_1.onnx"));
    auto test_case = ov::test::TestCase(function, s_device);

    std::vector<float> input = {
        0.02841483f, 0.47845092f, 0.14633700f, 0.54597300f, 0.40160629f, 0.55281311f, 0.14931096f, 0.64483738f,
        0.96559167f, 0.05262021f, 0.12391864f, 0.20093553f, 0.74290562f, 0.19367455f, 0.19253619f, 0.41593507f,
        0.91188699f, 0.61606920f, 0.72673517f, 0.86981291f, 0.19963337f, 0.22747350f, 0.34308898f, 0.57267183f,
    };
    std::vector<int> mask_index = {
        0,
        1,
    };
    std::vector<float> output = {
        0.08298690f, 0.12711772f, -0.19757506f, 0.54029012f, 0.08298548f, 0.12711433f, -0.19757731f, 0.54031140f,
        0.08298430f, 0.12711799f, -0.19757695f, 0.54031777f, 0.08298548f, 0.12711433f, -0.19757444f, 0.54028159f,
        0.05380550f, 0.10459180f, -0.19593412f, 0.50907606f, 0.05380550f, 0.10459180f, -0.19593412f, 0.50907606f,
        0.05380550f, 0.10459180f, -0.19593412f, 0.50907606f, 0.05380550f, 0.10459180f, -0.19593412f, 0.50907606f,
    };
    std::vector<float> present = {
        -0.58437425f, -0.29483819f, -0.59927911f, -0.30336475f, -0.59104657f, -0.37327260f, -0.59078789f, -0.29863101f,
        0.11751597f,  -0.04114649f, 0.09933343f,  -0.09884726f, 0.16250694f,  -0.12028439f, 0.09319257f,  -0.05129660f,
        -0.60341775f, -0.25221461f, -0.58933026f, -0.31912822f, -0.59271193f, -0.25470981f, -0.59399152f, -0.32643768f,
        0.05398282f,  -0.07468132f, 0.14743008f,  -0.09407346f, 0.10399222f,  -0.06682440f, 0.11632499f,  -0.08986320f,
        0.09104910f,  0.12973849f,  0.06917210f,  0.11059431f,  0.09356256f,  0.12594685f,  0.07814129f,  0.14221822f,
        -0.19329809f, 0.53526556f,  -0.19787431f, 0.53673857f,  -0.20045389f, 0.57165766f,  -0.19869246f, 0.51749766f,
        0.05380550f,  0.10459180f,  0.09169570f,  0.09892380f,  0.07746917f,  0.08042616f,  0.07953370f,  0.12909687f,
        -0.19593412f, 0.50907606f,  -0.19202785f, 0.56904894f,  -0.18689045f, 0.54643762f,  -0.19969353f, 0.53976399f,
    };

    test_case.add_input<float>(input);
    test_case.add_input<int>(mask_index);
    test_case.add_expected_output<float>(output);
    test_case.add_expected_output<float>(present);
    test_case.run_with_tolerance_as_fp();
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_model_attention_mask_index_2) {
    const auto function =
        onnx_import::import_onnx_model(file_util::path_join(ov::test::utils::getExecutableDirectory(),
                                                            SERIALIZED_ZOO,
                                                            "onnx/com.microsoft/attention_mask_index_2.onnx"));
    auto test_case = ov::test::TestCase(function, s_device);

    std::vector<float> input = {
        0.75259578f, 0.81492645f, 0.46713001f, 0.29483622f, 0.06768602f, 0.95105755f, 0.32065326f, 0.52417183f,
        0.73136383f, 0.77176476f, 0.60997742f, 0.64625764f, 0.16311000f, 0.89680773f, 0.01331447f, 0.42468646f,
        0.58711547f, 0.00345124f, 0.13053808f, 0.46278623f, 0.13786320f, 0.65182054f, 0.74864876f, 0.81506181f,
    };
    std::vector<int> mask_index = {
        3,
        3,
        1,
        1,
    };
    std::vector<float> output = {
        0.07524174f, 0.11320241f, -0.19909523f, 0.54785377f, 0.06825337f, 0.13981669f, -0.20774621f, 0.53718704f,
        0.07531278f, 0.12957911f, -0.20330518f, 0.54547405f, 0.07531209f, 0.12958010f, -0.20330583f, 0.54547292f,
        0.08900890f, 0.11150353f, -0.18931937f, 0.53757656f, 0.07915881f, 0.10416336f, -0.18914750f, 0.52921104f,
        0.08285815f, 0.11462159f, -0.19115375f, 0.53077918f, 0.08285838f, 0.11462225f, -0.19115454f, 0.53077984f,
    };
    std::vector<float> present = {
        -0.59630549f, -0.28110915f, -0.60274345f, -0.36154836f, -0.59437746f, -0.33717164f, -0.60134649f, -0.29849592f,
        0.11169122f,  -0.09345293f, 0.11103803f,  -0.13096604f, 0.13131849f,  -0.10597084f, 0.10463209f,  -0.11332577f,
        -0.57949269f, -0.27235535f, -0.58941406f, -0.25372508f, -0.58658379f, -0.28718373f, -0.59821802f, -0.32433146f,
        0.13244939f,  -0.02865628f, 0.09308393f,  -0.04083736f, 0.10948701f,  -0.04423397f, 0.13060363f,  -0.12316251f,
        0.07509718f,  0.08392500f,  0.06825337f,  0.13981669f,  0.08239168f,  0.11931328f,  0.06770951f,  0.09240761f,
        -0.19074154f, 0.55260652f,  -0.20774621f, 0.53718704f,  -0.19888818f, 0.55371630f,  -0.19559640f, 0.54754448f,
        0.09983939f,  0.10603377f,  0.07915881f,  0.10416336f,  0.08655046f,  0.12505992f,  0.07738422f,  0.09509270f,
        -0.18571433f, 0.55095005f,  -0.18914750f, 0.52921104f,  -0.19315663f, 0.53234470f,  -0.19601485f, 0.56322992f,
    };

    test_case.add_input<float>(input);
    test_case.add_input<int>(mask_index);
    test_case.add_expected_output<float>(output);
    test_case.add_expected_output<float>(present);
    test_case.run_with_tolerance_as_fp();
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_model_attention_mask_index_3) {
    const auto function =
        onnx_import::import_onnx_model(file_util::path_join(ov::test::utils::getExecutableDirectory(),
                                                            SERIALIZED_ZOO,
                                                            "onnx/com.microsoft/attention_mask_index_3.onnx"));
    auto test_case = ov::test::TestCase(function, s_device);

    std::vector<float> input = {
        0.33093750f, 0.39181390f, 0.14586255f, 0.39709702f, 0.98086524f, 0.03891133f, 0.72234219f, 0.21966648f,
        0.79986620f, 0.97251678f, 0.04131543f, 0.43971965f, 0.50185394f, 0.11452501f, 0.88111717f, 0.76076663f,
        0.31870860f, 0.54107893f, 0.91756296f, 0.58112669f, 0.99117357f, 0.00256292f, 0.58885485f, 0.93481058f,
    };
    std::vector<int> mask = {
        1,
        1,
        1,
        0,
        0,
        0,
        0,
        1,
    };
    std::vector<float> output = {
        0.07551830f, 0.10666487f, -0.19357042f, 0.53683108f, 0.07551410f, 0.10666656f, -0.19356072f, 0.53684169f,
        0.07552745f, 0.10666100f, -0.19358172f, 0.53682435f, 0.07552218f, 0.10666317f, -0.19358677f, 0.53681952f,
        0.09727416f, 0.13513327f, -0.20121223f, 0.57003713f, 0.09727416f, 0.13513327f, -0.20121223f, 0.57003713f,
        0.09727416f, 0.13513327f, -0.20121223f, 0.57003713f, 0.09727416f, 0.13513327f, -0.20121223f, 0.57003713f,
    };
    std::vector<float> present = {
        -0.59174627f, -0.27471560f, -0.58307797f, -0.25967693f, -0.60766846f, -0.31754097f, -0.61241394f, -0.26291698f,
        0.09206123f,  -0.05307099f, 0.12491645f,  -0.03853742f, 0.08732655f,  -0.13050151f, 0.04073093f,  -0.10792807f,
        -0.60556883f, -0.34055573f, -0.60474855f, -0.28785610f, -0.60757709f, -0.32514900f, -0.58872569f, -0.37967020f,
        0.09779400f,  -0.13136166f, 0.07915612f,  -0.10649752f, 0.11043755f,  -0.15124020f, 0.16626491f,  -0.11274654f,
        0.07639833f,  0.11762549f,  0.09370039f,  0.09133558f,  0.05661478f,  0.11096847f,  0.04019671f,  0.10117501f,
        -0.19371650f, 0.52530587f,  -0.18429738f, 0.55240726f,  -0.20283231f, 0.53265429f,  -0.20036045f, 0.50568837f,
        0.06171235f,  0.12687264f,  0.05802051f,  0.10266830f,  0.06172965f,  0.08967118f,  0.09727416f,  0.13513327f,
        -0.20576829f, 0.53365225f,  -0.19832623f, 0.52809310f,  -0.19971462f, 0.55584043f,  -0.20121223f, 0.57003713f,
    };

    test_case.add_input<float>(input);
    test_case.add_input<int>(mask);
    test_case.add_expected_output<float>(output);
    test_case.add_expected_output<float>(present);
    test_case.run_with_tolerance_as_fp(1e-7f);
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_model_attention_mask_index_4) {
    const auto function =
        onnx_import::import_onnx_model(file_util::path_join(ov::test::utils::getExecutableDirectory(),
                                                            SERIALIZED_ZOO,
                                                            "onnx/com.microsoft/attention_mask_index_4.onnx"));
    auto test_case = ov::test::TestCase(function, s_device);

    std::vector<float> input = {
        0.23565151f, 0.58627969f, 0.75137484f, 0.68586946f, 0.62750375f, 0.13284931f, 0.13347220f, 0.36357051f,
        0.56910241f, 0.48275986f, 0.49440190f, 0.45483324f, 0.63547862f, 0.97893149f, 0.40630588f, 0.38783622f,
        0.07172249f, 0.46385381f, 0.99764502f, 0.22219376f, 0.67735291f, 0.40799847f, 0.74337566f, 0.87263006f,
    };
    std::vector<int> mask = {
        1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1,
    };
    std::vector<float> output = {
        0.07771622f, 0.10724538f, -0.19453585f, 0.54342043f, 0.07459468f, 0.10934003f, -0.19561143f, 0.53936625f,
        0.07927690f, 0.10619678f, -0.19399606f, 0.54543519f, 0.07459468f, 0.10934003f, -0.19561143f, 0.53936625f,
        0.05485561f, 0.11278091f, -0.20117569f, 0.52096349f, 0.06629646f, 0.10195158f, -0.19900991f, 0.54654449f,
        0.06491723f, 0.10292297f, -0.19678673f, 0.53451663f, 0.06549793f, 0.11126325f, -0.19989857f, 0.53717279f,
    };
    std::vector<float> present = {
        -0.59188855f, -0.34495637f, -0.59508181f, -0.25013468f, -0.59176934f, -0.33229247f, -0.59576762f, -0.29731843f,
        0.14217430f,  -0.10403840f, 0.08584045f,  -0.06193545f, 0.12358667f,  -0.08588549f, 0.10515238f,  -0.08629489f,
        -0.59092808f, -0.28260738f, -0.60047609f, -0.30411413f, -0.61210287f, -0.28645760f, -0.59391296f, -0.34649473f,
        0.12789863f,  -0.08159252f, 0.08122411f,  -0.08866425f, 0.06395009f,  -0.12896645f, 0.14855847f,  -0.11978809f,
        0.08783118f,  0.12152332f,  0.07067389f,  0.09078297f,  0.08385989f,  0.13306075f,  0.07459468f,  0.10934003f,
        -0.19849420f, 0.55928540f,  -0.18948570f, 0.53154731f,  -0.19960676f, 0.54237455f,  -0.19561143f, 0.53936625f,
        0.08509844f,  0.08314656f,  0.06388859f,  0.12990499f,  0.04582624f,  0.09566365f,  0.08674107f,  0.10823163f,
        -0.18808734f, 0.56137776f,  -0.20168513f, 0.51830697f,  -0.20066255f, 0.52363914f,  -0.19737384f, 0.56921995f,
    };

    test_case.add_input<float>(input);
    test_case.add_input<int>(mask);
    test_case.add_expected_output<float>(output);
    test_case.add_expected_output<float>(present);
    test_case.run_with_tolerance_as_fp(1e-7f);
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_model_attention_past) {
    const auto function =
        onnx_import::import_onnx_model(file_util::path_join(ov::test::utils::getExecutableDirectory(),
                                                            SERIALIZED_ZOO,
                                                            "onnx/com.microsoft/attention_past.onnx"));
    auto test_case = ov::test::TestCase(function, s_device);

    std::vector<float> input = {
        0.82966000f, 0.77751911f, 0.08977074f, 0.06076468f, 0.40659550f, 0.19995944f, 0.55544919f, 0.83971608f,
        0.86254036f, 0.30894691f, 0.80156928f, 0.83092463f, 0.14506543f, 0.32196075f, 0.42209163f, 0.24465553f,
        0.93944097f, 0.73528159f, 0.23347616f, 0.60544974f, 0.93329269f, 0.67604774f, 0.56349903f, 0.26199624f,
    };
    std::vector<int> mask = {
        1,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        1,
        1,
        1,
        1,
    };
    std::vector<float> past = {
        0.92467678f, 0.79873562f, 0.00939191f, 0.34891853f, 0.35521412f, 0.21872006f, 0.89974332f, 0.74132687f,
        0.73566031f, 0.75168055f, 0.06773245f, 0.85702997f, 0.76256698f, 0.51739877f, 0.91567177f, 0.66617578f,
        0.88056499f, 0.08436447f, 0.54744655f, 0.25466520f, 0.08500137f, 0.19271941f, 0.86525357f, 0.21717627f,
        0.97158766f, 0.42288730f, 0.09890039f, 0.01148765f, 0.97024685f, 0.19697112f, 0.67671591f, 0.67960924f,
        0.46656516f, 0.30850092f, 0.73536104f, 0.73938161f, 0.91650903f, 0.57628596f, 0.51164514f, 0.11695814f,
        0.79792547f, 0.97192264f, 0.29246020f, 0.41030061f, 0.19014873f, 0.90233624f, 0.84986305f, 0.26141909f,
        0.84528726f, 0.81416380f, 0.00429944f, 0.31476986f, 0.00440918f, 0.77413058f, 0.13409913f, 0.20965169f,
        0.61764991f, 0.55266041f, 0.56107825f, 0.42051074f, 0.16804738f, 0.80362344f, 0.52392679f, 0.27550557f,
        0.66738850f, 0.39348483f, 0.31801429f, 0.30325863f, 0.37068403f, 0.92767614f, 0.60799408f, 0.01458820f,
        0.24194679f, 0.59596598f, 0.81762302f, 0.38094005f, 0.16618672f, 0.92488551f, 0.84298438f, 0.21752745f,
    };
    std::vector<float> output = {
        0.26186451f, 0.45950246f, -0.04001215f, 0.47680017f, 0.26333901f, 0.46158865f, -0.04006424f, 0.47588652f,
        0.26875457f, 0.47031689f, -0.03951600f, 0.47674999f, 0.26851410f, 0.46987134f, -0.03919901f, 0.47629333f,
        0.18083976f, 0.16579385f, -0.05161894f, 0.63075018f, 0.18228555f, 0.16642828f, -0.04873618f, 0.63316816f,
        0.18362364f, 0.16702136f, -0.05045432f, 0.63178891f, 0.18000112f, 0.16541445f, -0.05139139f, 0.63105792f,
    };
    std::vector<float> present = {
        0.92467678f,  0.79873562f,  0.00939191f,  0.34891853f,  0.35521412f,  0.21872006f,  0.89974332f,  0.74132687f,
        0.73566031f,  0.75168055f,  -0.59527576f, -0.23625080f, -0.58657664f, -0.29827437f, -0.59528387f, -0.33578828f,
        -0.59068960f, -0.34870598f, 0.06773245f,  0.85702997f,  0.76256698f,  0.51739877f,  0.91567177f,  0.66617578f,
        0.88056499f,  0.08436447f,  0.54744655f,  0.25466520f,  0.08536442f,  -0.06134639f, 0.11295843f,  -0.04818217f,
        0.14562836f,  -0.12305059f, 0.15695867f,  -0.11161390f, 0.08500137f,  0.19271941f,  0.86525357f,  0.21717627f,
        0.97158766f,  0.42288730f,  0.09890039f,  0.01148765f,  0.97024685f,  0.19697112f,  -0.59141791f, -0.31600696f,
        -0.58647990f, -0.34302223f, -0.59306550f, -0.36427227f, -0.59695083f, -0.26431620f, 0.67671591f,  0.67960924f,
        0.46656516f,  0.30850092f,  0.73536104f,  0.73938161f,  0.91650903f,  0.57628596f,  0.51164514f,  0.11695814f,
        0.11255538f,  -0.07302766f, 0.16620418f,  -0.09871224f, 0.15272795f,  -0.12076923f, 0.08827571f,  -0.07442430f,
        0.79792547f,  0.97192264f,  0.29246020f,  0.41030061f,  0.19014873f,  0.90233624f,  0.84986305f,  0.26141909f,
        0.84528726f,  0.81416380f,  0.07014155f,  0.07749540f,  0.08745074f,  0.13131952f,  0.08430066f,  0.09709007f,
        0.09247591f,  0.11065811f,  0.00429944f,  0.31476986f,  0.00440918f,  0.77413058f,  0.13409913f,  0.20965169f,
        0.61764991f,  0.55266041f,  0.56107825f,  0.42051074f,  -0.18658412f, 0.53568852f,  -0.19482780f, 0.53271860f,
        -0.19558203f, 0.57155901f,  -0.19633618f, 0.57260245f,  0.16804738f,  0.80362344f,  0.52392679f,  0.27550557f,
        0.66738850f,  0.39348483f,  0.31801429f,  0.30325863f,  0.37068403f,  0.92767614f,  0.08172131f,  0.13249113f,
        0.09947956f,  0.10781212f,  0.08890627f,  0.12280971f,  0.06911418f,  0.09499176f,  0.60799408f,  0.01458820f,
        0.24194679f,  0.59596598f,  0.81762302f,  0.38094005f,  0.16618672f,  0.92488551f,  0.84298438f,  0.21752745f,
        -0.19839945f, 0.53462923f,  -0.19349247f, 0.57778782f,  -0.20039621f, 0.56689924f,  -0.19190890f, 0.53286803f,
    };

    test_case.add_input<float>(input);
    test_case.add_input<int>(mask);
    test_case.add_input<float>(past);
    test_case.add_expected_output<float>(output);
    test_case.add_expected_output<float>(present);
    test_case.run_with_tolerance_as_fp(1e-6f);
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_model_attention_extra_add) {
    const auto function =
        onnx_import::import_onnx_model(file_util::path_join(ov::test::utils::getExecutableDirectory(),
                                                            SERIALIZED_ZOO,
                                                            "onnx/com.microsoft/attention_extra_add.onnx"));
    auto test_case = ov::test::TestCase(function, s_device);

    std::vector<float> input = {
        0.14930259f, 0.11199699f, 0.81292826f, 0.08368169f, 0.05704883f, 0.41276145f, 0.38760167f, 0.00146112f,
        0.14275745f, 0.54254925f, 0.07962929f, 0.31023681f, 0.09597706f, 0.60583973f, 0.90233743f, 0.33360451f,
        0.18193199f, 0.19159532f, 0.07869831f, 0.86026299f, 0.20683478f, 0.40150928f, 0.93124926f, 0.31805834f,
    };
    std::vector<int> mask = {
        0,
        0,
        1,
        0,
        1,
        1,
        1,
        0,
    };
    std::vector<float> extra_add = {
        0.73230380f, 0.61824518f, 0.19738488f, 0.57034588f, 0.22331032f, 0.53262889f, 0.60098642f, 0.72943515f,
        0.09009175f, 0.81116527f, 0.47240964f, 0.49679127f, 0.41110733f, 0.29418564f, 0.93818313f, 0.64175284f,
        0.06807775f, 0.66733366f, 0.78848422f, 0.48788327f, 0.38806340f, 0.14002480f, 0.72263688f, 0.22772972f,
        0.24000823f, 0.75820386f, 0.64254439f, 0.19385594f, 0.95595860f, 0.59840417f, 0.93769604f, 0.62474734f,
        0.36690548f, 0.76047903f, 0.62352085f, 0.58574778f, 0.64251810f, 0.78072041f, 0.43344691f, 0.75383639f,
        0.73950553f, 0.92625278f, 0.05066428f, 0.08448382f, 0.25980917f, 0.50312829f, 0.97800279f, 0.05422170f,
        0.05171391f, 0.82828254f, 0.42234898f, 0.95752198f, 0.96325767f, 0.97909677f, 0.35578200f, 0.48091716f,
        0.03637243f, 0.91552693f, 0.43403026f, 0.94275808f, 0.51182085f, 0.86773109f, 0.38459453f, 0.87822068f,
    };
    std::vector<float> output = {
        0.06090815f, 0.12919067f, -0.19883196f, 0.50295448f, 0.06090815f, 0.12919067f, -0.19883196f, 0.50295448f,
        0.06090815f, 0.12919067f, -0.19883196f, 0.50295448f, 0.06090815f, 0.12919067f, -0.19883196f, 0.50295448f,
        0.08714182f, 0.12259886f, -0.19516067f, 0.54010558f, 0.08671370f, 0.12369543f, -0.19658084f, 0.54502594f,
        0.08458151f, 0.12488046f, -0.19519810f, 0.53906947f, 0.09063499f, 0.12088943f, -0.19583938f, 0.54266596f,
    };
    std::vector<float> present = {
        -0.59800303f, -0.35666457f, -0.59420627f, -0.31881350f, -0.59887993f, -0.27025288f, -0.60216135f, -0.27772796f,
        0.11659990f,  -0.11224300f, 0.09693416f,  -0.07304113f, 0.06023501f,  -0.05941332f, 0.06434284f,  -0.07978789f,
        -0.59005713f, -0.37009716f, -0.59542215f, -0.27914333f, -0.57998544f, -0.29826957f, -0.58625919f, -0.28872511f,
        0.15994480f,  -0.11288825f, 0.07906821f,  -0.05991337f, 0.14479136f,  -0.04415035f, 0.13493451f,  -0.06541853f,
        0.07513385f,  0.14411135f,  0.07505661f,  0.14532046f,  0.06090815f,  0.12919067f,  0.05788904f,  0.12018456f,
        -0.20586906f, 0.53715372f,  -0.20203318f, 0.52092510f,  -0.19883196f, 0.50295448f,  -0.19937295f, 0.51055026f,
        0.09417956f,  0.12943678f,  0.06923291f,  0.12574309f,  0.10221909f,  0.11366953f,  0.09235901f,  0.09584601f,
        -0.20036517f, 0.56818324f,  -0.19709785f, 0.51547027f,  -0.18871340f, 0.55736589f,  -0.18826833f, 0.55965197f,
    };

    test_case.add_input<float>(input);
    test_case.add_input<int>(mask);
    test_case.add_input<float>(extra_add);
    test_case.add_expected_output<float>(output);
    test_case.add_expected_output<float>(present);
    test_case.run_with_tolerance_as_fp(1e-7f);
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_model_attention_dynamic_shapes) {
    const auto function =
        onnx_import::import_onnx_model(file_util::path_join(ov::test::utils::getExecutableDirectory(),
                                                            SERIALIZED_ZOO,
                                                            "onnx/com.microsoft/attention_dynamic_shapes.onnx"));
    auto test_case = ov::test::TestCase(function, s_device);

    std::vector<float> input = {
        0.42226878f, 0.50984067f, 0.80440795f, 0.68040705f, 0.93614250f, 0.45104721f, 0.71767306f, 0.48596525f,
        0.70076728f, 0.04500086f, 0.28930107f, 0.77435863f, 0.19392140f, 0.90290719f, 0.91955870f, 0.58811885f,
        0.76795286f, 0.62884814f, 0.23377730f, 0.49212688f, 0.87256873f, 0.11944817f, 0.57715887f, 0.91886938f,
    };
    std::vector<float> weights = {
        0.99377930f, 0.22733542f, 0.43217131f, 0.60717988f, 0.97224706f, 0.70020503f, 0.92439449f, 0.41512674f,
        0.47728160f, 0.40306625f, 0.72619593f, 0.37954643f, 0.36950976f, 0.84305370f, 0.61671126f, 0.22251014f,
        0.73839295f, 0.73471880f, 0.37428924f, 0.80240524f, 0.23120961f, 0.06072779f, 0.92840081f, 0.71558088f,
        0.08719950f, 0.51666921f, 0.53768843f, 0.48113129f, 0.46389169f, 0.01036468f, 0.37341005f, 0.67195475f,
        0.53599644f, 0.41795707f, 0.58081782f, 0.97939289f,
    };
    std::vector<float> bias = {
        0.77122736f,
        0.75600564f,
        0.86177206f,
        0.69982684f,
        0.74719858f,
        0.78054035f,
        0.80007398f,
        0.74902135f,
        0.81258053f,
        0.01575289f,
        0.08463049f,
        0.39671996f,
    };
    std::vector<int> mask = {
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        0,
    };
    std::vector<float> past = {
        0.27759778f, 0.18458818f, 0.63114458f, 0.09953160f, 0.59739488f, 0.63917851f, 0.18828323f, 0.65625650f,
        0.84574437f, 0.91846281f, 0.55102497f, 0.27506110f, 0.06816208f, 0.82616585f, 0.85912132f, 0.88682729f,
        0.14730524f, 0.61618829f, 0.89891797f, 0.27753425f, 0.57438278f, 0.33753166f, 0.88768929f, 0.35533753f,
        0.30193496f, 0.81678063f, 0.26569194f, 0.62769043f, 0.61990744f, 0.59077013f, 0.11058200f, 0.97370809f,
        0.81339806f, 0.57207322f, 0.80417949f, 0.54185718f, 0.80831683f, 0.29390740f, 0.29051417f, 0.51964313f,
        0.04341308f, 0.05925354f, 0.82397246f, 0.55753845f, 0.61247689f, 0.98571628f, 0.07566493f, 0.37537411f,
        0.42080343f, 0.21715857f, 0.57869565f, 0.55962265f, 0.82500041f, 0.60776925f, 0.19367239f, 0.88382334f,
        0.20328504f, 0.58192456f, 0.94542676f, 0.98562658f, 0.64355153f, 0.69856495f, 0.30377558f, 0.02857198f,
        0.96969068f, 0.48450547f, 0.98341352f, 0.03546083f, 0.84963584f, 0.94460547f, 0.90907097f, 0.22525074f,
        0.12530145f, 0.52223104f, 0.09549426f, 0.93127102f, 0.93429947f, 0.01428344f, 0.74249738f, 0.22606593f,
    };
    std::vector<float> output = {
        1.47439122f, 0.50951630f, 1.17974961f, 1.58501005f, 1.49403512f, 0.51560062f, 1.18972027f, 1.59668207f,
        1.48384988f, 0.51248586f, 1.18596375f, 1.59219086f, 1.44181466f, 0.50219649f, 1.15537691f, 1.55348074f,
        0.83429223f, 0.59521818f, 0.87688094f, 0.13611843f, 0.82936716f, 0.61004817f, 0.87633312f, 0.13887596f,
        0.83155584f, 0.59382534f, 0.87496555f, 0.14041223f, 0.83309680f, 0.58982348f, 0.87517864f, 0.13930768f,
    };
    std::vector<float> present = {
        0.27759778f, 0.18458818f, 0.63114458f, 0.09953160f, 0.59739488f, 0.63917851f, 0.18828323f, 0.65625650f,
        0.84574437f, 0.91846281f, 1.90736914f, 1.45914197f, 2.30920029f, 1.94944119f, 2.12886763f, 1.64736962f,
        1.36378694f, 1.03263116f, 0.55102497f, 0.27506110f, 0.06816208f, 0.82616585f, 0.85912132f, 0.88682729f,
        0.14730524f, 0.61618829f, 0.89891797f, 0.27753425f, 1.68161881f, 1.87394094f, 1.94785213f, 2.08572555f,
        1.90705216f, 1.90777159f, 1.23910809f, 1.52017307f, 0.57438278f, 0.33753166f, 0.88768929f, 0.35533753f,
        0.30193496f, 0.81678063f, 0.26569194f, 0.62769043f, 0.61990744f, 0.59077013f, 2.02901411f, 1.58923888f,
        2.17776394f, 1.76309133f, 1.74264824f, 1.31485105f, 1.71575761f, 1.29775190f, 0.11058200f, 0.97370809f,
        0.81339806f, 0.57207322f, 0.80417949f, 0.54185718f, 0.80831683f, 0.29390740f, 0.29051417f, 0.51964313f,
        1.66065478f, 2.17192268f, 1.86598253f, 2.03193212f, 1.52620018f, 1.82728052f, 1.46963060f, 1.87916136f,
        0.04341308f, 0.05925354f, 0.82397246f, 0.55753845f, 0.61247689f, 0.98571628f, 0.07566493f, 0.37537411f,
        0.42080343f, 0.21715857f, 1.56316149f, 0.55312467f, 1.59553123f, 0.53537023f, 1.64308119f, 0.62742490f,
        1.31600118f, 0.37510848f, 0.57869565f, 0.55962265f, 0.82500041f, 0.60776925f, 0.19367239f, 0.88382334f,
        0.20328504f, 0.58192456f, 0.94542676f, 0.98562658f, 1.33183134f, 1.70965421f, 1.70983100f, 1.76660407f,
        1.46399045f, 1.70318413f, 0.83565855f, 1.37921953f, 0.64355153f, 0.69856495f, 0.30377558f, 0.02857198f,
        0.96969068f, 0.48450547f, 0.98341352f, 0.03546083f, 0.84963584f, 0.94460547f, 1.60677671f, 0.53308368f,
        1.60789728f, 0.56227136f, 1.50563633f, 0.50456268f, 1.49554634f, 0.48299593f, 0.90907097f, 0.22525074f,
        0.12530145f, 0.52223104f, 0.09549426f, 0.93127102f, 0.93429947f, 0.01428344f, 0.74249738f, 0.22606593f,
        1.59781134f, 2.01703453f, 1.58993423f, 1.78536010f, 1.21809304f, 1.69219351f, 1.24090374f, 1.75499403f,
    };

    test_case.add_input<float>(Shape{2, 4, 3}, input);
    test_case.add_input<float>(Shape{3, 12}, weights);
    test_case.add_input<float>(Shape{12}, bias);
    test_case.add_input<int>(Shape{2, 9}, mask);
    test_case.add_input<float>(Shape{2, 2, 2, 5, 2}, past);
    test_case.add_expected_output<float>(Shape{2, 4, 4}, output);
    test_case.add_expected_output<float>(Shape{2, 2, 2, 9, 2}, present);
    test_case.run_with_tolerance_as_fp(1e-6f);
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_model_fusedgemm_abc) {
    const auto function = onnx_import::import_onnx_model(file_util::path_join(ov::test::utils::getExecutableDirectory(),
                                                                              SERIALIZED_ZOO,
                                                                              "onnx/com.microsoft/fusedgemm.onnx"));
    auto test_case = ov::test::TestCase(function, s_device);

    std::vector<float> inputA = {
        0.760289272f,
        0.155913759f,
        0.781790674f,
        -0.916164881f,
        -0.599392663f,
        0.264654594f,
        0.793851873f,
        0.177088557f,
        0.082737454f,
        0.070692121f,
        -0.811413035f,
        -0.098108588f,
        0.650090827f,
        -0.987659751f,
        -0.815909968f,
        -0.375566031f,
        -0.192777789f,
        -0.843511765f,
    };

    std::vector<float> inputB = {
        -0.599338344f, -0.893724541f, -0.362130441f, -0.510642812f, -0.943908814f, -0.247790266f,
        -0.732624930f, 0.660286910f,  -0.264866660f, -0.907203793f, 0.339617010f,  -0.322529173f,
        0.714601048f,  0.581729832f,  -0.609115490f, -0.369882312f, -0.462432785f, -0.554824440f,
        -0.833489997f, -0.899945507f, -0.088337136f, -0.253637339f, -0.443307744f, -0.677004897f,
    };

    std::vector<float> inputC = {
        -0.540039918f,
        -0.235745675f,
        -0.337291175f,
        -0.702340580f,
        0.532629731f,
        -0.794515569f,
        -0.532012999f,
        0.372558416f,
        0.582367524f,
        -0.483044018f,
        0.656635884f,
        -0.655929499f,
    };

    std::vector<float> output = {
        -8.75421E-05f,
        -9.65321E-05f,
        0.239491309f,
        -2.70329E-05f,
        0.151090653f,
        -5.53371E-05f,
        -1.22197E-05f,
        0.413963711f,
        0.618195780f,
        0.011654445f,
        0.815541101f,
        -2.46706E-05f,
    };

    test_case.add_input<float>(Shape{3, 6}, inputA);
    test_case.add_input<float>(Shape{6, 4}, inputB);
    test_case.add_input<float>(Shape{3, 4}, inputC);
    test_case.add_expected_output<float>(Shape{3, 4}, output);
    test_case.run_with_tolerance_as_fp(1e-6f);
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_com_microsoft_fused_conv_hard_sigmoid) {
    const auto function =
        onnx_import::import_onnx_model(file_util::path_join(ov::test::utils::getExecutableDirectory(),
                                                            SERIALIZED_ZOO,
                                                            "onnx/com.microsoft/fused_conv_hard_sigmoid.onnx"));
    auto test_case = ov::test::TestCase(function, s_device);

    const std::vector<float> X{1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f};
    const std::vector<float> W{0.125f, 0.125f, 0.125f, 0.125f, -0.125f, -0.125f, -0.125f, -0.125f};
    const std::vector<float> expected{0.8f, 0.9f, 1.0f, 1.0f, 0.2f, 0.1f, 0.0f, 0.0f};

    test_case.add_input<float>(Shape{1, 1, 3, 3}, X);
    test_case.add_input<float>(Shape{2, 1, 2, 2}, W);
    test_case.add_expected_output<float>(Shape{1, 2, 2, 2}, expected);
    test_case.run_with_tolerance_as_fp();
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_com_microsoft_fused_conv_relu) {
    const auto function =
        onnx_import::import_onnx_model(file_util::path_join(ov::test::utils::getExecutableDirectory(),
                                                            SERIALIZED_ZOO,
                                                            "onnx/com.microsoft/fused_conv_relu.onnx"));
    auto test_case = ov::test::TestCase(function, s_device);

    const std::vector<float> X{1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f};
    const std::vector<float> W{1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f};
    const std::vector<float> expected{12.0f, 16.0f, 24.0f, 28.0f, 0.0f, 0.0f, 0.0f, 0.0f};

    test_case.add_input<float>(Shape{1, 1, 3, 3}, X);
    test_case.add_input<float>(Shape{2, 1, 2, 2}, W);
    test_case.add_expected_output<float>(Shape{1, 2, 2, 2}, expected);
    test_case.run_with_tolerance_as_fp();
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_com_microsoft_fused_conv_tanh) {
    const auto function =
        onnx_import::import_onnx_model(file_util::path_join(ov::test::utils::getExecutableDirectory(),
                                                            SERIALIZED_ZOO,
                                                            "onnx/com.microsoft/fused_conv_tanh.onnx"));
    auto test_case = ov::test::TestCase(function, s_device);

    const std::vector<float> X{1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f};
    const std::vector<float> W{1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -2.0f, -1.0f, -2.0f};
    const std::vector<float> expected{1.f, 1.f, 1.f, 1.f, -1.f, -1.f, -1.f, -1.f};

    test_case.add_input<float>(Shape{1, 1, 3, 3}, X);
    test_case.add_input<float>(Shape{2, 1, 2, 2}, W);
    test_case.add_expected_output<float>(Shape{1, 2, 2, 2}, expected);
    test_case.run_with_tolerance_as_fp();
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_com_microsoft_fused_conv_sigmoid) {
    const auto function =
        onnx_import::import_onnx_model(file_util::path_join(ov::test::utils::getExecutableDirectory(),
                                                            SERIALIZED_ZOO,
                                                            "onnx/com.microsoft/fused_conv_sigmoid.onnx"));
    auto test_case = ov::test::TestCase(function, s_device);

    const std::vector<float> X{1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f};
    const std::vector<float> W{1.0f, 1.0f, 5.0f, 1.0f, -1.0f, -2.0f, -5.0f, -2.0f};
    const std::vector<float> expected{1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f};

    test_case.add_input<float>(Shape{1, 1, 3, 3}, X);
    test_case.add_input<float>(Shape{2, 1, 2, 2}, W);
    test_case.add_expected_output<float>(Shape{1, 2, 2, 2}, expected);
    test_case.run_with_tolerance_as_fp();
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_com_microsoft_fused_conv_clip) {
    const auto function =
        onnx_import::import_onnx_model(file_util::path_join(ov::test::utils::getExecutableDirectory(),
                                                            SERIALIZED_ZOO,
                                                            "onnx/com.microsoft/fused_conv_clip.onnx"));
    auto test_case = ov::test::TestCase(function, s_device);

    const std::vector<float> X{1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f};
    const std::vector<float> W{1.0f, 1.0f, 5.0f, 1.0f, -1.0f, -2.0f, -5.0f, -2.0f};
    const std::vector<float> expected{8.f, 8.f, 8.f, 8.f, 2.f, 2.f, 2.f, 2.f};

    test_case.add_input<float>(Shape{1, 1, 3, 3}, X);
    test_case.add_input<float>(Shape{2, 1, 2, 2}, W);
    test_case.add_expected_output<float>(Shape{1, 2, 2, 2}, expected);
    test_case.run_with_tolerance_as_fp();
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_com_microsoft_fused_conv_leaky_relu) {
    const auto function =
        onnx_import::import_onnx_model(file_util::path_join(ov::test::utils::getExecutableDirectory(),
                                                            SERIALIZED_ZOO,
                                                            "onnx/com.microsoft/fused_conv_leaky_relu.onnx"));
    auto test_case = ov::test::TestCase(function, s_device);

    const std::vector<float> X{1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f};
    const std::vector<float> W{1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f};
    const std::vector<float> expected{12.0f, 16.0f, 24.0f, 28.0f, -1.2f, -1.6f, -2.4f, -2.8f};

    test_case.add_input<float>(Shape{1, 1, 3, 3}, X);
    test_case.add_input<float>(Shape{2, 1, 2, 2}, W);
    test_case.add_expected_output<float>(Shape{1, 2, 2, 2}, expected);
    test_case.run_with_tolerance_as_fp();
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_com_microsoft_fused_conv_relu_z_input) {
    const auto function =
        onnx_import::import_onnx_model(file_util::path_join(ov::test::utils::getExecutableDirectory(),
                                                            SERIALIZED_ZOO,
                                                            "onnx/com.microsoft/fused_conv_relu_z_input.onnx"));
    auto test_case = ov::test::TestCase(function, s_device);

    const std::vector<float> X{1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f};
    const std::vector<float> W{1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f};
    const std::vector<float> B{1.0f, -1.0f};
    const std::vector<float> Z{0.0f, -10.0f, 0.0f, 10.0f, 0.0f, -10.0f, 0.0f, 10.0f};
    const std::vector<float> expected{.0f, .0f, .0f, 9.0f, .0f, .0f, .0f, 7.0f};

    test_case.add_input<float>(Shape{1, 1, 3, 3}, X);
    test_case.add_input<float>(Shape{2, 1, 2, 2}, W);
    test_case.add_input<float>(Shape{2}, B);
    test_case.add_input<float>(Shape{1, 2, 2, 2}, Z);
    test_case.add_expected_output<float>(Shape{1, 2, 2, 2}, expected);
    test_case.run_with_tolerance_as_fp();
}

OPENVINO_TEST(${BACKEND_NAME}, onnx_com_microsoft_trilu_lower) {
    const auto function = onnx_import::import_onnx_model(file_util::path_join(ov::test::utils::getExecutableDirectory(),
                                                                              SERIALIZED_ZOO,
                                                                              "onnx/com.microsoft/trilu_lower.onnx"));

    auto test_case = ov::test::TestCase(function, s_device);
    // clang-format off
    test_case.add_input<float>(Shape{4, 5},
        std::vector<float>{ 1,  2,  3,  4,  5,
                            6,  7,  8,  9, 10,
                           11, 12, 13, 14, 15,
                           16, 17, 18, 19, 20});
    test_case.add_input<int64_t>(Shape{}, {0}); // k
    test_case.add_expected_output<float>(Shape{4, 5},
        std::vector<float>{ 1,  0,  0,  0,  0,
                            6,  7,  0,  0,  0,
                           11, 12, 13,  0,  0,
                           16, 17, 18, 19,  0});
    test_case.run();

    test_case.add_input<float>(Shape{4, 5},
        std::vector<float>{ 1,  2,  3,  4,  5,
                            6,  7,  8,  9, 10,
                           11, 12, 13, 14, 15,
                           16, 17, 18, 19, 20});
    test_case.add_input<int64_t>(Shape{}, {2}); // k
    test_case.add_expected_output<float>(Shape{4, 5},
        std::vector<float>{ 1,  2,  3,  0,  0,
                            6,  7,  8,  9,  0,
                           11, 12, 13, 14, 15,
                           16, 17, 18, 19, 20});
    test_case.run();

    test_case.add_input<float>(Shape{4, 5},
        std::vector<float>{ 1,  2,  3,  4,  5,
                            6,  7,  8,  9, 10,
                           11, 12, 13, 14, 15,
                           16, 17, 18, 19, 20});
    test_case.add_input<int64_t>(Shape{}, {-2}); // k
    test_case.add_expected_output<float>(Shape{4, 5},
        std::vector<float>{ 0,  0,  0,  0,  0,
                            0,  0,  0,  0,  0,
                           11,  0,  0,  0,  0,
                           16, 17,  0,  0,  0});
    test_case.run();

    // clang-format on
}