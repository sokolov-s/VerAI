#!/usr/bin/env python3.5

import base
import tensorflow as tf
from inspect import _sentinel


class SoftmaxCrossEntropyWithLogits(base.Base):
    """
    Computes softmax cross entropy between logits and labels
    """
    class Input:
        LABELS = "labels"
        LOGITS = "logits"

    class Output:
        RESULT = "result"

    class Params:
        SENTINEL = "_sentinel"
        NAME = "name"
        DIM = "dim"

    def __init__(self, tf_session, name):
        base.Base.__init__(self, tf_session=tf_session, name=name)
        desc = "Each row labels[i] must be a valid probability distribution"
        self.add_input(self.Input.LABELS, desc=desc, important=True)
        
        desc = "Unscaled log probabilities"
        self.add_input(self.Input.LOGITS, desc=desc, important=True)
        
        desc = "A 1-D Tensor of length batch_size of the same type as logits with the softmax cross entropy loss."
        self.add_output(self.Output.RESULT, desc=desc)
        
        self.add_param(self.Params.SENTINEL, desc="Used to prevent positional parameters. Internal, do not use", important=False)
        self.add_param(self.Params.NAME, desc="Operation nn.softmax_cross_entropy_with_logits", value=name, important=False)
        self.add_param(self.Params.DIM, desc="The class dimension. Defaulted to -1 which is the last dimension.", important=False)

    def init(self):
        res = tf.nn.softmax_cross_entropy_with_logits(_sentinel=self.get_param(self.Params.SENTINEL), 
                        labels=self.get_input(self.Input.LABELS),
                        logits=self.get_input(self.Input.LOGITS),
                        dim=self.get_param(self.Params.DIM),
                        name=self.get_param(self.Params.NAME)
                        )
        self.set_output(self.Output.RESULT, res)
        return {self.Output.RESULT: res}

    def run(self):
        pass

    def stop(self):
        pass
