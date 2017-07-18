#!/usr/bin/env python3.5

import baseparser as bp
import multiply as mul
import minus
import square
import reduce_sum

class TransofrmProxy(bp.BaseParser):
    """
    Some text about class
    """

    def __init__(self, name):
        bp.BaseParser.__init__(self, name, "transform")
        self.action_obj = None

    def parse(self):
        json_obj = self.get_json()
        method = json_obj["operation"]
        if method == "multiply" or method == "mul":
            self.set_proxy(mul.Multiply(self.get_name()))
        elif method == "minus":
            self.set_proxy(minus.Minus(self.get_name()))
        elif method == "square":
            self.set_proxy(square.Square(self.get_name()))
        elif method == "reduce_sum":
            self.set_proxy(reduce_sum.ReduceSum(self.get_name()))
        else:
            print("Unknown operation : object %s, operation %s" % (self.get_name(), method))
            return

        self.get_proxy().set_json(json_obj)
        self.get_proxy().parse()
