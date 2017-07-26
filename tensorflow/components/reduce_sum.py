#!/usr/bin/env python3.5

import base
import tensorflow as tf


class Variable(base.Base):
    """
    Some text about class
    """

    def __init__(self, name):
        base.Base.__init__(self, name=name)

        self._add_input("input_tensor", desc="The tensor to reduce. Should have numeric type", important=True)

        self._add_output("out", desc="A Tensor")

        self._add_param("axis", desc="The dimensions to reduce. If None (the default), reduces all dimensions",
                        important=False)
        self._add_param("keep_dims", value=False, desc="If true, retains reduced dimensions with length 1",
                        important=False)
        self._add_param("name", value=name, desc="Operation reduce_sum", important=False)
        self._add_param("reduction_indices", desc="The old (deprecated) name for axis", important=False)

    @staticmethod
    def get_operation_name():
        return "reduce_sum"

    @staticmethod
    def get_version():
        return "1.0"

    def init(self):
        res = tf.reduce_sum(input_tensor=self.get_input("input_tensor"),
                            axis=self.get_param("axis"),
                            keep_dims=self.get_param("keep_dims"),
                            name=self.get_param("name"),
                            reduction_indices=self.get_param("reduction_indices")
                            )
        self.set_output("out", res)
        return {"out": res}

    def run(self):
        pass

    def stop(self):
        pass
