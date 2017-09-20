#!/usr/bin/env python3.5

import base
import tensorflow as tf
import importlib
import sys
import importlib.util

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
        self.add_param(self.Params.DESCRIPTION, important=True)
        self.add_param(self.Params.HANDLER, important=False)
        self.handler = None
    
    def get_handler(self):
        return self.handler
    
    # Base class interface implementation
    def init(self):
        folder = self.get_param(self.Params.FOLDER)
        handler_script_name = self.get_param(self.Params.HANDLER) 
        component_name = handler_script_name.split(".")[0]
        
        sys.path.append(folder)
        spec = importlib.util.spec_from_file_location(component_name, folder + "/" + handler_script_name)
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        instance = getattr(module, "DataSetHandler")
        
        self.handler = instance()
        outputs = self.handler.get_output_placeholders()
        for key, value in outputs.items():
            self.add_output(key, value=value)
        return outputs

    def run(self):
        pass

    def stop(self):
        pass
