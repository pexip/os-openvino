// Copyright (C) 2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "layer_transformation.hpp"

#include <string>
#include <sstream>
#include <memory>

#include <gtest/gtest.h>

#include <utility>
#include <transformations/utils/utils.hpp>
#include <transformations/init_node_info.hpp>
#include <low_precision/network_helper.hpp>

#include "common_test_utils/ngraph_test_utils.hpp"
#include "ngraph_functions/low_precision_transformations/move_dequantization_after_with_int_constant_function.hpp"
#include "ngraph_functions/low_precision_transformations/common/dequantization_operations.hpp"

using namespace testing;
using namespace ngraph::pass;
using namespace ngraph::builder::subgraph;

class MoveDequantizationAfterTransformationParams {
public:
    class Actual {
    public:
        ngraph::builder::subgraph::DequantizationOperations dequantization;
    };

    class Expected {
    public:
        ngraph::builder::subgraph::DequantizationOperations dequantizationBefore;
        ngraph::element::Type precisionAfterOperation;
        ngraph::builder::subgraph::DequantizationOperations dequantizationAfter;
    };

    ngraph::element::Type originalPrecision;
    ngraph::pass::low_precision::LayerTransformation::Params params;
    bool updatePrecision;
    bool moveSubtract;
    Actual actual;
    Expected expected;
};

typedef std::tuple<
    ngraph::Shape,
    MoveDequantizationAfterTransformationParams> MoveDequantizationAfterTransformationTestValues;

class MoveDequantizationAfterWithIntConstantTransformation :
    public LayerTransformation,
    public testing::WithParamInterface<MoveDequantizationAfterTransformationTestValues> {
public:
    void SetUp() override {
        const auto inputShape = std::get<0>(GetParam());
        const auto testValues = std::get<1>(GetParam());
        actualFunction = ngraph::builder::subgraph::MoveDequantizationAfterWithIntConstantFunction::getOriginal(
            testValues.originalPrecision,
            inputShape,
            testValues.actual.dequantization);

        const auto targetNode = actualFunction->get_output_op(0)->get_input_node_shared_ptr(0);
        const auto dequantization = ngraph::pass::low_precision::NetworkHelper::getDequantization(targetNode);
        ngraph::pass::low_precision::NetworkHelper::moveDequantizationAfter(
            targetNode,
            dequantization,
            testValues.updatePrecision,
            testValues.moveSubtract);

        referenceFunction = ngraph::builder::subgraph::MoveDequantizationAfterWithIntConstantFunction::getReference(
            testValues.originalPrecision,
            inputShape,
            testValues.expected.dequantizationBefore,
            testValues.expected.precisionAfterOperation,
            testValues.expected.dequantizationAfter);
    }

    static std::string getTestCaseName(testing::TestParamInfo<MoveDequantizationAfterTransformationTestValues> obj) {
        const auto inputShape = std::get<0>(obj.param);
        const auto testValues = std::get<1>(obj.param);

        std::ostringstream result;
        result <<
            testValues.originalPrecision << "_" <<
            inputShape << "_" <<
            testValues.actual.dequantization << "_" <<
            (testValues.moveSubtract ? "move_subtract_" : "don't_move_subtract_") <<
            (testValues.updatePrecision ? "updatePrecision" : "don't_update_precision");
        return result.str();
    }
};

TEST_P(MoveDequantizationAfterWithIntConstantTransformation, CompareFunctions) {
    actualFunction->validate_nodes_and_infer_types();
    auto res = compare_functions(referenceFunction, actualFunction, true, false, true);
    ASSERT_TRUE(res.first) << res.second;
}

const std::vector<ngraph::Shape> inputShapes = {
    { 1, 3, 16, 16 },
    { 4, 3, 16, 16 }
};

const std::vector<MoveDequantizationAfterTransformationParams> testValues = {
    // I8 & I8: Multiply
    {
        ngraph::element::i8,
        LayerTransformation::createParamsU8I8(),
        false,
        true,
        {
            { {},  {}, { {10.f}, ngraph::element::f32, {}, true, 1ul, ngraph::element::u8 } },
        },
        {
            { {},  {}, {} },
            ngraph::element::f32,
            { {},  {}, { 10.f } },
        },
    },
    // I8 & I8: Subtract + Multiply
    {
        ngraph::element::i8,
        LayerTransformation::createParamsU8I8(),
        false,
        true,
        {
            {
                {},
                { {5.f}, ngraph::element::f32, {}, true, 1ul, ngraph::element::u8 },
                { {10.f}, ngraph::element::f32, {}, true, 1ul, ngraph::element::u8 }
            },
        },
        {
            { {},  {}, {} },
            ngraph::element::f32,
            { {},  {5.f}, { 10.f } },
        },
    },
    // FP32 & I8: Multiply
    {
        ngraph::element::u8,
        LayerTransformation::createParamsU8I8(),
        false,
        true,
        {
            { {ngraph::element::f32},  {}, { {10.f}, ngraph::element::f32, {}, true, 1ul, ngraph::element::i8 } },
        },
        {
            { {},  {}, {} },
            ngraph::element::f32,
            { {},  {}, { 10.f } },
        },
    },
    // FP32 & I8: Subtract + Multiply
    {
        ngraph::element::u8,
        LayerTransformation::createParamsU8I8(),
        false,
        true,
        {
            {
                {ngraph::element::f32},
                { {5.f}, ngraph::element::f32, {}, true, 1ul, ngraph::element::i8 },
                { {10.f}, ngraph::element::f32, {}, true, 1ul, ngraph::element::i8 }
            },
        },
        {
            { {},  {}, {} },
            ngraph::element::f32,
            { {},  {5.f}, { 10.f } },
        },
    }
};

INSTANTIATE_TEST_CASE_P(
    LPT,
    MoveDequantizationAfterWithIntConstantTransformation,
    ::testing::Combine(
        ::testing::ValuesIn(inputShapes),
        ::testing::ValuesIn(testValues)),
    MoveDequantizationAfterWithIntConstantTransformation::getTestCaseName);
