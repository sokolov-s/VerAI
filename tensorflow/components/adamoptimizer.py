#!/usr/bin/env python3.5

import base
import tensorflow as tf


class AdamOptimizer(base.Base):
    """
    Some text about class
    """

    def __init__(self, name):
        base.Base.__init__(self, name=name)
        self._add_input("input_tensor", desc="A tensor to optimize", important=True)
        self._add_input("iteration_count", value=0, important=False)

        self._add_output("out", desc="A Tensor")

        self._add_param("iteration_count", value=0, important=False)
        self._add_param("learning_rate", value=0.001, important=True)
        self._add_param("beta1", value=0.9, important=False)
        self._add_param("beta2", value=0.999, important=False)
        self._add_param("epsilon", value=1e-08, important=False)
        self._add_param("use_locking", value=False, important=False)
        self._add_param("name", desc="Operation AdamOptimizer", value=name, important=False)

        self.iteration_cnt = 0
        self.optimize_function = "minimize"
        self.gate_params = {"GATE_NONE": tf.train.AdamOptimizer.GATE_NONE,
                            "GATE_OP": tf.train.AdamOptimizer.GATE_OP,
                            "GATE_GRAPH": tf.train.AdamOptimizer.GATE_GRAPH
                            }
        self.minimize_params = dict()

        self._add_optimize_param(fn_name="minimize", param_name="global_step", important=False)
        self._add_optimize_param(fn_name="minimize", param_name="var_list", important=False)
        self._add_optimize_param(fn_name="minimize", param_name="gate_gradients", value="GATE_OP", important=False)
        self._add_optimize_param(fn_name="minimize", param_name="aggregation_method", important=False)
        self._add_optimize_param(fn_name="minimize", param_name="colocate_gradients_with_ops", value=False, important=False)
        self._add_optimize_param(fn_name="minimize", param_name="name", important=False)
        self._add_optimize_param(fn_name="minimize", param_name="grad_loss", important=False)

    @staticmethod
    def get_operation_name():
        return "AdamOptimizer"

    @staticmethod
    def get_version():
        return "1.0"

    def __get_optimize_param_obj(self, fn_name=None):
        name = fn_name if fn_name else self.optimize_function
        if name == "minimize":
            return self.minimize_params
        else:
            return None

    def set_optimize_function(self, fn_name):
        self.optimize_function = fn_name

    def _add_optimize_param(self, fn_name, param_name: str, value=None, desc="", important=True):
        self._add_param_to_obj(obj=self.__get_optimize_param_obj(fn_name), name=param_name, value=value, desc=desc,
                               important=important)

    def set_optimize_param(self, name, value):
        self._set_param_value_in_obj(obj=self.__get_optimize_param_obj(), name=name, value=value)

    def get_optimize_param(self, name):
        return self._get_param_value_from_obj(self.__get_optimize_param_obj(), name)

    def init(self):
        self.iteration_cnt = self.get_input("iteration_count") if self.get_input("iteration_count") \
            else self.get_param("iteration_count")

        optimizer = tf.train.AdamOptimizer(learning_rate=self.get_param("learning_rate"),
                                           beta1=self.get_param("beta1"),
                                           beta2=self.get_param("beta2"),
                                           epsilon=self.get_param("epsilon"),
                                           use_locking=self.get_param("use_locking"),
                                           name=self.get_param("name")
                                           )
        res = None
        print(self.get_optimize_param("grad_loss"))
        if self.optimize_function == "minimize":
            res = optimizer.minimize(loss=self.get_input("input_tensor"),
                                     global_step=self.get_optimize_param("global_step"),
                                     var_list=self.get_optimize_param("var_list"),
                                     gate_gradients=self.gate_params[self.get_optimize_param("gate_gradients")],
                                     aggregation_method=self.get_optimize_param("aggregation_method"),
                                     colocate_gradients_with_ops=self.get_optimize_param("colocate_gradients_with_ops"),
                                     name=self.get_optimize_param("name"),
                                     grad_loss=self.get_optimize_param("grad_loss")
                                     )

        self.set_output("out", res)
        return {"out": res}

    def run(self):
        pass

    def stop(self):
        pass


