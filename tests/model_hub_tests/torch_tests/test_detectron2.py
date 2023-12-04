# Copyright (C) 2018-2023 Intel Corporation
# SPDX-License-Identifier: Apache-2.0

import os
import pytest
import torch
from models_hub_common.test_convert_model import TestConvertModel
from openvino import convert_model
from models_hub_common.utils import get_models_list, compare_two_tensors


class TestDetectron2ConvertModel(TestConvertModel):
    def setup_class(self):
        from PIL import Image
        import requests

        url = "http://images.cocodataset.org/val2017/000000039769.jpg"
        self.image = Image.open(requests.get(url, stream=True).raw)
        self.image = self.image.resize([640, 480])

    def load_model(self, model_name, model_link):
        from detectron2 import model_zoo, export
        from detectron2.modeling import build_model, PanopticFPN
        from detectron2.checkpoint import DetectionCheckpointer
        from detectron2.config import CfgNode
        import torchvision.transforms as transforms

        transform = transforms.Compose([transforms.PILToTensor()])
        image = transform(self.image)

        cfg = model_zoo.get_config(model_name + ".yaml", trained=True)
        assert isinstance(cfg, CfgNode), "Unexpected config"
        cfg.MODEL.DEVICE = "cpu"
        model = build_model(cfg)
        DetectionCheckpointer(
            model, save_to_disk=False).load(cfg.MODEL.WEIGHTS)

        model.eval()
        inputs = [{"image": image,
                   "height": torch.tensor(image.shape[1]),
                   "width": torch.tensor(image.shape[2])}]
        # https://github.com/facebookresearch/detectron2/blob/4e80df1e58901557e2824ce3b488d30209a9be33/tools/deploy/export_model.py#L123
        # This is done only for Panoptic models, but it may be incorrect to do that, because one of outputs of panoptic model is getting lost
        if isinstance(model, PanopticFPN):
            def inference(model, inputs):
                # use do_postprocess=False so it returns ROI mask
                inst = model.inference(inputs, do_postprocess=False)[0]
                return [{"instances": inst}]
        else:
            inference = None  # assume that we just call the model directly
        adapter = export.TracingAdapter(model, inputs, inference)

        self.example = adapter.flattened_inputs
        return adapter

    def get_inputs_info(self, model_obj):
        return None

    def prepare_inputs(self, inputs_info):
        return [i.numpy() for i in self.example]

    def convert_model(self, model_obj):
        ov_model = convert_model(model_obj, example_input=self.example)
        return ov_model

    def infer_fw_model(self, model_obj, inputs):
        fw_outputs = model_obj(*[torch.from_numpy(i) for i in inputs])
        if isinstance(fw_outputs, dict):
            for k in fw_outputs.keys():
                fw_outputs[k] = fw_outputs[k].numpy(force=True)
        elif isinstance(fw_outputs, (list, tuple)):
            fw_outputs = [o.numpy(force=True) for o in fw_outputs]
        else:
            fw_outputs = [fw_outputs.numpy(force=True)]
        return fw_outputs

    def compare_results(self, fw_outputs, ov_outputs):
        assert len(fw_outputs) == len(ov_outputs), \
            "Different number of outputs between TensorFlow and OpenVINO:" \
            " {} vs. {}".format(len(fw_outputs), len(ov_outputs))

        fw_eps = 5e-2
        is_ok = True
        for i in range(len(ov_outputs)):
            cur_fw_res = fw_outputs[i]
            cur_ov_res = ov_outputs[i]
            l = min(len(cur_fw_res), len(cur_ov_res))
            assert l > 0 or len(cur_fw_res) == len(
                cur_ov_res), "No boxes were selected."
            print(f"fw_re: {cur_fw_res};\n ov_res: {cur_ov_res}")
            is_ok = compare_two_tensors(cur_ov_res[:l], cur_fw_res[:l], fw_eps)
        assert is_ok, "Accuracy validation failed"

    @pytest.mark.parametrize("name,type,mark,reason",
                             get_models_list(os.path.join(os.path.dirname(__file__), "detectron2_precommit")))
    @pytest.mark.precommit
    def test_detectron2_precommit(self, name, type, mark, reason, ie_device):
        self.run(name, None, ie_device)

    @pytest.mark.parametrize("name",
                             [pytest.param(n, marks=pytest.mark.xfail(reason=r)) if m == "xfail" else n for n, _, m, r in get_models_list(os.path.join(os.path.dirname(__file__), "detectron2_models"))])
    @pytest.mark.nightly
    def test_detectron2_all_models(self, name, ie_device):
        self.run(name, None, ie_device)
