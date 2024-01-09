#include <gtk/gtk.h>
#include <string.h>
#include <netinet/in.h> //structure for storing address information 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h> //for socket APIs 
#include <sys/types.h> 

static GtkWidget *combobox;
static GtkWidget *entryime;
static GtkWidget *entryprezime;
static GtkWidget *entryadresa;
static GtkWidget *entryjmbg;
static GtkWidget *entrynaziv;
static GtkWidget *entrypisac;
static GtkWidget *entryISBN;
static GtkWidget *entryclanskibroj;
static GtkWidget *kalendar;
static GtkWidget *labelresult;

static int je_broj(gchar *str) {
  for (int i = 0; i < strlen(str); i++) {
    if (str[i] < '0' || str[i] > '9') {
      return 0;
    }
  }
  return 1;
}

static void
add_user (GtkWidget *widget,
             gpointer   data)
{
  g_print ("TODO: make new user\n");
  
  const gchar *entry_text1;
  const gchar *entry_text2;
  const gchar *entry_text3;
  const gchar *entry_text4;
  const gchar *combo_text;
  
  entry_text1 = gtk_entry_get_text (GTK_ENTRY (entryime));
  entry_text2 = gtk_entry_get_text (GTK_ENTRY (entryprezime));
  entry_text3 = gtk_entry_get_text (GTK_ENTRY (entryadresa));
  entry_text4 = gtk_entry_get_text (GTK_ENTRY (entryjmbg));
  combo_text = gtk_combo_box_text_get_active_text (combobox);
  
  g_print("IME: %s, PREZIME: %s, ADRESA: %s, JMBG: %s, GRAD: %s\n"
  	,entry_text1,entry_text2,entry_text3,entry_text4,combo_text);
  
  int sockD = socket(AF_INET, SOCK_STREAM, 0); 
  
  struct sockaddr_in servAddr; \
  servAddr.sin_family = AF_INET; 
  if (strcmp(combo_text,"Novi Sad")==0) {
  	servAddr.sin_port = htons(9001);
  } else if (strcmp(combo_text,"Beograd")==0) {
  	servAddr.sin_port = htons(9002);
  } else if (strcmp(combo_text,"Nis")==0) {
  	servAddr.sin_port = htons(9003);
  } else {
  	g_print("CITY NOT PICKED\n");
  	gtk_label_set_text(GTK_LABEL (labelresult), "Pick a city!");
  	return;
  }
  servAddr.sin_addr.s_addr = INADDR_ANY; 
  int connectStatus  = connect(sockD, (struct sockaddr*)&servAddr, sizeof(servAddr));\
  if (connectStatus == -1) { 
      g_print("Error...\n"); 
  } 
  
  else { 
      char req[10] = {'\0'};
      char response[10] = {'\0'};
      char ime[25] = {'\0'}; 
      char prezime[25] = {'\0'};
      char adresa[35] = {'\0'};
      char jmbg[15] = {'\0'};
      strcpy(req,"USER");
      strcat(ime,entry_text1);
      strcat(prezime,entry_text2);
      strcat(adresa,entry_text3);
      strcat(jmbg,entry_text4);
  
      send(sockD, req, sizeof(req), 0);
      send(sockD, ime, sizeof(ime), 0);
      send(sockD, prezime, sizeof(prezime), 0);
      send(sockD, adresa, sizeof(adresa), 0);
      send(sockD, jmbg, sizeof(jmbg), 0);
      recv(sockD, response, sizeof(response), 0);
      
      g_print("Response: %s\n",response);
      if (response[0] == 'Y') {
      	    response[0] = ' ';
      	    char message[20];
      	    strcpy(message, "New user id:");
      	    strcat(message,response);
      	    gtk_label_set_text(GTK_LABEL (labelresult), message);
      } else if (response[0] == 'N'){
      	    gtk_label_set_text(GTK_LABEL (labelresult), "User already exists!");
      } else {
      	    gtk_label_set_text(GTK_LABEL (labelresult), "ERROR!");
      }
  } 
  close(sockD);
}

static void
add_book (GtkWidget *widget,
             gpointer   data)
{
  g_print ("TODO: add book to user\n");
  
  const gchar *entry_text1;
  const gchar *entry_text2;
  const gchar *entry_text3;
  const gchar *entry_text4;
  const gchar *combo_text;
  guint year, month, day;
  
  entry_text1 = gtk_entry_get_text (GTK_ENTRY (entrynaziv));
  entry_text2 = gtk_entry_get_text (GTK_ENTRY (entrypisac));
  entry_text3 = gtk_entry_get_text (GTK_ENTRY (entryISBN));
  entry_text4 = gtk_entry_get_text (GTK_ENTRY (entryclanskibroj));
  combo_text = gtk_combo_box_text_get_active_text (combobox);
  gtk_calendar_get_date(kalendar,&year,&month,&day);
  
  if (je_broj(entry_text3) && je_broj(entry_text4)) {
  	g_print("NAZIV: %s, PISAC: %s, ISBN: %s, ID: %s, GRAD: %s, DATUM: %02d.%02d.%d.\n"
  	,entry_text1,entry_text2,entry_text3,entry_text4,combo_text,day,month,year);
  } else {
  	g_print("ISBN I ID MORAJU BITI BROJEVI\n");
  }
  
  int sockD = socket(AF_INET, SOCK_STREAM, 0); 
  
  struct sockaddr_in servAddr; \
  servAddr.sin_family = AF_INET; 
  if (strcmp(combo_text,"Novi Sad")==0) {
  	servAddr.sin_port = htons(9001);
  } else if (strcmp(combo_text,"Beograd")==0) {
  	servAddr.sin_port = htons(9002);
  } else if (strcmp(combo_text,"Nis")==0) {
  	servAddr.sin_port = htons(9003);
  } else {
  	g_print("CITY NOT PICKED\n");
  	gtk_label_set_text(GTK_LABEL (labelresult), "Pick a city!");
  	return;
  }
  servAddr.sin_addr.s_addr = INADDR_ANY; 
  int connectStatus  = connect(sockD, (struct sockaddr*)&servAddr, sizeof(servAddr));\
  if (connectStatus == -1) { 
      g_print("Error...\n"); 
  } else { 
      char req[10] = {'\0'};
      char response[10] = {'\0'};
      char naziv[75] = {'\0'}; 
      char pisac[35] = {'\0'};
      char isbn[25] = {'\0'};
      char id[10] = {'\0'};
      char date[15] = {'\0'};
      strcpy(req,"BOOK");
      strcat(naziv,entry_text1);
      strcat(pisac,entry_text2);
      strcat(isbn,entry_text3);
      strcat(id,entry_text4);
      sprintf(date,"%d-%d-%d",year,month,day);
  
      send(sockD, req, sizeof(req), 0);
      send(sockD, naziv, sizeof(naziv), 0);
      send(sockD, pisac, sizeof(pisac), 0);
      send(sockD, isbn, sizeof(isbn), 0);
      send(sockD, id, sizeof(id), 0);
      send(sockD, date, sizeof(date), 0);
      
      recv(sockD, response, sizeof(response), 0);
      g_print("The response: %s\n",response);
      if (strcmp(response,"Y")==0) {
      	    gtk_label_set_text(GTK_LABEL (labelresult), "Added book!");
      } else if (strcmp(response,"N")==0){
      	    gtk_label_set_text(GTK_LABEL (labelresult), "Too many books!");
      } else {
      	    gtk_label_set_text(GTK_LABEL (labelresult), "ERROR!");
      }
  } 
  close(sockD);
}

static void
delete_everything (GtkWidget *widget,
             gpointer   data)
{
  int sockD = socket(AF_INET, SOCK_STREAM, 0); 
  
  struct sockaddr_in servAddr; \
  servAddr.sin_family = AF_INET; 
  servAddr.sin_port = htons(9001);
  servAddr.sin_addr.s_addr = INADDR_ANY; 
  int connectStatus  = connect(sockD, (struct sockaddr*)&servAddr, sizeof(servAddr));\
  if (connectStatus == -1) { 
      g_print("Error...\n"); 
  } else {
      g_print("Deleting Novi Sad\n");
      char req[10] = {'\0'};
      strcpy(req,"DEL");
      send(sockD, req, sizeof(req), 0);
  }
  close(sockD);
  
  sockD = socket(AF_INET, SOCK_STREAM, 0); 
  
  servAddr.sin_family = AF_INET; 
  servAddr.sin_port = htons(9002); // use some unused port number 
  servAddr.sin_addr.s_addr = INADDR_ANY; 
  connectStatus  = connect(sockD, (struct sockaddr*)&servAddr, sizeof(servAddr));\
  if (connectStatus == -1) { 
      g_print("Error...\n"); 
  } else {
      g_print("Deleting Beograd\n");
      char req[10] = {'\0'};
      strcpy(req,"DEL");
      send(sockD, req, sizeof(req), 0);
  }
  close(sockD);
  
  sockD = socket(AF_INET, SOCK_STREAM, 0); 
  
  servAddr.sin_family = AF_INET; 
  servAddr.sin_port = htons(9003); // use some unused port number 
  servAddr.sin_addr.s_addr = INADDR_ANY; 
  connectStatus  = connect(sockD, (struct sockaddr*)&servAddr, sizeof(servAddr));\
  if (connectStatus == -1) { 
      g_print("Error...\n"); 
  } else {
      g_print("Deleting Nis\n");
      char req[10] = {'\0'};
      strcpy(req,"DEL");
      send(sockD, req, sizeof(req), 0);
  }
  close(sockD);
  
  sockD = socket(AF_INET, SOCK_STREAM, 0); 
  
  servAddr.sin_family = AF_INET; 
  servAddr.sin_port = htons(9004); // use some unused port number 
  servAddr.sin_addr.s_addr = INADDR_ANY; 
  connectStatus  = connect(sockD, (struct sockaddr*)&servAddr, sizeof(servAddr));\
  if (connectStatus == -1) { 
      g_print("Error...\n"); 
  } else {
      g_print("Deleting Central\n");
      char req[10] = {'\0'};
      strcpy(req,"DEL");
      send(sockD, req, sizeof(req), 0);
  }
  close(sockD);
  gtk_label_set_text(GTK_LABEL (labelresult), "Deleted Everything!");
}

static void
return_book (GtkWidget *widget,
             gpointer   data)
{
  g_print ("TODO: return book from user\n");
  
  const gchar *entry_text1;
  const gchar *entry_text2;
  const gchar *entry_text3;
  const gchar *entry_text4;
  const gchar *combo_text;
  
  entry_text1 = gtk_entry_get_text (GTK_ENTRY (entrynaziv));
  entry_text2 = gtk_entry_get_text (GTK_ENTRY (entrypisac));
  entry_text3 = gtk_entry_get_text (GTK_ENTRY (entryISBN));
  entry_text4 = gtk_entry_get_text (GTK_ENTRY (entryclanskibroj));
  combo_text = gtk_combo_box_text_get_active_text (combobox);
  
  if (je_broj(entry_text3) && je_broj(entry_text4)) {
  	g_print("NAZIV: %s, PISAC: %s, ISBN: %s, ID: %s, GRAD: %s.\n"
  	,entry_text1,entry_text2,entry_text3,entry_text4,combo_text);
  } else {
  	g_print("ISBN I ID MORAJU BITI BROJEVI\n");
  	gtk_label_set_text(GTK_LABEL (labelresult), "ISBN AND ID MUST BE NUMBERS!");
  }
  
  int sockD = socket(AF_INET, SOCK_STREAM, 0); 
  
  struct sockaddr_in servAddr; \
  servAddr.sin_family = AF_INET; 
  if (strcmp(combo_text,"Novi Sad")==0) {
  	servAddr.sin_port = htons(9001);
  } else if (strcmp(combo_text,"Beograd")==0) {
  	servAddr.sin_port = htons(9002);
  } else if (strcmp(combo_text,"Nis")==0) {
  	servAddr.sin_port = htons(9003);
  } else {
  	g_print("CITY NOT PICKED\n");
  	gtk_label_set_text(GTK_LABEL (labelresult), "Pick a city!");
  	return;
  }
  servAddr.sin_addr.s_addr = INADDR_ANY; 
  int connectStatus  = connect(sockD, (struct sockaddr*)&servAddr, sizeof(servAddr));\
  if (connectStatus == -1) { 
      g_print("Error...\n"); 
  } else { 
      char req[10] = {'\0'};
      char response[10] = {'\0'};
      char naziv[75] = {'\0'}; 
      char pisac[35] = {'\0'};
      char isbn[25] = {'\0'};
      char id[10] = {'\0'};
      strcpy(req,"RET");
      strcat(naziv,entry_text1);
      strcat(pisac,entry_text2);
      strcat(isbn,entry_text3);
      strcat(id,entry_text4);
  
      send(sockD, req, sizeof(req), 0);
      send(sockD, naziv, sizeof(naziv), 0);
      send(sockD, pisac, sizeof(pisac), 0);
      send(sockD, isbn, sizeof(isbn), 0);
      send(sockD, id, sizeof(id), 0);
      
      recv(sockD, response, sizeof(response), 0);
      g_print("The response: %s\n",response);
      if (strcmp(response,"Y")==0) {
      	    gtk_label_set_text(GTK_LABEL (labelresult), "Returned book!");
      } else if (strcmp(response,"N")==0){
      	    gtk_label_set_text(GTK_LABEL (labelresult), "Couldnt find book!");
      } else {
      	    gtk_label_set_text(GTK_LABEL (labelresult), "ERROR!");
      }
  } 
  close(sockD);
}

static void
apps_off (GtkWidget *widget,
             gpointer   data)
{
  int sockD = socket(AF_INET, SOCK_STREAM, 0); 
  
  struct sockaddr_in servAddr; \
  servAddr.sin_family = AF_INET; 
  servAddr.sin_port = htons(9001);
  servAddr.sin_addr.s_addr = INADDR_ANY; 
  int connectStatus  = connect(sockD, (struct sockaddr*)&servAddr, sizeof(servAddr));\
  if (connectStatus == -1) { 
      g_print("Error...\n"); 
  } else {
      g_print("Turning off Novi Sad\n");
      char req[10] = {'\0'};
      strcpy(req,"EXIT");
      send(sockD, req, sizeof(req), 0);
  }
  close(sockD);
  
  sockD = socket(AF_INET, SOCK_STREAM, 0); 
  
  servAddr.sin_family = AF_INET; 
  servAddr.sin_port = htons(9002);
  servAddr.sin_addr.s_addr = INADDR_ANY; 
  connectStatus  = connect(sockD, (struct sockaddr*)&servAddr, sizeof(servAddr));\
  if (connectStatus == -1) { 
      g_print("Error...\n"); 
  } else {
      g_print("Turning off Beograd\n");
      char req[10] = {'\0'};
      strcpy(req,"EXIT");
      send(sockD, req, sizeof(req), 0);
  }
  close(sockD);
  
  sockD = socket(AF_INET, SOCK_STREAM, 0); 
  
  servAddr.sin_family = AF_INET; 
  servAddr.sin_port = htons(9003);
  servAddr.sin_addr.s_addr = INADDR_ANY; 
  connectStatus  = connect(sockD, (struct sockaddr*)&servAddr, sizeof(servAddr));\
  if (connectStatus == -1) { 
      g_print("Error...\n"); 
  } else {
      g_print("Turning off Nis\n");
      char req[10] = {'\0'};
      strcpy(req,"EXIT");
      send(sockD, req, sizeof(req), 0);
  }
  close(sockD);
  
  sockD = socket(AF_INET, SOCK_STREAM, 0); 
  
  servAddr.sin_family = AF_INET; 
  servAddr.sin_port = htons(9004);
  servAddr.sin_addr.s_addr = INADDR_ANY; 
  connectStatus  = connect(sockD, (struct sockaddr*)&servAddr, sizeof(servAddr));\
  if (connectStatus == -1) { 
      g_print("Error...\n"); 
  } else {
      g_print("Turning off Central\n");
      char req[10] = {'\0'};
      strcpy(req,"EXIT");
      send(sockD, req, sizeof(req), 0);
  }
  close(sockD);
}

static void
activate (GtkApplication* app,
          gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *grid;
  GtkWidget *labelgrad;
  GtkWidget *labelime;
  GtkWidget *labelprezime;
  GtkWidget *labeladresa;
  GtkWidget *labeljmbg;
  GtkWidget *labelnaziv;
  GtkWidget *labelpisac;
  GtkWidget *labelISBN;
  GtkWidget *labeldatum;
  GtkWidget *labelclanskibroj;
  GtkWidget *button1;
  GtkWidget *button2;
  GtkWidget *buttonquit;
  GtkWidget *buttondelete;
  GtkWidget *buttonreturn;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Admin Biblioteka");
  gtk_window_set_default_size (GTK_WINDOW (window), 500, 600);
  
  grid = gtk_grid_new();
  gtk_container_add (GTK_CONTAINER (window), grid);
  gtk_grid_set_row_spacing(grid, 7);
  gtk_grid_set_column_spacing(grid,7);

  labelgrad = gtk_label_new("Izabrana biblioteka:");
  labelime = gtk_label_new("Ime:");
  labelprezime = gtk_label_new("Prezime:");
  labeladresa = gtk_label_new("Adresa:");
  labeljmbg = gtk_label_new("Jmbg:");
  gtk_grid_attach(GTK_GRID (grid), labelgrad, 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID (grid), labelime, 0, 2, 1, 1);
  gtk_grid_attach(GTK_GRID (grid), labelprezime, 0, 3, 1, 1);
  gtk_grid_attach(GTK_GRID (grid), labeladresa, 0, 4, 1, 1);
  gtk_grid_attach(GTK_GRID (grid), labeljmbg, 0, 5, 1, 1);


  combobox = gtk_combo_box_text_new();
  gtk_combo_box_text_append(combobox, NULL, "Beograd");
  gtk_combo_box_text_append(combobox, NULL, "Novi Sad");
  gtk_combo_box_text_append(combobox, NULL, "Nis");
  gtk_grid_attach(GTK_GRID (grid), combobox, 1, 0, 1, 1);

  entryime = gtk_entry_new();
  entryprezime = gtk_entry_new();
  entryadresa = gtk_entry_new();
  entryjmbg = gtk_entry_new();
  gtk_grid_attach(GTK_GRID (grid), entryime, 1, 2, 1, 1);
  gtk_grid_attach(GTK_GRID (grid), entryprezime, 1, 3, 1, 1);
  gtk_grid_attach(GTK_GRID (grid), entryadresa, 1, 4, 1, 1);
  gtk_grid_attach(GTK_GRID (grid), entryjmbg, 1, 5, 1, 1);

  labelnaziv = gtk_label_new("Naziv knjige:");
  labelpisac = gtk_label_new("Pisac:");
  labelISBN = gtk_label_new("ISBN:");
  labeldatum = gtk_label_new("Datum izdavanja:");
  labelclanskibroj = gtk_label_new("Clanski broj:");
  gtk_grid_attach(GTK_GRID (grid), labelnaziv, 2, 2, 1, 1);
  gtk_grid_attach(GTK_GRID (grid), labelpisac, 2, 3, 1, 1);
  gtk_grid_attach(GTK_GRID (grid), labelISBN, 2, 4, 1, 1);
  gtk_grid_attach(GTK_GRID (grid), labeldatum, 2, 6, 1, 1);
  gtk_grid_attach(GTK_GRID (grid), labelclanskibroj, 2, 5, 1, 1);

  entrynaziv = gtk_entry_new();
  entrypisac = gtk_entry_new();
  entryISBN = gtk_entry_new();
  entryclanskibroj = gtk_entry_new();
  kalendar = gtk_calendar_new();
  gtk_grid_attach(GTK_GRID (grid), entrynaziv, 3, 2, 1, 1);
  gtk_grid_attach(GTK_GRID (grid), entrypisac, 3, 3, 1, 1);
  gtk_grid_attach(GTK_GRID (grid), entryISBN, 3, 4, 1, 1);
  gtk_grid_attach(GTK_GRID (grid), kalendar, 3, 6, 1, 1);
  gtk_grid_attach(GTK_GRID (grid), entryclanskibroj, 3, 5, 1, 1);

  button1 = gtk_button_new_with_label ("Dodaj korisnika");
  g_signal_connect (button1, "clicked", G_CALLBACK (add_user), NULL);
  gtk_grid_attach(GTK_GRID (grid), button1, 1, 7, 1, 1);
  
  button2 = gtk_button_new_with_label ("Zaduzi knjigu");
  g_signal_connect (button2, "clicked", G_CALLBACK (add_book), NULL);
  gtk_grid_attach(GTK_GRID (grid), button2, 2, 7, 1, 1);
  
  labelresult = gtk_label_new("");
  gtk_grid_attach(GTK_GRID (grid), labelresult, 1, 8, 2, 1);
  
  buttondelete = gtk_button_new_with_label ("OBRISI SVE");
  g_signal_connect (buttondelete, "clicked", G_CALLBACK (delete_everything), NULL);
  gtk_grid_attach(GTK_GRID (grid), buttondelete, 1, 9, 1, 1);
  
  buttonreturn = gtk_button_new_with_label ("Vrati knjigu");
  g_signal_connect (buttonreturn, "clicked", G_CALLBACK (return_book), NULL);
  gtk_grid_attach(GTK_GRID (grid), buttonreturn, 2, 9, 1, 1);
  
  buttonquit = gtk_button_new_with_label ("Izadji");
  g_signal_connect (buttonquit, "clicked", G_CALLBACK (apps_off), NULL);
  g_signal_connect_swapped (buttonquit, "clicked", G_CALLBACK (gtk_widget_destroy), window);
  gtk_grid_attach(GTK_GRID (grid), buttonquit, 2, 10, 1, 1);
  
  gtk_widget_show_all (window);
}

int
main (int    argc,
      char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}

