import json
import re

def main(menu_file):
    output = ['#include "menu.h"']

    with open(menu_file) as f:
        menu = json.load(f)

    eeprom_idx = 0
    menus = []
    menu_types = []
    eeprom_map = []

    for i, menu_item in enumerate(menu):
        menu_name = "menu_" + re.sub(r"[^\w]", "_", menu_item["description"].lower())
        menu_item["name"] = menu_name
        output.append("// %(description)s" % menu_item)
        output.append("extern volatile int %(variable)s;" % menu_item)
        if "onselect" in menu_item:
            output.append("int %s(int);" % menu_item["onselect"])
        else:
            menu_item["onselect"] = "NULL";
        if menu_item.get("eeprom"):
            eeprom_idx += 1;
            menu_item["eeprom"] = eeprom_idx
        else:
            menu_item["eeprom"] = 0

        names = []
        values = []
        default_val = 0
        output.append('static const PROGMEM char %(name)s_desc[] = "%(description)s";' % menu_item)
        
        if "options" in menu_item:
            if isinstance(menu_item["options"][0], dict):
                menu_types.append("NamedListSelector::class_id")
                for j, option in enumerate(menu_item["options"]):
                    name, value = option.items()[0]
                    if menu_item["default"] == name:
                        default_val = value
    
                    var_name = "%s_name_%d" % (menu_name, j)
                    output.append('static const PROGMEM char %s[] = "%s";' % (var_name, name))
                    names.append(var_name)
                    values.append(str(value))
                    
                menu_item["names"] = ", ".join(names)
                output_fmt = """
static const PROGMEM char * const %(name)s_names[%(size)d] = {%(names)s};
static const PROGMEM int %(name)s_values[%(size)d] = {%(values)s};
static NamedListSelector %(name)s(%(name)s_desc, &%(variable)s, %(default_val)d, %(eeprom)d * sizeof(int), %(onselect)s, %(size)d, %(name)s_names, %(name)s_values);
"""
            else:
                menu_types.append("ListSelector::class_id")
                for j, value in enumerate(menu_item["options"]):
                    if menu_item["default"] == value:
                        default_val = value
    
                    values.append(str(value))

                output_fmt = """
static const PROGMEM int %(name)s_values[%(size)d] = {%(values)s};
static ListSelector %(name)s(%(name)s_desc, &%(variable)s, %(default_val)d, %(eeprom)d * sizeof(int), %(onselect)s, %(size)d, %(name)s_values);
"""
            menu_item["default_val"] = default_val
            menu_item["size"] = len(values)
            menu_item["values"] = ", ".join(values)
            output.append(output_fmt % menu_item)
        
        elif "step" in menu_item:
            menu_types.append("RangeSelector::class_id")
            output.append(
"""
static RangeSelector %(name)s(%(name)s_desc, &%(variable)s, %(default)d, %(eeprom)d * sizeof(int), %(onselect)s, %(min)d, %(max)d, %(step)d);
""" % menu_item)
            
        
        menus.append(menu_name)
        

    output.append(
"""
static const PROGMEM union MenuItem menus[] = {&%s};
static const PROGMEM int menu_types[] = {%s};
static const PROGMEM char menu_desc[] = "Main Menu";
Menu menu(menu_desc, %d, menus, menu_types);
""" % (", &".join(menus), 
       ", ".join(menu_types),
       len(menus)))

    return output

if __name__ == "__main__":
    output = main("menu_cfg.json")
    print "\n".join(output)
