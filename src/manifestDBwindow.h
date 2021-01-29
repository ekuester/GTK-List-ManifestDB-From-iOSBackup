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
#include <glibmm/i18n.h>
#include <gtkmm.h>
#include <iomanip>
#include <iostream>
#include <locale.h>
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
        // BLOB: binary large object
        std::vector<unsigned char>* fileBlob;
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
    void on_button_about();
    void on_button_open();
    void on_button_quit();
    void on_button_write_csv();
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

    // application icon
    std::string icon_svg = "\
<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\
<svg xmlns:xlink=\"http://www.w3.org/1999/xlink\" enable-background=\"new 0 0 256 256\" width=\"256px\" height=\"256\" version=\"1.1\" viewBox=\"0 0 256 256\" xml:space=\"preserve\">\
<g>\
  <path id=\"path1\"\
  d=\"m227.9 90.788c-15.879 11.527-24.958 26.778-23.626 47.493 1.3287 20.684 12.057 34.656 29.491 43.7-0.71438 2.0937-1.4168 4.2982-2.2111 6.4798-3.8923 10.654-9.0925 20.566-15.422 29.85-2.918 4.2675-5.8164 8.5351-9.202 12.423-3.3516 3.8616-7.0349 7.2826-11.536 9.6448-3.9684 2.0885-8.161 2.976-12.589 2.8131-4.8345-0.18245-9.3405-1.633-13.733-3.6387-3.2226-1.4727-6.4313-2.9728-9.827-3.9776-5.4029-1.5887-10.891-1.908-16.453-1.2538-4.7533 0.56953-9.219 2.1322-13.588 4.0812-3.3611 1.5033-6.6957 3.0646-10.262 4.0004-3.1873 0.83213-6.4124 1.2994-9.7062 0.98461-3.7374-0.35188-7.1287-1.7796-10.287-3.8349-4.3027-2.7955-7.9023-6.4388-11.164-10.439-14.364-17.653-24.497-37.614-29.978-60.122-1.527-6.2837-2.6914-12.623-3.3661-19.074-0.76977-7.3348-0.92272-14.683-0.24421-22.031 0.91705-9.9426 3.2415-19.471 7.7537-28.338 6.3973-12.55 15.771-21.73 28.379-27.156 6.2066-2.6639 12.693-3.9515 19.402-3.8935 4.54 0.03323 8.9351 1.0328 13.201 2.6059 4.2145 1.5659 8.3605 3.3259 12.561 4.9159 2.0915 0.80151 4.2246 1.4922 6.3703 2.1041 3.0986 0.87775 6.1506 0.36687 9.1019-0.75264 3.8923-1.4597 7.7556-3.0353 11.63-4.5666 3.8772-1.545 7.7928-2.9148 11.845-3.8668 4.5412-1.0856 9.1164-1.3469 13.767-0.99244 7.1338 0.55258 13.963 2.284 20.417 5.5252 7.7493 3.9 14.096 9.538 19.02 16.927 0.10197 0.14857 0.19764 0.29714 0.2568 0.38902z\"\
  clip-rule=\"evenodd\" fill=\"url(#linearGradient4902)\"\
  fill-rule=\"evenodd\" stroke=\"#008080\" stroke-width=\"1.28\" />\
  <path id=\"path3\"\
    d=\"m187.85 12.011c0.0516 1.7268 0.20708 3.4432 0.13721 5.1551-0.52241 14.232-5.9637 26.166-15.606 36.048-4.5809 4.6924-9.985 8.1265-16.088 10.292-3.9804 1.4101-8.0722 2.0794-12.289 1.8611-0.43115-0.02541-0.54319-0.18311-0.59102-0.6171-0.80817-7.832 0.8025-15.199 3.8992-22.264 6.3615-14.49 16.978-23.941 31.392-28.836 2.6158-0.8771 5.3003-1.4101 8.0426-1.6349 0.32792-0.02998 0.65459-0.0039 1.1027-0.0039z\"\
    clip-rule=\"evenodd\" fill=\"url(#linearGradient4910)\"\
    fill-rule=\"evenodd\" stroke=\"#008080\" stroke-width=\"1.28\" />\
  <text id=\"text49\" transform=\"scale(.99995 1.0001)\"\
    x=\"23.444899\" y=\"172.85513\" fill=\"#ff2a2a\"\
    font-family=\"'Noto Sans'\" font-size=\"93.655px\" font-weight=\"700\"\
    letter-spacing=\"0\" stroke-width=\"2.5093\" word-spacing=\"0\" style=\"line-height:1.25\">\
    <tspan id=\"tspan47\" x=\"23.444899\" y=\"172.85513\" fill=\"#ff2a2a\"\
      font-size=\"107.03px\" stroke-width=\"9.0442\">iOS\
    </tspan>\
  </text>\
  <text id=\"text47\" transform=\"scale(.99995 1.0001)\"\
    x=\"42.206013\" y=\"213.31171\" fill=\"#ff2a2a\"\
    font-family=\"'Noto Sans'\" font-size=\"48px\" font-weight=\"700\"\
    letter-spacing=\"0\" stroke-width=\"2.5093\" word-spacing=\"0\" style=\"line-height:1.25\">\
    <tspan id=\"tspan48\" x=\"42.206013\" y=\"213.31171\" fill=\"#ff2a2a\"\
      font-size=\"48px\" stroke-width=\"9.0442\">Backup\
    </tspan>\
  </text></g>\
  <linearGradient id=\"linearGradient4902\" x1=\"53.129\" x2=\"234.55\"\
      y1=\"155.64\" y2=\"155.64\" gradientUnits=\"userSpaceOnUse\">\
    <stop id=\"stop4896\" stop-color=\"#008080\" offset=\"0\" />\
    <stop id=\"stop4898\" stop-color=\"#008080\" stop-opacity=\"0\"\
      offset=\"1\" />\
  </linearGradient>\
  <linearGradient id=\"linearGradient4910\" x1=\"142.57\" x2=\"188.65\"\
      y1=\"38.703\" y2=\"38.703\" gradientUnits=\"userSpaceOnUse\">\
    <stop id=\"stop4904\" stop-color=\"#008080\" offset=\"0\" />\
    <stop id=\"stop4906\" stop-color=\"#008080\" stop-opacity=\"0\"\
      offset=\"1\" />\
  </linearGradient>\
</svg>";

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
    Gtk::Box m_HBoxBottom;
    Gtk::Box m_HBoxCombo;
    Gtk::ButtonBox m_ExportButtonBox;
    Gtk::Button m_ButtonExport;
    Gtk::Frame m_StatusFrame;
    unsigned m_ContextId;
    Gtk::Statusbar m_Statusbar;
};

#endif /* MANIFESTDBWINDOW_H */

