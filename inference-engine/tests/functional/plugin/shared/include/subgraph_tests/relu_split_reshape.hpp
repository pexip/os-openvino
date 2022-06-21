// Copyright (C) 2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "shared_test_classes/subgraph/relu_split_reshape.hpp"

namespace SubgraphTestsDefinitions {

TEST_P(ReluSplitReshape, CompareWithRefs) {
    Run();
};

}  // namespace SubgraphTestsDefinitions
