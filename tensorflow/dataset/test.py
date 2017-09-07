#!/usr/bin/env python3.5

import logging
import sys

log_level = logging.NOTSET
logging.basicConfig(stream=sys.stdout, level=log_level)
logger = logging.getLogger('root')


from dataset import DataSet

ds = DataSet()
ds.switch_iteration(DataSet.Iterations.TEST)



ds.switch_iteration(DataSet.Iterations.TRAIN)
ds.switch_iteration(DataSet.Iterations.VALIDATE)
ds.switch_iteration(DataSet.Iterations.INFERENCE)
