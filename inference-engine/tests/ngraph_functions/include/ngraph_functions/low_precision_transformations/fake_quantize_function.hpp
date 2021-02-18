// Copyright (C) 2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <algorithm>
#include <memory>
#include <ngraph/ngraph.hpp>
#include "low_precision/layer_transformation.hpp"
#include "common/fake_quantize_on_data.hpp"

namespace ngraph {
namespace builder {
namespace subgraph {

class FakeQuantizeFunction {
public:
    static std::shared_ptr<ngraph::Function> getOriginal(
        const ngraph::element::Type precision,
        const ngraph::Shape& inputShape,
        const FakeQuantizeOnData& fakeQuantizeOnData);

    static std::shared_ptr<ngraph::Function> getReference(
        const ngraph::element::Type precision,
        const ngraph::Shape& inputShape,
        const bool updatePrecisions,
        const FakeQuantizeOnData& fakeQuantizeOnData,
        const ngraph::element::Type fakeQuantizeOutputPrecision,
        const std::vector<float>& expectedSubtractValues,
        const std::vector<float>& expectedMultiplyValues);
};

}  // namespace subgraph
}  // namespace builder
}  // namespace ngraph
