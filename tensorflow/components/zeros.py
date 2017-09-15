#!/usr/bin/env python3.5

import base
import tensorflow as tf


class Zeros(base.Base):
    """
    Creates a tensor with all elements set to zero.
    """

    class Output:
        RESULT = "result"

    class Params:
        SHAPE = "shape"
        DTYPE = "dtype"
        NAME = "name"

    def __init__(self, tf_session, name):
        base.Base.__init__(self, tf_session=tf_session, name=name)

        self.add_output(self.Output.RESULT, desc="A Tensor")

        self.add_param(self.Params.SHAPE, desc="A list of integers, a tuple of integers, or a 1-D Tensor of type int32", 
                       important=True)
        self.add_param(self.Params.DTYPE, desc="The type of an element in the resulting Tensor", important=False)
        self.add_param(self.Params.NAME, desc="A name for the operation (optional).", value=name, important=False)

    def init(self):
        res = tf.zeros(shape=self.get_param(self.Params.SHAPE),
                       dtype=tf.to_dtype(self.get_param(self.Params.DTYPE)), 
                       name=self.get_param(self.Params.NAME)
                       )
        self.set_output(self.Output.RESULT, res)
        return {self.Output.RESULT: res}

    def run(self):
        pass

    def stop(self):
        pass
