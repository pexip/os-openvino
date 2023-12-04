# Copyright (C) 2018-2023 Intel Corporation
# SPDX-License-Identifier: Apache-2.0

from openvino.tools.mo.middle.passes.convert_data_type import destination_type_to_np_data_type

from openvino.tools.mo.utils.graph import Node
from openvino.tools.mo.utils.ir_reader.extender import Extender


class EyeExtender(Extender):
    op = 'Eye'

    @staticmethod
    def extend(op: Node):
        if op.has_valid('output_type'):
            op['output_type'] = destination_type_to_np_data_type(op.output_type)
