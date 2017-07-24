#!/usr/bin/env python3.5

import json
import sys
from collections import OrderedDict


class JsonParser:
    """
    Some text about class
    """

    def __init__(self, file_name):
        with open(file_name) as fp:
            try:
                self.js_data = json.load(fp,  object_pairs_hook=OrderedDict)

            except ValueError:
                print("Bad json format in file %s" % file_name)
                sys.exit(2)

    def parse(self):

        pass




