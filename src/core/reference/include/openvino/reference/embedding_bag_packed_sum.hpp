// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "openvino/core/shape.hpp"

namespace ov {
namespace reference {
template <typename T, typename U>
void embeddingBagPackedSum(const T* emb_table,
                           const U* indices,
                           const T* weights,
                           T* out,
                           const Shape& indicesShape,
                           const Shape& outShape) {
    const size_t indices_per_bag = indicesShape[1];

    size_t embDepth = 1lu;
    for (size_t i = 1; i < outShape.size(); i++) {
        embDepth *= outShape[i];
    }
    std::fill(out, out + shape_size(outShape), T{0});

    bool with_weights = (weights != nullptr);
    size_t idx_idx = 0lu;

    for (size_t obi = 0lu; obi < outShape.at(0); obi++) {
        size_t dst_index = obi * embDepth;
        for (size_t in_idx = 0lu; in_idx < indices_per_bag; in_idx++, idx_idx++) {
            size_t src_index = indices[idx_idx] * embDepth;

            if (with_weights) {
                for (size_t i = 0lu; i < embDepth; i++) {
                    out[dst_index + i] += emb_table[src_index + i] * weights[idx_idx];
                }
            } else {
                for (size_t i = 0lu; i < embDepth; i++) {
                    out[dst_index + i] += emb_table[src_index + i];
                }
            }
        }
    }

}  // embeddingBagPackedSum

}  // namespace reference
}  // namespace ov
