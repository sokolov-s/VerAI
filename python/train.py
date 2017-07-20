#!/usr/bin/env python3.5

import baseparser as bp
import collections


class Train(bp.BaseParser):
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
        self.train_tensor_name = ""

    def parse(self):
        self.parse_params()
        self.train_tensor_name = self.var_name_form_json(self.get_name(), self.get_json(), 0)
        return

    def generate_imports_code(self):
        code = self.get_head_code()
        self.set_head_code(code)

    def generate_body_code(self):
        code = "\n# Create train tensor\n"
        optimizer_name = bp.BaseParser.generate_unique_name("optimizer")
        code += optimizer_name + " = "

        # Declare tensorflow optimizer
        if self.get_json()["operation"] == "AdamOptimizer":
            code += "tf.train.AdamOptimizer("
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

        code += self.train_tensor_name + " = " + optimizer_name + "." + \
                self.get_json()["optimize_function"]["name"] + "("
        for key, value in self.get_json()["optimize_function"]["init"].items():
            if value is not None:
                code += key + "="
                if key == "name":
                    code += "\"" + value + "\""
                elif key == "loss":
                    code += self.to_python_var(value)
                elif key == "gate_gradients" and self.get_json()["operation"] == "AdamOptimizer":
                    code += "tf.train.AdamOptimizer." + value
                else:
                    code += self.to_tf_param(value)
                code += ", "
        code = code[:-2] + ")"

        self.set_body_code(code)

    def generate_action_code(self):
        code = "\n# Train loop\n"
        code += "for i in range(" + str(self.get_json()["iteration_count"]) + "):\n"
        code += "\tsess.run(" + self.train_tensor_name + ")\n"
        self.set_action_code(code)
