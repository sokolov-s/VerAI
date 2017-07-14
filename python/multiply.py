#!/usr/bin/env python3.5

import collections
import baseparser as bp


class Multiply(bp.BaseParser):
    """
    Some text about class
    """

    def __init__(self, name):
        params = collections.OrderedDict(
            [("x", None),
             ("y", None),
             ("name", None)
             ]
        )
        bp.BaseParser.__init__(self, name, "multiply", params)

    def generate_code(self):
        code = "\n# Multiply\n"
        code += self.var_name_form_json(self.get_name(), self.get_json(), 0) + " = tf.multiply("
        for key, value in self.get_params().items():
            if value:
                if key == "name":
                    code += "name=\"" + value + "\""
                else:
                    code += self.to_python_var(value)
                code += ", "
        code = code[:-2] + ")"
        self._set_body_code(code)
