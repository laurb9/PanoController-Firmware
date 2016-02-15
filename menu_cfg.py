import json
import re

def main(menu_file):
    print("""
#include "menu.h"
#define NAMES (const char* [])
#define VALUES (const int[])
""")
    with open(menu_file) as f:
        menu = json.load(f)

    menus = []

    for i, menu_item in enumerate(menu):
        menu_name = "menu_" + re.sub(r"[^\w]", "_", menu_item["description"].lower())
        menu_item["name"] = menu_name
        print('const char *%(name)s_desc = "%(description)s";' % menu_item)
        names = []
        values = []
        default_val = None
        if "step" in menu_item:
            menu_item["options"] = range(menu_item["min"], menu_item["max"], menu_item["step"])

        if "options" in menu_item:
            for j, option in enumerate(menu_item["options"]):
                try:
                    name, value = option.items()[0]
                except AttributeError:
                    name, value = str(option), option
                    menu_item["default"] = str(menu_item["default"])

                var_name = "%s_name_%d" % (menu_name, j)
                var_value = "%s_val_%d" % (menu_name, j)
                if menu_item["default"] == name:
                    default_val = value
                print('const static char *%s = "%s";' % (var_name, name))
                print("const static int %s = %s;" % (var_value, value))
                names.append(var_name)
                values.append(var_value)

        menu_item["default_val"] = default_val or 0
        menu_item["size"] = len(names)
        menu_item["names"] = ", ".join(names)
        menu_item["values"] = ", ".join(values)
        
        menus.append(menu_name)
        
        print(
"""
// %(description)s
volatile int %(variable)s;
const static int %(name)s_size = %(size)d;
const static char *%(name)s_names[%(size)d] = {%(names)s};
const static int %(name)s_values[%(size)d] = {%(values)s};
static NamedOptionMenu %(name)s(%(name)s_desc, &%(variable)s, %(default_val)d, %(size)d, %(name)s_names, %(name)s_values);
""" % menu_item)

    print(
"""
union MenuItem menu[%d] = {&%s};
const int menu_size = %d;
""" % (len(menus), ", &".join(menus), len(menus)))

if __name__ == "__main__":
    main("menu_cfg.json")
