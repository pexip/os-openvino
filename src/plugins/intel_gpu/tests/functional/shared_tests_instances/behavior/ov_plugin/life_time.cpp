// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "behavior/ov_plugin/life_time.hpp"

using namespace ov::test::behavior;
namespace {
    INSTANTIATE_TEST_SUITE_P(smoke_BehaviorTests, OVHoldersTest,
            ::testing::Values(ov::test::utils::DEVICE_GPU),
            OVHoldersTest::getTestCaseName);

    INSTANTIATE_TEST_SUITE_P(smoke_VirtualPlugin_BehaviorTests, OVHoldersTest,
                    ::testing::Values(//ov::test::utils::DEVICE_BATCH,
                                        "HETERO:GPU"),
            OVHoldersTest::getTestCaseName);
}  // namespace