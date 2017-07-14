#!/usr/bin/env python3.5

from abc import ABC, abstractmethod
import json
import collections


class BaseParser(ABC):
    """
    Some text about class
    """

    def __init__(self, name, obj_type, params=collections.OrderedDict({})):
        self.name = name
        self.js_obj = None
        self.obj_type = obj_type
        self.params = params
        self.head_code = ""
        self.body_code = ""
        self.action_code = ""

    def get_name(self):
        return self.name

    def set_params(self, params):
        self.params = params

    def get_params(self):
        return self.params

    def _set_head_code(self, code):
        self.head_code = code

    def get_head_code(self):
        return self.head_code

    def _set_body_code(self, code):
        self.body_code = code

    def get_body_code(self):
        return self.body_code

    def _set_action_code(self, code):
        self.action_code = code

    def get_action_code(self):
        return self.action_code

    def set_json(self, json_obj):
        self.js_obj = BaseParser.str_to_json(json_obj)

    def get_json(self):
        return self.js_obj

    def is_json_known(self, json_obj):
        obj = json_obj if json_obj is not None else self.get_json()
        if obj is None:
            return False
        return self.get_json_type(obj) == self.obj_type

    @staticmethod
    def get_json_type(json_obj):
        return json_obj["type"]

    @staticmethod
    def str_to_json(string):
        return json.loads(json.dumps(string))

    @staticmethod
    def var_name_form_json(json_obj_name, json_obj, var_number):
        return str(json_obj_name + "_output_" + json_obj["output"][var_number]["name"]).lower()

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

    @staticmethod
    def to_python_var(json_string):
        return json_string.replace('.', '_').lower()

    def parse_params(self):
        params = self.get_params()
        if "name" in params:
            params["name"] = self.get_name()
        for key, value in self.get_json()["init"].items():
            if key in params:
                params[key] = BaseParser.to_tf_param(value)
        self.set_params(params)

    def parse(self):
        self.parse_params()

    @abstractmethod
    def generate_code(self):
        pass
