// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "helper_ops/loop_cond.hpp"

#include "common_op_table.hpp"
#include "openvino/frontend/tensorflow/node_context.hpp"
#include "utils.hpp"

using namespace std;
using namespace ov;
using namespace ov::op;
using namespace ov::frontend::tensorflow;

namespace ov {
namespace frontend {
namespace tensorflow {
namespace op {

OutputVector translate_loop_cond_op(const NodeContext& node) {
    default_op_checks(node, 1, {"LoopCond"});
    auto input = node.get_input(0);

    // TODO 123651: remove this fallback to the legacy FE once GPU fixes dynamism for Loop operation
    TENSORFLOW_OP_VALIDATION(node, false, "Fallback to legacy FE: Switch off TF1 While support due to GPU limitation");

    auto loop_cond_node = make_shared<LoopCond>(input, node.get_decoder());
    set_node_name(node.get_name(), loop_cond_node);

    return loop_cond_node->outputs();
}

}  // namespace op
}  // namespace tensorflow
}  // namespace frontend
}  // namespace ov