# Copyright (C) 2018-2023 Intel Corporation
# SPDX-License-Identifier: Apache-2.0

import pytest
from common.layer_test_class import check_ir_version
from common.onnx_layer_test_class import OnnxRuntimeLayerTest

from unit_tests.utils.graph import build_graph


class TestAbs(OnnxRuntimeLayerTest):
    def create_net(self, shape, ir_version):
        """
            ONNX net                   IR net

            Input->Abs->Output   =>    Input->Abs

        """

        #
        #   Create ONNX model
        #

        import onnx
        from onnx import helper
        from onnx import TensorProto

        input = helper.make_tensor_value_info('input', TensorProto.FLOAT, shape)
        output = helper.make_tensor_value_info('output', TensorProto.FLOAT, shape)

        node_def = onnx.helper.make_node(
            'Abs',
            inputs=['input'],
            outputs=['output']
        )

        # Create the graph (GraphProto)
        graph_def = helper.make_graph(
            [node_def],
            'test_model',
            [input],
            [output],
        )

        # Create the model (ModelProto)
        onnx_net = helper.make_model(graph_def, producer_name='test_model')

        #
        #   Create reference IR net
        #
        ref_net = None
        if check_ir_version(10, None, ir_version):
            nodes_attributes = {
                'input': {'kind': 'op', 'type': 'Parameter'},
                'input_data': {'shape': shape, 'kind': 'data'},
                'node': {'kind': 'op', 'type': 'Abs'},
                'node_data': {'shape': shape, 'kind': 'data'},
                'result': {'kind': 'op', 'type': 'Result'}
            }
            ref_net = build_graph(nodes_attributes,
                                  [('input', 'input_data'),
                                   ('input_data', 'node'),
                                   ('node', 'node_data'),
                                   ('node_data', 'result')
                                   ])

        return onnx_net, ref_net

    def create_net_const(self, shape, precision, ir_version):
        """
            ONNX net                                  IR net

            Input->Concat(+abs const)->Output   =>    Input->Concat(+const)

        """

        #
        #   Create ONNX model
        #

        import onnx
        from onnx import helper
        from onnx import TensorProto
        import numpy as np

        concat_axis = 0
        output_shape = shape.copy()
        output_shape[concat_axis] *= 2

        input = helper.make_tensor_value_info('input', TensorProto.FLOAT, shape)
        output = helper.make_tensor_value_info('output', TensorProto.FLOAT, output_shape)

        constant = np.random.randint(-127, 127, shape).astype(float)

        node_const_def = onnx.helper.make_node(
            'Constant',
            inputs=[],
            outputs=['const'],
            value=helper.make_tensor(
                name='const_tensor',
                data_type=TensorProto.FLOAT,
                dims=constant.shape,
                vals=constant.flatten(),
            ),
        )

        node_def = onnx.helper.make_node(
            'Abs',
            inputs=['const'],
            outputs=['abs']
        )

        node_concat_def = onnx.helper.make_node(
            'Concat',
            inputs=['input', 'abs'],
            outputs=['output'],
            axis=concat_axis
        )

        # Create the graph (GraphProto)
        graph_def = helper.make_graph(
            [node_const_def, node_def, node_concat_def],
            'test_model',
            [input],
            [output],
        )

        # Create the model (ModelProto)
        onnx_net = helper.make_model(graph_def, producer_name='test_model')

        #
        #   Create reference IR net
        #
        constant = abs(constant)
        if precision == 'FP16':
            constant = constant.astype(np.float16)

        ref_net = None
        if check_ir_version(10, None, ir_version):
            nodes_attributes = {
                'input': {'kind': 'op', 'type': 'Parameter'},
                'input_data': {'shape': shape, 'kind': 'data'},
                'input_const_data': {'kind': 'data', 'value': constant.flatten()},
                'const': {'kind': 'op', 'type': 'Const'},
                'const_data': {'shape': shape, 'kind': 'data'},
                'concat': {'kind': 'op', 'type': 'Concat', 'axis': concat_axis},
                'concat_data': {'shape': output_shape, 'kind': 'data'},
                'result': {'kind': 'op', 'type': 'Result'}
            }

            ref_net = build_graph(nodes_attributes,
                                  [('input', 'input_data'),
                                   ('input_const_data', 'const'),
                                   ('const', 'const_data'),
                                   ('input_data', 'concat'),
                                   ('const_data', 'concat'),
                                   ('concat', 'concat_data'),
                                   ('concat_data', 'result')
                                   ])

        return onnx_net, ref_net

    test_data = [dict(shape=[10, 12]),
                 dict(shape=[8, 10, 12]),
                 dict(shape=[6, 8, 10, 12]),
                 dict(shape=[4, 6, 8, 10, 12])]

    @pytest.mark.parametrize("params", test_data)
    @pytest.mark.nightly
    def test_abs(self, params, ie_device, precision, ir_version, temp_dir, use_old_api):
        self._test(*self.create_net(**params, ir_version=ir_version), ie_device, precision,
                   ir_version, temp_dir=temp_dir, use_old_api=use_old_api)

    @pytest.mark.parametrize("params", test_data)
    @pytest.mark.nightly
    def test_abs_const(self, params, ie_device, precision, ir_version, temp_dir, use_old_api):
        self._test(*self.create_net_const(**params, precision=precision, ir_version=ir_version),
                   ie_device, precision, ir_version, temp_dir=temp_dir, use_old_api=use_old_api)