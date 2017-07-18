#!/usr/bin/env python3.5

import collections
import baseparser as bp


class Minus(bp.BaseParser):
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
        bp.BaseParser.__init__(self, name, "minus", params)

    def generate_body_code(self):
        code = "\n# Minus\n"
        code += self.var_name_form_json(self.get_name(), self.get_json(), 0) + " = " + \
                self.to_python_var(self.get_params()["x"]) + " - " + self.to_python_var(self.get_params()["y"]) + "\n"
        self.set_body_code(code)
