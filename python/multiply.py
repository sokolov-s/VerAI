#!/usr/bin/env python3.5

import collections
import baseparser as bp


class Multiply(bp.BaseParser):

    def __init__(self, name):
        params = collections.OrderedDict(
            {"x": None,
             "y": None,
             "name": None
             }
        )
        bp.BaseParser.__init__(self, name, "multiply", params)

    def parse(self):
        self.params["name"] = self.get_name()
        for key, value in self.get_json()["init"].items():
            if key in self.params:
                self.params[key] = bp.BaseParser.to_tf_param(value)

    def generate_code(self):
        code = self.get_json()["output"][0] + " = tf.multiply("
        for key, value in self.get_params().items():
            if value:
                if key == "name":
                    code += "name=\"" + value + "\""
                else:
                    code += value
                code += ", "
        code = code[:-2] + ")"
        return code
