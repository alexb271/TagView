src_files = files(
                 # A simple main function launching a mainwindow.
                 'main.cc',

                 # The main application window. Other than various
                 # controls in the header bar, it hosts the three
                 # main derived widgets in a horizontal GtkBox.
                 'mainwindow.cc',

                 # The menu object for the main menu button. It
                 # is derived from GtkPopOver.
                 'mainmenu.cc',

                 # Zoom controls and a back button for the
                 # image viewer, derived from a GtkBox.
                 'viewercontrols.cc',

                 # The widget responsible for displaying images
                 # in their original size, as well as allow zooming.
                 # It is a GtkPicture placed in a GtkScrolledWindow.
                 # For zooming it relies on GdkPixbuf's scaling feature.
                 'imageviewer.cc',

                 # A GtkIconView placed in a GtkScrolledWindow. Shows
                 # previews of images in a query. Selecting an item
                 # in this widget needs to update the Tag Picker's
                 # current image tags, and activating an item
                 # needs to open the imageviewer.
                 'previewgallery.cc',

                 # This widget allows for the filtering of tags. It has
                 # a GtkEntry with completion and different sections for
                 # tags in relation to the query, such as included,
                 # excluded or the tags of the currently selected image.
                 'tagpicker.cc',

                 # This object is responsible for the implementation
                 # of a tag as a widget that can be added to one of the
                 # sections in the Tag Picker. Tags are derived from
                 # GtkBox and typically contain a label with the tag's
                 # name and buttons for actions such as add or exclude.
                 # It also contains implementation of a dynamic list
                 # of tags as widgets and a base class for tag pickers
                 # to build on
                 'tagutils.cc',

                 # The database in the backend. It can load from a file
                 # as well as write to a file and it is responsible for
                 # actually fulfilling the queries based on tags set
                 # buy the user in the Tag Picker widget.
                 'tagdb.cc',

                 # Window for adding images to the database.
                 'itemwindow.cc',

                 # Window for managing database options, such as
                 # registered directories and tags excluded by default
                 'dbsettingswindow.cc'
                 )
