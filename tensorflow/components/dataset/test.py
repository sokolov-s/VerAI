#!/usr/bin/env python3.5

from dataset import DataSet


ds = DataSet()
ds.switch_iteration(DataSet.Iterations.TEST)
ds.switch_iteration(DataSet.Iterations.TRAIN)
ds.switch_iteration(DataSet.Iterations.VALIDATE)
ds.switch_iteration(DataSet.Iterations.INFERENCE)
