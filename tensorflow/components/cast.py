#!/usr/bin/env python3.5

import base
import tensorflow as tf


class Cast(base.Base):
    """
    Some text about class
    """
    class Input:
        X = "x"

    class Output:
        RESULT = "result"

    class Params:
        NAME = "name"
        DTYPE = "dtype"

    def __init__(self, tf_session, name):
        base.Base.__init__(self, tf_session=tf_session, name=name)
        desc = "A Tensor or SparseTensor."
        self.add_input(self.Input.X, desc=desc, important=True)

        self.add_output(self.Output.RESULT, desc="A Tensor or SparseTensor. Has the same type as x")

        self.add_param(self.Params.DTYPE, desc="The destination type", important=True)
        self.add_param(self.Params.NAME, desc="Operation square", value=name, important=False)

    def init(self):
        res = tf.cast(x=self.get_input(self.Input.X), 
                      dtype=tf.as_dtype(self.get_param(self.Params.DTYPE)),
                      name=self.get_param(self.Params.DTYPE))
        self.set_output(self.Output.RESULT, res)
        return {self.Output.RESULT: res}

    def run(self):
        pass

    def stop(self):
        pass
