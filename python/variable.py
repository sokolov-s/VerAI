#!/usr/bin/env python3.5

import collections
import baseparser as bp

from pyparsing import col


class Variable(bp.BaseParser):

    def __init__(self, name):
        params = collections.OrderedDict(
            [("initial_value", None),
             ("trainable", None),
             ("collections", None),
             ("validate_shape", None),
             ("caching_device", None),
             ("name", None),
             ("variable_def", None),
             ("dtype", None),
             ("expected_shape", None),
             ("import_scope", None)
             ]
        )
        bp.BaseParser.__init__(self, name, "variable", params)

    def parse(self):
        self.params["name"] = self.get_name()
        for key, value in self.get_json()["init"].items():
            if key in self.params:
                self.params[key] = bp.BaseParser.to_tf_param(value)

    def generate_code(self):
        code = self.get_json()["output"][0] + " = tf.Variable("
        for key, value in self.get_params().items():
            if value is not None:
                code += key + "="
                if key == "name":
                    code += "\"" + value + "\""
                else:
                    code += value
                code += ", "
        code = code[:-2] + ")"
        return code
