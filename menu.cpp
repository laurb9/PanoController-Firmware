/*
 * Menu
 *
 * Copyright (C)2016 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include "eeprom.h"
#include "menu.h"

/*
 * Class hierarchy
 * BaseMenu
 *     ActionItem
 *     MultiSelect
 *           RangeSelector
 *           ListSelector
 *                NamedListSelector
 *           Menu
 */

/*
 * BaseMenu: shared functionality
 */
BaseMenu::BaseMenu(const char *description, int(*onselect)(int))
:onselect(onselect),
 description(description)
{
};

void BaseMenu::cancel(void){
}

void BaseMenu::select(void){
    if (onselect){
        onselect(0);
    }
}

void BaseMenu::sync(void){
}

int BaseMenu::render(DISPLAY_DEVICE display, int rows){
    Serial.println(description);
    Serial.println("---------------------");
    display.println(description); rows--;
    if (rows > 4){
        display.print("---------------------"); rows--;
    }
    return rows;
}

ActionItem::ActionItem(const char *description, int(*onselect)(int))
:BaseMenu(description, onselect)
{
};

/*
 * MultiSelect: share functionality among the other menu classes
 */
MultiSelect::MultiSelect(const char *description, volatile int *value, int default_val, int eeprom, int(*onselect)(int))
:BaseMenu(description, onselect),
 value(value),
 default_val(default_val),
 eeprom(eeprom)
{
    // override default with eeprom value, if set
    if (eeprom){
        EEPROM.get(eeprom, this->default_val);
    }
    if (this->default_val <= 0){
        this->default_val = default_val;
    }
    if (value){
        *value = this->default_val;
    }
}

void MultiSelect::cancel(void){
    //pointer = pos;
    active = false;
}

void MultiSelect::next(void){
    if (pointer < count-1){
        pointer++;
    }
}
void MultiSelect::prev(void){
    if (pointer > 0){
        pointer--;
    }
}
void MultiSelect::select(void){
    pos = pointer;
    MultiSelect::sync();
    if (onselect){
        onselect(*value);
    }
}
void MultiSelect::sync(void){
    if (eeprom){
        int v = *value;
        EEPROM.put(eeprom, v);
    }
}

int MultiSelect::calc_start(int rows){
    int start = 0;
    if (count > rows && pointer > rows/2){
        if (pointer < count - rows/2){
            start = pointer - rows/2;
        } else {
            start = count - rows;
        }
    }
    return start;
}

int MultiSelect::render(DISPLAY_DEVICE display, int rows){
    active = true;
    return BaseMenu::render(display, rows);
}

/*
 * RangeSelector: a number with up/down controls
 */
RangeSelector::RangeSelector(const char *description, volatile int *value, int default_val, int eeprom, int(*onselect)(int),
                             int min_val, int max_val, int step)
:MultiSelect(description, value, default_val, eeprom, onselect),
 min_val(min_val), max_val(max_val), step(step)
{
    pointer = this->default_val;
    pos = pointer;
};

void RangeSelector::next(void){
    if (pointer > min_val){
        pointer -= step;
    }
}
void RangeSelector::prev(void){
    if (pointer < max_val){
        pointer += step;
    }
}
void RangeSelector::select(void){
    *value = pointer;
    MultiSelect::select();
    cancel();
}
void RangeSelector::sync(void){
    pointer = *value % step;
    if (pointer > max_val) pointer = max_val;
    if (pointer < min_val) pointer = min_val;
    pos = pointer;
    MultiSelect::sync();
}

int RangeSelector::render(DISPLAY_DEVICE display, int rows){
    const char *marker;
    rows = MultiSelect::render(display, rows);

    marker = (pointer < max_val) ? " \x1e": "";
    display.println(marker); rows--;

    if (pointer == pos) display.setTextColor(BLACK, WHITE);

    Serial.println(pointer);
    display.println(pointer); rows--;

    if (pointer == pos) display.setTextColor(WHITE, BLACK);

    marker = (pointer > min_val) ? " \x1f": "";
    display.println(marker); rows--;
    return rows;
}

/*
 * ListSelector: list of numeric options
 */
ListSelector::ListSelector(const char *description, volatile int *value, int default_val, int eeprom, int(*onselect)(int),
                           int count, const int values[])
:MultiSelect(description, value, default_val, eeprom, onselect),
 values(values)
{
    this->count = count;
    // find the position corresponding to the default value
    for (pos=count-1; pos > 0; pos--){
        if (values[pos] == this->default_val){
            break;
        }
    }
    pointer = pos;
};

void ListSelector::select(void){
    *value = values[pointer];
    MultiSelect::select();
    cancel();
}
void ListSelector::sync(void){
    // find the position corresponding to the default value
    for (pos=count-1; pos > 0; pos--){
        if (values[pos] == *value){
            break;
        }
    }
    pointer = pos;
    MultiSelect::sync();
}

int ListSelector::render(DISPLAY_DEVICE display, int rows){
    char buf[16];
    int start;

    rows = MultiSelect::render(display, rows);
    start = calc_start(rows);

    for (int i=start; i<start+rows && i<count; i++){
        snprintf(buf, sizeof(buf), "%d", values[i]);

        Serial.print((i==pointer) ? ">" : " ");
        display.print((i==pointer) ? '\x10' : ' ');

        if (i == pos) display.setTextColor(BLACK, WHITE);
        Serial.println(buf);
        display.print(buf);
        if (i == pos) display.setTextColor(WHITE, BLACK);

        display.println((i==pointer) ? '\x11' : ' ');
    }
    return 0;
}

/*
 * NamedListSelector: list of named numeric options
 */
NamedListSelector::NamedListSelector(const char *description, volatile int *value, int default_val, int eeprom, int(*onselect)(int),
                                     int count, const char * const names[], const int values[])
:ListSelector(description, value, default_val, eeprom, onselect, count, values),
 names(names)
{
}

int NamedListSelector::render(DISPLAY_DEVICE display, int rows){
    int start;

    rows = MultiSelect::render(display, rows);
    start = calc_start(rows);

    for (int i=start; i<start+rows && i<count; i++){
        Serial.print((i==pointer) ? ">" : " ");
        display.print((i==pointer) ? '\x10' : ' ');

        if (i == pos) display.setTextColor(BLACK, WHITE);
        Serial.println(names[i]);
        display.print(names[i]);
        if (i == pos) display.setTextColor(WHITE, BLACK);

        display.println((i==pointer) ? '\x11' : ' ');
    }
    return 0;
}

/*
 * Menu: this is regular navigation menu, nothing is set here
 * but all events are passed down to active children
 */
Menu::Menu(const char *description, int count, BaseMenu* const *menus)
:MultiSelect(description, NULL, 0, 0, NULL),
 menus(menus)
{
    pos = 0;
    this->count = count;
    active_submenu = NULL;
}

/*
 * CANCEL action, should go back one level
 */
void Menu::cancel(void){
    if (active_submenu){         // passthrough
        active_submenu->cancel();
        // check if submenu is still active,
        // as the cancel action may have propagated more than one level
        if (active_submenu->getClassID() != Menu::class_id || !active_submenu->active){
            active_submenu = NULL;
        }
    } else {
        // not resetting pos via MultiSelect::cancel() allows us to return to same menu position
        MultiSelect::cancel();
    }
}

/*
 * Move selector to next item in list
 */
void Menu::next(void){
    if (active_submenu){         // passthrough
        active_submenu->next();
    } else {
        MultiSelect::next();
    }
}

/*
 * Move selector to previous item in list
 */
void Menu::prev(void){
    if (active_submenu){         // passthrough
        active_submenu->prev();
    } else {
        MultiSelect::prev();
    }
}

/*
 * Open the currently selected item
 * which can be a submenu, a selection list or action
 */
void Menu::select(void){
    if (active_submenu){        // passthrough
        active_submenu->select();
        if (!active_submenu->active){
            active_submenu = NULL;
        }
    } else {
        MultiSelect::select();
        if (menus[pos]->getClassID() != ActionItem::class_id){
            active_submenu = (MultiSelect*)menus[pos];
        } else {
            menus[pos]->select();
        }
    }
}

/*
 * cascade sync (write all current settings to EEPROM)
 */
void Menu::sync(void){
    for (int i=0; i<count; i++){
        menus[i]->sync();
    }
}

/*
 * Display either the menu or active submenu/selection
 */
int Menu::render(DISPLAY_DEVICE display, int rows){
    int start;

    if (active_submenu){        // passthrough
        rows = active_submenu->render(display, rows);
        return rows;
    }

    rows = MultiSelect::render(display, rows);
    start = calc_start(rows);

    for (int i=start; i<start+rows && i<count; i++){
        if (i == pointer){
            Serial.print(">");
            display.setTextColor(BLACK, WHITE);
        }
        Serial.println(menus[i]->description);
        display.println(menus[i]->description);
        if (i == pointer){
            display.setTextColor(WHITE, BLACK);
        }
    }
    return 0;
}

/*
 * Main entry point for menu.
 * Check HID inputs, display and navigate main menu
 * This must be called in a loop.
 */
void displayMenu(Menu& menu, DISPLAY_DEVICE display, const int rows,
                 AllHID& hid, void(*onMenuLoop)(void)){
    static int last_event_timestamp = 0;

    if (!hid.read() && last_event_timestamp){
        if (millis() - last_event_timestamp > DISPLAY_SLEEP){
            display.clearDisplay();
            display.display();
        } else {
            if (onMenuLoop) onMenuLoop();
            display.display();
        }
        return;
    }

    if (hid.isLastEventLeft() || hid.isLastEventCancel()) menu.cancel();
    else if (hid.isLastEventRight() || hid.isLastEventOk()) menu.select();
    else if (hid.isLastEventDown()) menu.next();
    else if (hid.isLastEventUp()) menu.prev();

    last_event_timestamp = millis();
    Serial.println();
    display.clearDisplay();
    display.setTextCursor(0,0);
    menu.render(display, rows);
    if (onMenuLoop) onMenuLoop();
    display.display();
    delay(100);
}
