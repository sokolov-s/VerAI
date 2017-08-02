#!/usr/bin/env python3.5

import base


class DataSet(base.Base):
    """
    Some text about class
    """

    class Params:
        DB_TYPE = "db_type"
        FOLDER = "folder"
        FILES = "files"

    class Output:
        INSTANCE = "instance"

    def __init__(self, name):
        base.Base.__init__(self, name=name)

        self.add_output(self.Output.INSTANCE, desc="Instance of class")

        self.add_param(self.Params.DB_TYPE, important=True)
        self.add_param(self.Params.FOLDER, important=True)
        self.add_param(self.Params.FILES, important=True)

    # Base class interface implementation
    def init(self):
        res = self
        self.set_output(self.Output.INSTANCE, res)
        return {self.Output.INSTANCE: res}

    def run(self):
        pass

    def stop(self):
        pass
