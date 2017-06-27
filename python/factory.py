#!/usr/bin/env python3.5

import sys
import json
from collections import OrderedDict
import reader_bridge as reader

class Factory(object):
    """
    Some text about class
    """
    def __init__(self):
        self.json = None

    def __open_json(self, path_to_json):
        with open(path_to_json) as fp:
            try:
                return json.load(fp,  object_pairs_hook=OrderedDict)

            except ValueError:
                print("Bad json format in file %s" % path_to_json)
                sys.exit(2)

    def create(self, path_to_json):
        data = self.__open_json(path_to_json)
        print("Parse json file %s" % path_to_json)
        result = []
        for key, value in data.items():
            if reader.ReaderBridge(key).is_json_known(value):
                r = reader.ReaderBridge(key)
                r.set_json(value)
                dname = r.get_dataset_name()
                r.set_dataset(data[dname])
                result.append(r)
            else:
                print("Unknown json object : %s (%s)" % (key, value))
                continue
        return result

