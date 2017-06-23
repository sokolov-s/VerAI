#!/usr/bin/env python3.5
# -*- coding: utf-8 -*-
import tensorflow as tf
from tensorflow.python import debug as tf_debug

# model parameters
a = tf.Variable(0.7, name="Weight_a")
b = tf.Variable(0.1, name="Weight_b", trainable=False)

# model input and output
x = tf.placeholder(tf.float32, name="Input")
a1 = tf.multiply(a, x)
a2 = tf.multiply(b, x)
linear_model = tf.multiply(a1, a2, name="linear_model")
y = tf.placeholder(tf.float32, name="Output")

# loss
squared_deltas = tf.square(linear_model - y)
loss = tf.reduce_sum(squared_deltas, name="loss")

# optimizer
optimizer = tf.train.AdamOptimizer(0.001)
train = optimizer.minimize(loss)

# Add ops to save and restore all the variables.
# saver = tf.train.Saver()

# training loop
init = tf.global_variables_initializer()
sess = tf.Session()
# sess = tf_debug.LocalCLIDebugWrapperSession(sess)
sess.run(init)

x_train = [2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
y_train = [4, 9, 16, 25, 36, 49, 64, 81, 100, 121]

for i in range(10000):
    sess.run(train, {x: x_train, y: y_train})

print("a=%f , b=%f " % (sess.run(a), sess.run(b)))
print(sess.run(linear_model, {x: 16}))
summary_writer = tf.summary.FileWriter('./', sess.graph)
print(sess.run(linear_model, {x: 12}))
print(sess.run(linear_model, {x: 1}))



#---

# class Add:
#     INPUTS:
#     {
#         Op1: { shape: None, desc: "First operand for Add" },
#         Op2: { shape: None, desc: "Second operand for Add" }
#     }
#     OUTPUTS:
#     {
#         Result: { shape: None, desc: "Result" }
#     }
#     PARAMETERS: { }
#
#     def instantiate():
#         OUTPUTS.Result = tf.add(INPUTS.Op1, INPUTS.Op2)
#
# def CommitHook:
#     cls = git.Get()
#     structure = parse(cls)
#     structure.filter("INPUTS, OUTPUTS, PARAMETERS")
#     json = structure.toJSON()
#     db.insert(json)
#
# def create_tf():
#     c = new comp()
#     c.INPUTS["Op1"] = map_tensors[t1];
#     c.INPUTS["Op2"] = map_tensors[t2];
#     c.instantiate()
#     map_tensors[c.OUTPUTS.Result.key] = c.OUTPUTS.Result

#json
[
    {
    "Component1" : {
        INTPUT : [
        ]
    OUTPUT : [
        ]
    TARGET :[
        Component2 : {
        }
    ]
    }
    }
}
]