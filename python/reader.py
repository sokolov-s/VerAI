#!/usr/bin/env python3.5

import baseparser as bp
import reader_file as freader
from pydoc import locate
import collections


class Reader(bp.BaseParser):
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
        self.dataset_json = None
        self.output_cnt = 0
        self.output_dtypes = list()
        self.threads_coord_name = ""
        self.threads_list_name = ""
        self.placeholders_list = list()
        self.enqueue_op_name = ""

    def get_dataset_name(self):
        name = self.get_json()["init"]["dataset"]
        name = name.split('.')
        return name[0]

    def set_dataset(self, dataset_json):
        self.dataset_json = self.str_to_json(dataset_json)

    def parse(self):
        self.parse_params()
        for obj in self.get_json()["output"]:
            v_type = obj["type"]
            self.output_dtypes.append(v_type)
            self.output_cnt += 1
        db_type = self.dataset_json["init"]["db_type"]
        if db_type == "file":
            self.set_head_code(self.get_head_code() + "import reader_file as freader\n")
        else:
            print("\033[91mCan't find reader for database : object %s, database type %s\033[0m" % (self.get_name(), db_type))
            return

    def generate_imports_code(self):
        code = self.get_head_code()
        code += "from threading import Thread\n"
        self.set_head_code(code)

    def generate_body_code(self):
        INPUT_NAME_TEMPLATE = "input"
        QUEUE_NAME_TEMPLATE = "queue"
        ENQUEUE_NAME_TEMPLATE = "enqueue_op"
        params = self.get_params()
        code = "\n# Create queues\n"
        code += "queue_size = " + str(params["queue_capacity"]) + "\n"
        dtypes_code = "["
        shapes_code = "["
        placeholders_list_code = "["
        result_values_code = ""
        queue_name = bp.BaseParser.generate_unique_name(QUEUE_NAME_TEMPLATE)
        self.enqueue_op_name = bp.BaseParser.generate_unique_name(ENQUEUE_NAME_TEMPLATE)
        for i in range(self.output_cnt):
            placeholder_name = bp.BaseParser.generate_unique_name(INPUT_NAME_TEMPLATE)
            code += placeholder_name + " = tf.placeholder(tf." + self.get_json()["output"][i]["type"] + \
                    ", name=\"" + placeholder_name + "\")\n"
            dtypes_code += "tf." + str(self.output_dtypes[i])
            shapes_code += "[1]"
            placeholders_list_code += placeholder_name
            self.placeholders_list.append(placeholder_name)
            result_values_code += self.var_name_form_json(self.get_name(), self.get_json(), i)
            if i < self.output_cnt - 1:
                dtypes_code += ", "
                shapes_code += ", "
                placeholders_list_code += ", "
                result_values_code += ", "

        dtypes_code += "]"
        shapes_code += "]"
        placeholders_list_code += "]"

        code += queue_name + " = tf.FIFOQueue(capacity=" + params["queue_capacity"] + \
                ", dtypes=" + dtypes_code + ", shapes=" + shapes_code + ", name=\"" + params["name"] + "\"" + \
                ")\n"

        code += self.enqueue_op_name + " = " + queue_name + ".enqueue(" + placeholders_list_code + \
                ", name=\"" + self.enqueue_op_name + "\")\n"
        code += result_values_code + " = " + queue_name + ".dequeue()\n"

        self.set_body_code(code)

    def generate_action_code(self):
        code = self.get_action_code()
        code += "\n# Add coordinator\n"
        coord_name = bp.BaseParser.generate_unique_name("coord")
        self.threads_coord_name = coord_name
        code += coord_name + " = tf.train.Coordinator()"

        fn_name = bp.BaseParser.generate_unique_name("read_db")
        code += "\n\ndef " + fn_name + "():\n"

        reader_name = bp.BaseParser.generate_unique_name("reader")
        code += "\t# Create reader\n"

        if self.dataset_json["init"]["db_type"] == "file":
            code += "\t" + reader_name + " = freader.ReaderFile(\"" + self.dataset_json["init"]["path"] + "\"" + \
                    ", " + str(self.output_dtypes) + ")\n"
        else:
            print("\033[91mUnknown database type: %s\033[0m" % (self.dataset_json["init"]["db_type"]))
            return

        code += "\twhile not " + self.threads_coord_name + ".should_stop():\n"
        code += "\t\t"
        db_outputs_list = list()
        for i in range(len(self.dataset_json["output"])):
            db_outputs_list.append(self.var_name_form_json(self.get_dataset_name(), self.dataset_json, i))
            code += db_outputs_list[i] + ", "
        code = code[:-2] + " = " + reader_name + ".readline()\n"

        code += "\t\tif "
        for i in range(len(db_outputs_list)):
            code += "not " + db_outputs_list[i] + " or "
        code = code[:-4] + ":\n"
        code += "\t\t\t" + reader_name + ".set_position(0)\n"
        code += "\t\t\tcontinue\n"

        code += "\t\tsess.run(" + self.enqueue_op_name + ",feed_dict={"
        for i in range(len(self.dataset_json["output"])):
            code += self.placeholders_list[i] + ": " + db_outputs_list[i] + ", "
        code = code[:-2] + "})\n"
        code += "\treturn\n"

        code += "\n# Run threads\n"
        threads_name = bp.BaseParser.generate_unique_name("threads")
        self.threads_list_name = threads_name
        code += threads_name + " = [Thread(target=" + fn_name + ")]\n"
        code += "for thr in " + threads_name + ": thr.start()\n"

        self.set_action_code(code)

    def generate_pos_action_code(self):
        code = "\n# Stop threads\n"
        code += self.threads_coord_name + ".request_stop()\n"
        code += self.threads_coord_name + ".join(" + self.threads_list_name + ")\n"

        self.set_post_action_code(code)
