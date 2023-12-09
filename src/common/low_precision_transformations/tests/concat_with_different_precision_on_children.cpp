// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "layer_transformation.hpp"

#include <string>
#include <sstream>
#include <memory>

#include <gtest/gtest.h>

#include "transformations/utils/utils.hpp"
#include "low_precision/concat.hpp"
#include "low_precision/fake_quantize_decomposition.hpp"
#include "low_precision/max_pool.hpp"

#include "common_test_utils/ov_test_utils.hpp"
#include "ov_lpt_models/concat.hpp"
#include "ov_lpt_models/common/fake_quantize_on_data.hpp"
#include "simple_low_precision_transformer.hpp"
#include "low_precision/common/quantization_granularity_restriction.hpp"


using namespace testing;
using namespace ov;
using namespace ov::pass;

namespace {

class ConcatTransformationActualValues {
public:
    ngraph::builder::subgraph::FakeQuantizeOnData fakeQuantize1;
    ngraph::builder::subgraph::FakeQuantizeOnData fakeQuantize2;
};

inline std::ostream& operator<<(std::ostream& out, const ConcatTransformationActualValues& values) {
    return out << "_" << values.fakeQuantize1 << "_" << values.fakeQuantize2;
}

class ConcatTransformationResultValues {
public:
    ngraph::builder::subgraph::FakeQuantizeOnData fakeQuantize1;
    ngraph::builder::subgraph::FakeQuantizeOnData fakeQuantize2;
    ov::element::Type precisionBeforeOp;
    ngraph::builder::subgraph::DequantizationOperations dequantizationBefore1;
    ngraph::builder::subgraph::DequantizationOperations dequantizationBefore2;
    ov::element::Type precisionAfterOperation;
    ngraph::builder::subgraph::DequantizationOperations dequantizationAfter1;
    ngraph::builder::subgraph::DequantizationOperations dequantizationAfter2;
};

inline std::ostream& operator<<(std::ostream& out, const ConcatTransformationResultValues& values) {
    return out << "_" <<
        values.fakeQuantize1 << "_" <<
        values.fakeQuantize2 << "_" <<
        values.dequantizationAfter1 << "_" <<
        values.dequantizationAfter2;
}

class ConcatTransformationTestValues {
public:
    TestTransformationParams params;
    bool multiChannels;
    std::int64_t axis;
    ConcatTransformationActualValues actual;
    ConcatTransformationResultValues result;
};

inline std::ostream& operator<<(std::ostream& out, const ConcatTransformationTestValues& values) {
    return out << "_" << values.multiChannels << "_" << values.actual << "_" << values.result;
}

typedef std::tuple <
    ov::element::Type,
    ov::PartialShape,
    ConcatTransformationTestValues
> ConcatTransformationParams;

class ConcatWithDifferentChildrenTransformation : public LayerTransformation, public testing::WithParamInterface<ConcatTransformationParams> {
public:
    void SetUp() override {
        const ov::element::Type precision = std::get<0>(GetParam());
        const ov::PartialShape inputShape = std::get<1>(GetParam());
        ConcatTransformationTestValues testValues = std::get<2>(GetParam());

        actualFunction = ngraph::builder::subgraph::ConcatFunction::getOriginalWithDifferentPrecisionOnChildren(
            precision,
            inputShape,
            testValues.axis,
            testValues.actual.fakeQuantize1,
            testValues.actual.fakeQuantize2);

        auto quantizationRestrictions = testValues.multiChannels ?
            std::vector<ov::pass::low_precision::QuantizationGranularityRestriction>() :
            std::vector<ov::pass::low_precision::QuantizationGranularityRestriction>({
                ov::pass::low_precision::QuantizationGranularityRestriction::create<ov::op::v1::AvgPool>()
            });

        SimpleLowPrecisionTransformer transform({}, quantizationRestrictions);
        transform.add<ov::pass::low_precision::ConcatTransformation, ov::op::v0::Concat>(testValues.params);
        transform.add<ov::pass::low_precision::FakeQuantizeDecompositionTransformation, ov::op::v0::FakeQuantize>(testValues.params);
        transform.add<ov::pass::low_precision::MaxPoolTransformation, ov::op::v1::MaxPool>(testValues.params);
        transform.transform(actualFunction);

        referenceFunction = ngraph::builder::subgraph::ConcatFunction::getReferenceWithDifferentPrecisionOnChildren(
            precision,
            inputShape,
            testValues.multiChannels,
            testValues.axis,
            testValues.result.fakeQuantize1,
            testValues.result.fakeQuantize2,
            testValues.result.precisionBeforeOp,
            testValues.result.dequantizationBefore1,
            testValues.result.dequantizationBefore2,
            testValues.result.precisionAfterOperation,
            testValues.result.dequantizationAfter1,
            testValues.result.dequantizationAfter2);
    }

    static std::string getTestCaseName(testing::TestParamInfo<ConcatTransformationParams> obj) {
        const ov::element::Type precision = std::get<0>(obj.param);
        const ov::PartialShape inputShape = std::get<1>(obj.param);
        const ConcatTransformationTestValues testValues = std::get<2>(obj.param);

        std::ostringstream result;
        result <<
            LayerTransformation::getTestCaseNameByParams(precision, inputShape, testValues.params) << "_" <<
            (testValues.multiChannels ? "multiChannels_" : "notMultiChannels_") <<
            "axis" << testValues.axis <<
            testValues.actual << "_" <<
            testValues.result << "_";
        return result.str();
    }
};

TEST_P(ConcatWithDifferentChildrenTransformation, CompareFunctions) {
    actualFunction->validate_nodes_and_infer_types();
    auto res = compare_functions(actualFunction, referenceFunction, true, true, false);
    ASSERT_TRUE(res.first) << res.second;
}

const std::vector<ov::element::Type> precisions = {
    ov::element::f32,
    // ov::element::f16
};

const std::vector<ov::PartialShape> shapes = {
    { 1, 3, 10, 10 },
    { 4, 3, 10, 10 },
    { -1, 3, 10, -1 }
};

const std::vector<ConcatTransformationTestValues> testValues = {
    // U8
    {
        LayerTransformation::createParamsU8I8(),
        false,
        1,
        {
            { 256ul, ov::Shape({}), {0.f}, {2.55f}, {0.f}, {2.55f} },
            { 256ul, ov::Shape({}), {0.f}, {2.55f / 2.f}, {0.f}, {2.55f / 2.f} }
        },
        {
            { 256ul, ov::Shape({}), {0.f}, {2.55f}, {0.f}, {255.f} },
            { 256ul, ov::Shape({}), {0.f}, {2.55f / 2.f}, {0.f}, { 128.f} },
            ov::element::u8,
            {{}, {}, {}},
            {{}, {}, {}},
            ov::element::u8,
            { ov::element::f32, {}, { 0.01f } },
            { ov::element::f32, {}, { 0.01f } }
        }
    },
    // U8 concatenation by spatial dimension
    {
        LayerTransformation::createParamsU8I8(),
        false,
        2,
        {
            { 256ul, ov::Shape({}), {0.f}, {2.55f}, {0.f}, {2.55f} },
            { 256ul, ov::Shape({}), {0.f}, {2.55f / 2.f}, {0.f}, {2.55f / 2.f} }
        },
        {
            { 256ul, ov::Shape({}), {0.f}, {2.55f}, {0.f}, {255.f} },
            { 256ul, ov::Shape({}), {0.f}, {2.55f / 2.f}, {0.f}, {128.f} },
            ov::element::u8,
            {},
            {},
            ov::element::f32,
            { ov::element::f32, {}, { 0.01f } },
            { ov::element::f32, {}, { 0.01f } }
        }
    },
    // I8
    {
        LayerTransformation::createParamsI8I8(),
        false,
        1,
        {
            { 256ul, ov::Shape({}), {-1.28f}, {1.27f}, {-1.28f}, {1.27f} },
            { 256ul, ov::Shape({}), {-1.28f / 2.f}, {1.27f / 2.f}, {-1.28f / 2.f}, {1.27f / 2.f} }
        },
        {
            { 256ul, ov::Shape({}), {-1.28f}, {1.27f}, {-128.f}, {127.f} },
            { 256ul, ov::Shape({}), {-1.28f / 2.f}, {1.27f / 2.f}, {-64.f}, { 64.f} },
            ov::element::i8,
            {{}, {}, {}},
            {{}, {}, {}},
            ov::element::i8,
            { ov::element::f32, {}, { 0.01f } },
            { ov::element::f32, {}, { 0.01f } }
        }
    },
    // U8: concat multi channels
    {
        LayerTransformation::createParamsU8I8(),
        true,
        1,
        {
            { 256ul, ov::Shape({}), {0.f}, {2.55f}, {0.f}, {2.55f} },
            { 256ul, ov::Shape({}), {0.f}, {2.55f / 2.f}, {0.f}, {2.55f / 2.f} }
        },
        {
            { 256ul, ov::Shape({}), {0.f}, {2.55f}, {0.f}, {255.f} },
            { 256ul, ov::Shape({}), {0.f}, {2.55f / 2.f}, {0.f}, { 255.f} },
            ov::element::u8,
            {{}, {}, {}},
            {{}, {}, {}},
            ov::element::u8,
            { ov::element::f32, {}, {{ 0.01f, 0.01f, 0.01f, 0.005f, 0.005f, 0.005f }} },
            { ov::element::f32, {}, {{ 0.01f, 0.01f, 0.01f, 0.005f, 0.005f, 0.005f }} },
        }
    },
    // I8: concat multi channels
    {
        LayerTransformation::createParamsI8I8(),
        true,
        1,
        {
            { 256ul, ov::Shape({}), {-1.28f}, {1.27f}, {-1.28f}, {1.27f} },
            { 256ul, ov::Shape({}), {-1.28f / 2.f}, {1.27f / 2.f}, {-1.28f / 2.f}, {1.27f / 2.f} }
        },
        {
            { 256ul, ov::Shape({}), {-1.28f}, {1.27f}, {-128.f}, {127.f} },
            { 256ul, ov::Shape({}), {-1.28f / 2.f}, {1.27f / 2.f}, {-128.f}, {127.f} },
            ov::element::i8,
            {{}, {}, {}},
            {{}, {}, {}},
            ov::element::i8,
            { ov::element::f32, {}, {{ 0.01f, 0.01f, 0.01f, 0.005f, 0.005f, 0.005f }} },
            { ov::element::f32, {}, {{ 0.01f, 0.01f, 0.01f, 0.005f, 0.005f, 0.005f }} },
        }
    },
    // not update precisions
    {
        LayerTransformation::createParamsU8I8().setUpdatePrecisions(false),
        false,
        1,
        {
            { 256ul, ov::Shape({}), {0.f}, {2.55f}, {0.f}, {2.55f} },
            { 256ul, ov::Shape({}), {0.f}, {2.55f / 2.f}, {0.f}, {2.55f / 2.f} }
        },
        {
            { 256ul, ov::Shape({}), {0.f}, {2.55f}, {0.f}, {255.f} },
            { 256ul, ov::Shape({}), {0.f}, {2.55f / 2.f}, {0.f}, { 128.f} },
            ov::element::f32,
            {{}, {}, {}},
            {{}, {}, {}},
            ov::element::f32,
            { {}, {}, { 0.01f } },
            { {}, {}, { 0.01f } }
        }
    },
};

INSTANTIATE_TEST_SUITE_P(
    smoke_LPT,
    ConcatWithDifferentChildrenTransformation,
    ::testing::Combine(
        ::testing::ValuesIn(precisions),
        ::testing::ValuesIn(shapes),
        ::testing::ValuesIn(testValues)),
    ConcatWithDifferentChildrenTransformation::getTestCaseName);
}  // namespace