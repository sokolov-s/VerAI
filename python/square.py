#!/usr/bin/env python3.5

import collections
import baseparser as bp


class Square(bp.BaseParser):
    """
    Some text about class
    """

    def __init__(self, name):
        params = collections.OrderedDict(
            [("x", None),
             ("name", None)
             ]
        )
        bp.BaseParser.__init__(self, name, "square", params)

    def generate_body_code(self):
        code = "\n# Square\n"
        code += self.var_name_form_json(self.get_name(), self.get_json(), 0) + " = tf.square("
        for key, value in self.get_params().items():
            code += key + "="
            if value:
                if key == "name":
                    code += "\"" + value + "\""
                else:
                    code += self.to_python_var(value)
                code += ", "
        code = code[:-2] + ")"
        self.set_body_code(code)
