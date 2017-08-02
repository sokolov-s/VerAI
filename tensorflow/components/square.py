#!/usr/bin/env python3.5

import base
import tensorflow as tf


class Square(base.Base):
    """
    Some text about class
    """
    class Input:
        X = "x"

    class Output:
        RESULT = "result"

    class Params:
        NAME = "name"

    def __init__(self, name):
        base.Base.__init__(self, name=name)
        desc = "A Tensor or SparseTensor. Must be one of the following types: half, float32, float64, int32, int64, " \
               "complex64, complex128"
        self.add_input(self.Input.X, desc=desc, important=True)

        self.add_output(self.Output.RESULT, desc="A Tensor or SparseTensor. Has the same type as x")

        self.add_param(self.Params.NAME, desc="Operation square", value=name, important=False)

    def init(self):
        res = tf.square(self.get_input(self.Input.X), self.get_param(self.Params.NAME))
        self.set_output(self.Output.RESULT, res)
        return {self.Output.RESULT: res}

    def run(self):
        pass

    def stop(self):
        pass
