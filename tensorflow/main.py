#!/usr/bin/env python3.5

import sys
import getopt
import os.path
import json
from collections import OrderedDict

from os.path import dirname, basename, isfile
import glob
from ast import ClassDef, parse
import importlib
import inspect
from enum import Enum
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


class ModelInfo(Enum):
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


def get_model_instance(class_name: str, version: str, obj_name: str):
    global known_classes
    for obj in known_classes:
        if obj[ModelInfo.CLASS_NAME] == class_name and obj[ModelInfo.VERSION] == version:
            return obj[ModelInfo.INSTANCE](obj_name)
    return None

with open(json_file) as fp:
    try:
        js_data = json.load(fp,  object_pairs_hook=OrderedDict)

    except ValueError:
        print("Bad json format in file %s" % json_file)
        sys.exit(2)

    known_classes = load_models()
    json_items = []
    for key in js_data.keys():
        json_items.append(key)

    i = 0
    while len(json_items) > 0:

        i = i + 1 if i < len(json_items) else 0

    for key, value in known_classes.items():
        print("Init %s" % key)
        value[ModelInfo.INSTANCE](key).init()
