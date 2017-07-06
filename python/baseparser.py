#!/usr/bin/env python3.5

from abc import ABC, abstractmethod
import json
import collections

class BaseParser(ABC):

    def __init__(self, name, obj_type, params=collections.OrderedDict({})):
        self.name = name
        self.js_obj = None
        self.obj_type = obj_type
        self.params = params

    def get_name(self):
        return self.name

    def set_params(self, params):
        self.params = params

    def get_params(self):
        return self.params

    @staticmethod
    def str_to_json(str):
        return json.loads(json.dumps(str))

    def set_json(self, json_obj):
        self.js_obj = BaseParser.str_to_json(json_obj)

    def get_json(self):
        return self.js_obj

    def is_json_known(self, json_obj):
        obj = json_obj if json_obj is not None else self.get_json()
        if obj is None:
            return False
        return obj["type"] == self.obj_type

    @staticmethod
    def to_tf_param(param):
        if param == "true" or param is True:
            return "True"
        if param == "false" or param is False:
            return "False"
        elif param == "null":
            return "None"
        else:
            return str(param)

    @abstractmethod
    def parse(self):
        pass

    @abstractmethod
    def generate_code(self):
        pass
