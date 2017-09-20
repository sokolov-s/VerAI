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
from plainbox.impl import result

json_file = None
created_components_outputs = dict()
components_instance = dict()
components_test_json = dict()
components_train_json = dict()
dataset = None

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
    
def set_inputs(module_instance, json_item, created_components):
    if "inputs" in json_item and json_item["inputs"]:
        for dependence in json_item["inputs"]:
            dep_obj_name, dep_var_name = str(dependence["value"]).split('.')
            if dep_obj_name in created_components.keys():
                if dep_var_name in created_components[dep_obj_name].keys():
                    module_instance.set_input(dependence["name"], created_components[dep_obj_name][dep_var_name])
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

def set_propertie(module_instance, json_item, property_name):
    if property_name in json_item:
        try:
            module_instance.set_property(property_name, json_item[property_name])
        except Exception as ex:
            print("\033[91m%s\033[0m" % ex)
        
def set_properties(module_instance, json_item):
    set_propertie(module_instance, json_item, module_instance.ComponentProperties.VERSION)
    set_propertie(module_instance, json_item, module_instance.ComponentProperties.TYPE)
    set_propertie(module_instance, json_item, module_instance.ComponentProperties.TIMESTAMP)
    
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

def create_components(js_data):
    json_items = list(js_data.keys())
    modules = ComponentLoader()
    global created_components_outputs
    global components_train_json
    global components_test_json
    global dataset
    i = 0
    while len(json_items) > 0:
        item_name = json_items[i]
        json_item = js_data[item_name]
        
        print("Parse object %s :%s" % (item_name, json_item))
        ready_to_create = check_dependences(json_item, created_components_outputs)
        if ready_to_create:
            print("Create object %s :%s" % (item_name, json_item))
            new_obj = modules.get_component_instance(class_name=json_item["class"], version=json_item["version"], name=item_name,
                                                 tf_session=None)
            if not new_obj:
                raise ValueError("Can't find module instance: class_name = %s, version = %s" %
                                 (json_item["class"], json_item["version"]))
            set_inputs(new_obj, json_item, created_components_outputs)
            set_params(new_obj, json_item)
            set_properties(new_obj, json_item)
            results = new_obj.init()
            
            created_components_outputs[item_name] = results
            components_instance[item_name] = new_obj
            if new_obj.get_property(new_obj.ComponentProperties.TYPE) == "train":
                components_train_json[item_name] = json_item
            elif new_obj.get_property(new_obj.ComponentProperties.TYPE) == "test":
                components_test_json[item_name] = json_item
            
            if json_item["class"] == "DataSet":
                dataset = new_obj
             
            json_items.remove(item_name)
    
        i = i + 1 if i < len(json_items) - 1 else 0

def get_dependence_names(json_item):
    res = list()
    if "inputs" in json_item and json_item["inputs"]:
        for dependence in json_item["inputs"]:
            dep_obj_name, dep_var_name = str(dependence["value"]).split('.')
            if dep_obj_name:
                res.append(dep_obj_name)
    return res
    
def get_components_to_run(components):
    global created_components_outputs
    component_names = set(components.keys())
    dependence_names = set()
    for component_name, json_value in components.items():
        names = set(get_dependence_names(json_value))
        dependence_names.update(names)
    components_to_run = component_names.difference(set(dependence_names))
    res = list()
    for component_name in components_to_run:
        outputs = created_components_outputs[component_name]
        for tf_component in outputs.values(): 
            res.append(tf_component) 
    return res

def get_dataset(components):
    #TODO: looking for all input parameters and find datasets 
    global dataset
    res = list()
    res.append(dataset)
    return res
    
def run_train(session):
    global components_train_json;
    components = get_components_to_run(components_train_json)
    datasets = get_dataset(components)
    feed_dicts = {}
    for dataset in datasets:
        dataset.get_handler().switch_iteration(dataset.get_handler().Iterations.TRAIN)
    for _ in range(1000):
        for dataset in datasets:
            feed_dict = dataset.get_handler().fill_next_feed_dict(100)
            feed_dicts.update(feed_dict)
        session.run(components, feed_dict=feed_dicts)

def run_test(session):
    global components_test_json;
    components = get_components_to_run(components_test_json)
    datasets = get_dataset(components)
    feed_dicts = {}
    for dataset in datasets:
        dataset.get_handler().switch_iteration(dataset.get_handler().Iterations.TEST)
    for dataset in datasets:
        feed_dict = dataset.get_handler().fill_next_feed_dict(100)
        feed_dicts.update(feed_dict)
    print(session.run(components, feed_dict=feed_dicts))

def run_model():
    sess = tf.InteractiveSession()
    tf.global_variables_initializer().run()
    run_train(sess)
    run_test(sess)
         
def main():
    parse_argv()
    global json_file
    print("Load model from %s" % json_file)
    if json_file is None or not os.path.isfile(json_file):
        print("Can't find json file")
        exit(2)
    js_data = load_json(json_file)
    
    create_components(js_data)
    
    run_model()
    

main()
print("Model has been built")
