#!/usr/bin/env python3.5

from abc import ABCMeta, abstractmethod
from typing import Any, Dict
from enum import Enum
import inspect


class Base(metaclass=ABCMeta):
    """
    Some text about class
    """

    class ParamType(Enum):
        VALUE = "value"
        DESC = "description"
        IMPORTANT = "important"

    def __init__(self, name):
        self.name = name
        self.inputs = dict()
        self.outputs = dict()
        self.params = dict()

    def get_name(self):
        return self.name

    @staticmethod
    def __retrieve_name(var):
        callers_local_vars = inspect.currentframe().f_back.f_locals.items()
        return [var_name for var_name, var_val in callers_local_vars if var_val is var]

    @staticmethod
    def _add_param_to_obj(obj: dict, name: str, value=None, desc="", important=True):
        obj[name] = {Base.ParamType.DESC: desc, Base.ParamType.VALUE: value, Base.ParamType.IMPORTANT: important}

    @staticmethod
    def _check_param_name_in_obj(obj: dict, name: str):
        if name not in obj.keys():
            raise ValueError("\"%s\" did not match as key in %s" % (name, Base.__retrieve_name(obj)))

    @staticmethod
    def _set_param_value_in_obj(obj: dict, name: str, value):
        Base._check_param_name_in_obj(obj=obj, name=name)
        obj[name][Base.ParamType.VALUE] = value

    @staticmethod
    def _get_param_value_from_obj(obj: dict, name: str):
        Base._check_param_name_in_obj(obj=obj, name=name)
        return obj[name][Base.ParamType.VALUE]

    def _add_input(self, name: str, value=None, desc="", important=True):
        self._add_param_to_obj(obj=self.inputs, name=name, value=value, desc=desc, important=important)

    def set_input(self, name, value):
        self._set_param_value_in_obj(obj=self.inputs, name=name, value=value)

    def get_input(self, name):
        return self._get_param_value_from_obj(self.inputs, name)

    def _add_output(self, name: str, value=None, desc=""):
        self._add_param_to_obj(obj=self.outputs, name=name, value=value, desc=desc)

    def set_output(self, name, value):
        self._set_param_value_in_obj(obj=self.outputs, name=name, value=value)

    def get_output(self, name):
        return self._get_param_value_from_obj(self.outputs, name)

    def _add_param(self, name: str, value=None, desc="", important=True):
        self._add_param_to_obj(obj=self.params, name=name, value=value, desc=desc, important=important)

    def set_param(self, name, value):
        self._set_param_value_in_obj(obj=self.params, name=name, value=value)

    def get_param(self, name):
        return self._get_param_value_from_obj(self.params, name)

    @abstractmethod
    def init(self):
        pass

    @abstractmethod
    def run(self):
        pass

    @abstractmethod
    def stop(self):
        pass


