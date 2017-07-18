#!/usr/bin/env python3.5

import sys
import getopt
import os.path

import factory

json_file = None
output_file = None


def print_no_argument_and_exit():
    print("Bad parameters")
    print("Run script with command --help to show usage page")
    exit(2)

try:
    if len(sys.argv) <= 1:
        print_no_argument_and_exit()
    opts, args = getopt.getopt(sys.argv[1:], "hi:o:", ["help", "ifile=", "ofile="])
except getopt.GetoptError:
    print_no_argument_and_exit()

for opt, arg in opts:
    if opt in ('-h', "--help"):
        print("""Script parse json file and generate python code to import it to tensorflow library
        
Usage: generator.py [OPTIONS]
Example: python3 generator.py -i file_with_info.json -o generated_script.py

Options:
    -h, --help          : show help dialog
    -i, --ifile=FILE    : path to json file
    -o, --ofile=FILE    : path to generate file
        """)
        sys.exit()
    elif opt in ("-i", "--ifile"):
        json_file = arg
    elif opt in ("-o", "--ofile"):
        output_file = arg
    else:
        print("Unknown parameter %s" % opt)
        exit(2)

if json_file is None or not os.path.isfile(json_file):
    print("Can't find json file")
    exit(2)

if output_file is None:
    output_file = json_file[:-5] + ".py"

print("Generating %s from file %s" % (output_file, json_file))

creator = factory.Factory()
tensors = creator.create(json_file)

f_out = open(output_file, "w+")
f_out.write("#!/usr/bin/env python3.5\n\n")
f_out.write("import tensorflow as tf\n\n")

for obj in tensors:
    obj.parse()
    obj.generate_code()

for obj in tensors:
    code = obj.get_head_code()
    if code:
        f_out.write(code)
        f_out.write("\n")

for obj in tensors:
    code = obj.get_body_code()
    if code:
        f_out.write(code)
        f_out.write("\n")

f_out.write("\n# Init tensorflow session\n")
f_out.write("init = tf.global_variables_initializer()\n")
f_out.write("sess = tf.Session()\n")
f_out.write("sess.run(init)\n")

for obj in tensors:
    code = obj.get_action_code()
    if code:
        f_out.write(code)
        f_out.write("\n")

for obj in tensors:
    code = obj.get_post_action_code()
    if code:
        f_out.write(code)
        f_out.write("\n")

f_out.close()

