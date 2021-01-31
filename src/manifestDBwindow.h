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
<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<svg id=\"svg154\" width=\"256\" height=\"256\" version=\"1.1\" viewBox=\"0 0 256 256\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:cc=\"http://creativecommons.org/ns#\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\
 <defs id=\"defs142\">\
  <linearGradient id=\"linearGradient868\" x1=\"98\" x2=\"98\" y1=\"98\" y2=\"195.5\" gradientTransform=\"matrix(1.2308 0 0 1.2308 8.8767 8.7226)\" gradientUnits=\"userSpaceOnUse\">\
   <stop id=\"stop864\" stop-color=\"#ff0035\" offset=\"0\"/>\
   <stop id=\"stop866\" stop-color=\"#fd5\" offset=\"1\"/>\
  </linearGradient>\
  <linearGradient id=\"linearGradient862\" x1=\"138.69\" x2=\"221.78\" y1=\"167.91\" y2=\"167.91\" gradientUnits=\"userSpaceOnUse\" xlink:href=\"#linearGradient868\"/>\
  <linearGradient id=\"linearGradient887\" x1=\"93.205\" x2=\"128.06\" y1=\"140.09\" y2=\"98.375\" gradientTransform=\"translate(-3.0196)\" gradientUnits=\"userSpaceOnUse\" xlink:href=\"#linearGradient868\"/>\
  <linearGradient id=\"linearGradient893\" x1=\"105.77\" x2=\"103.04\" y1=\"35.507\" y2=\"16.058\" gradientTransform=\"matrix(.29209 0 0 3.4236 12.98 1.357)\" gradientUnits=\"userSpaceOnUse\" xlink:href=\"#linearGradient868\"/>\
  <linearGradient id=\"linearGradient895\" x1=\"149.64\" x2=\"201.03\" y1=\"47.501\" y2=\"108.7\" gradientTransform=\"matrix(.82996 0 0 1.2049 12.98 1.357)\" gradientUnits=\"userSpaceOnUse\">\
   <stop id=\"stop121\" stop-color=\"#ffb380\" offset=\"0\"/>\
   <stop id=\"stop123\" stop-color=\"#ff2a2a\" offset=\"1\"/>\
  </linearGradient>\
  <filter id=\"filter854\" x=\"-.072\" y=\"-.072\" width=\"1.144\" height=\"1.144\" color-interpolation-filters=\"sRGB\">\
   <feGaussianBlur id=\"feGaussianBlur856\" stdDeviation=\"3.5141602\"/>\
  </filter>\
 </defs>\
 <rect id=\"rect1206\" x=\"8.1453\" y=\"8.1453\" width=\"240\" height=\"240\" rx=\"32\" ry=\"32\" fill=\"#fff\" fill-opacity=\".93891\" stroke=\"#e9e5e5\" stroke-width=\"3.7795\"/>\
 <path id=\"path146\" d=\"m45.862 138.9h-4.5633v-60.979h4.5633zm-2.3466-75.927c-1.9244 0-3.5158-1.5265-3.5158-3.4509 0-1.9812 1.5833-3.5158 3.5158-3.5158 1.9812 0 3.5564 1.5265 3.5564 3.5158 0 1.9244-1.5752 3.4509-3.5564 3.4509z\" fill=\"url(#linearGradient893)\" fill-rule=\"evenodd\"/>\
 <path id=\"path148\" d=\"m90.185 140.09c-22.313 0-36.409-16.248-36.409-42.076 0-25.699 14.161-42.011 36.409-42.011s36.401 16.313 36.401 42.011c0 25.829-14.096 42.076-36.401 42.076zm0-79.89c-19.422 0-31.821 14.664-31.821 37.813 0 23.166 12.456 37.887 31.821 37.887 19.422 0 31.821-14.721 31.821-37.887 0-23.157-12.399-37.813-31.821-37.813z\" fill=\"url(#linearGradient887)\" fill-rule=\"evenodd\"/>\
 <path id=\"path150\" d=\"m159.08 140.09c-16.751 0-28.76-9.4595-29.442-22.987h4.474c0.68206 11.092 11.1 18.854 25.309 18.854 13.868 0 23.547-7.8761 23.547-18.513 0-8.5582-5.7731-13.479-19.471-16.93l-9.6787-2.3791c-15.111-3.8569-21.972-9.971-21.972-20.21 0-12.74 11.895-21.915 26.787-21.915 15.395 0 26.892 9.0616 27.404 21.063h-4.474c-0.62522-9.7924-10.19-16.93-23.044-16.93-12.293 0-22.086 7.3646-22.086 17.668 0 8.1603 6.0005 12.854 19.13 16.134l9.1184 2.3222c15.793 3.9056 22.873 9.971 22.873 20.835 0 13.527-11.376 22.987-28.476 22.987z\" fill=\"url(#linearGradient895)\" fill-rule=\"evenodd\"/>\
 <text id=\"text1188\" x=\"-110.63005\" y=\"153.78233\" fill=\"#000000\" font-family=\"sans-serif\" font-size=\"40px\" style=\"line-height:1.25\" xml:space=\"preserve\"><tspan id=\"tspan1186\" x=\"-110.63005\" y=\"153.78233\"/></text>\
 <text id=\"text1192\" x=\"87.968163\" y=\"180.4267\" fill=\"url(#linearGradient862)\" font-family=\"sans-serif\" font-size=\"42.667px\" style=\"line-height:1.25;mix-blend-mode:normal\" xml:space=\"preserve\"><tspan id=\"tspan1190\" x=\"87.968163\" y=\"180.4267\" fill=\"url(#linearGradient862)\" font-family=\"'Noto Sans'\" font-size=\"42.667px\">Backup</tspan></text>\
 <path id=\"path849\" transform=\"matrix(2 0 0 2 .14531 .14531)\" d=\"m117.14 6.8613c1.8003 2.5889 2.8613 5.7328 2.8613 9.1387v88c0 8.864-7.136 16-16 16h-88c-3.4059 0-6.5498-1.061-9.1387-2.8613 2.8851 4.1488 7.6806 6.8613 13.139 6.8613h88c8.864 0 16-7.136 16-16v-88c0-5.4581-2.7126-10.254-6.8613-13.139z\" filter=\"url(#filter854)\" opacity=\".36\"/>\
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

