#!/usr/bin/env python3.5

import base
import tensorflow as tf


class Multiply(base.Base):
    """
    Some text about class
    """

    class Input:
        X = "x"
        Y = "y"

    class Output:
        RESULT = "result"

    class Params:
        NAME = "name"

    def __init__(self, tf_session, name):
        base.Base.__init__(self, tf_session=tf_session, name=name)
        desc = "A Tensor. Must be one of the following types: half, float32, float64, uint8, int8, uint16, int16, " \
               "int32, int64, complex64, complex128"
        self.add_input(self.Input.X, desc=desc, important=True)
        self.add_input(self.Input.Y, desc=desc, important=True)

        self.add_output(self.Output.RESULT, desc="A Tensor. Has the same type as x")

        self.add_param(self.Params.NAME, desc="A name for the operation (optional)", value=name, important=False)

    def init(self):
        res = tf.multiply(self.get_input(self.Input.X), self.get_input(self.Input.Y), self.get_param(self.Params.NAME))
        self.set_output(self.Output.RESULT, res)
        return {self.Output.RESULT: res}

    def run(self):
        pass

    def stop(self):
        pass
