#!/usr/bin/env python3.5

import sys
import os.path
from os.path import basename, isfile
import glob
from ast import ClassDef, parse
import importlib
import inspect
import subprocess
from singleton import Singleton
import importlib.util


class ComponentLoader(metaclass=Singleton):
    __shared_state = {}
    DEF_COMPONENTS_DIR = "./components/"

    class ModuleInfo:
        INSTANCE = "instance"
        CLASS_NAME = "class_name"
        PATH = "path"
        VERSION = "version"

    def __init__(self):
        print("ModuleLoaderId = %s" % id(self))
        self.__loaded_folders = {}
        self.__components = {}
        self.load_components(ComponentLoader.DEF_COMPONENTS_DIR)

    def load_components(self, folder: str):
        if folder in self.__loaded_folders:
            return
        print("Load components from %s" % folder)
        sys.path.append(folder)
        files = glob.glob(folder + "./*.py")
        component_names = [basename(f)[:-3] for f in files if isfile(f) and not f.endswith('__init__.py')]
        components = {}
        for component_name in component_names:
            print("Load component %s" % component_name)
            spec = importlib.util.spec_from_file_location(component_name, folder + "/" + component_name + ".py")
            instance = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(instance)

            p = parse(inspect.getsource(instance))
            classes_in_module = [cls.name for cls in p.body if isinstance(cls, ClassDef)]
            #TODO: Do not search in cicle 
            for class_name in classes_in_module:
                if class_name == "Base":
                    continue
                version = subprocess.check_output(['git', 'rev-parse', ":" + folder + component_name + ".py"])
                version = version.decode("utf-8")[:-1]
                components[component_name] = {ComponentLoader.ModuleInfo.INSTANCE: getattr(instance, class_name),
                                              ComponentLoader.ModuleInfo.CLASS_NAME: class_name,
                                              ComponentLoader.ModuleInfo.PATH: os.path.abspath(folder + component_name + ".py"),
                                              ComponentLoader.ModuleInfo.VERSION: version
                                              }
                print("Add module %s version %s" % (components[component_name][ComponentLoader.ModuleInfo.CLASS_NAME],
                                                    components[component_name][ComponentLoader.ModuleInfo.VERSION]))
        self.__components.update(components)

    def get_component_instance(self, class_name: str, version: str, name: str, tf_session):
        for obj_name, obj_value in self.__components.items():
            if obj_value[ComponentLoader.ModuleInfo.CLASS_NAME] == class_name and \
                            obj_value[ComponentLoader.ModuleInfo.VERSION] == version:
                return obj_value[ComponentLoader.ModuleInfo.INSTANCE](tf_session=tf_session, name=name)
        return None

    def get_loaded_components(self):
        return self.__components

