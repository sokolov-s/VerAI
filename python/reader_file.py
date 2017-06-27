#!/usr/bin/env python3.5


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
        result = [x.strip('\n') for x in self.fp.readlines()]
        return str(result)

    def readline(self):
        return self.fp.readline()
