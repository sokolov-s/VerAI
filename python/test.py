#!/usr/bin/env python3.5
# -*- coding: utf-8 -*-
import tensorflow as tf
#from tensorflow.python import debug as tf_debug
import reader_file as freader
from threading import Thread

# input_values = [2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
# result_values = [4, 9, 16, 25, 36, 49, 64, 81, 100, 121]

# model parameters
a = tf.Variable(0.7, name="Weight_a")
b = tf.Variable(0.1, name="Weight_b")

future_input = tf.placeholder(tf.float32, name="future_input")
future_result = tf.placeholder(tf.float32, name="future_result")

# create queue
queue_size = 15
queue_input = tf.FIFOQueue(queue_size, [tf.float32, tf.float32], shapes=[[1], [1]], name="input_queue")
enqueue_op_input = queue_input.enqueue([future_input, future_result])

x, y = queue_input.dequeue()
# x = tf.Print(x, [x], "x = ")
# y = tf.Print(y, [y], "y = ")
# create model and learning model
a1 = tf.multiply(a, x)
a2 = tf.multiply(b, x)
linear_model = tf.multiply(a1, a2, name="linear_model")

# loss
squared_deltas = tf.square(linear_model - y)
loss = tf.reduce_sum(squared_deltas, name="loss")

# optimizer
optimizer = tf.train.AdamOptimizer(0.001)
train = optimizer.minimize(loss)

# Add ops to save and restore all the variables.
# saver = tf.train.Saver()

init = tf.global_variables_initializer()

# add coordinator
coord = tf.train.Coordinator()

# sess = tf_debug.LocalCLIDebugWrapperSession(sess)
sess = tf.Session()
# with tf.Session() as sess:
sess.run(init)


def read_db():
    reader = freader.ReaderFile("/home/serhii/Projects/VerAI/python/input.data", ["int32", "int32"])
    while not coord.should_stop():
        input_values, result_values = reader.readline()
        if not input_values:
            reader.set_position(0)
            continue
        sess.run(enqueue_op_input,
                 feed_dict={future_input: input_values, future_result: result_values})
    return

    # run threads
thread = Thread(target=read_db)
threads = [thread]
thread.start()

# check the accuracy before training
sess.run(linear_model)

# training loop
for i in range(10000):
    sess.run(train)

    # stop data queue
coord.request_stop()
coord.join(threads)


def clear():
    size = sess.run(queue_input.size())
    for i in range(size):
        sess.run(queue_input.dequeue())

clear()
print("a=%f , b=%f " % (sess.run(a), sess.run(b)))
print(sess.run([enqueue_op_input, linear_model], feed_dict={future_input: [2.0], future_result: [0.0]}))
print(sess.run([enqueue_op_input, linear_model], feed_dict={future_input: [3.12], future_result: [0.0]}))
print(sess.run([enqueue_op_input, linear_model], feed_dict={future_input: [1.0], future_result: [0.0]}))
#     # summary_writer = tf.summary.FileWriter('./', sess.graph)
