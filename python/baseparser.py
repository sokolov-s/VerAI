#!/usr/bin/env python3.5

from abc import ABC, abstractmethod
import json


class BaseParser(ABC):

    def __init__(self, name, type):
        self.name = name
        self.js_obj = None
        self.type = type

    @staticmethod
    def str_to_json(str):
        return json.loads(json.dumps(str))

    def set_json(self, json_obj):
        self.js_obj = BaseParser.str_to_json(json_obj)

    def is_json_known(self, json_obj):
        obj = json_obj if json_obj is not None else self.js_obj
        if obj is None:
            return False
        return obj["type"] == self.type

    @abstractmethod
    def parse(self):
        pass

    @abstractmethod
    def generate_code(self):
        pass
