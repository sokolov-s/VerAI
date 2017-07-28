#!/usr/bin/env python3.5

import base
import enum as Enum


class DataSet(base.Base):
    """
    Some text about class
    """

    class Params(Enum):
        DB_TYPE = "db_type"
        FOLDER = "folders"
        FILES = "files"

    class Output(Enum):
        INSTANCE = "instance"

    def __init__(self, name):
        base.Base.__init__(self, name=name)

        self._add_output(self.Output.INSTANCE, desc="Instance of class")

        self._add_param(self.Params.DB_TYPE, important=True)
        self._add_param(self.Params.FOLDER, important=True)
        self._add_param(self.Params.FILES, important=True)

    # Base class interface implementation
    def init(self):
        res = self
        self.set_output(self.Output.INSTANCE, res)
        return {self.Output.INSTANCE: res}

    def run(self):
        pass

    def stop(self):
        pass