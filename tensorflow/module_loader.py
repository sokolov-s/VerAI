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


class ModuleLoader(metaclass=Singleton):
    __shared_state = {}
    DEF_MODULES_FOLDER = "./components/"

    class ModuleInfo:
        INSTANCE = "instance"
        CLASS_NAME = "class_name"
        PATH = "path"
        VERSION = "version"

    def __init__(self):
        print("ModuleLoaderId = %s" % id(self))
        self.loaded_folders = {}
        self.modules = {}
        self.load_modules(ModuleLoader.DEF_MODULES_FOLDER)

    def load_modules(self, folder: str):
        if folder in self.loaded_folders:
            return
        print("Load modules from %s" % folder)
        sys.path.append(folder)
        files = glob.glob(folder + "./*.py")
        module_names = [basename(f)[:-3] for f in files if isfile(f) and not f.endswith('__init__.py')]
        modules = {}
        for module_name in module_names:
            mod = importlib.import_module(module_name)
            p = parse(inspect.getsource(mod))
            classes_in_module = [cls.name for cls in p.body if isinstance(cls, ClassDef)]
            for class_name in classes_in_module:
                if class_name == "Base":
                    continue
                version = subprocess.check_output(['git', 'rev-parse', ":" + folder + module_name + ".py"])
                version = version.decode("utf-8")[:-1]
                modules[module_name] = {ModuleLoader.ModuleInfo.INSTANCE: getattr(mod, class_name),
                                        ModuleLoader.ModuleInfo.CLASS_NAME: class_name,
                                        ModuleLoader.ModuleInfo.PATH: os.path.abspath(folder + module_name + ".py"),
                                        ModuleLoader.ModuleInfo.VERSION: version
                                        }
                print("Add module %s version %s" % (modules[module_name][ModuleLoader.ModuleInfo.CLASS_NAME],
                                                    modules[module_name][ModuleLoader.ModuleInfo.VERSION]))
        self.modules.update(modules)

    def get_model_instance(self, class_name: str, version: str, name: str):
        for obj_name, obj_value in self.modules.items():
            if obj_value[ModuleLoader.ModuleInfo.CLASS_NAME] == class_name and \
                            obj_value[ModuleLoader.ModuleInfo.VERSION] == version:
                return obj_value[ModuleLoader.ModuleInfo.INSTANCE](name)
        return None
