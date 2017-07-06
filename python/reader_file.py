#!/usr/bin/env python3.5

import re

class ReaderFile(object):
    """
    Some text about class
    """

    def __init__(self, path):
        self.path = path
        self.fp = open(path, "r")

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.fp.close()

    def read(self):
        data = []
        res = []
        for value in self.fp.readlines():
            re_res = re.split("\W+", value)
            data.append(re_res[0])
            res.append(re_res[1])
        return data, res
