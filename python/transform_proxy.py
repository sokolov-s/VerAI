#!/usr/bin/env python3.5

import baseparser as bp
import multiply as mul


class TransofrmProxy(bp.BaseParser):
    """
    Some text about class
    """

    def __init__(self, name):
        bp.BaseParser.__init__(self, name, "transform")
        self.action_obj = None

    def parse(self):
        method = self.get_json()["operation"]
        if method == "multiply" or method == "mul":
            self.action_obj = mul.Multiply(self.get_name())

        self.action_obj.set_json(self.get_json())
        self.action_obj.parse()

    def generate_code(self):
        if self.action_obj:
            return self.action_obj.generate_code()
        else:
            return ""
