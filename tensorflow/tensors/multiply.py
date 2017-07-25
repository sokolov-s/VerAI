#!/usr/bin/env python3.5

import base
import tensorflow as tf


class Multiply(base.Base):
    """
    Some text about class
    """

    def __init__(self, name):
        base.Base.__init__(self, name=name)
        desc = "A Tensor. Must be one of the following types: half, float32, float64, uint8, int8, uint16, int16, " \
               "int32, int64, complex64, complex128"
        self._add_input("x", desc=desc)
        self._add_input("y", desc=desc)

        self._add_output("out", desc="A Tensor. Has the same type as x")

        self._add_param("name", desc="A name for the operation (optional)", value=name)

    @staticmethod
    def get_operation_name():
        return "multiply"

    @staticmethod
    def get_version():
        return "1.0"

    def init(self):
        res = tf.multiply(self.get_input("x"), self.get_input("y"), self.get_param("name"))
        self.set_output("out", res)
        return {"out": res}


