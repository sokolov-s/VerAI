#!/usr/bin/env python3.5

from enum import Enum
from abc import ABCMeta, abstractclassmethod
import logging


class DataSetBase(object, metaclass=ABCMeta):
    """
    Some text about class
    """

    class Iterations(Enum):
        TRAIN = 0
        TEST = 1
        VALIDATE = 2
        INFERENCE = 3

    class Output:
        INSTANCE = "instance"
        TRAIN_DATA = "train_data"
        TRAIN_TARGET = "train_target"
        TEST_DATA = "test_data"
        TEST_TARGET = "test_target"
        VALIDATE_DATA = "validate_data"
        VALIDATE_TARGET = "validate_target"
        INFERENCE_DATA = "inference_data"

    def __init__(self):
        self.__iteration = DataSetBase.Iterations.TRAIN
        self.train_data_files = list()
        self.train_target_files = list()
        self.test_data_files = list()
        self.test_target_files = list()
        self.validate_data_files = list()
        self.validate_target_files = list()
        self.inference_data_files = list()

        self.logger = logging.getLogger('root')

    @property
    def logger(self):
        return self._logger

    @logger.setter
    def logger(self, value):
        self._logger = value

    def iteration(self):
        return self.__iteration

    def _set_iteration(self, value: Iterations):
        self.__iteration = value
        self.logger.info("DataSet sets to %s" % self.__iteration)

    def get_dataset(self, iteration: Iterations):
        if self.Iterations.TRAIN == iteration:
            return {self.Output.TRAIN_DATA: self.train_data_files,
                    self.Output.TRAIN_TARGET: self.train_target_files}
        elif self.Iterations.TEST == self.iteration:
            return {self.Output.TEST_DATA: self.test_data_files,
                    self.Output.TEST_TARGET: self.test_target_files}
        elif self.Iterations.VALIDATE == self.iteration:
            return {self.Output.VALIDATE_DATA: self.validate_data_files,
                    self.Output.VALIDATE_TARGET: self.validate_target_files}
        elif self.Iterations.INFERENCE == self.iteration:
            return {self.Output.INFERENCE_DATA: self.inference_data_files,
                    self.Output.VALIDATE_TARGET: None}
        else:
            raise

    def get_current_dataset(self):
         return self.get_dataset(self.iteration)

    @abstractclassmethod
    def switch_iteration(self, iteration: Iterations):
        pass

    @abstractclassmethod
    def fill_next_feed_dict(self, batch_size):
        pass

    @abstractclassmethod
    def get_output_placeholders(self):
        pass

