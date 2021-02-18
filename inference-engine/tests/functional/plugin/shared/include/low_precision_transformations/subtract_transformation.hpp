// Copyright (C) 2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <string>
#include <memory>

#include "functional_test_utils/low_precision_transformations/layer_transformation.hpp"

namespace LayerTestsDefinitions {

class SubtractTransformation :
    public testing::WithParamInterface<LayerTestsUtils::LayerTransformationParams>,
    public LayerTestsUtils::LayerTransformation {
public:
    static std::string getTestCaseName(testing::TestParamInfo<LayerTestsUtils::LayerTransformationParams> obj);

protected:
    void SetUp() override;
};

}  // namespace LayerTestsDefinitions
