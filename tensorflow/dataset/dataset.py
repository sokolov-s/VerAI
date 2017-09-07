#!/usr/bin/env python3.5

from dataset_base import DataSetBase


class DataSet(DataSetBase):

    def __init__(self):
        DataSetBase.__init__(self)

    def switch_iteration(self, value: DataSetBase.Iterations):
        self._set_iteration(value)

    def get_next(self):
        result = None

        return result

