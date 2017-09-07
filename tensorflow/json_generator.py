#!/usr/bin/env python3.5

import json
from component_loader import ComponentLoader


def main():
    loader = ComponentLoader()
    for component_key, component_value in loader.get_loaded_components().items():
        print("Value : %s" % component_value)
        class_name = component_value[loader.ModuleInfo.CLASS_NAME]
        version = component_value[loader.ModuleInfo.VERSION]
        instance = loader.get_component_instance(class_name=class_name, version=version, name="Test", tf_session=None)

        data = dict()

        data["class"] = class_name
        data["version"] = version

        data["inputs"] = [instance.get_inputs()]
        data["outputs"] = [instance.get_outputs()]
        data["params"] = [instance.get_params()]

        json_dir = "./json"
        filename = class_name + "_" + version + ".json"
        with open(json_dir + "/" + filename, "w") as out:
            json.dump(data, out)

main()


