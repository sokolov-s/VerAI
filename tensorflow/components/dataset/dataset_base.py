#!/usr/bin/env python3.5

from enum import Enum
from abc import ABCMeta, abstractclassmethod


class DataSetBase(object, metaclass=ABCMeta):
    """
    Some text about class
    """

    class Iterations(Enum):
        TRAIN = 0
        TEST = 1
        VALIDATE = 2
        INFERENCE = 3

    def __init__(self):
        self.iteration = DataSetBase.Iterations.TRAIN

    @property
    def iteration(self):
        return self._iteration

    @iteration.setter
    def iteration(self, value: Iterations):
        self._iteration = value
        print("DataSet sets to %s" % self.iteration)

    @abstractclassmethod
    def switch_iteration(self, iteration: Iterations):
        pass

    @abstractclassmethod
    def get_next(self):
        pass


