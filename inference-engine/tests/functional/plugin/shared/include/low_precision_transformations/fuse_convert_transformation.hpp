// Copyright (C) 2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <string>
#include <memory>

#include "functional_test_utils/low_precision_transformations/layer_transformation.hpp"
#include "ngraph_functions/low_precision_transformations/common/fake_quantize_on_data.hpp"
#include "ngraph_functions/low_precision_transformations/common/dequantization_operations.hpp"

using namespace ngraph;

namespace LayerTestsDefinitions {

typedef std::tuple <
    element::Type,
    Shape,
    std::string,
    ngraph::builder::subgraph::DequantizationOperations,
    bool> FuseConvertTransformationParams;

class FuseConvertTransformation :
        public testing::WithParamInterface<FuseConvertTransformationParams>,
        public LayerTestsUtils::LayerTransformation {
public:
    static std::string getTestCaseName(testing::TestParamInfo<FuseConvertTransformationParams> obj);

protected:
    void SetUp() override;

private:
    void validate();
};

}  // namespace LayerTestsDefinitions
