#!/usr/bin/env python3.5

from abc import ABCMeta, abstractmethod
from typing import Any, Dict
from enum import Enum

class Base(metaclass=ABCMeta):
    """
    Some text about class
    """

    class ParamType(Enum):
        VALUE = "value"
        DESC = "description"

    def __init__(self, name):
        self.name = name
        self.inputs = dict()
        self.outputs = dict()
        self.params = dict()

    @staticmethod
    def __desc_to_list(desc: dict):
        res = list()
        for param in desc.keys():
            res.append(param)
        return res if len(res) else None

    def get_name(self):
        return self.name

    def _add_input(self, name: str, value=None, desc=""):
        self.inputs[name] = {Base.ParamType.DESC: desc, Base.ParamType.VALUE: value}

    def __check_input(self, name):
        if name not in self.inputs.keys():
            raise ValueError("\"%s\" did not match as input value" % name)

    def set_input(self, name, value):
        self.__check_input(name)
        self.inputs[name][Base.ParamType.VALUE] = value

    def get_input(self, name):
        self.__check_input(name)
        return self.inputs[name][Base.ParamType.VALUE]

    def _add_output(self, output_name: str, value=None, desc=""):
        self.outputs[output_name] = {Base.ParamType.DESC: desc, Base.ParamType.VALUE: value}

    def __check_output(self, name):
        if name not in self.outputs.keys():
            raise ValueError("\"%s\" did not match as input value" % name)

    def set_output(self, name, value):
        self.__check_output(name)
        self.outputs[name][Base.ParamType.VALUE] = value

    def get_output(self, name):
        self.__check_output(name)
        return self.outputs[name][Base.ParamType.VALUE]

    def _add_param(self, param_name: str, value=None, desc=""):
        self.params[param_name] = {Base.ParamType.DESC: desc, Base.ParamType.VALUE: value}

    def get_param_list(self):
        return self.__desc_to_list(self.params)

    def __check_param(self, name):
        if name not in self.params.keys():
            raise ValueError("\"%s\" did not match as parameter name" % name)

    def set_param(self, name, value):
        self.__check_param(name)
        self.params[name][Base.ParamType.VALUE] = value

    def get_param(self, name):
        self.__check_param(name)
        return self.params[name][Base.ParamType.VALUE]

    @staticmethod
    @abstractmethod
    def get_operation_name():
        pass

    @staticmethod
    @abstractmethod
    def get_version():
        pass




