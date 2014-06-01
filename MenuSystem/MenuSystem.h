/*
  MenuSystem.h - Library for creating menu structures.
  Created by Jon Black, August 8th 2011.
  Released into the public domain.

  License: LGPL 3
*/

#ifndef MENUSYSTEM_H
#define MENUSYSTEM_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

extern boolean _cur_menu_item_selected;
extern boolean _cur_menu_item_selected_previous;
extern boolean _cur_menu_just_selected;

class MenuComponent
{
public:
    MenuComponent(char* name);

    void set_name(char* name);
    char* get_name() const;

    virtual MenuComponent* select() = 0;

protected:
    char* _name;
};


class MenuItem : public MenuComponent
{
public:
    MenuItem(char* name);

    void set_select_function(void (*on_select)(MenuItem*));
 
    virtual MenuComponent* select();
   
private:
    void (*_on_select)(MenuItem*);
};


class Menu : public MenuComponent
{
public:
    Menu(char* name);

    boolean next(boolean loop=false);
    boolean prev(boolean loop=false);
    MenuComponent* activate();
    virtual MenuComponent* select();

    void add_item(MenuItem* pItem, void (*on_select)(MenuItem*));
    Menu const* add_menu(Menu* pMenu);

    void set_parent(Menu* pParent);
    Menu const* get_parent() const;

    MenuComponent const* get_selected() const;
    MenuComponent const* get_menu_component(byte index) const;

    byte get_num_menu_components() const;
    byte get_cur_menu_component_num() const;

private:
    MenuComponent* _p_sel_menu_component;
    MenuComponent** _menu_components;
    Menu* _p_parent;
    byte _num_menu_components;
    byte _cur_menu_component_num;
};


class MenuSystem
{
public:
    MenuSystem();

    boolean next(boolean loop=false);
    boolean prev(boolean loop=false);
    boolean select(boolean return_to_root=true);
    boolean back();
    
    void set_root_menu(Menu* p_root_menu);
    Menu const* get_current_menu() const;
    
    void deselect_set_menu_item(void);
    boolean menu_item_is_selected(void);
    boolean menu_item_was_just_selected (void);

    private:
    Menu* _p_root_menu;
    Menu* _p_curr_menu;
};


#endif
