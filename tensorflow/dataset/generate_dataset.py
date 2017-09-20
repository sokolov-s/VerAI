#!/usr/bin/env python3.5

import random
import sys
import getopt


def print_no_argument_and_exit():
    print("Bad parameters")
    print("Run script with command --help to show usage page")
    exit(2)

try:
    if len(sys.argv) <= 4:
        print_no_argument_and_exit()
    opts, args = getopt.getopt(sys.argv[1:], "hc:d:t:", ["help", "count=", "data=", "target="])
except getopt.GetoptError:
    print_no_argument_and_exit()

for opt, arg in opts:
    if opt in ('-h', "--help"):
        print("""Script parse json file, translate it into tensorflow model and execute this model
        
Usage: main.py [OPTIONS]
Example: python3 main.py -i file_with_model_info.json

Options:
    -h, --help                : show help dialog
    -c, --count=n             : dataset size
    -d, --data=FILE       : path to data file
    -t, --target=FILE     : path to target file
        """)
        sys.exit()
    elif opt in ("-c", "--count"):
        ds_size = int(arg)
    elif opt in ("-d", "--datafile"):
        d_file = arg
    elif opt in ("-t", "--target"):
        target_file = arg
    else:
        print("Unknown parameter %s" % opt)
        exit(2)

i = 0
random_max_value = 100000
f_data = open(d_file, "w")
f_target = open(target_file, "w")

while i < ds_size:
    is_float = random.randint(0, 1)
    number = 0
    if is_float:
        number = random.uniform(0.0, random_max_value)
    else:
        number = random.randint(0, random_max_value)
    target_number = number * number
    f_data.write("%s\n" % str(number))
    f_target.write("%s\n" % str(target_number))
    i += 1

f_data.close()
f_target.close()
