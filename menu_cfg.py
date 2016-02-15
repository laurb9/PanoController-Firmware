import json
import re

def main(menu_file):
    output = ['#include "menu.h"']

    with open(menu_file) as f:
        menu = json.load(f)

    menus = []
    menu_types = []

    for i, menu_item in enumerate(menu):
        menu_name = "menu_" + re.sub(r"[^\w]", "_", menu_item["description"].lower())
        menu_item["name"] = menu_name
        output.append("// %(description)s" % menu_item)
        output.append("volatile int %(variable)s;" % menu_item);
        names = []
        values = []
        default_val = 0
        output.append('static const char %(name)s_desc[] PROGMEM = "%(description)s";' % menu_item)
        
        if "step" in menu_item:
            menu_item["options"] = range(menu_item["min"], menu_item["max"], menu_item["step"])

        if "options" in menu_item:
            menu_types.append("NamedOptionMenu::class_id")
            for j, option in enumerate(menu_item["options"]):
                try:
                    name, value = option.items()[0]
                    if menu_item["default"] == name:
                        default_val = value
                except AttributeError:
                    name, value = str(option) + menu_item.get("unit", ""), option
                    if menu_item["default"] == value:
                        default_val = value

                var_name = "%s_name_%d" % (menu_name, j)
                var_value = "%s_val_%d" % (menu_name, j)
                output.append('static const char %s[] PROGMEM = "%s";' % (var_name, name))
                output.append("static const int %s = %s;" % (var_value, value))
                names.append(var_name)
                values.append(var_value)

        menu_item["default_val"] = default_val
        menu_item["size"] = len(names)
        menu_item["names"] = ", ".join(names)
        menu_item["values"] = ", ".join(values)
        
        menus.append(menu_name)
        
        output.append(
"""
static const char *%(name)s_names[%(size)d] = {%(names)s};
static const int %(name)s_values[%(size)d] = {%(values)s};
static NamedOptionMenu %(name)s(%(name)s_desc, &%(variable)s, %(default_val)d, %(size)d, %(name)s_names, %(name)s_values);
""" % menu_item)

    output.append(
"""
static union MenuItem menus[%d] = {&%s};
static const int menu_types[%d] = {%s};
Menu menu("Main Menu", %d, menus, menu_types);
""" % (len(menus), ", &".join(menus), 
       len(menus), ",".join(menu_types), 
       len(menus)))

    return output

if __name__ == "__main__":
    output = main("menu_cfg.json")
    print "\n".join(output)
