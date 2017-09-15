#!/usr/bin/env python3.5

import base
import tensorflow as tf


class Matmul(base.Base):
    """
    Multiplies matrix a by matrix b, producing a * b
    """

    class Input:
        X = "a"
        Y = "b"

    class Output:
        RESULT = "result"

    class Params:
        TRANSPOSE_A = "transpose_a"
        TRANSPOSE_B = "transpose_b"
        ADJOINT_A = "adjoint_a"
        ADJOINT_B = "adjoint_b"
        A_IS_SPARSE = "a_is_sparse"
        B_IS_SPARSE = "b_is_sparse"
        NAME = "name"

    def __init__(self, tf_session, name):
        base.Base.__init__(self, tf_session=tf_session, name=name)
        desc = "A Tensor. Must be one of the following types: float16, float32, float64, int32, complex64, complex128"
        self.add_input(self.Input.X, desc=desc, important=True)
        self.add_input(self.Input.Y, desc=desc, important=True)

        self.add_output(self.Output.RESULT, desc="A Tensor. Has the same type as x")

        self.add_param(self.Params.TRANSPOSE_A, desc="If True, a is transposed before multiplication", important=False)
        self.add_param(self.Params.TRANSPOSE_B, desc="If True, b is transposed before multiplication", important=False)
        self.add_param(self.Params.ADJOINT_A, desc="If True, a is conjugated and transposed before multiplication.", important=False)
        self.add_param(self.Params.ADJOINT_B, desc="If True, b is conjugated and transposed before multiplication.", important=False)
        self.add_param(self.Params.A_IS_SPARSE, desc="If True, a is treated as a sparse matrix.", important=False)
        self.add_param(self.Params.B_IS_SPARSE, desc="If True, b is treated as a sparse matrix.", important=False)
        self.add_param(self.Params.NAME, desc="A name for the operation (optional)", value=name, important=False)

    def init(self):
        res = tf.matmul(x=self.get_input(self.Input.X),
                        y=self.get_input(self.Input.Y),
                        transpose_a=self.get_param(self.Params.TRANSPOSE_A),
                        transpose_b=self.get_param(self.Params.TRANSPOSE_B),
                        adjoint_a=self.get_param(self.Params.ADJOINT_A),
                        adjoint_b=self.get_param(self.Params.ADJOINT_B),
                        a_is_sparse=self.get_param(self.Params.A_IS_SPARSE),
                        b_is_sparse=self.get_param(self.Params.B_IS_SPARSE),
                        name=self.get_param(self.Params.NAME)
                        )
        self.set_output(self.Output.RESULT, res)
        return {self.Output.RESULT: res}

    def run(self):
        pass

    def stop(self):
        pass
