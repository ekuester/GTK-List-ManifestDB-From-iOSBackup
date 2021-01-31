//  List ManifestDB
//  See what is covered under the hood of IOS
//
//  File: manifestDBwindow.cpp
//
//  Program for parsing the Backup files of IOS for iPad and iPhone, generated
//    by iTunes versions higher than 12.5 or so ( older versions are using a
//    different method for a backup ).
//   The main information is stored in a SQLite database named Manifest.db .
//   To access the database from Linux
//   1. mount the device where macOS is installed
//    $ udisksctl mount --block-device /dev/sda2
//   2. authentificate yourself, then you will see where the device is mounted,
//    for instance: /dev/sda2 at /run/media/kuestere/MacBookPro SSD
//   Standard path then - going out from above - is
//     userdirectory /Users/kuestere/ (depending of registered user),
//     default subdirectory then is Library/Application\ Support/MobileSync/Backup,
//     the SQLite database is found in a subdirectory of that.
//
//  Use the program for extracting of stored files which could not be recovered otherwise.
//           Created by Erich Küster on October 3, 2016
//
//  Access to SQLite under C++ was realized analogous to the first answer given at
//  <http://stackoverflow.com/questions/24102775/accessing-an-sqlite-database-in-swift>
//  The old code (in Swift, now abandoned) was rewritten as of July 25, 2018
//  now in C++ with the GTK+ wrapper gtkmm, last changes January 2021
//
//  Copyright © 2016-2021 Erich Küster. All rights reserved.
//

#include "about.xpm"
#include "manifestDBwindow.h"

const Glib::ustring app_title = N_("Gtk+: iOS Backup - Read Manifest.db");

ManifestDBWindow::ManifestDBWindow(const Glib::RefPtr<Gtk::Application>& app)
: m_VBox(Gtk::ORIENTATION_VERTICAL, 8),
  t_Toolbar(),
  t_ToolButtonOpen(Gtk::Stock::OPEN),
  t_ToolButtonQuit(Gtk::Stock::QUIT),
  t_ToolItemSpace(),
  t_ToolButtonAbout(Gtk::Stock::ABOUT), 
  m_ExportButtonBox(Gtk::ORIENTATION_HORIZONTAL),
  m_ButtonExport(_("Export CSV..."), true),
  m_Combo(true),//with entry if set true
  m_HBoxCombo(Gtk::ORIENTATION_HORIZONTAL),
  m_HBoxBottom(Gtk::ORIENTATION_HORIZONTAL),
  m_ScrolledWindow(),
  m_TextWindow(),
  m_StatusFrame(),
  m_Statusbar()
{
    // Set up window with application icon and the top-level container
    Glib::RefPtr<Gio::MemoryInputStream> stream = Gio::MemoryInputStream::create();
    stream->add_data(icon_svg);
    set_icon(Gdk::Pixbuf::create_from_stream(stream));
    stream->close();
    set_title(app_title);
    set_border_width(5);
    set_default_size(1024, 768);
    // horizontal tool bar above
    t_Toolbar.set_toolbar_style(Gtk::TOOLBAR_BOTH);
    t_ToolButtonOpen.set_tooltip_text(N_("Open data base"));
    t_ToolButtonOpen.signal_clicked().connect(sigc::mem_fun(*this,
        &ManifestDBWindow::on_button_open));
    t_ToolButtonQuit.set_tooltip_text(_("Exit program"));
    t_ToolButtonQuit.signal_clicked().connect(sigc::mem_fun(*this,
        &ManifestDBWindow::on_button_quit));
    t_ToolButtonAbout.set_tooltip_text(N_("About program"));
    t_ToolButtonAbout.signal_clicked().connect(sigc::mem_fun(*this,
        &ManifestDBWindow::on_button_about));
    t_Toolbar.append(t_ToolButtonOpen);
    t_Toolbar.append(t_ToolButtonQuit);
    t_ToolItemSpace.set_expand(true);
    t_Toolbar.append(t_ToolItemSpace);
    t_Toolbar.append(t_ToolButtonAbout);
    m_VBox.pack_start(t_Toolbar, Gtk::PACK_SHRINK);

    //Create the Tree model for combobox
    m_refComboTreeModel = Gtk::ListStore::create(m_Columns);
    m_Combo.set_model(m_refComboTreeModel);
     //Connect signal handler:
    m_Combo.signal_changed().connect( sigc::mem_fun(*this, &ManifestDBWindow::on_combo_changed) );
    // add the model columns to the Combo (which is a kind of view).
    // without entry rendering them in the default way
    m_Combo.pack_start(m_Columns.m_col_id);
    //m_Combo.pack_start(m_Columns.m_col_domain);
    // with entry rendering is done automatically
    m_Combo.set_entry_text_column(1);
    m_ComboEntry = m_Combo.get_entry();
    if (m_ComboEntry) {
        // handles ENTER pressed
        m_ComboEntry->signal_activate().connect(
            sigc::mem_fun(*this, &ManifestDBWindow::on_entry_activate));
    }
    //Create the Tree model for tableview
    m_refTableTreeModel = Gtk::ListStore::create(m_TableColumns);
    m_TreeView.set_model(m_refTableTreeModel);

    //Create the Text Buffer
    m_refTextBuffer = Gtk::TextBuffer::create();
    m_TextView.set_buffer(m_refTextBuffer);

    //add the TreeView, inside a ScrolledWindow, with the combobox above
    m_ScrolledWindow.set_size_request(-1, 240);
    m_ScrolledWindow.add(m_TreeView);
    //only show the scrollbars when they are necessary:
    m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    //Add the TextView, inside a ScrolledWindow under the tableview
    m_TextWindow.set_size_request(-1, 60);
    m_TextWindow.add(m_TextView);
    //Only show the scrollbars when they are necessary:
    m_TextWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    //Add the TreeView's view columns:
    //This number will be shown with the default numeric formatting.
    m_TreeView.append_column(_("File ID"), m_TableColumns.m_tablecol_fileID);
    m_TreeView.append_column(_("File Path"), m_TableColumns.m_tablecol_path);
    m_TreeView.append_column(_("File Flags"), m_TableColumns.m_tablecol_flags);

    //Make all the columns reorderable and resizable:
    //This is not necessary, but it's nice to show the feature.
    //You can use TreeView::set_column_drag_function() to more
    //finely control column drag and drop.
    for ( auto* column : m_TreeView.get_columns() ) {
        column->set_reorderable();
        column->set_resizable(true);
        //column->set_alignment(Gtk::ALIGN_FILL);
    }
    //Connect signal handler
    m_TreeView.signal_row_activated().connect(sigc::mem_fun(*this, \
      &ManifestDBWindow::on_treeview_row_activated) );

    // add horizontal box for csv export
    m_HBoxBottom.pack_end(m_ExportButtonBox, Gtk::PACK_SHRINK);
    m_HBoxBottom.set_border_width(2);
    m_ExportButtonBox.pack_start(m_ButtonExport, Gtk::PACK_SHRINK);
    m_ExportButtonBox.set_border_width(2);
    m_ExportButtonBox.set_layout(Gtk::BUTTONBOX_CENTER);
    m_ButtonExport.signal_clicked().connect(sigc::mem_fun(*this, \
      &ManifestDBWindow::on_button_write_csv));

    m_StatusFrame.add(m_Statusbar);
    m_VBox.pack_end(m_StatusFrame, Gtk::PACK_SHRINK);
    m_ContextId = m_Statusbar.push(_("choose a data base, click Open"), m_ContextId);

    //Add the vertical Box to the window.
    add(m_VBox);
    show_all_children();

    m_Dialog.set_transient_for(*this);
    m_Dialog.set_logo(Gdk::Pixbuf::create_from_xpm_data(about));
    m_Dialog.set_program_name(app_title);
    m_Dialog.set_size_request(480, -1);
    m_Dialog.set_version(_("Version 1.1.5"));
    m_Dialog.set_copyright("Copyright © 2018-2021 Erich Küster. All rights reserved.");
    m_Dialog.set_comments(_("Read and extract iOS Backup Data Base Manifest.db"));
    std::ifstream licenseFile("LICENSE");
    std::stringstream licenseStream;
    licenseStream << licenseFile.rdbuf();
    m_Dialog.set_license(licenseStream.str());
    std::ifstream commentsFile("COMMENTS");
    std::stringstream commentsStream;
    commentsStream << commentsFile.rdbuf();
    m_Dialog.set_comments(commentsStream.str());

    m_Dialog.set_website("http://www.gtkmm.org");
    m_Dialog.set_website_label("gtkmm Website - C++ Interfaces for GTK+ and GNOME");

    std::vector<Glib::ustring> list_authors;
    list_authors.push_back("Erich Küster, Krefeld/Germany");
    m_Dialog.set_authors(list_authors);

    m_Dialog.signal_response().connect(
        sigc::mem_fun(*this, &ManifestDBWindow::on_about_dialog_response));
}

ManifestDBWindow::ManifestDBWindow(const ManifestDBWindow& orig) {
}

ManifestDBWindow::~ManifestDBWindow() {
}

string ManifestDBWindow::choose_database_file() {
    // File Open Dialog
    Gtk::FileChooserDialog dialog(N_("Select Manifest.db file"),
            Gtk::FileChooserAction::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_transient_for(*this);

    //Add response buttons the the dialog:
    dialog.add_button(_("Cancel"), Gtk::ResponseType::RESPONSE_CANCEL);
    dialog.add_button(_("Open"), Gtk::ResponseType::RESPONSE_OK);

    //Add filters, so that only certain file types can be selected:
    auto filter_db = Gtk::FileFilter::create();
    filter_db->set_name(_("Database files"));
    filter_db->add_pattern("*.db");
    dialog.add_filter(filter_db);

    auto filter_any = Gtk::FileFilter::create();
    filter_any->set_name(_("Any files"));
    filter_any->add_pattern("*");
    dialog.add_filter(filter_any);

    //Show the dialog and wait for a user response:
    int result = dialog.run();

    //Handle the response:
    switch (result) {
        case Gtk::ResponseType::RESPONSE_OK:
        {
            //Notice that this is a std::string, not a Glib::ustring
            return dialog.get_filename();
            break;
        }
        case Gtk::ResponseType::RESPONSE_CANCEL:
        {
            m_ContextId = m_Statusbar.push(_("no data base chosen"), m_ContextId);
            break;
        }
        default:
        {
            std::cout << _("unexpected button clicked.") << std::endl;
            break;
        }
    }
    return string();
}

void ManifestDBWindow::on_button_open() {
    sqlite3 *sqliteDB;
    const char* szErrMsg = 0;
    fileURL = choose_database_file();
    pathURL = fileURL.substr(0, fileURL.find_last_of("/\\"));
    if (!fileURL.empty()) {
        //will be displayed when combobox become visible
        display_status(_("backup data base was found at: "), fileURL);
        const char* filename = fileURL.c_str();
        if (backupItems.size() > 0) {
            backupItems.clear();
        }
        // open database
        int result = sqlite3_open(filename, &sqliteDB);
        if (result == SQLITE_OK) {
            //cout << "Open database successfully" << endl;
            const char* command = "select fileID, domain, relativePath, flags, file from Files";
            sqlite3_stmt *stmt;
            if (sqlite3_prepare_v2(sqliteDB, command, -1, &stmt, NULL) != SQLITE_OK) {
                szErrMsg = sqlite3_errmsg(sqliteDB);
                cout << _("error preparing select: ") << szErrMsg << endl;
            } else {
                while (sqlite3_step(stmt) == SQLITE_ROW) {
                    FileInfo* fileInfo = new FileInfo;
                    basic_string<unsigned char> item1 = sqlite3_column_text(stmt, 0);
                    fileInfo->fileID = string(item1.begin(), item1.end());
                    basic_string<unsigned char> item2 = sqlite3_column_text(stmt, 1);
                    fileInfo->domain = string(item2.begin(), item2.end());
                    basic_string<unsigned char> item3 = sqlite3_column_text(stmt, 2);
                    fileInfo->relativePath = string(item3.begin(), item3.end());
                    fileInfo->flags = sqlite3_column_int64(stmt, 3);
                    const unsigned char* blob = \
                      reinterpret_cast<const unsigned char*>(sqlite3_column_blob(stmt, 4));
                    // Get the number of blob bytes
	                int blob_bytes = sqlite3_column_bytes(stmt, 4);
                    // new vector of blob_bytes size + 2 for quotes
                    std::vector<unsigned char>* file_blob = new std::vector<unsigned char>(blob_bytes+2);
                    (*file_blob)[0] = '\"';
                    // copy blob data into vector
                    //std::copy(blob, blob + blob_bytes, &((*file_blob)[1]));
                    std::copy(blob, blob + blob_bytes, &(file_blob->at(1)));
                    (*file_blob)[blob_bytes+1] = '\"';
                    fileInfo->fileBlob = file_blob;
                    backupItems.push_back(fileInfo);
                }
                //cout << "added " << backupItems.size() << " backup items" << endl;
                if (filteredItems.size() > 0) {
                    filteredItems.clear();
                    // first remove ScrolledWindow, otherwise the combobox slips beneath
                    m_VBox.remove(m_ScrolledWindow);
                    m_VBox.remove(m_TextWindow);
                    // make place for new combobox rows
                    m_refComboTreeModel->clear();
                }
                // filter out "naked" domain when relativePath is empty
                for (auto* backupItem : backupItems) {
                    if (backupItem->relativePath.empty())
                        filteredItems.push_back(backupItem->domain);
                }
                //cout << "added " << filteredItems.size() << " filtered items" << endl;
                sort(filteredItems.begin(), filteredItems.end());
                //fill the ComboBox's Tree Model
                Gtk::TreeModel::Row row = *(m_refComboTreeModel->append());
                row[m_Columns.m_col_id] = 1;
                row[m_Columns.m_col_domain] = _("Please select domain to display");
                //remove child(ren) from main window
                remove();
                m_VBox.remove(m_HBoxBottom);
                m_VBox.remove(m_HBoxCombo);
                m_HBoxCombo.remove(m_Combo);
                // first row is active
                m_Combo.set_active(0);
                // add the combobox to a horizontal box, horizontally centered
                m_HBoxCombo.pack_start(m_Combo, Gtk::PACK_EXPAND_WIDGET);
                // add combobox row for every backupItem object
                int i = 2;
                for (auto filteredItem : filteredItems) {
                    Gtk::TreeModel::Row row = *(m_refComboTreeModel->append());
                    row[m_Columns.m_col_id] = i++;
                    row[m_Columns.m_col_domain] = filteredItem;
                }
                add(m_VBox);
                m_VBox.pack_start(m_HBoxCombo, Gtk::PACK_SHRINK);
                m_VBox.pack_start(m_ScrolledWindow);
                m_VBox.pack_start(m_TextWindow);
                m_VBox.pack_end(m_HBoxBottom, Gtk::PACK_SHRINK);
                m_VBox.set_border_width(8);
                show_all_children();
           }
            if (sqlite3_finalize(stmt) != SQLITE_OK) {
                szErrMsg = sqlite3_errmsg(sqliteDB);
                cout << _("error finalizing prepared statement: ") << szErrMsg << endl;
            }
            stmt = NULL;
        } else {
            m_ContextId = m_Statusbar.push(_("Can't open database"), m_ContextId);
        }
        //close database
        if (sqliteDB) {
            sqlite3_close(sqliteDB);
            m_ContextId = m_Statusbar.push(_("data base closed"), m_ContextId);
        }
    }
}

void ManifestDBWindow::on_button_write_csv() {
    // export data records into .csv file
    ofstream infoFile;
    // simple beginning: save file in same directory
    infoFile.open("Manifest.csv");
    for (auto* backupItem : backupItems) {
        stringstream fileStream;
        fileStream << backupItem->fileID << ";";
        fileStream << backupItem->domain << ";";
        if (backupItem->relativePath.empty())
            fileStream << "\"\";";
        else
            fileStream << backupItem->relativePath << ";";;
        fileStream << backupItem->flags << ";";
        infoFile << fileStream.str();
        // infoFile << "size: " << backupItem->fileBlob->size();           
        for (vector<unsigned char>::const_iterator \
          i = backupItem->fileBlob->begin(); i != backupItem->fileBlob->end(); ++i) {
    	    infoFile << *i;
        }
        infoFile << endl;
    }
    infoFile.close();
    m_ContextId = m_Statusbar.push(_("CSV file exported"), m_ContextId);
 }

void ManifestDBWindow::on_button_quit() {
    hide();
}

void ManifestDBWindow::on_button_about() {
    m_Dialog.show();
    //bring it to the front, in case it was already shown:
    m_Dialog.present();    
}

void ManifestDBWindow::on_about_dialog_response(int response_id) {
    switch (response_id) {
        case Gtk::RESPONSE_CLOSE:
        case Gtk::RESPONSE_CANCEL:
        case Gtk::RESPONSE_DELETE_EVENT:
            m_Dialog.hide();
            break;
        default:
            m_ContextId = m_Statusbar.push(_("unexpected response"), m_ContextId);
            break;
    }
}

void ManifestDBWindow::on_combo_changed() {
    Gtk::TreeModel::iterator iter = m_Combo.get_active();
    if (iter) {
        Gtk::TreeModel::Row row = *iter;
        if (row) {
            // delete data of an earlier run
            if (domainItems.size() > 0) {
                domainItems.clear();
                // make place for new table rows
                m_refTableTreeModel->clear();
            }
            //Get the data for the selected row,
            //using our knowledge of the tree model:
            int id = row[m_Columns.m_col_id];
            Glib::ustring domain = row[m_Columns.m_col_domain];
            //std::cout << "ID=" << id << ", domain=" << domain << std::endl;
            if (id > 1) {
                //remove child(ren) from main window
                remove();
                m_VBox.remove(m_ScrolledWindow);
                m_VBox.remove(m_TextWindow);
                // filter out file info of the choosen domain 
                for (auto* backupItem : backupItems) {
                    if (backupItem->domain == domain)
                        domainItems.push_back(backupItem);
                }
                // Fill the Table TreeView's model
                // Add table row for every domainItem object
                for (auto* domainItem : domainItems) {
                    Gtk::TreeModel::Row row = *(m_refTableTreeModel->append());
                    row[m_TableColumns.m_tablecol_fileID] = domainItem->fileID;
                    row[m_TableColumns.m_tablecol_path] = domainItem->relativePath;
                    row[m_TableColumns.m_tablecol_flags] = domainItem->flags;
                }
                m_ContextId = m_Statusbar.push(_("content of choosen domain displayed"), m_ContextId);
                m_VBox.pack_start(m_ScrolledWindow);
                m_VBox.pack_end(m_TextWindow);
                m_VBox.pack_end(m_HBoxBottom, Gtk::PACK_SHRINK);
                m_VBox.set_border_width(8);
                add(m_VBox);
                show_all_children();
            }
        }
    }
}

void ManifestDBWindow::on_entry_activate() {
    // search first row where domain begins with actual entry text
    Glib::ustring small = m_ComboEntry->get_text();
    auto children = m_refComboTreeModel->children();
    int first_match = 0;
    m_ContextId = m_Statusbar.push(_("no match found for entry"), m_ContextId);
    for (auto child : children) {
        Gtk::TreeModel::Row row = *child;
        Glib::ustring domain = row[m_Columns.m_col_domain];
        if (domain.compare(0, small.length(), small) == 0) {
            first_match = row[m_Columns.m_col_id] - 1;
            m_Combo.set_active(first_match);
            m_ContextId = m_Statusbar.push(_("first match for entry"), m_ContextId);
            break;
        }
    }
}

string ManifestDBWindow::choose_folder_for_copying() {
    // Folder for Copying Dialog
    Gtk::FileChooserDialog dialog(_("Please choose a folder for copying"),
            Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);
    dialog.set_transient_for(*this);

    //Add response buttons the the dialog:
    dialog.add_button(_("Cancel"), Gtk::RESPONSE_CANCEL);
    dialog.add_button(_("Select"), Gtk::RESPONSE_OK);

    int result = dialog.run();

    //Handle the response:
    switch (result) {
        case(Gtk::RESPONSE_OK):
        {
            return dialog.get_filename();
            break;
        }
        case(Gtk::RESPONSE_CANCEL):
        {
            m_ContextId = m_Statusbar.push(_("no file chosen, nothing stored"), m_ContextId);
            break;
        }
        default:
        {
            std::cout << _("Unexpected button clicked.") << std::endl;
            break;
        }
    }
    return string();
}

void ManifestDBWindow::on_treeview_row_activated(const Gtk::TreeModel::Path& path,
        Gtk::TreeViewColumn* /* column */) {
    Gtk::TreeModel::iterator iter = m_refTableTreeModel->get_iter(path);
    if (iter) {
        Gtk::TreeModel::Row row = *iter;
        Glib::ustring fileID = row[m_TableColumns.m_tablecol_fileID];
        Glib::ustring relativePath = row[m_TableColumns.m_tablecol_path];
        long flags = row[m_TableColumns.m_tablecol_flags];
        //cout << "Row activated: File ID=" << fileID << " flags=" << flags << std::endl;
        //only if flags == 1 there is a file to copy
        if (flags == 1) {
            //get first two characters of file ID
            auto hash = fileID.substr(0, 2);
            string backupURL = pathURL + "/" + hash + "/" + fileID;
            display_status("backupURL: ", backupURL);
            string lastComponent = relativePath.substr(relativePath.find_last_of("/\\") + 1);
            string copyPath = choose_folder_for_copying();
            if (!copyPath.empty()) {
                string copyURL = copyPath + "/" + lastComponent;
                display_status(_("copyURL: "), copyURL);
                // copy backup into another file
                ifstream source(backupURL, ios::binary);
                if (source) {
                    ofstream target(copyURL, ios::binary);
                    target << source.rdbuf();

                    source.close();
                    target.close();

                    display_status(_("backup copied to new file: "), copyURL);
                    m_ContextId = m_Statusbar.push(_("file copying successful"), m_ContextId);
                }
                else {
                    display_status(_("cannot find file: "), copyURL);
                    m_ContextId = m_Statusbar.push(_("file to copy not found"), m_ContextId);
                }
            }
        }
    }
}

//analogous to
// <https://stackoverflow.com/questions/21806561/concatenating-strings-and-numbers-in-variadic-template-function>
template< typename ... Args >
void ManifestDBWindow::display_status(Args const& ... args) {
    std::ostringstream stream;
    using List= int[];
    (void)List{0, ( (void)(stream << args), 0 ) ... };
    //every call will give a new line
    stream << endl;
    Gtk::TextBuffer::iterator iter_end = 
        m_refTextBuffer->get_iter_at_offset(m_refTextBuffer->get_char_count());
    m_refTextBuffer->insert(iter_end, stream.str());
}
