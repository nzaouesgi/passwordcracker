#include "interface.h"
#include "../logs/logs.h"
#include "../export/export.h"

void startApp(struct MainWindow * windowHandler){

  /* Cette fonction a pour but de transformer la fenêtre principale
  pour y ajouter tous les widgets du menu principal et mettre en place
  les listeners de signaux. */

  // configuration des variables de contexte.

  windowHandler->optMethodBrute = 0;
  windowHandler->optMethodDic = 1; // la méthode dictionnaire est la méthode sélectionnée par défaut.
  windowHandler->optMethodBruteUppercase = 0;
  windowHandler->optMethodBruteNumbers = 0;
  windowHandler->optMethodBruteSpecialChars = 0;

  windowHandler->filename = NULL;
  windowHandler->stopped = 1;

  windowHandler->crCount = 0;
  windowHandler->neededThreads = 0;
  windowHandler->threadsLaunched = 0;

  // configuration de la GtkWindow.
  gtk_window_set_title(GTK_WINDOW(windowHandler->window), "Password Cracker v1.0");
  gtk_widget_set_size_request(windowHandler->window, 1000 , 600);

  // on crée un nouveau conteneur de type grid. Plus utile pour organiser notre fenêtre principale.

  windowHandler->grid=gtk_grid_new();
  gtk_container_add(GTK_CONTAINER(windowHandler->window), windowHandler->grid);

  // options de la grid.

  gtk_orientable_set_orientation(GTK_ORIENTABLE(windowHandler->grid), GTK_ORIENTATION_VERTICAL);
  gtk_grid_set_column_homogeneous(GTK_GRID(windowHandler->grid),TRUE);
  gtk_grid_set_column_spacing(GTK_GRID(windowHandler->grid),20);

  // Label des options de target.

  gchar * textTargetLabel= "<big><b>Target options</b></big>";
  windowHandler->targetLabel= gtk_label_new (g_locale_to_utf8(textTargetLabel,-1,NULL,NULL,NULL));
  gtk_label_set_justify(GTK_LABEL(windowHandler->targetLabel),GTK_JUSTIFY_CENTER);
  gtk_label_set_use_markup(GTK_LABEL(windowHandler->targetLabel),TRUE);

  // Label de la liste des protocoles.

  gchar * textProtoLabel= "Protocol:";
  windowHandler->protoLabel= gtk_label_new (g_locale_to_utf8(textProtoLabel,-1,NULL,NULL,NULL));
  gtk_label_set_justify(GTK_LABEL(windowHandler->protoLabel),GTK_JUSTIFY_LEFT);
  gtk_label_set_use_markup(GTK_LABEL(windowHandler->protoLabel),TRUE);

  // Liste des protocoles.
  windowHandler->protoList = gtk_combo_box_text_new();
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(windowHandler->protoList),"http","HTTP");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(windowHandler->protoList),"https","HTTPS");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(windowHandler->protoList),"ftp","FTP");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(windowHandler->protoList),"ftps (implicit ssl/tls)","FTPS (Implicit SSL/TLS)");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(windowHandler->protoList),"ftp (explicit ssl/tls)","FTP (Explicit SSL/TLS)");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(windowHandler->protoList),"smtp","SMTP");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(windowHandler->protoList),"smtps (ssl/tls)","SMTPS (SSL/TLS)");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(windowHandler->protoList),"smtp (starttls)","SMTP (STARTTLS)");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(windowHandler->protoList),"pop3","POP3");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(windowHandler->protoList),"pop3s (ssl/tls)","POP3S (SSL/TLS)");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(windowHandler->protoList),"pop3 (starttls)","POP3 (STARTTLS)");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(windowHandler->protoList),"imap","IMAP");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(windowHandler->protoList),"imaps (ssl/tls)","IMAPS (SSL/TLS)");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(windowHandler->protoList),"imap (starttls)","IMAP (STARTTLS)");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(windowHandler->protoList),"ssh","SSH");

  // Label de l'hôte.
  gchar * textHostLabel= "Host:";
  windowHandler->hostLabel= gtk_label_new (g_locale_to_utf8(textHostLabel,-1,NULL,NULL,NULL));
  gtk_label_set_justify(GTK_LABEL(windowHandler->hostLabel),GTK_JUSTIFY_LEFT);
  gtk_label_set_use_markup(GTK_LABEL(windowHandler->hostLabel),TRUE);

  // Input de l'hôte.
  windowHandler->hostInput= gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(windowHandler->hostInput), 255);
  gtk_entry_set_placeholder_text(GTK_ENTRY(windowHandler->hostInput), "eg: user 192.168.1.11, user hostname.com...");

  // Label du port.
  gchar * textPortLabel= "Port:";
  windowHandler->portLabel= gtk_label_new (g_locale_to_utf8(textPortLabel,-1,NULL,NULL,NULL));
  gtk_label_set_justify(GTK_LABEL(windowHandler->portLabel),GTK_JUSTIFY_LEFT);
  gtk_label_set_use_markup(GTK_LABEL(windowHandler->portLabel),TRUE);

  // Input du port.
  windowHandler->portInput= gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(windowHandler->portInput), 40);
  gtk_entry_set_placeholder_text(GTK_ENTRY(windowHandler->portInput), "1-65535");

  // Label des threads.
  gchar * textThreadsLabel= "Threads:";
  windowHandler->threadsLabel= gtk_label_new (g_locale_to_utf8(textThreadsLabel,-1,NULL,NULL,NULL));
  gtk_label_set_justify(GTK_LABEL(windowHandler->threadsLabel),GTK_JUSTIFY_LEFT);
  gtk_label_set_use_markup(GTK_LABEL(windowHandler->threadsLabel),TRUE);

  // scale des threads.

  windowHandler->threadsScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,1,MAX_THREADS,1);
  gtk_scale_set_draw_value(GTK_SCALE(windowHandler->threadsScale),TRUE);

  // Label de la partie méthode.

  gchar * textMethodLabel= "<big><b>Method</b></big>";
  windowHandler->methodLabel= gtk_label_new (g_locale_to_utf8(textMethodLabel,-1,NULL,NULL,NULL));
  gtk_label_set_justify(GTK_LABEL(windowHandler->methodLabel),GTK_JUSTIFY_LEFT);
  gtk_label_set_use_markup(GTK_LABEL(windowHandler->methodLabel),TRUE);

  // Bouton de type radio pour la méthode dictionnaire.

  windowHandler->dicRadio = gtk_radio_button_new_with_label(NULL, "Dictionnary attack");

  // Bouton pour le file chooser.

  windowHandler->fileButton = gtk_button_new_with_label("Choose a file");

  // Label pour le fichier.

  gchar * textFileLabel= "<i>No file selected.</i>";
  windowHandler->fileInputLabel= gtk_label_new (g_locale_to_utf8(textFileLabel,-1,NULL,NULL,NULL));
  gtk_label_set_justify(GTK_LABEL(windowHandler->fileInputLabel),GTK_JUSTIFY_LEFT);
  gtk_label_set_use_markup(GTK_LABEL(windowHandler->fileInputLabel),TRUE);

  // Bouton de type radio pour la méthode bruteforce.

  windowHandler->bruteRadio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(windowHandler->dicRadio), "Bruteforce attack");

  // Création de l'input de lengthstart.

  windowHandler->bruteLengthStart = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(windowHandler->bruteLengthStart), 2);
  gtk_entry_set_placeholder_text(GTK_ENTRY(windowHandler->bruteLengthStart), "MinLength");

  // Création de l'input de lengthend.

  windowHandler->bruteLengthEnd = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(windowHandler->bruteLengthEnd), 2);
  gtk_entry_set_placeholder_text(GTK_ENTRY(windowHandler->bruteLengthEnd), "MaxLength");

  // Création du checkbutton "Add Uppercase".

  windowHandler->bruteUpperCase = gtk_check_button_new_with_label("Add uppercase");

  // Création du checkbutton "Add Numbers".

  windowHandler->bruteNumbers = gtk_check_button_new_with_label("Add numbers");

  // Création du checkbutton "Add Special Characters".

  windowHandler->bruteSpecialChars = gtk_check_button_new_with_label("Add special characters");

  // Création de la fenêtre de console (scroll window widget).

  windowHandler->consoleWindow = gtk_scrolled_window_new(NULL,NULL);

  // Création de la console (textview widget).
  windowHandler->consoleBuffer = gtk_text_buffer_new(NULL);
  windowHandler->console = gtk_text_view_new_with_buffer(windowHandler->consoleBuffer);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(windowHandler->console),FALSE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(windowHandler->console),FALSE);
  //gtk_widget_set_sensitive(windowHandler->console,FALSE);

  // Définition des tags de style.
  gtk_text_buffer_create_tag(windowHandler->consoleBuffer,"strong_console","weight",PANGO_WEIGHT_BOLD,
  "weight-set",TRUE,NULL);
  GdkRGBA red;
  gdk_rgba_parse(&red,"rgb(200,25,50)");
  gtk_text_buffer_create_tag(windowHandler->consoleBuffer,"error_console","weight",PANGO_WEIGHT_BOLD,
  "weight-set",TRUE,"foreground-rgba",&red,"foreground-set",TRUE,NULL);

  GdkRGBA green;
  gdk_rgba_parse(&green,"rgb(25,150,50)");
  gtk_text_buffer_create_tag(windowHandler->consoleBuffer,"success_console","weight",PANGO_WEIGHT_BOLD,
  "weight-set",TRUE,"foreground-rgba",&green,"foreground-set",TRUE,NULL);


  // Création Bouton start.
  windowHandler->startButton = gtk_button_new_with_label("Start");
  // Création Bouton stop.
  windowHandler->stopButton = gtk_button_new_with_label("Stop");
  // Création Bouton export.
  windowHandler->exportButton = gtk_button_new_with_label("Export logs");
  // Création Bouton start.
  windowHandler->logsButton = gtk_button_new_with_label("View logs");

  // ajout des target options dans la première colonne (partie target).

  gtk_grid_attach(GTK_GRID(windowHandler->grid),windowHandler->targetLabel,   0,0,2,1);
  gtk_grid_attach(GTK_GRID(windowHandler->grid),windowHandler->protoLabel,    0,1,2,1);
  gtk_grid_attach(GTK_GRID(windowHandler->grid),windowHandler->protoList,     0,2,2,1);
  gtk_grid_attach(GTK_GRID(windowHandler->grid),windowHandler->hostLabel,     0,3,2,1);
  gtk_grid_attach(GTK_GRID(windowHandler->grid),windowHandler->hostInput,     0,4,2,1);
  gtk_grid_attach(GTK_GRID(windowHandler->grid),windowHandler->portLabel,     0,5,2,1);
  gtk_grid_attach(GTK_GRID(windowHandler->grid),windowHandler->portInput,     0,6,2,1);
  gtk_grid_attach(GTK_GRID(windowHandler->grid),windowHandler->threadsLabel,  0,7,2,1);
  gtk_grid_attach(GTK_GRID(windowHandler->grid),windowHandler->threadsScale,  0,8,2,1);

  //Ajout des method options dans la deuxième colonne (partie methode).

  gtk_grid_attach(GTK_GRID(windowHandler->grid),windowHandler->methodLabel,      2,0,2,1);
  gtk_grid_attach(GTK_GRID(windowHandler->grid),windowHandler->dicRadio,         2,1,2,1);
  gtk_grid_attach(GTK_GRID(windowHandler->grid),windowHandler->fileButton,       2,2,2,1);
  gtk_grid_attach(GTK_GRID(windowHandler->grid),windowHandler->fileInputLabel,   2,3,2,1);
  gtk_grid_attach(GTK_GRID(windowHandler->grid),windowHandler->bruteRadio,       2,5,2,1);
  gtk_grid_attach(GTK_GRID(windowHandler->grid),windowHandler->bruteLengthStart, 2,6,1,1);
  gtk_grid_attach(GTK_GRID(windowHandler->grid),windowHandler->bruteLengthEnd,   3,6,1,1);
  gtk_grid_attach(GTK_GRID(windowHandler->grid),windowHandler->bruteUpperCase,   2,7,1,1);
  gtk_grid_attach(GTK_GRID(windowHandler->grid),windowHandler->bruteNumbers,     3,7,1,1);
  gtk_grid_attach(GTK_GRID(windowHandler->grid),windowHandler->bruteSpecialChars,2,8,1,1);

  // Ajout de la console

  gtk_container_add(GTK_CONTAINER(windowHandler->consoleWindow),windowHandler->console);
  gtk_grid_attach(GTK_GRID(windowHandler->grid),windowHandler->consoleWindow,0,9,4,1);
  gtk_widget_set_margin_top(windowHandler->consoleWindow, 20);
  gtk_widget_set_size_request(windowHandler->consoleWindow, 150, 250);

  // Ajout des boutons de contrôle

  gtk_grid_attach(GTK_GRID(windowHandler->grid),windowHandler->startButton,  0,11,1,1);
  gtk_grid_attach(GTK_GRID(windowHandler->grid),windowHandler->stopButton,   1,11,1,1);
  gtk_grid_attach(GTK_GRID(windowHandler->grid),windowHandler->exportButton, 2,11,1,1);
  gtk_grid_attach(GTK_GRID(windowHandler->grid),windowHandler->logsButton,   3,11,1,1);

  // ajustements des marges.

  gtk_widget_set_margin_top(windowHandler->startButton, 10);
  gtk_widget_set_margin_top(windowHandler->stopButton,  10);
  gtk_widget_set_margin_top(windowHandler->exportButton,10);
  gtk_widget_set_margin_top(windowHandler->logsButton,  10);

  gtk_widget_set_margin_top(windowHandler->bruteUpperCase,  10);
  gtk_widget_set_margin_top(windowHandler->bruteNumbers,10);
  gtk_widget_set_margin_top(windowHandler->bruteSpecialChars,  10);

  // désactivation par défaut du bouton stop.

  gtk_widget_set_sensitive(windowHandler->stopButton,FALSE);

  /* on désactive la partie bruteforce, vu que la partie dictionnaire
  est sélectionnée par défaut.*/

  gtk_widget_set_sensitive(windowHandler->bruteLengthStart, FALSE);
  gtk_widget_set_sensitive(windowHandler->bruteLengthEnd,   FALSE);
  gtk_widget_set_sensitive(windowHandler->bruteUpperCase,   FALSE);
  gtk_widget_set_sensitive(windowHandler->bruteNumbers,     FALSE);
  gtk_widget_set_sensitive(windowHandler->bruteSpecialChars,FALSE);

  // apparition des widgets.

  gtk_widget_show_all(windowHandler->window);

  // connexion des signaux.

  g_signal_connect(G_OBJECT(windowHandler->fileButton), "clicked",
  G_CALLBACK(openFile),windowHandler);
  g_signal_connect(G_OBJECT(windowHandler->bruteRadio), "toggled",
  G_CALLBACK(turnOffDicPart),windowHandler);
  g_signal_connect(G_OBJECT(windowHandler->dicRadio), "toggled",
  G_CALLBACK(turnOffBrutePart),windowHandler);
  g_signal_connect(G_OBJECT(windowHandler->bruteUpperCase), "clicked",
  G_CALLBACK(setUpperCase),windowHandler);
  g_signal_connect(G_OBJECT(windowHandler->bruteNumbers), "clicked",
  G_CALLBACK(setNumbers),windowHandler);
  g_signal_connect(G_OBJECT(windowHandler->bruteSpecialChars), "clicked",
  G_CALLBACK(setSpecialChars),windowHandler);
  g_signal_connect(G_OBJECT(windowHandler->startButton), "clicked",
  G_CALLBACK(disableButtons),windowHandler);
  g_signal_connect(G_OBJECT(windowHandler->stopButton), "clicked",
  G_CALLBACK(enableButtons),windowHandler);
  g_signal_connect(G_OBJECT(windowHandler->consoleBuffer), "changed",
  G_CALLBACK(scrollDown),windowHandler);
  g_signal_connect(G_OBJECT(windowHandler->exportButton), "clicked",
  G_CALLBACK(exportLogs),windowHandler);
  g_signal_connect(G_OBJECT(windowHandler->logsButton), "clicked",
  G_CALLBACK(viewLogs),windowHandler);

  gchar * text = "Welcome to Password Cracker v1.0.\n";
  writeConsoleWithTag(windowHandler,text,"strong_console");
  gchar * text2 = "This application is a security tool, don't use it for illegal purposes.\n";
  writeConsole(windowHandler,text2);

}
