#!/usr/bin/env python3.5

import base
from enum import Enum
import dataset


class Reader(base.Base):
    """
    Some text about class
    """

    class Input(Enum):
        DATASET = "dataset"

    class Params(Enum):
        MODULE = "module"
        QUEUE_CAPACITY = "queue_capacity"
        NAME = "name"

    # class Output(Enum):
    #     INSTANCE = "instance"

    def __init__(self, name):
        base.Base.__init__(self, name=name)

        self.add_input(self.Input.DATASET, important=True)

        self.add_param(self.Params.NAME, important=False)
        self.add_param(self.Params.QUEUE_CAPACITY, important=False)
        self.add_param(self.Params.MODULE, important=True)
        self.reader = None

    # Base class interface implementation

    def init(self):
        # dataset = self.get_input(self.Input.DATASET)
        # if dataset.get_param(dataset.DataSet.Params.DB_TYPE) == "file":
        #
        #     self.reader =
        pass

    def run(self):
        pass

    def stop(self):
        pass
