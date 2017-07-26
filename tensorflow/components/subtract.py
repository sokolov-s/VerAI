#!/usr/bin/env python3.5

import base
import tensorflow as tf

class Subtract(base.Base):
    """
    Some text about class
    """

    def __init__(self, name):
        base.Base.__init__(self, name=name)

        desc = "A Tensor. Must be one of the following types: half, float32, float64, int32, " \
               "int64, complex64, complex128"
        self._add_input("x", desc=desc, important=True)
        self._add_input("y", desc=desc, important=True)

        self._add_output("out", desc="A Tensor")

        self._add_param("name", desc="Operation subtract", value=name, important=False)

    @staticmethod
    def get_operation_name():
        return "minus"

    @staticmethod
    def get_version():
        return "1.0"

    def init(self):
        res = tf.subtract(x=self.get_input("x"), y=self.get_input("y"), name=self.get_param("name"))
        self.set_output("out", res)
        return {"out": res}

    def run(self):
        pass

    def stop(self):
        pass
