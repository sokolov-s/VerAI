#!/usr/bin/env python3.5

import base
import tensorflow as tf


class DataSet(base.Base):
    """
    Some text about class
    """

    class Params:
        TYPE = "file_type"
        FOLDER = "folder"
        TRAIN_INPUT = "train_input"
        TRAIN_TARGET = "train_target"
        NAME = "name"

    class Output:
        TRAIN_INPUT = "train_input"
        TRAIN_TARGET = "train_target"
        TEST_INPUT = "test_input"
        TEST_TARGET = "test_target"
        VALIDATE_INPUT = "validate_input"
        VALIDATE_TARGET = "validate_target"

    def __init__(self, tf_session, name):
        base.Base.__init__(self, tf_session=tf_session, name=name)

        self.add_output(self.Output.TRAIN_INPUT, desc="Data for train")
        self.add_output(self.Output.TRAIN_TARGET, desc="Expected results for train data")
        self.add_output(self.Output.TEST_INPUT, desc="Data for test")
        self.add_output(self.Output.TEST_TARGET, desc="Expected results for test data")
        self.add_output(self.Output.VALIDATE_INPUT, desc="Data for validate")
        self.add_output(self.Output.VALIDATE_TARGET, desc="Expected results for validate data")

        self.add_param(self.Params.TYPE, important=True)
        self.add_param(self.Params.FOLDER, important=True)
        self.add_param(self.Params.TRAIN_INPUT, important=True)
        self.add_param(self.Params.TRAIN_TARGET, important=True)
        self.add_param(self.Params.NAME, important=False)

        self.dataset_in = None
        self.dataset_target = None
        self.it_in = None
        self.it_target = None

    # Base class interface implementation
    def init(self):
        if self.get_param(self.Params.TYPE) == "text":
            self.dataset_in = tf.contrib.data.TextLineDataset(self.get_param(self.Params.TRAIN_INPUT)).repeat()
            self.dataset_target = tf.contrib.data.TextLineDataset(self.get_param(self.Params.TRAIN_TARGET)).repeat()
        self.it_in = self.dataset_in.make_one_shot_iterator()
        self.it_target = self.dataset_target.make_one_shot_iterator()
        train_input = tf.Variable(0.0)
        train_target = tf.Variable(0.0)
        test_input = tf.Variable(0.0)
        test_target = tf.Variable(0.0)
        validate_input = tf.Variable(0.0)
        validate_target = tf.Variable(0.0)
        outputs = {self.Output.TRAIN_INPUT: train_input,
                   self.Output.TRAIN_TARGET: train_target,
                   self.Output.TEST_INPUT: test_input,
                   self.Output.TEST_TARGET: test_target,
                   self.Output.VALIDATE_INPUT: validate_input,
                   self.Output.VALIDATE_TARGET: validate_target
                   }
        self.set_outputs(outputs)
        return outputs

    def run(self):
        train_in = tf.string_to_number(self.it_in.get_next())
        train_target = tf.string_to_number(self.it_target.get_next())
        copy_in = self.get_param(self.Output.TRAIN_INPUT).assign(train_in)
        copy_target = self.get_param(self.Output.TRAIN_TARGET).assign(train_target)
        self.get_session().run(copy_in)
        self.get_session().run(copy_target)

    def stop(self):
        pass
