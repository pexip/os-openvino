// Copyright (C) 2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "ngraph_functions/low_precision_transformations/move_dequantization_after_with_int_constant_function.hpp"
#include "low_precision/network_helper.hpp"

#include <ngraph/opsets/opset1.hpp>
#include "ngraph_functions/subgraph_builders.hpp"
#include "ngraph_functions/low_precision_transformations/common/builders.hpp"

using namespace ngraph::pass::low_precision;

namespace ngraph {
namespace builder {
namespace subgraph {
    std::shared_ptr<ngraph::Function> MoveDequantizationAfterWithIntConstantFunction::getOriginal(
        const ngraph::element::Type precision,
        const ngraph::Shape& inputShape,
        const ngraph::builder::subgraph::DequantizationOperations dequantization) {
        const auto input = std::make_shared<ngraph::op::v0::Parameter>(precision, inputShape);

        const auto deq = makeDequantization(input, dequantization);
        const auto avgPool = std::make_shared<op::TypeRelaxed<opset1::AvgPool>>(
            ngraph::opset1::AvgPool(
                deq,
                Strides{ 1, 1 },
                Shape{ 1, 1 },
                Shape{ 0, 0 },
                Shape{ 2, 2 },
                true,
                op::RoundingType::FLOOR),
            std::vector<element::Type>{ element::f32, element::f32 },
            std::vector<element::Type>{});

        auto& rtInfo = avgPool->get_rt_info();
        rtInfo["Variant::std::string"] = std::make_shared<VariantWrapper<std::string>>("targetOp");

        return std::make_shared<ngraph::Function>(
            ngraph::ResultVector{ std::make_shared<ngraph::opset1::Result>(avgPool) },
            ngraph::ParameterVector{ input },
            "MoveDequantizationAfterFunction");
    }

    std::shared_ptr<ngraph::Function> MoveDequantizationAfterWithIntConstantFunction::getReference(
        const ngraph::element::Type precision,
        const ngraph::Shape& inputShape,
        const ngraph::builder::subgraph::DequantizationOperations dequantizationBefore,
        const ngraph::element::Type precisionAfterOperation,
        const ngraph::builder::subgraph::DequantizationOperations dequantizationAfter) {
        const auto input = std::make_shared<ngraph::op::v0::Parameter>(precision, inputShape);

        const auto deqBefore = makeDequantization(input, dequantizationBefore);
        const auto targetOp = std::make_shared<op::TypeRelaxed<opset1::AvgPool>>(
            ngraph::opset1::AvgPool(
                deqBefore,
                Strides{ 1, 1 },
                Shape{ 1, 1 },
                Shape{ 0, 0 },
                Shape{ 2, 2 },
                true,
                op::RoundingType::FLOOR),
            std::vector<element::Type>{ element::f32, element::f32 },
            std::vector<element::Type>{});
        ngraph::pass::low_precision::NetworkHelper::setOutDataPrecisionForTypeRelaxed(targetOp, precisionAfterOperation);
        auto& rtInfo = targetOp->get_rt_info();
        rtInfo["Variant::std::string"] = std::make_shared<VariantWrapper<std::string>>("targetOp");

        const auto deqAfter = makeDequantization(targetOp, dequantizationAfter);

        return std::make_shared<ngraph::Function>(
            ngraph::ResultVector{ std::make_shared<ngraph::opset1::Result>(deqAfter) },
            ngraph::ParameterVector{ input },
            "MoveDequantizationAfterFunction");
    }

}  // namespace subgraph
}  // namespace builder
}  // namespace ngraph
