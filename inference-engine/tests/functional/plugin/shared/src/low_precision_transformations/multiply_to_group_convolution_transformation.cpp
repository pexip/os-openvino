// Copyright (C) 2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "low_precision_transformations/multiply_to_group_convolution_transformation.hpp"

#include <memory>
#include <tuple>
#include <vector>
#include <string>

#include <ie_core.hpp>

#include "common_test_utils/common_utils.hpp"
#include "functional_test_utils/plugin_cache.hpp"
#include "functional_test_utils/layer_test_utils.hpp"
#include "functional_test_utils/blob_utils.hpp"

#include "ngraph_functions/pass/convert_prc.hpp"
#include "ngraph_functions/low_precision_transformations/multiply_to_group_convolution_function.hpp"

#include <ngraph/pass/visualize_tree.hpp>

namespace LayerTestsDefinitions {

std::string MultiplyToGroupConvolutionTransformation::getTestCaseName(testing::TestParamInfo<MultiplyToGroupConvolutionTransformationParams> obj) {
    std::string targetDevice;
    ngraph::element::Type precision;
    ngraph::Shape shape;
    auto params = LayerTestsUtils::LayerTransformationParamsNGraphFactory::createParamsU8I8();
    builder::subgraph::FakeQuantizeOnData fqOnData;
    std::tie(precision, shape, targetDevice, fqOnData) = obj.param;

    std::ostringstream result;
    result << getTestCaseNameByParams(precision, shape, targetDevice, params) << "_" << fqOnData;
    return result.str();
}

void MultiplyToGroupConvolutionTransformation::SetUp() {
    ngraph::Shape shape;
    ngraph::element::Type precision;
    auto params = LayerTestsUtils::LayerTransformationParamsNGraphFactory::createParamsU8I8();
    builder::subgraph::FakeQuantizeOnData fqOnData;
    std::tie(precision, shape, targetDevice, fqOnData) = this->GetParam();

    function = ngraph::builder::subgraph::MultiplyToGroupConvolutionFunction::getOriginal(
        precision,
        shape,
        fqOnData);
}

TEST_P(MultiplyToGroupConvolutionTransformation, CompareWithRefImpl) {
    Run();
};

}  // namespace LayerTestsDefinitions
