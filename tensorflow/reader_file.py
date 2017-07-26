#!/usr/bin/env python3.5

import re
import numpy
from pydoc import locate

class ReaderFile(object):
    """
    Some text about class
    """

    def __init__(self, path, dtype: list):
        self.path = path
        self.fp = open(path, "r")
        self.dtype = dtype
        self.pos = 0

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.fp.close()

    def __create_result_list(self):
        res = []
        i = 0
        while i < len(self.dtype):
            res.append(list())
            i += 1
        return list(res)

    @staticmethod
    def __split_result(string):
        line_res = re.split("\W+", string)
        line_res = filter(None, line_res)
        return line_res

    def read(self):
        res = self.__create_result_list()
        self.fp.seek(self.pos)
        for line in self.fp.readlines():
            line_res = self.__split_result(line)
            i = 0
            for value in line_res:
                res[i].append(self.dtype[i](value))
                i += 1
        return res

    def readline(self, nlines=1):
        res = self.__create_result_list()
        cnt = 0
        self.fp.seek(self.pos)
        for line in self.fp.readlines():
            self.pos += len(line)
            line_res = self.__split_result(line)
            i = 0
            for value in line_res:
                res[i].append(locate("numpy." + self.dtype[i])(value))
                i += 1
            cnt += 1
            if cnt >= nlines:
                break
        return res

    def set_position(self, pos):
        self.pos = pos


