#!/usr/bin/env python3.5

import sys
import getopt
import os.path
import json
from collections import OrderedDict

from os.path import basename, isfile
import glob
from ast import ClassDef, parse
import importlib
import inspect
import subprocess

json_file = None


def print_no_argument_and_exit():
    print("Bad parameters")
    print("Run script with command --help to show usage page")
    exit(2)

try:
    if len(sys.argv) <= 1:
        print_no_argument_and_exit()
    opts, args = getopt.getopt(sys.argv[1:], "hi:o:", ["help", "ifile="])
except getopt.GetoptError:
    print_no_argument_and_exit()

for opt, arg in opts:
    if opt in ('-h', "--help"):
        print("""Script parse json file, translate it into tensorflow model and execute this model
        
Usage: main.py [OPTIONS]
Example: python3 main.py -i file_with_model_info.json

Options:
    -h, --help          : show help dialog
    -i, --ifile=FILE    : path to json file
        """)
        sys.exit()
    elif opt in ("-i", "--ifile"):
        json_file = arg
    else:
        print("Unknown parameter %s" % opt)
        exit(2)

if json_file is None or not os.path.isfile(json_file):
    print("Can't find json file")
    exit(2)


class ModelInfo:
    INSTANCE = "instance"
    CLASS_NAME = "class_name"
    PATH = "path"
    VERSION = "version"

known_classes = {}


def load_models():
    folder = "./components/"
    print("Load models from %s" % folder)
    sys.path.append(folder)
    files = glob.glob(folder + "./*.py")
    module_names = [basename(f)[:-3] for f in files if isfile(f) and not f.endswith('__init__.py')]
    models = {}
    for mod_name in module_names:
        mod = importlib.import_module(mod_name)
        p = parse(inspect.getsource(mod))
        classes_in_module = [cls.name for cls in p.body if isinstance(cls, ClassDef)]
        for class_name in classes_in_module:
            if class_name == "Base":
                continue
            version = subprocess.check_output(['git', 'rev-parse', ":" + folder + mod_name + ".py"])
            version = version.decode("utf-8")[:-1]
            models[mod_name] = {ModelInfo.INSTANCE: getattr(mod, class_name),
                                ModelInfo.CLASS_NAME: class_name,
                                ModelInfo.PATH: os.path.abspath(folder + mod_name + ".py"),
                                ModelInfo.VERSION: version
                                }
            print("Add component %s version %s" % (models[mod_name][ModelInfo.CLASS_NAME],
                                                   models[mod_name][ModelInfo.VERSION]))
    return models


def get_model_instance(class_name: str, version: str, name: str):
    global known_classes
    for obj_name, obj_value in known_classes.items():
        if obj_value[ModelInfo.CLASS_NAME] == class_name and obj_value[ModelInfo.VERSION] == version:
            return obj_value[ModelInfo.INSTANCE](name)
    return None


def set_inputs(module_instance, json_item):
    if "input" in json_item and json_item["input"]:
        for dependence in json_item["input"]:
            dep_obj_name, dep_var_name = str(dependence["value"]).split('.')
            if dep_obj_name in created_models.keys():
                if dep_var_name in created_models[dep_obj_name].keys():
                    module_instance.set_input(dependence["name"], created_models[dep_obj_name][dep_var_name])
                else:
                    raise ValueError("Ca't find dependence value")
                    break


def set_params(module_instance, json_item):
    if "params" in json_item and json_item["params"]:
        for param in json_item["params"]:
            try:
                module_instance.set_param(param["name"], param["value"])
            except Exception as ex:
                print("\033[91m%s\033[0m" % ex)


with open(json_file) as fp:
    try:
        js_data = json.load(fp,  object_pairs_hook=OrderedDict)

    except ValueError:
        print("Bad json format in file %s" % json_file)
        sys.exit(2)

    known_classes = load_models()
    json_items = []
    created_models = {}
    for key in js_data.keys():
        json_items.append(key)

    i = 0
    while len(json_items) > 0:
        item_name = json_items[i]
        item = js_data[item_name]
        print("Parse object %s" % item)
        ready_to_create = True
        dep_obj_name = ""
        dep_var_name = ""
        if "input" in item and item["input"]:
            for dependence in item["input"]:
                if "important" not in dependence.keys() or dependence["important"]:
                    dep_list = []
                    if isinstance(dependence["value"], dict) or isinstance(dependence["value"], list):
                        for obj in dependence["value"]:
                            dep_obj_name, dep_var_name = obj.split('.')
                            dep_list.append([dep_obj_name, dep_var_name])
                    else:
                        dep_obj_name, dep_var_name = str(dependence["value"]).split('.')
                        dep_list.append([dep_obj_name, dep_var_name])
                    for dep in dep_list:
                        dep_obj_name, dep_var_name = dep
                        print("Find dependence: object = %s, variable = %s" % (dep_obj_name, dep_var_name))
                        if dep_obj_name in created_models.keys():
                            if dep_var_name in created_models[dep_obj_name].keys():
                                ready_to_create = True
                            else:
                                print("\033[91mCan't find dependence variable in object %s: %s\033[0m" %
                                      (dep_obj_name, dep_var_name))
                                ready_to_create = False
                                raise Exception("\033[93mCan't find dependence variable in object %s: %s\033[0m" %
                                            (dep_obj_name, dep_var_name))
                                break
                        else:
                            print("\033[93mCan't find dependence: %s\033[0m. %s Object creation has postponed " %
                                  (dep_obj_name, item_name))
                            ready_to_create = False
                            break
        if ready_to_create:
            print("Crate object :%s" % item)
            new_obj = get_model_instance(item["class"], item["version"], item_name)
            if not new_obj:
                raise ValueError("Can't find module instance: class_name = %s, version = %s" %
                                 (item["class"], item["version"]))
            set_inputs(new_obj, item)
            set_params(new_obj, item)
            results = new_obj.init()
            created_models[item_name] = results
            json_items.remove(item_name)

        i = i + 1 if i < len(json_items) - 1 else 0

    for key, value in known_classes.items():
        print("Init %s" % key)
        value[ModelInfo.INSTANCE](key).init()
