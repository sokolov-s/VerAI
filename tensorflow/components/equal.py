#!/usr/bin/env python3.5

import base
import tensorflow as tf


class Equal(base.Base):
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
        desc = "A Tensor. Must be one of the following types: \
            half, float32, float64, uint8, int8, int16, int32, int64, \
            complex64, quint8, qint8, qint32, string, bool, complex128"
        self.add_input(self.Input.X, desc=desc, important=True)
        
        desc = "A Tensor. Must have the same type as x"
        self.add_input(self.Input.Y, desc=desc, important=True)

        self.add_output(self.Output.RESULT, desc="A Tensor or SparseTensor. Has the same type as x")

        self.add_param(self.Params.NAME, desc="Operation equal", value=name, important=False)

    def init(self):
        res = tf.equal(x=self.get_input(self.Input.X), 
                       y=self.get_input(self.Input.Y),
                       name=self.get_param(self.Params.NAME))
        self.set_output(self.Output.RESULT, res)
        return {self.Output.RESULT: res}

    def run(self):
        pass

    def stop(self):
        pass
