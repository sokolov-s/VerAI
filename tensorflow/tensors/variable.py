#!/usr/bin/env python3.5

import base
import tensorflow as tf


class Variable(base.Base):
    """
    Some text about class
    """

    def __init__(self, name):
        base.Base.__init__(self, name=name)

        self._add_input("value", desc="A Tensor")

        self._add_output("out", desc="A Tensor")

        self._add_param("initial_value")
        self._add_param("trainable", value=True)
        self._add_param("collections", value=None)
        self._add_param("validate_shape", value=True)
        self._add_param("caching_device")
        self._add_param("variable_def")
        self._add_param("dtype")
        self._add_param("expected_shape")
        self._add_param("import_scope")
        self._add_param("name", desc="A name for the operation (optional)", value=name)

    @staticmethod
    def get_operation_name():
        return None

    @staticmethod
    def get_version():
        return "1.0"

    def init(self):
        value = self.get_input("value") if self.get_input("value") else self.get_param("initial_value")
        res = tf.Variable(initial_value=value,
                          trainable=self.get_param("trainable"),
                          collections=self.get_param("collections"),
                          validate_shape=self.get_param("validate_shape"),
                          caching_device=self.get_param("caching_device"),
                          name=self.get_param("name"),
                          variable_def=self.get_param("variable_def"),
                          dtype=self.get_param("dtype"),
                          expected_shape=self.get_param("expected_shape"),
                          import_scope=self.get_param("import_scope")
                          )
        self.set_output("out", res)
        return {"out": res}


