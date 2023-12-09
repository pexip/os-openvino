// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "openvino/op/cos.hpp"

#include "element_visitor.hpp"
#include "itt.hpp"
#include "openvino/reference/cos.hpp"

using namespace std;

namespace ov {
namespace op {
namespace cos {
struct Evaluate : ov::element::NoAction<bool> {
    using ov::element::NoAction<bool>::visit;

    template <element::Type_t ET>
    static result_type visit(const Tensor& arg0, Tensor& out, const size_t count) {
        using T = typename element_type_traits<ET>::value_type;
        reference::cos(arg0.data<T>(), out.data<T>(), count);
        return true;
    }
};
}  // namespace cos

namespace v0 {
Cos::Cos(const Output<Node>& arg) : UnaryElementwiseArithmetic(arg) {
    constructor_validate_and_infer_types();
}

bool Cos::visit_attributes(AttributeVisitor& visitor) {
    OV_OP_SCOPE(v0_Cos_visit_attributes);
    return true;
}

shared_ptr<Node> Cos::clone_with_new_inputs(const OutputVector& new_args) const {
    OV_OP_SCOPE(v0_Cos_clone_with_new_inputs);
    check_new_args_count(this, new_args);
    return make_shared<Cos>(new_args.at(0));
}

bool Cos::evaluate(TensorVector& outputs, const TensorVector& inputs) const {
    OV_OP_SCOPE(v0_Cos_evaluate);
    OPENVINO_ASSERT(inputs.size() == 1 && outputs.size() == 1);
    outputs[0].set_shape(inputs[0].get_shape());

    using namespace ov::element;
    return IfTypeOf<i32, i64, u32, u64, f16, f32>::apply<cos::Evaluate>(inputs[0].get_element_type(),
                                                                        inputs[0],
                                                                        outputs[0],
                                                                        shape_size(inputs[0].get_shape()));
}

bool Cos::has_evaluate() const {
    OV_OP_SCOPE(v0_Cos_has_evaluate);
    switch (get_input_element_type(0)) {
    case element::i32:
    case element::i64:
    case element::u32:
    case element::u64:
    case element::f16:
    case element::f32:
        return true;
    default:
        return false;
    }
}
}  // namespace v0
}  // namespace op
}  // namespace ov