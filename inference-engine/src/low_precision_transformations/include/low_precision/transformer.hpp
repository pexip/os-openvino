// Copyright (C) 2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <ngraph/ngraph.hpp>
#include <ngraph_ops/type_relaxed.hpp>

#include "layer_transformation.hpp"
#include "iparams_manager.hpp"
#include "ilayer_transformations_manager.hpp"

namespace ngraph {
namespace pass {
namespace low_precision {

struct StandaloneCleanup {
    std::string typeName;
    std::string typeId;
    LayerTransformationPtr transformation;
};

class TRANSFORMATIONS_API LowPrecisionTransformations {
public:
    LowPrecisionTransformations() {}
    LowPrecisionTransformations(
        const std::map<std::string, LayerTransformationPtr>& branchSpecificTransformations,
        const std::map<std::string, LayerTransformationPtr>& transformations,
        const std::map<std::string, std::vector<std::pair<std::string, LayerTransformationPtr>>>& cleanupTransformations,
        const std::vector<StandaloneCleanup>& standaloneCleanupTransformations);

    void setUpdatePrecisions(const bool updatePrecisions);
    void setQuantizedTensorAlignmentOnActivations(const LayerTransformation::QuantizedTensorAlignment quantizedTensorAlignmentOnActivations);
    void setQuantizedTensorAlignmentOnWeights(const LayerTransformation::QuantizedTensorAlignment quantizedTensorAlignmentOnWeights);
    LowPrecisionTransformations& remove(const std::string& operationType);
    LowPrecisionTransformations& removeBranchSpecificTransformations(const std::string& operationType);
    LowPrecisionTransformations& removeTransformations(const std::string& operationType);
    LowPrecisionTransformations& removeCleanupTransformations(const std::string& operationType);

    /**
     * Add branch specific transformation. Transformation type and operation type are required.
     * Operation type is used to find transformation by operation during precision definition.
     */
    template <class Transformation, class Operation>
    LowPrecisionTransformations& addBranchSpecific(const LayerTransformation::Params& params) {
        const std::string typeName = getType<Operation>();
        const auto it = branchSpecificTransformations.find(typeName);
        if (it != branchSpecificTransformations.end()) {
            branchSpecificTransformations.erase(it);
        }

        branchSpecificTransformations.emplace(typeName, std::make_shared<Transformation>(params));
        return *this;
    }

    /**
     * Add transformation. Transformation type and operation type are required.
     * Operation type is used to find transformation by operation during precision definition.
     */
    template <class Transformation, class Operation>
    LowPrecisionTransformations& add(const LayerTransformation::Params& params) {
        const std::string typeName = getType<Operation>();
        const auto it = transformations.find(typeName);
        if (it != transformations.end()) {
            transformations.erase(it);
        }

        transformations.emplace(typeName, std::make_shared<Transformation>(params));
        return *this;
    }

    /**
     * Add cleanup transformation. Transformation type and operation type are required.
     * Operation type is used to find transformation by operation during precision definition.
     */
    template <class Transformation, class Operation>
    LowPrecisionTransformations& addCleanup(const LayerTransformation::Params& params) {
        const std::string typeName = getType<Operation>();
        const std::string typeId = typeid(Transformation).name();
        const auto it = cleanupTransformations.find(typeName);
        if (it == cleanupTransformations.end()) {
            cleanupTransformations.emplace(typeName,
                std::vector<std::pair<std::string, LayerTransformationPtr>>{ std::make_pair(typeId, std::make_shared<Transformation>(params)) });
        } else {
            const auto it1 = std::find_if(it->second.begin(), it->second.end(),
                [&](const std::pair<std::string, LayerTransformationPtr>& transformation) {
                    return transformation.first == typeName;
                });
            if (it1 != it->second.end()) {
                it->second.erase(it1);
            }
            it->second.emplace_back(std::make_pair(typeId, std::make_shared<Transformation>(params)));
        }
        return *this;
    }

    /**
     * Add cleanup transformation. Transformation type and operation type are required.
     * Operation type is used to find transformation by operation during precision definition.
     */
    template <class Transformation, class Operation>
    LowPrecisionTransformations& addStandaloneCleanup(const LayerTransformation::Params& params) {
        const std::string typeName = getType<Operation>();
        const std::string typeId = typeid(Transformation).name();
        const auto it = std::find_if(standaloneCleanupTransformations.begin(), standaloneCleanupTransformations.end(),
            [&](const StandaloneCleanup& transformation) {
                return transformation.typeName == typeName && transformation.typeId == typeId;
            });
        if (it == standaloneCleanupTransformations.end()) {
            standaloneCleanupTransformations.emplace_back(StandaloneCleanup{ typeName, typeId, std::make_shared<Transformation>(params) });
        } else {
            *it = { typeName, typeId, std::make_shared<Transformation>(params) };
        }

        return *this;
    }

    template <class Operation>
    static std::string getType() {
        return Operation::get_type_info_static().name;
    }

    static std::string getType(const Node& operation) {
        return operation.get_type_name();
    }

    std::vector<LayerTransformationPtr> find(const std::string& transformationName) const;

    template <class Operation>
    std::vector<LayerTransformationPtr> find() const {
        const std::string transformationKey = getType<Operation>();
        return find(transformationKey);
    }

    void setParamsManager(IParamsManager* paramsManager) noexcept;
    void setLayerTransformationsManager(ILayerTransformationsManager* layerTransformationsManager) noexcept;

    // Key is not a layer type, but just a name of transformation
    // Layer type (or a pattern) is defined by transformation itself as an ngraph matcher
    std::map<std::string, LayerTransformationPtr> branchSpecificTransformations;
    std::map<std::string, LayerTransformationPtr> transformations;
    std::map<std::string, std::vector<std::pair<std::string, LayerTransformationPtr>>> cleanupTransformations;
    std::vector<StandaloneCleanup> standaloneCleanupTransformations;

private:
    static void setParamsManager(IParamsManager* paramsManager, std::map<std::string, LayerTransformationPtr>& transformations) noexcept;
    static void setParamsManager(
        IParamsManager* paramsManager,
        std::map<std::string, std::vector<std::pair<std::string, LayerTransformationPtr>>>& transformations) noexcept;
    static void setParamsManager(IParamsManager* paramsManager, std::vector<StandaloneCleanup>& transformations) noexcept;
    static void setLayerTransformationsManager(
        ILayerTransformationsManager* layerTransformationsManager,
        std::map<std::string, LayerTransformationPtr>& transformations) noexcept;
    static void setLayerTransformationsManager(
        ILayerTransformationsManager* layerTransformationsManager,
        std::map<std::string, std::vector<std::pair<std::string, LayerTransformationPtr>>>& transformations) noexcept;
    static void setLayerTransformationsManager(
        ILayerTransformationsManager* layerTransformationsManager,
        std::vector<StandaloneCleanup>& transformations) noexcept;
};

/**
 * @brief low precision transformation component.
  */
class TRANSFORMATIONS_API LowPrecisionTransformer : public IParamsManager, ILayerTransformationsManager {
public:
    static LowPrecisionTransformations getAllTransformations(const LayerTransformation::Params& params = LayerTransformation::Params());

    static bool isFunctionQuantized(const std::shared_ptr<Function>& function);

    LowPrecisionTransformer();
    LowPrecisionTransformer(const LowPrecisionTransformations& transformations);
    void transform(std::shared_ptr<Function> network);

    // IParamsManager interface implementation
    std::vector<element::Type> getPrecisionsOnActivations(const Node& op) const noexcept override;

    // ILayerTransformationsManager interface implementation
    bool isQuantized(const std::shared_ptr<Node>& layer) const noexcept override;
    bool isPrecisionPreserved(const std::shared_ptr<Node>& layer) const noexcept override;

private:
    LowPrecisionTransformations transformations;

    void registerAllMatchers(
        std::map<std::string, LayerTransformationPtr> transformations,
        GraphRewrite& pass,
        TransformationContext& context);

    void registerAllMatchers(
        std::map<std::string, std::vector<std::pair<std::string, LayerTransformationPtr>>> transformations,
        GraphRewrite& pass,
        TransformationContext& context);

    std::vector<element::Type> precisionIntersection(
        const std::vector<element::Type>& v1,
        const std::vector<element::Type>& v2) const noexcept;
};

class TRANSFORMATIONS_API TypeRelaxedReplacer : public GraphRewrite {
public:
    TypeRelaxedReplacer();
};

} // namespace low_precision
} // namespace pass
} // namespace ngraph
