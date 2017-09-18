#!/usr/bin/env python3.5

import sys
import getopt
import os.path
import json
import copy
from collections import OrderedDict
from component_loader import ComponentLoader
import tensorflow as tf
from asyncore import read

json_file = None

def print_no_argument_and_exit():
    print("Bad parameters")
    print("Run script with command --help to show usage page")
    exit(2)

def parse_argv():
    global json_file
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
    
def set_inputs(module_instance, json_item, created_models):
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


def load_json(file):    
    with open(file) as fp:
        try:
            data = json.load(fp, object_pairs_hook=OrderedDict)
        except ValueError:
            print("Bad json format in %s" % file)
            sys.exit(2)
    return data

def check_dependences(json_item, created_components):
    ready_to_create = True
    dep_obj_name = ""
    dep_var_name = ""
    if "inputs" in json_item and json_item["inputs"]:
        for dependence in json_item["inputs"]:
            if "important" not in dependence.keys() or dependence["important"]:
                dep_list = list()
                if isinstance(dependence["value"], dict) or isinstance(dependence["value"], list):
                    for obj in dependence["value"]:
                        dep_obj_name, dep_var_name = obj.split('.')
                        dep_list.append({"obj_name" : dep_obj_name, "var_name" : dep_var_name})
                else:
                    dep_obj_name, dep_var_name = str(dependence["value"]).split('.')
                    dep_list.append({"obj_name" : dep_obj_name, "var_name" : dep_var_name})
                    
                for dep in dep_list:
                    obj_name = dep["obj_name"] 
                    var_name = dep["var_name"]
                    print("Find for dependence: object = %s, variable = %s" % (obj_name, var_name))
                    if obj_name in created_components.keys():
                        if var_name not in created_components[obj_name].keys():
                            print("\033[91mCan't find dependence variable in object %s: %s\033[0m" %
                                  (obj_name, var_name))
                            raise Exception("\033[93mCan't find dependence variable in object %s: %s\033[0m" %
                                        (obj_name, var_name))
                            return False
                    else:
                        print("\033[93mCan't find dependence: %s.%s. Object creation has been postponed\033[0m" %
                              (obj_name, var_name))
                        return False
    return True
                        
def main():
    parse_argv()
    global json_file
    print(json_file)
    if json_file is None or not os.path.isfile(json_file):
        print("Can't find json file")
        exit(2)
        
    js_data = load_json(json_file)
    json_items = list(js_data.keys())
    
    modules = ComponentLoader()
    created_models = {}
    sess = tf.Session()
    i = 0
    while len(json_items) > 0:
        item_name = json_items[i]
        item = js_data[item_name]
        
        print("Parse object %s :%s" % (item_name, item))
        ready_to_create = check_dependences(item, created_models)
        if ready_to_create:
            print("Create object %s :%s" % (item_name, item))
            new_obj = modules.get_component_instance(class_name=item["class"], version=item["version"], name=item_name,
                                                 tf_session=sess)
            if not new_obj:
                raise ValueError("Can't find module instance: class_name = %s, version = %s" %
                                 (item["class"], item["version"]))
            set_inputs(new_obj, item, created_models)
            set_params(new_obj, item)
            results = new_obj.init()
            created_models[item_name] = results
            json_items.remove(item_name)
    
        i = i + 1 if i < len(json_items) - 1 else 0

main()
print("Model has been built")
