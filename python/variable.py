#!/usr/bin/env python3.5
import tensorflow as tf
from enum import Enum


class Variable(object):
    def __init__(self, *params):
        self.value = tf.Variable(*params)
