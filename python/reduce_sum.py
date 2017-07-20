#!/usr/bin/env python3.5

import collections
import baseparser as bp


class ReduceSum(bp.BaseParser):
    """
    Some text about class
    """

    def __init__(self, name):
        params = collections.OrderedDict(
            [("input_tensor", None),
             ("axis", None),
             ("keep_dims", False),
             ("name", None),
             ("reduction_indices", None)
             ]
        )
        bp.BaseParser.__init__(self, name, "reduce_sum", params)

    def generate_body_code(self):
        code = "\n# Square\n"
        code += self.var_name_form_json(self.get_name(), self.get_json(), 0) + " = tf.reduce_sum("
        for key, value in self.get_params().items():
            if value:
                code += key + "="
                if key == "name":
                    code += "\"" + value + "\""
                elif key == "input_tensor":
                    code += self.to_python_var(value)
                else:
                    code += value
                code += ", "
        code = code[:-2] + ")"
        self.set_body_code(code)
