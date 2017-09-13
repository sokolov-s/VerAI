#!/usr/bin/env python3.5

import logging
import sys
import dataset_reader as reader
import tensorflow as tf
from numpy import shape
import json
from _collections import OrderedDict
from dataset_handler import DataSetHandler 

def get_properties(value):
    cur_dtype = value.dtype
    cur_shape = shape(value)
    return {"dtype": cur_dtype, "shape": cur_shape}

def get_outputs_properties(dataset):
    res = dataset.next_batch(1)
    result_properties = dict()
    for key,value in res.items():
        result_properties[key] = get_properties(value)
    return result_properties

def compare_data(dataset, target):
    i = 1
    while i < dataset.num_examples:
        res = dataset.next_batch(1)
        assert len(res) == len(target)
        for key, value in res.items():
             properties = get_properties(value)
             assert key in target.keys()
             assert target[key]["dtype"] == properties["dtype"]
             assert target[key]["shape"] == properties["shape"]
        i += 1

def check_dataset(dataset):
    properties = get_outputs_properties(dataset)
    compare_data(dataset, properties)

def main():
    dsets = reader.read_data_sets(one_hot=True)
#TODO: !!!!!!!!!!!!     use     outputs_description() !!!!!!!!!!!!!!!!!!    
    cur_ds = dsets.train
    properties_train = get_outputs_properties(cur_ds)

    print(properties_train)
    descr = cur_ds.outputs_description()
    print(descr)
    exit -1
    
    compare_data(cur_ds, properties_train)
    print("Train data Ok")
    
    cur_ds = dsets.test
    properties_test = get_outputs_properties(cur_ds)
    compare_data(cur_ds, properties_test)
    print("Test data Ok")
    
    cur_ds = dsets.validation
    properties_validation = get_outputs_properties(cur_ds)
    compare_data(cur_ds, properties_validation)
    print("Validation data Ok")
    
    assert properties_train == properties_train
    assert properties_train == properties_validation
    print("Outputs parameters is the same")
    
    data = dict()
    outputs = list()
    for key, value in properties_train.items():
        output_info = dict()
        output_info["name"] = key
        output_info["dtype"] = str(value["dtype"])
        output_info["shape"] = str(value["shape"])
        outputs.append(output_info)
    data["outputs"] = outputs     
    filename = "outputs.json"
    with open("./" + filename, "w") as out:
        json.dump(data, out)
    

handler = DataSetHandler()
feed_dict_result = handler.fill_next_feed_dict(1)
outputs = handler.output_placeholders()

x = tf.Variable(2.0, dtype=outputs["labels"].dtype)
y = tf.multiply(x, outputs["labels"])  

init = tf.global_variables_initializer()
sess = tf.Session()
sess.run(init)

print(feed_dict_result)
print(sess.run(y, feed_dict=feed_dict_result)) 
    
#main()