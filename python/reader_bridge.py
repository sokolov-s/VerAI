#!/usr/bin/env python3.5

import baseparser as bp
import reader_file as freader

class ReaderBridge(bp.BaseParser):
    """
    Some text about class
    """

    def __init__(self, name):
        bp.BaseParser.__init__(self, name, "reader")
        self.dataset = None
        self.reader = None

    def get_dataset_name(self):
        name = self.js_obj["init"]["dataset"]
        name = name.split('.')
        return name[0]

    def set_dataset(self, dataset_json):
        self.dataset = bp.BaseParser.str_to_json(dataset_json)

    def parse(self):
        db_type = self.dataset["init"]["db_type"]
        path = self.dataset["init"]["path"]
        if db_type == "file":
            self.reader = freader.ReaderFile(path)

    def generate_code(self):
        code = self.js_obj["output"][0] + " = " + self.reader.read()
        return code
