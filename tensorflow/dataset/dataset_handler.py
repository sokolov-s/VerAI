#!/usr/bin/env python3.5

from dataset_base import DataSetBase
import dataset_reader as dsreader
import tensorflow as tf


class DataSetHandler(DataSetBase):

    def __init__(self):
        DataSetBase.__init__(self)
        self.datasets = dsreader.read_data_sets(one_hot=True)
        self.cur_dataset = None
        self.switch_iteration(DataSetBase.Iterations.TRAIN)
        
        outputs_descr = self.cur_dataset.outputs_description()
        self.tf_outputs = {}
        for v in outputs_descr: 
            self.tf_outputs[v["name"]] = tf.placeholder(v["dtype"], v["shape"] , v["name"])
             

    def switch_iteration(self, value: DataSetBase.Iterations):
        self._set_iteration(value)
        if DataSetBase.Iterations.TRAIN == value:
            self.cur_dataset = self.datasets.train
        elif DataSetBase.Iterations.TEST == value:
            self.cur_dataset = self.datasets.test
        elif DataSetBase.Iterations.VALIDATION == value:
            self.cur_dataset = self.datasets.validation
        else:
            raise ValueError("Unsupported iteration: %s" % value)
        

    def fill_next_feed_dict(self, batch_size):
        outputs = self.cur_dataset.next_batch(batch_size)
        feed_dict = {}
        for key,value in self.tf_outputs.items():
            feed_dict[value] = outputs[key]
        return feed_dict
    
    def output_placeholders(self):
        return self.tf_outputs
