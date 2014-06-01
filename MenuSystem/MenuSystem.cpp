/*
  MenuSystem.h - Library for creating menu structures.
  Created by Jon Black, August 8th 2011.
  Released into the public domain.

  License: LGPL 3
*/

#include "MenuSystem.h"

boolean _cur_menu_item_selected = false;
boolean _cur_menu_item_selected_previous = false;
boolean _cur_menu_just_selected = false;

// *********************************************************
// MenuComponent
// *********************************************************

MenuComponent::MenuComponent(char* name) 
: _name(name)
{
}

char* MenuComponent::get_name() const
{
    return _name;
}

void MenuComponent::set_name(char* name)
{
    _name = name;
}

// *********************************************************
// Menu
// *********************************************************

Menu::Menu(char* name)
: MenuComponent(name),
  _p_sel_menu_component(NULL),
  _menu_components(NULL),
  _p_parent(NULL),
  _num_menu_components(0),
  _cur_menu_component_num(0)
{
}

boolean Menu::next(boolean loop)
{
    if (_cur_menu_component_num != _num_menu_components - 1)
    {
        _cur_menu_component_num++;
        _p_sel_menu_component = _menu_components[_cur_menu_component_num];

        return true;
    } else if (loop)
    {
        _cur_menu_component_num = 0;
        _p_sel_menu_component = _menu_components[_cur_menu_component_num];

        return true;
    }

    return false;
}

boolean Menu::prev(boolean loop)
{
    if (_cur_menu_component_num != 0)
    {
        _cur_menu_component_num--;
        _p_sel_menu_component = _menu_components[_cur_menu_component_num];

        return true;
    } else if (loop)
    {
        _cur_menu_component_num = _num_menu_components - 1;
        _p_sel_menu_component = _menu_components[_cur_menu_component_num];

        return true;
    }

    return false;
}

MenuComponent* Menu::activate()
{
    MenuComponent* pComponent = _menu_components[_cur_menu_component_num];
    if (pComponent == NULL)
    {
      return NULL;
    }
    return pComponent->select();  //I think this calls the callback function of the menu item thru the virtual function thing
}

MenuComponent* Menu::select()
{
    return this;
}

void Menu::add_item(MenuItem* pItem, void (*on_select)(MenuItem*))
{
    // Resize menu component list, keeping existing items.
    // If it fails, then the item is not added and the function returns.
    _menu_components = (MenuComponent**) realloc(_menu_components,
                                                 (_num_menu_components + 1)
                                                 * sizeof(MenuComponent*));
    if (_menu_components == NULL)
      return;

    _menu_components[_num_menu_components] = pItem;

    pItem->set_select_function(on_select);

    if (_num_menu_components == 0)
        _p_sel_menu_component = pItem;

    _num_menu_components++;
}

Menu const* Menu::get_parent() const
{
  return _p_parent;
}

void Menu::set_parent(Menu* pParent)
{
  _p_parent = pParent;
}

Menu const* Menu::add_menu(Menu* pMenu)
{
    // Resize menu component list, keeping existing items.
    // If it fails, then the item is not added and the function returns.
    _menu_components = (MenuComponent**) realloc(_menu_components,
                                                 (_num_menu_components + 1)
                                                 * sizeof(MenuComponent*));
    if (_menu_components == NULL)
      return NULL;

    pMenu->set_parent(this);

    _menu_components[_num_menu_components] = pMenu;

    if (_num_menu_components == 0)
        _p_sel_menu_component = pMenu;

    _num_menu_components++;

    return pMenu;
}

MenuComponent const* Menu::get_menu_component(byte index) const
{
  return _menu_components[index];
}

MenuComponent const* Menu::get_selected() const
{
    return _p_sel_menu_component;
}

byte Menu::get_num_menu_components() const
{
    return _num_menu_components;
}

byte Menu::get_cur_menu_component_num() const
{
    return _cur_menu_component_num;
}

// *********************************************************
// MenuItem
// *********************************************************

MenuItem::MenuItem(char* name)
: MenuComponent(name),
  _on_select(0)
{
}

void MenuItem::set_select_function(void (*on_select)(MenuItem*))
{
    _on_select = on_select;
}

MenuComponent* MenuItem::select()  // this is called when a Menu Item is selected
{
    if (_on_select != NULL)
    {
      if (_cur_menu_item_selected == true && _cur_menu_item_selected_previous == false)
        _cur_menu_just_selected = true;

      _cur_menu_item_selected_previous = _cur_menu_item_selected;
      _cur_menu_item_selected=true;
      _on_select(this);       // This calls the callback function
    }
    return NULL;
}

// *********************************************************
// MenuSystem
// *********************************************************

MenuSystem::MenuSystem()
: _p_root_menu(NULL),
  _p_curr_menu(NULL)
{
}

boolean MenuSystem::next(boolean loop)
{
  return _p_curr_menu->next(loop);
}

boolean MenuSystem::prev(boolean loop)
{
    return _p_curr_menu->prev(loop);
}

boolean MenuSystem::select(boolean return_to_root)
{
    MenuComponent* pComponent = _p_curr_menu->activate();

    if (pComponent != NULL) // a menu was selected
    {
      _p_curr_menu = (Menu*) pComponent;
      return(true);         // this flags the calling of displayMenu()
    }
    else // A menu item was selected.
    {
        // the menuItem callback function has completed and left us here
        // If return_to_root is true, reset the menu ready for when it's used
        // again; otherwise, the current menu is remembered.
        if (return_to_root)
        {
          _p_curr_menu = _p_root_menu;
         }
    }
    return(false);
}

boolean MenuSystem::back()
{
    if (_p_curr_menu != _p_root_menu)
    {
        _p_curr_menu = const_cast<Menu*>(_p_curr_menu->get_parent());
        return true;
    }

    // We are already in the root menu
    return false;
}

void MenuSystem::set_root_menu(Menu* p_root_menu)
{
    _p_root_menu = p_root_menu;
    _p_curr_menu = p_root_menu;
}

Menu const* MenuSystem::get_current_menu() const
{
  return _p_curr_menu;
}

void MenuSystem::deselect_set_menu_item(void)
{
  _cur_menu_item_selected = false;
}

boolean MenuSystem::menu_item_is_selected(void)
{
  return (_cur_menu_item_selected);
}

boolean MenuSystem::menu_item_was_just_selected (void)
{
    boolean justSelected = _cur_menu_just_selected;
    _cur_menu_just_selected = false;  // clear the flag once we read it
    return (justSelected);
}
