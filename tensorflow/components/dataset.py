#!/usr/bin/env python3.5

import base
import tensorflow as tf


class DataSet(base.Base):
    """
    Some text about class
    """

    class Params:
        TYPE = "file_type"
        FOLDER = "folder"
        NAME = "name"

    class Output:
        DATA = "raw_data"
        TARGET = "target_data"

    def __init__(self, tf_session, name):
        base.Base.__init__(self, tf_session=tf_session, name=name)

        self.add_param(self.Params.TYPE, important=True)
        self.add_param(self.Params.FOLDER, important=True)
        self.add_param(self.Params.NAME, important=False)

        self.add_output(self.Output.DATA, desc="raw_data")
        self.add_output(self.Output.TARGET, desc="target_data")

    # Base class interface implementation
    def init(self):
        outputs = {}
        self.set_outputs(outputs)
        return outputs

    def run(self):
        pass

    def stop(self):
        pass
