#!/usr/bin/env python3.5

import baseparser as bp
import reader_file as freader
from pydoc import locate
import numpy


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

    def generate_imports(self):
        code = "import numpy as np\n"
        code += self.reader.generate_imports()
        return code

    def generate_code(self):
        code = ""
        i = 0
        data = self.reader.read()
        value_type = self.dataset["data_type"]
        for value in data:
            if len(self.get_json()["output"]) > i:

                code += self.var_name_form_json(self.get_dataset_name(), self.dataset, i) + " = " + \
                        str(list(map(locate("numpy." + value_type), value))) + "\n"
                code += self.var_name_form_json(self.get_name(), self.get_json(), i) + " = tf.placeholder(tf." + value_type + \
                        ", name=\"" + self.get_json()["output"][i] + "\")\n"
            else:
                break
            i += 1
        self.set_head_code("import numpy\n")
        self.set_body_code(code)
