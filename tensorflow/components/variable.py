#!/usr/bin/env python3.5

import base
import tensorflow as tf


class Variable(base.Base):
    """
    Some text about class
    """
    class Input:
        X = "x"

    class Output:
        RESULT = "result"

    class Params:
        NAME = "name"
        INIT_VALUE = "initial_value"
        TRAINABLE = "trainable"
        COLLECTIONS = "collections"
        VALIDATE_SHAPES = "validate_shape"
        CACHING_DEVICE = "caching_device"
        VARIABLE_DEF = "variable_def"
        DTYPE = "dtype"
        EXPECTED_SHAPE = "expected_shape"
        IMPORT_SCOPE = "import_scope"

    def __init__(self, name):
        base.Base.__init__(self, name=name)

        self.add_input(self.Input.X, desc="A Tensor", important=False)

        self.add_output(self.Output.RESULT, desc="A Tensor")

        self.add_param(self.Params.INIT_VALUE, important=True)
        self.add_param(self.Params.TRAINABLE, value=True, important=True)
        self.add_param(self.Params.COLLECTIONS, value=None, important=False)
        self.add_param(self.Params.VALIDATE_SHAPES, value=True, important=False)
        self.add_param(self.Params.CACHING_DEVICE, important=False)
        self.add_param(self.Params.VARIABLE_DEF, important=False)
        self.add_param(self.Params.DTYPE, important=False)
        self.add_param(self.Params.EXPECTED_SHAPE, important=False)
        self.add_param(self.Params.IMPORT_SCOPE, important=False)
        self.add_param(self.Params.NAME, desc="A name for the operation (optional)", value=name, important=False)

    def init(self):
        value = self.get_input(self.Input.X) if self.get_input(self.Input.X) else self.get_param(self.Params.INIT_VALUE)
        res = tf.Variable(initial_value=value,
                          trainable=self.get_param(self.Params.TRAINABLE),
                          collections=self.get_param(self.Params.COLLECTIONS),
                          validate_shape=self.get_param(self.Params.VALIDATE_SHAPES),
                          caching_device=self.get_param(self.Params.CACHING_DEVICE),
                          name=self.get_param(self.Params.NAME),
                          variable_def=self.get_param(self.Params.VARIABLE_DEF),
                          dtype=self.get_param(self.Params.DTYPE),
                          expected_shape=self.get_param(self.Params.EXPECTED_SHAPE),
                          import_scope=self.get_param(self.Params.IMPORT_SCOPE)
                          )
        self.set_output(self.Output.RESULT, res)
        return {self.Output.RESULT: res}

    def run(self):
        pass

    def stop(self):
        pass
