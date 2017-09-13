#!/usr/bin/env python3.5

import base
import tensorflow as tf
import sys.modules
import importlib


class DataSet(base.Base):
    """
    Some text about class
    """

    class Params:
        GIT_LINK = "git_link"
        FOLDER = "folder"
        NAME = "name"
        DESCRIPTION = "outputs_description"
        HANDLER = "handler"

#     class Output:
#         DATA = "raw_data"
#         TARGET = "target_data"

    def __init__(self, tf_session, name):
        base.Base.__init__(self, tf_session=tf_session, name=name)

        self.add_param(self.Params.GIT_LINK, important=True)
        self.add_param(self.Params.FOLDER, important=True)
        self.add_param(self.Params.NAME, important=False)
        self.handler = None
    
    def get_handler(self):
        return self.handler
    
    # Base class interface implementation
    def init(self):
        module = importlib.import_module(self.get_param(self.Params.FOLDER) + "." + self.get_param(self.Params.HANDLER).split(".")[0])
        instance = getattr(module, "DataSetHandler")
        self.handler = instance()
        outputs = {}
        self.set_outputs(outputs)
        return outputs

    def run(self):
        pass

    def stop(self):
        pass
