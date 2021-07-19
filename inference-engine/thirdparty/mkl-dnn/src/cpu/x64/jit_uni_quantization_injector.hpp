/*******************************************************************************
* Copyright 2019-2020 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#ifndef CPU_X64_JIT_UNI_QUANTIZATION_INJECTOR_HPP
#define CPU_X64_JIT_UNI_QUANTIZATION_INJECTOR_HPP

#include <assert.h>

#include "common/c_types_map.hpp"
#include "common/primitive_attr.hpp"
#include "common/type_helpers.hpp"
#include "common/utils.hpp"

#include "cpu/x64/jit_generator.hpp"

namespace dnnl {
namespace impl {
namespace cpu {
namespace x64 {

template <cpu_isa_t isa, typename Vmm = typename cpu_isa_traits<isa>::Vmm>
struct jit_uni_quantization_injector_f32 {
    jit_uni_quantization_injector_f32(jit_generator* host, dnnl_post_ops::entry_t post_op,
            Vmm vmm_d_weights, Vmm vmm_d_bias, Xbyak::Reg64 reg_d_weights, Xbyak::Reg64 reg_d_bias)
        : h(host), post_op_(post_op), vmm_d_weights_(vmm_d_weights), vmm_d_bias_(vmm_d_bias), reg_d_weights_(reg_d_weights), reg_d_bias_(reg_d_bias) {
        assert(post_op.is_quantization());
        assert(utils::one_of(post_op.quantization.alg, alg_kind::quantization_quantize, alg_kind::quantization_quantize_dequantize));

        do_dequantization = post_op_.quantization.alg == alg_kind::quantization_quantize_dequantize;

        xmm_d_weights_ = Xbyak::Xmm(vmm_d_weights.getIdx());
        xmm_d_bias_ = Xbyak::Xmm(vmm_d_bias.getIdx());
    }

    void init_crop_ptrs(const Xbyak::Operand& ch_off);
    void init_input_scale_shift_ptrs(const Xbyak::Operand& ch_off);
    void init_output_scale_shift_ptrs(const Xbyak::Operand& ch_off);

    void compute_crop(int start_idx, int end_idx, int offset, bool is_scalar = false, bool is_broadcast = false);
    void compute_input_scale_shift(int start_idx, int end_idx, int offset, bool do_rounding, bool is_scalar = false, bool is_broadcast = false);
    void compute_output_scale_shift(int start_idx, int end_idx, int offset, bool is_scalar = false, bool is_broadcast = false);

private:
    jit_generator* h;

    size_t vlen = cpu_isa_traits<isa>::vlen;

    dnnl_post_ops::entry_t post_op_;

    Vmm vmm_d_weights_;
    Vmm vmm_d_bias_;
    Xbyak::Xmm xmm_d_weights_;
    Xbyak::Xmm xmm_d_bias_;

    Xbyak::Reg64 reg_d_weights_;
    Xbyak::Reg64 reg_d_bias_;

    bool do_dequantization;
};

} // namespace x64
} // namespace cpu
} // namespace impl
} // namespace dnnl

#endif
