#!/usr/bin/env python3.5

import baseparser as bp
import collections


class Reader(bp.BaseParser):
    """
    Some text about class
    """

    def __init__(self, name):
        params = collections.OrderedDict(
            [("learning_rate", None),
             ("beta1", None),
             ("beta2", None),
             ("epsilon", None),
             ("use_locking", None),
             ("name", "train_tensor")
             ]
        )
        bp.BaseParser.__init__(self, name, "train", params)

    def parse(self):
        self.parse_params()
        print("\033[91mCan't find reader for database : object %s, database type %s\033[0m" % (self.get_name(), db_type))
        return

    def generate_imports_code(self):
        code = self.get_head_code()
        self.set_head_code(code)

    def generate_body_code(self):
        code = "\n# Create train tensor\n"
        if self.get_json()["operation"] == "AdamOptimizer":
            code += ""
        else:
            print("\033[91mCan't find optimizer for object %s: %s\033[0m" % (self.get_name(), self.get_json()["operation"]))
            return
        for key, value in self.get_params().items():
            if value is not None:
                code += key + "="
                if key == "name":
                    code += "\"" + value + "\""
                else:
                    code += value
                code += ", "
        code = code[:-2] + ")"
        code += "\n"

        self.set_body_code(code)

    def generate_action_code(self):
        code = self.get_action_code()

        self.set_action_code(code)
