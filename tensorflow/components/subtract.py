#!/usr/bin/env python3.5

import base
import tensorflow as tf
import enum as Enum


class Subtract(base.Base):
    """
    Some text about class
    """
    class Input(Enum):
        X = "x"
        Y = "y"

    class Output(Enum):
        RESULT = "result"

    class Params(Enum):
        NAME = "name"

    def __init__(self, name):
        base.Base.__init__(self, name=name)

        desc = "A Tensor. Must be one of the following types: half, float32, float64, int32, " \
               "int64, complex64, complex128"
        self.add_input(self.Input.X, desc=desc, important=True)
        self.add_input(self.Input.Y, desc=desc, important=True)

        self.add_output(self.Output.RESULT, desc="A Tensor")

        self.add_param(self.Params.NAME, desc="Operation subtract", value=name, important=False)

    def init(self):
        res = tf.subtract(x=self.get_input(self.Input.X), y=self.get_input(self.Input.Y),
                          name=self.get_param(self.Params.NAME))
        self.set_output(self.Output.RESULT, res)
        return {self.Output.RESULT: res}

    def run(self):
        pass

    def stop(self):
        pass
