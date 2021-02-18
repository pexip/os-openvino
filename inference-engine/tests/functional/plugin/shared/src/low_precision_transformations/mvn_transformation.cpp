// Copyright (C) 2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "low_precision_transformations/mvn_transformation.hpp"

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
#include "ngraph_functions/low_precision_transformations/mvn_function.hpp"

#include <ngraph/pass/visualize_tree.hpp>

namespace LayerTestsDefinitions {

std::string MVNTransformation::getTestCaseName(testing::TestParamInfo<MVNTransformationParams> obj) {
    std::string targetDevice;
    ngraph::Shape shape;
    ngraph::element::Type precision;
    auto params = LayerTestsUtils::LayerTransformationParamsNGraphFactory::createParamsU8I8();
    ngraph::AxisSet reductionAxes;
    bool normalizeVariance;
    std::tie(precision, shape, targetDevice, reductionAxes, normalizeVariance) = obj.param;

    std::ostringstream result;
    result << getTestCaseNameByParams(precision, shape, targetDevice, params) <<
        "_" << reductionAxes << "_" << normalizeVariance;
    return result.str();
}

void MVNTransformation::SetUp() {
    ngraph::Shape shape;
    ngraph::element::Type precision;
    auto params = LayerTestsUtils::LayerTransformationParamsNGraphFactory::createParamsU8I8();
    ngraph::AxisSet reductionAxes;
    bool normalizeVariance;
    std::tie(precision, shape, targetDevice, reductionAxes, normalizeVariance) = this->GetParam();

    function = ngraph::builder::subgraph::MVNFunction::getOriginal(
        precision,
        shape,
        reductionAxes,
        normalizeVariance);
}

TEST_P(MVNTransformation, CompareWithRefImpl) {
    Run();
};

}  // namespace LayerTestsDefinitions
