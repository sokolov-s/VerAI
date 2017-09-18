#!/usr/bin/env python3.5

import base
import tensorflow as tf


class Argmax(base.Base):
    """
    Some text about class
    """
    class Input:
        X = "input"

    class Output:
        RESULT = "result"

    class Params:
        AXIS = "axis"
        NAME = "name"
        OUTPUT_TYPE = "output_type"

    def __init__(self, tf_session, name):
        base.Base.__init__(self, tf_session=tf_session, name=name)
        desc = "A Tensor. Must be one of the following types: float32, float64, int64, int32, uint8, \
            uint16, int16, int8, complex64, complex128, qint8, quint8, qint32, half"
        self.add_input(self.Input.X, desc=desc, important=True)
        
        self.add_output(self.Output.RESULT, desc="A Tensor or SparseTensor. Has the same type as x")
        
        desc = "A Tensor. Must be one of the following types: int32, int64. int32 or int64, 0 <= axis < rank(input). \
            Describes which axis of the input Tensor to reduce across. For vectors, use axis = 0"
        self.add_param(self.Params.AXIS, desc=desc, important=False)
        self.add_param(self.Params.NAME, desc="Operation argmax", value=name, important=False)
        self.add_param(self.Params.OUTPUT_TYPE, desc="An optional tf.DType from: tf.int32, tf.int64. Defaults to tf.int64", 
                       important=False)

    def init(self):
        output_type = tf.as_dtype(self.get_param(self.Params.OUTPUT_TYPE)) if self.get_param(self.Params.OUTPUT_TYPE) else tf.int64
        
        res = tf.argmax(input=self.get_input(self.Input.X), 
                        axis=self.get_param(self.Params.AXIS),
                        name=self.get_param(self.Params.NAME),
                        output_type=output_type
                        )
        self.set_output(self.Output.RESULT, res)
        return {self.Output.RESULT: res}

    def run(self):
        pass

    def stop(self):
        pass
