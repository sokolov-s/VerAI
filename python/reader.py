#!/usr/bin/env python3.5

import json
import importlib
import base

class Reader(base.Base):
    """
    Some text about class
    """

    def __init__(self, name):
        base.Base.__init__(self, name)
        self.dataset = None

    def is_json_known(self, json_obj):
        obj = json_obj if json_obj is not None else self.js_obj
        if obj is None:
            return False
        return obj["type"] == "reader"

    def get_dataset_name(self):
        name = self.js_obj["init"]["dataset"]
        name = name.split('.')
        return name[0]

    def set_dataset(self, dataset):
        # ds_name = self.get_dataset_json_name()
        # if not ds_name ==
        # importlib.import_module(ds_name)
        self.dataset = dataset

    def parse(self):
        pass

