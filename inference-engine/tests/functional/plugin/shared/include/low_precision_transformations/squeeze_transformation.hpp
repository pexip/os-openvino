// Copyright (C) 2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <string>
#include <memory>

#include "functional_test_utils/low_precision_transformations/layer_transformation.hpp"
#include "ngraph_functions/low_precision_transformations/common/fake_quantize_on_data.hpp"

namespace LayerTestsDefinitions {

class SqueezeTransformationParam {
public:
    ngraph::builder::subgraph::FakeQuantizeOnData fakeQuantize;
    std::vector<float> squeezeAxes;
    ngraph::Shape shape;
};

std::string stringifySqueezeArgs(const std::vector<float>& axes);

typedef std::tuple<
    InferenceEngine::Precision,
    std::string,
    ngraph::pass::low_precision::LayerTransformation::Params,
    SqueezeTransformationParam
> SqueezeTransformationParams;

class SqueezeTransformation :
    public testing::WithParamInterface<SqueezeTransformationParams>,
    public LayerTestsUtils::LayerTransformation {
public:
    InferenceEngine::Blob::Ptr GenerateInput(const InferenceEngine::InputInfo& info) const override;
    static std::string getTestCaseName(testing::TestParamInfo<SqueezeTransformationParams> obj);

protected:
    void SetUp() override;
};

}  // namespace LayerTestsDefinitions
