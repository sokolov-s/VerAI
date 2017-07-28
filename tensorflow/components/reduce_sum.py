#!/usr/bin/env python3.5

import base
import tensorflow as tf
import enum as Enum


class ReduceSum(base.Base):
    """
    Some text about class
    """

    class Input(Enum):
        X = "x"

    class Output(Enum):
        RESULT = "result"

    class Params(Enum):
        NAME = "name"
        AXIS = "axis"
        KEEP_DIMS = "keep_dims"
        REDUCE_INDICES = "reduction_indices"

    def __init__(self, name):
        base.Base.__init__(self, name=name)

        self._add_input(self.Input.X, desc="The tensor to reduce. Should have numeric type", important=True)

        self._add_output(self.Output.RESULT, desc="A Tensor")

        self._add_param(self.Params.AXIS, desc="The dimensions to reduce. If None (the default), reduces all dimensions",
                        important=False)
        self._add_param(self.Params.KEEP_DIMS, value=False, desc="If true, retains reduced dimensions with length 1",
                        important=False)
        self._add_param(self.Params.NAME, value=name, desc="Operation reduce_sum", important=False)
        self._add_param(self.Params.REDUCE_INDICES, desc="The old (deprecated) name for axis", important=False)

    def init(self):
        res = tf.reduce_sum(input_tensor=self.get_input(self.Input.X),
                            axis=self.get_param(self.Params.AXIS),
                            keep_dims=self.get_param(self.Params.KEEP_DIMS),
                            name=self.get_param(self.Params.NAME),
                            reduction_indices=self.get_param(self.Params.REDUCE_INDICES)
                            )
        self.set_output(self.Output.RESULT, res)
        return {self.Output.RESULT: res}

    def run(self):
        pass

    def stop(self):
        pass
