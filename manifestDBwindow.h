/* 
 * File:   manifestDBwindow.h
 * Author: kuestere
 *
 * Created on 25. Juli 2018, 22:41
 */

#ifndef MANIFESTDBWINDOW_H
#define MANIFESTDBWINDOW_H

#include <cstdlib>
#include <fstream>
#include <gtkmm.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <sqlite3.h>

using namespace std;

class ManifestDBWindow : public Gtk::Window {
public:
    ManifestDBWindow(const Glib::RefPtr<Gtk::Application>& app);
    ManifestDBWindow(const ManifestDBWindow& orig);
    virtual ~ManifestDBWindow();

    struct FileInfo {
        Glib::ustring fileID = "";
        // depending of domain display belonging fileID, relative path and flags
        Glib::ustring domain = ""; 
        Glib::ustring relativePath = ""; 
        long flags = 0;
    };
    // full filename of Manifest.db
    string fileURL = "";
    // path to folder where Manifest.db resides
    string pathURL = "";
    vector<FileInfo*> backupItems = {};
    vector<FileInfo*> domainItems = {};
    vector<Glib::ustring> filteredItems = {};

private:

protected:
    //Functions
    std::string choose_database_file();
    std::string choose_folder_for_copying();

    template< typename ... Args >
    void display_status(Args const& ... args);

    //Signal handlers
    void on_button_open();
    void on_button_quit();
    void on_button_about();
    void on_combo_changed();
    void on_entry_activate();
    void on_treeview_row_activated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);
     //Signal handler for AboutDialog
    void on_about_dialog_response(int response_id);

  //Tree model columns:
  class ModelColumns : public Gtk::TreeModel::ColumnRecord
  {
  public:
    // model columns for combobox
    ModelColumns() {
        add(m_col_id); add(m_col_domain); }
        Gtk::TreeModelColumn<int> m_col_id;
        Gtk::TreeModelColumn<Glib::ustring> m_col_domain;
    };
    ModelColumns m_Columns;

  class TableColumns : public Gtk::TreeModel::ColumnRecord
  {
  public:
    // model columns for tableview
    TableColumns() {
        add(m_tablecol_fileID); add(m_tablecol_path); add(m_tablecol_flags); }
        Gtk::TreeModelColumn<Glib::ustring> m_tablecol_fileID;
        Gtk::TreeModelColumn<Glib::ustring> m_tablecol_path;
        Gtk::TreeModelColumn<long> m_tablecol_flags;
    };
    TableColumns m_TableColumns;

    // child widgets
    Gtk::ComboBox m_Combo;
    Gtk::Entry* m_ComboEntry;
    Glib::RefPtr<Gtk::ListStore> m_refComboTreeModel;
    Gtk::ScrolledWindow m_ScrolledWindow;
    Gtk::TreeView m_TreeView;
    Glib::RefPtr<Gtk::ListStore> m_refTableTreeModel;
    Gtk::ScrolledWindow m_TextWindow;
    Gtk::TextView m_TextView;
    Glib::RefPtr<Gtk::TextBuffer> m_refTextBuffer;
    Gtk::AboutDialog m_Dialog;

    Gtk::Box m_VBox;
    Gtk::Toolbar t_Toolbar;
    Gtk::ToolButton t_ToolButtonOpen;
    Gtk::ToolButton t_ToolButtonQuit;
    Gtk::SeparatorToolItem t_ToolItemSpace;
    Gtk::ToolButton t_ToolButtonAbout;
    Gtk::Box m_HBoxCombo;
    Gtk::Frame m_StatusFrame;
    unsigned m_ContextId;
    Gtk::Statusbar m_Statusbar;
};

#endif /* MANIFESTDBWINDOW_H */

