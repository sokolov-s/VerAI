#!/usr/bin/env python3.5

from abc import ABC, abstractmethod
import json

class Base(ABC):

    def __init__(self, name):
        self.name = name
        self.js_obj = None

    def set_json(self, json_obj):
        self.js_obj = json.loads(json.dumps(json_obj))

    @abstractmethod
    def is_json_known(self, json_obj):
        pass

    @abstractmethod
    def parse(self):
        pass
