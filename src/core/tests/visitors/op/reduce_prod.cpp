// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "openvino/op/reduce_prod.hpp"

#include "reduce_ops.hpp"

using Type = ::testing::Types<ReduceOperatorType<ov::op::v1::ReduceProd, ov::element::f32>>;
INSTANTIATE_TYPED_TEST_SUITE_P(attributes_reduce_op, ReduceOperatorVisitor, Type, ReduceOperatorTypeName);