#!/usr/bin/env python3.5

import baseparser as bp
import reader_file as freader
from pydoc import locate


class ReaderProxy(bp.BaseParser):
    """
    Some text about class
    """

    def __init__(self, name):
        bp.BaseParser.__init__(self, name, "reader")
        self.dataset = None
        self.reader = None

    def get_dataset_name(self):
        name = self.get_json()["init"]["dataset"]
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
        code = ""
        i = 0
        data = self.reader.read()
        for value in data:
            if len(self.get_json()["output"]) > i:
                code += self.get_json()["output"][i] + " = " + \
                        str(list(map(locate(self.dataset["data_type"]), value))) + "\n"
                code += self.get_json()["output"][i] + " = tf.placeholder(tf.float32, name="Output")"
            else:
                break
            i += 1
        print(code)
        return code
