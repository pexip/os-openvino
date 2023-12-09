// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "shared_test_classes/subgraph/preprocess.hpp"

#include <vector>

using namespace ov::test;

INSTANTIATE_TEST_SUITE_P(
    smoke_PrePostProcess,
    PrePostProcessTest,
    ::testing::Combine(::testing::ValuesIn(ov::builder::preprocess::generic_preprocess_functions()),
                       ::testing::Values(ov::test::utils::DEVICE_CPU)),
    PrePostProcessTest::getTestCaseName);