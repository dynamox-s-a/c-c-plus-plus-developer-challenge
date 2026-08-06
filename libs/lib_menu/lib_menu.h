#ifndef LIB_MENU_H_INCLUDED
#define LIB_MENU_H_INCLUDED

#include <vector>
#include <string>

/** 
 * @brief Class to represents a menu item entity.
 * @param name string of Item name.
 * @param id   index id of the item in the menu.
 * */
class MenuItem
{
private: 
    /// @brief String with the description of a Menu Item.
    std::string name;
    /// @brief Identifier value to select the menu by user.
    int id;           
    /// @brief Index of Reference of menu association
    int id_m;
    /// @brief Reference Index of next menu association 
    int id_mx; 
    /// @brief Menu Item option index. idx=0 means th title of menu.
    int ind;

public:
    /**
     * @brief Class Constructor
     * @param name Description of the item.
     * @param id Identifier value to select the menu by user.
     */
    MenuItem(const std::string& name, int ind);
    /**
     * @brief Class Destructor
     */
    ~MenuItem();
    /**
     * @brief Method to read the name description of Item.
     * @return String of name.
     */
    std::string getName() const { return name;}
    /**
     * @brief Method to read the menu item ID.
     * @return int index ID value of Item.
     */
    int getId() const { return id; }
    /**
     * @brief Method to configure the id of the menu item.
     * @details it's use to primary key of database.
     */
    void set_id(const int id);
    /**
     * @brief Method to read the menu ID associated.
     */
    int get_id_m() const;
    /**
     * @brief Method to configure the menu id of item
     * @details It is used to indicate what menu it is associated.
     */
    void set_id_m(const int id_m);
    /**
     * @brief Method to read the menu ID associated to go.
     */
    int get_id_mx() const {return id_mx;}
    /**
     * @brief Method to configure the id of the next menu.
     * @details If the item is associate to another menu this
     * variable store the index of this next menu.
     */
    void set_id_mx(const int id_mx);
    /**
     * @brief Method to read the index of Menu Item showed to user.
     * @return int of Index value showed to the user. 
     */
    int get_ind() const {return ind;}
    /**
     * @brief Method to configure the index of Menu.
     * @param ind int value used to identify the item to the user.
     */
    void set_ind(const int ind);
};

/** 
 * @brief Class to represents a menu abstraction.
 * */
class Menu
{
private:
    std::vector<MenuItem> items;       ///< Vector list of Itens in a Menu
    std::string title;                 ///< String of Menu's title.
    Menu* back_menu;
public:
    /**
     * @brief Class Constructor 
     */
    Menu(){};
    /**
     * @brief Class Constructor
     * @param strTitle String of title menu.
     */
    Menu(std::string strTitle) : title(strTitle), back_menu(nullptr) {};
    /**
     * @brief Class destructor
     */
    virtual ~Menu();
    /**
     * @brief Method to add a new item in the menu.
     * @param item MenuItem Object to be added in the Menu list.
     */
    void addItem(const MenuItem& item);
    /**
     * @brief Method to set the menu title
     * @param title std::string of menu title.
     */
    void setTitle(const std::string title);
    /**
     * @brief Method to read the string Menu's title.
     * @return String of title.
     */
    std::string getTitle() const { return title;}
    /**
     * @brief Method to read a Itens list of menu.
     * @return std::vector<MenuItem>& Vector reference of list menu.
     */
    const std::vector<MenuItem>& getItems() const { return items;}
    /**
     * @brief Method to show the menu
     * @details This method could be overrade to implement according the interface.
     */
    virtual void display() const;
};



#endif // LIB_MENU_H_INCLUDED   