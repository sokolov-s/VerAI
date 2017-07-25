#!/usr/bin/env python3.5

import base


class Minus(base.Base):
    """
    Some text about class
    """

    def __init__(self, name):
        base.Base.__init__(self, name=name)

        self._add_input("x", desc="A Tensor")
        self._add_input("y", desc="A Tensor")

        self._add_output("out", desc="A Tensor")

        self._add_param("name", desc="Operation minus", value=name)

    @staticmethod
    def get_operation_name():
        return "minus"

    @staticmethod
    def get_version():
        return "1.0"

    def init(self):
        res = self.get_input("x") - self.get_input("y")
        self.set_output("out", res)
        return {"out": res}


