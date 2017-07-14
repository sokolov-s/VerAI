#!/usr/bin/env python3.5

import baseparser as bp
import reader_file as freader
from pydoc import locate
import collections
import uuid


class ReaderProxy(bp.BaseParser):
    """
    Some text about class
    """

    def __init__(self, name):
        params = collections.OrderedDict(
            [("queue_capacity", 10),
             ("name", "fifo_queue")
             ]
        )
        bp.BaseParser.__init__(self, name, "reader", params)
        self.dataset = None
        self.output_cnt = 0
        self.output_dtypes = list()

    def get_dataset_name(self):
        name = self.get_json()["init"]["dataset"]
        name = name.split('.')
        return name[0]

    def set_dataset(self, dataset_json):
        self.dataset = bp.BaseParser.str_to_json(dataset_json)

    def parse(self):
        self.parse_params()
        for obj in self.get_json()["output"]:
            v_type = obj["type"]
            self.output_dtypes.append(v_type)
            self.output_cnt += 1
        db_type = self.dataset["init"]["db_type"]
        path = self.dataset["init"]["path"]
        if db_type == "file":
            self._set_head_code(self.get_head_code() + "import reader_file as freader\n")

    def __generate_imports(self):
        code = self.get_head_code()
        code += "from threading import Thread\n"
        self._set_head_code(code)

    def __generate_action(self):
        code = self.get_action_code()
        code += "\n# Add coordinator\n"
        coord_name = "coord" + uuid.uuid4().hex
        code += coord_name + " = tf.train.Coordinator()"

        fn_name = "read_db_" + uuid.uuid4().hex
        code += "\n\ndef " + fn_name + "():\n"

        reader_name = "reader" + uuid.uuid4().hex
        code += "\t# Create reader\n"
        code += "\t" + reader_name + " = freader.ReaderFile(\"" + self.dataset["init"]["path"] + "\"" + \
                ", " + str(self.output_dtypes) + ")\n"

        code += "\twhile not coord.should_stop():\n"
        code += "\t\tinput_values, result_values = " + reader_name + ".readline()\n"
        code += "\t\tif not input_values:\n"
        code += "\t\t\treader.set_position(0)\n"
        code += "\t\t\tcontinue\n"
        code += "\t\tsess.run([enqueue_op_input, enqueue_op_result],feed_dict={future_input: input_values, future_result: result_values})\n"
        code += "\t\treturn\n"

        code += "\n# Run threads\n"
        threads_name = "threads" + uuid.uuid4().hex
        code += threads_name + " = list(Thread(target=" + fn_name + "))\n"
        code += threads_name + ".start()\n"

        self._set_action_code(code)

    def generate_code(self):
        INPUT_NAME_TEMPLATE = "input_" + uuid.uuid4().hex
        QUEUE_NAME_TEMPLATE = "queue_" + uuid.uuid4().hex
        ENQUEUE_NAME_TEMPLATE = "enqueue_op_" + uuid.uuid4().hex
        params = self.get_params()
        code = "\n# Create queues\n"
        code += "queue_size = " + str(params["queue_capacity"]) + "\n"
        for i in range(self.output_cnt):
            queue_name = QUEUE_NAME_TEMPLATE + str(i)
            placeholder_name = INPUT_NAME_TEMPLATE + str(i)
            enqueue_name = ENQUEUE_NAME_TEMPLATE + str(i)
            code += placeholder_name + " = tf.placeholder(tf." + self.get_json()["output"][i]["type"] + \
                    ", name=\"" + INPUT_NAME_TEMPLATE + str(i) + "\")\n"
            code += queue_name + " = tf.FIFOQueue(capacity=" + params["queue_capacity"] + \
                    ", dtypes=tf." + str(self.output_dtypes[i]) + \
                    ", name=\"" + params["name"] + str(i) + "\"" + \
                    ")\n"

            code += enqueue_name + " = " + queue_name + ".enqueue_many([" + placeholder_name + "]" \
                    ", name=\"" + enqueue_name + "\")\n"
            code += self.get_json()["output"][i]["name"] + " = " + queue_name + ".dequeue()\n"
            code += "\n"
        self._set_body_code(code)
        self.__generate_imports()
        self.__generate_action()
