#!/usr/bin/env python3.5

from abc import ABCMeta, abstractmethod
import tensorflow as tf
from email.policy import default


class Base(metaclass=ABCMeta):
    """
    Some text about class
    """

    class ParamType:
        VALUE = "value"
        DESC = "description"
        IMPORTANT = "important"
    
    class ComponentProperties:
        VERSION = "version"
        TYPE = "component_type"
        TIMESTAMP = "timestamp"
        
    def __init__(self, tf_session, name):
        self.__name = name
        self.__inputs = dict()
        self.__outputs = dict()
        self.__params = dict()
        self.__sess = tf_session
        self.__properties = {
            self.ComponentProperties.VERSION: None,
            self.ComponentProperties.TYPE: None,
            self.ComponentProperties.TIMESTAMP: None
            }

    def get_name(self):
        return self.__name

    def get_class_name(self):
        return self.__class__.__name__

    @staticmethod
    def _add_param_to_obj(obj: dict, name: str, value=None, desc="", important=True):
        obj[name] = {Base.ParamType.DESC: desc, Base.ParamType.VALUE: value, Base.ParamType.IMPORTANT: important}

    @staticmethod
    def _check_param_name_in_obj(obj: dict, name: str):
        if name not in obj.keys():
            raise ValueError("\"%s\" did not match as key in dictionary (%s)" % (name, obj.keys()))

    def _set_param_value_in_obj(self, obj: dict, name: str, value):
        self._check_param_name_in_obj(obj=obj, name=name)
        obj[name][Base.ParamType.VALUE] = value

    def _set_params_value_in_obj(self, obj: dict, params: dict):
        for key, value in params.items():
            self._set_param_value_in_obj(obj, key, value)

    def _get_param_value_from_obj(self, obj: dict, name: str):
        self._check_param_name_in_obj(obj=obj, name=name)
        return obj[name][Base.ParamType.VALUE]

    def add_input(self, name: str, value=None, desc="", important=True):
        self._add_param_to_obj(obj=self.__inputs, name=name, value=value, desc=desc, important=important)

    def set_input(self, name, value):
        self._set_param_value_in_obj(obj=self.__inputs, name=name, value=value)

    def set_inputs(self, inputs: dict):
        self._set_params_value_in_obj(obj=self.__inputs, params=inputs)

    def get_input(self, name):
        return self._get_param_value_from_obj(self.__inputs, name)

    def get_inputs(self):
        return self.__inputs

    def add_output(self, name: str, value=None, desc=""):
        self._add_param_to_obj(obj=self.__outputs, name=name, value=value, desc=desc)

    def set_output(self, name, value):
        self._set_param_value_in_obj(obj=self.__outputs, name=name, value=value)

    def set_outputs(self, outputs: dict):
        self._set_params_value_in_obj(obj=self.__outputs, params=outputs)

    def get_output(self, name):
        return self._get_param_value_from_obj(self.__outputs, name)

    def get_outputs(self):
        return self.__outputs

    def add_param(self, name: str, value=None, desc="", important=True):
        self._add_param_to_obj(obj=self.__params, name=name, value=value, desc=desc, important=important)

    def set_param(self, name, value):
        self._set_param_value_in_obj(obj=self.__params, name=name, value=value)

    def set_params(self, params: dict):
        self._set_params_value_in_obj(obj=self.__params, params=params)

    def get_param(self, name):
        return self._get_param_value_from_obj(self.__params, name)

    def get_params(self):
        return self.__params

    def get_session(self):
        return self.__sess
    
    def __check_property(self, property: ComponentProperties):
        if property in self.get_properties():
            return True
        else:
            raise ValueError("%s is not a module property" % property)
    
    def get_property(self, property: ComponentProperties):
        self.__check_property(property)
        return self.__properties[property]
    
    def get_properties(self):
        return self.__properties
    
    def set_property(self, property: ComponentProperties, value):
        self.__check_property(property)
        self.__properties[property] = value
    
    def set_properties(self, properties: dict):
        for key, value in properties.items():
            self.set_property(key, value)
        
    @staticmethod
    def to_tf_dtype(dtype: str, default=None):
        res = tf.as_dtype(dtype) if dtype else default
        return res 
        
    @abstractmethod
    def init(self):
        pass

    @abstractmethod
    def run(self):
        pass

    @abstractmethod
    def stop(self):
        pass


