#!/usr/bin/env python3.5

import base
import enum as Enum


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

    class Output(Enum):
        INSTANCE = "instance"

    def __init__(self, name):
        base.Base.__init__(self, name=name)

        self._add_input(self.Input, important=True, value=[])

        self._add_param(self.Params.NAME, important=False)
        self._add_param(self.Params.QUEUE_CAPACITY, important=False)
        self._add_param(self.Params.MODULE, important=True)

    # Base class interface implementation
    def init(self):
        res = self
        self.set_output(self.Output.INSTANCE, res)
        return {self.Output.INSTANCE: res}

    def run(self):
        pass

    def stop(self):
        pass
