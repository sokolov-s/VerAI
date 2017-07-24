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


class TensorInfo(Enum):
    INSTANCE = "instance"
    CLASS_NAME = "class_name"
    PATH = "path"
    VERSION = "version"

know_classes = {}


def load_tensors():
    folder = "./tensors/"
    sys.path.append(folder)
    files = glob.glob(folder + "./*.py")
    module_names = [basename(f)[:-3] for f in files if isfile(f) and not f.endswith('__init__.py')]
    tensors = {}
    for mod_name in module_names:
        mod = importlib.import_module(mod_name)
        p = parse(inspect.getsource(mod))
        classes_in_module = [cls.name for cls in p.body if isinstance(cls, ClassDef)]
        for class_name in classes_in_module:
            if class_name == "Base":
                continue
            tensors[mod_name] = {TensorInfo.INSTANCE: getattr(mod, class_name),
                                 TensorInfo.CLASS_NAME: class_name,
                                 TensorInfo.PATH: os.path.abspath(folder + mod_name + ".py"),
                                 TensorInfo.VERSION: getattr(mod, class_name).get_version()
                                 }
    return tensors


def get_t_instance(class_name, version):
    global know_classes
    for inst in know_classes:
        if inst[TensorInfo.CLASS_NAME] == class_name and inst[TensorInfo.VERSION] == version:
            return inst[TensorInfo.INSTANCE]
    return None

with open(json_file) as fp:
    try:
        js_data = json.load(fp,  object_pairs_hook=OrderedDict)

    except ValueError:
        print("Bad json format in file %s" % json_file)
        sys.exit(2)

    known_classes = load_tensors()

