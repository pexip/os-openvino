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

#include "common/c_types_map.hpp"
#include "common/dnnl_thread.hpp"
#include "common/nstl.hpp"
#include "common/utils.hpp"

#include "cpu/x64/jit_uni_depthwise_injector.hpp"

namespace dnnl {
namespace impl {
namespace cpu {
namespace x64 {

template <cpu_isa_t isa>
int jit_uni_depthwise_injector_f32<isa>::aux_vecs_count(alg_kind_t depthwise_alg, bool is_broadcast) {
    switch (depthwise_alg) {
        case alg_kind::depthwise_scale_shift: return isa == sse41 || is_broadcast ? 1 : 0;
        case alg_kind::depthwise_prelu: return 2;
        default: assert(!"unsupported depthwise algorithm");
    }

    return 0;
}

template <cpu_isa_t isa>
void jit_uni_depthwise_injector_f32<isa>::injector_preamble(size_t start_idx, size_t end_idx, bool is_broadcast) {
    preserved_vecs_count = 0;
    vecs_to_preserve = (size_t)jit_uni_depthwise_injector_f32<isa>::aux_vecs_count(depthwise_alg, is_broadcast);

    for (size_t i = 0; i < vecs_count; i++) {
        if (preserved_vecs_count >= vecs_to_preserve)
            break;

        if (i < start_idx || i >= end_idx) {
            preserved_vec_idxs[preserved_vecs_count] = i;
            preserved_vecs_count++;
        }
    }

    start_idx_tail = start_idx;
    size_t preserved_vecs_count_tail = vecs_to_preserve - preserved_vecs_count;
    for (size_t i = 0; i < preserved_vecs_count_tail; i++) {
        preserved_vec_idxs[preserved_vecs_count] = start_idx + i;
        preserved_vecs_count++;
        start_idx_tail = start_idx + i + 1;
    }

    h->sub(h->rsp, preserved_vecs_count * vlen);
    for (size_t i = 0; i < preserved_vecs_count; ++i)
        h->uni_vmovups(h->ptr[h->rsp + i * vlen], Vmm(preserved_vec_idxs[i]));

    assign_regs();
}

template <cpu_isa_t isa>
void jit_uni_depthwise_injector_f32<isa>::injector_preamble_tail(size_t start_idx, size_t end_idx) {
    size_t tail_vecs_to_preserve = start_idx_tail - start_idx;
    int idx_off = (vecs_to_preserve - tail_vecs_to_preserve);

    if (tail_vecs_to_preserve > 0) {
        h->add(h->rsp, idx_off * vlen);
        for (size_t i = 0; i < tail_vecs_to_preserve; ++i)
            h->uni_vmovups(Vmm(preserved_vec_idxs[idx_off + i]), h->ptr[h->rsp + i * vlen]);

        for (size_t i = 0; i < tail_vecs_to_preserve; ++i) {
            preserved_vec_idxs[idx_off + i] += tail_vecs_to_preserve;
        }

        for (size_t i = 0; i < tail_vecs_to_preserve; ++i)
            h->uni_vmovups(h->ptr[h->rsp + i * vlen], Vmm(preserved_vec_idxs[idx_off + i]));
        h->sub(h->rsp, idx_off * vlen);

        assign_regs();
    }
}

template <cpu_isa_t isa>
void jit_uni_depthwise_injector_f32<isa>::injector_postamble() {
    for (size_t i = 0; i < preserved_vecs_count; ++i)
        h->uni_vmovups(Vmm(preserved_vec_idxs[i]), h->ptr[h->rsp + i * vlen]);
    h->add(h->rsp, preserved_vecs_count * vlen);
}

template <cpu_isa_t isa>
void jit_uni_depthwise_injector_f32<isa>::assign_regs() {
    vmm_mask = Vmm(preserved_vec_idxs[0]);
    vmm_aux0 = Vmm(preserved_vec_idxs[1]);
}

template <cpu_isa_t isa>
void jit_uni_depthwise_injector_f32<isa>::scale_shift_compute_vector(const Vmm &vmm_src,
        const Xbyak::Reg64& p_weights, const Xbyak::Reg64& p_bias, bool is_broadcast) {
    if (isa == sse41) {
        if (is_broadcast)
            h->uni_vbroadcastss(vmm_mask, h->ptr[p_weights]);
        else
            h->movups(vmm_mask, h->ptr[p_weights]);
        h->mulps(vmm_src, vmm_mask);
        if (is_broadcast)
            h->uni_vbroadcastss(vmm_mask, h->ptr[p_bias]);
        else
            h->movups(vmm_mask, h->ptr[p_bias]);
        h->addps(vmm_src, vmm_mask);
    } else {
        if (is_broadcast) {
            h->uni_vbroadcastss(vmm_mask, h->ptr[p_weights]);
            h->uni_vmulps(vmm_src, vmm_src, vmm_mask);
            h->uni_vbroadcastss(vmm_mask, h->ptr[p_bias]);
            h->uni_vaddps(vmm_src, vmm_src, vmm_mask);
        } else {
            h->uni_vmulps(vmm_src, vmm_src, h->ptr[p_weights]);
            h->uni_vaddps(vmm_src, vmm_src, h->ptr[p_bias]);
        }
    };
}

template <cpu_isa_t isa>
void jit_uni_depthwise_injector_f32<isa>::prelu_compute_vector(const Vmm &vmm_src,
        const Xbyak::Reg64& p_weights, const Xbyak::Reg64& p_bias, bool is_broadcast) {
    const unsigned char _cmp_gt_os = 6;
    const unsigned char _cmp_lt_os = 1;

    if (isa == sse41) {
        h->pxor(vmm_mask, vmm_mask);
        h->cmpps(vmm_mask, vmm_src, _cmp_gt_os);
        if (is_broadcast)
            h->uni_vbroadcastss(vmm_aux0, h->ptr[p_weights]);
        else
            h->movups(vmm_aux0, h->ptr[p_weights]);
        h->mulps(vmm_aux0, vmm_src);
        h->blendvps(vmm_src, vmm_aux0);
    } else if (isa == avx2) {
        if (is_broadcast) {
            h->uni_vbroadcastss(vmm_mask, h->ptr[p_weights]);
            h->vmulps(vmm_aux0, vmm_src, vmm_mask);
        } else
            h->vmulps(vmm_aux0, vmm_src, h->ptr[p_weights]);
        h->vxorps(vmm_mask, vmm_mask, vmm_mask);
        h->vcmpgtps(vmm_mask, vmm_src, vmm_mask);
        h->vblendvps(vmm_src, vmm_aux0, vmm_src, vmm_mask);
    } else if (isa == avx512_common) {
        h->vxorpd(vmm_mask, vmm_mask, vmm_mask);
        h->vmovups(vmm_aux0, vmm_src);
        h->vcmpps(k_mask, vmm_src, vmm_mask, _cmp_lt_os);
        if (is_broadcast) {
            h->uni_vbroadcastss(vmm_mask, h->ptr[p_weights]);
            h->vmulps(vmm_src | k_mask, vmm_aux0, vmm_mask);
        } else
            h->vmulps(vmm_src | k_mask, vmm_aux0, h->ptr[p_weights]);
    }
}

template <cpu_isa_t isa>
void jit_uni_depthwise_injector_f32<isa>::compute_body(size_t start_idx, size_t end_idx,
        const Xbyak::Reg64& p_weights, const Xbyak::Reg64& p_bias, bool is_broadcast) {
    for (size_t idx = start_idx; idx < end_idx; idx++) {
        switch (depthwise_alg) {
            case alg_kind::depthwise_scale_shift:
                scale_shift_compute_vector(Vmm(idx), p_weights, p_bias, is_broadcast); break;
            case alg_kind::depthwise_prelu:
                prelu_compute_vector(Vmm(idx), p_weights, p_bias, is_broadcast); break;
            default: assert(!"unsupported depthwise algorithm");
        }
    }
}

template <cpu_isa_t isa>
void jit_uni_depthwise_injector_f32<isa>::compute_vector_range(int start_idx, int end_idx,
        const Xbyak::Reg64& p_weights, const Xbyak::Reg64& p_bias, bool is_broadcast) {
    injector_preamble(start_idx, end_idx, is_broadcast);
    compute_body(start_idx_tail, end_idx, p_weights, p_bias, is_broadcast);
    injector_preamble_tail(start_idx, end_idx);
    compute_body(start_idx, start_idx_tail, p_weights, p_bias, is_broadcast);
    injector_postamble();
}

template struct jit_uni_depthwise_injector_f32<avx512_common>;
template struct jit_uni_depthwise_injector_f32<avx2>;
template struct jit_uni_depthwise_injector_f32<sse41>;

} // namespace x64
} // namespace cpu
} // namespace impl
} // namespace dnnl
