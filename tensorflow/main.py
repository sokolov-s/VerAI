#!/usr/bin/env python3.5

import sys
import getopt
import os.path
import json
from collections import OrderedDict
from module_loader import ModuleLoader
import tensorflow as tf

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


def set_inputs(module_instance, json_item):
    if "inputs" in json_item and json_item["inputs"]:
        for dependence in json_item["inputs"]:
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

sess = tf.Session()

with open(json_file) as fp:
    try:
        js_data = json.load(fp,  object_pairs_hook=OrderedDict)

    except ValueError:
        print("Bad json format in file %s" % json_file)
        sys.exit(2)

    modules = ModuleLoader()

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
        if "inputs" in item and item["inputs"]:
            for dependence in item["inputs"]:
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
                            print("\033[93mCan't find dependence: %s. %s Object creation has postponed\033[0m" %
                                  (dep_obj_name, item_name))
                            ready_to_create = False
                            break
        if ready_to_create:
            print("Crate object :%s" % item)
            new_obj = modules.get_model_instance(class_name=item["class"], version=item["version"], name=item_name,
                                                 tf_session=sess)
            if not new_obj:
                raise ValueError("Can't find module instance: class_name = %s, version = %s" %
                                 (item["class"], item["version"]))
            set_inputs(new_obj, item)
            set_params(new_obj, item)
            results = new_obj.init()
            created_models[item_name] = results
            json_items.remove(item_name)

        i = i + 1 if i < len(json_items) - 1 else 0

