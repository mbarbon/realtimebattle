#include <string>
#include <iostream.h>
#include "Gadget.h"
#include "Arena.h"
#include "MainWindow.h"

GtkWidget* Status;
GtkWidget* GadgetTree;
Arena* myArena;


void hello( GtkWidget *widget,
	    gpointer   data )
{
  g_print ("Hello World - by %s\n", (char*) data);
}


void message(char* mes)
{
  static guint statusbar_id = 0;
  gint context_id;

  if(statusbar_id != 0)
    gtk_statusbar_pop(GTK_STATUSBAR (Status), statusbar_id);
  context_id = gtk_statusbar_get_context_id(GTK_STATUSBAR(Status), 
					    "Statusbar example");
  statusbar_id = gtk_statusbar_push(GTK_STATUSBAR (Status), context_id, mes);
}


gint delete_event( GtkWidget *widget,
		   GdkEvent  *event,
		   gpointer   data )
{
  g_print ("delete event occurred\n");
  return(TRUE);
}

void Destroy( GtkWidget *widget,
	      gpointer   data )
{
  gtk_main_quit();
}

GtkWidget* init_main_window()
{
  GtkWidget* button;
  
  GtkWidget* window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW (window), 
		       "RealTimeBattle Map Editor - A New Hope");
  gtk_signal_connect (GTK_OBJECT (window), "delete_event",
		      GTK_SIGNAL_FUNC (delete_event), NULL);
  gtk_signal_connect (GTK_OBJECT (window), "destroy",
		      GTK_SIGNAL_FUNC (Destroy), NULL);
  gtk_widget_set_usize(GTK_WIDGET (window), 300,400);
                                               
  GtkWidget* vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_add( GTK_CONTAINER (window), vbox);

  GtkWidget* Menu = init_menu_bar();

  gtk_widget_set_usize(GTK_WIDGET (Menu), 300, 25);
  gtk_box_pack_start(GTK_BOX(vbox), Menu, FALSE, TRUE, 0);
 
 gtk_widget_show (Menu);

  button = gtk_button_new_with_label ("Bar d'outils");
  gtk_widget_set_usize(GTK_WIDGET (button), 300, 25);
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (hello), (gpointer) "bouton 2" );
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (Destroy), NULL);
  gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, TRUE, 0);
  gtk_widget_show (button);


  GtkWidget* Vue = gtk_hbox_new(FALSE, 0);
  //gtk_widget_set_usize(GTK_WIDGET(Vue), 300, 340);


  char *titles[] = { "Gadget" };

  GtkWidget *ctree;
  ctree = gtk_ctree_new_with_titles(1, 0, titles);

  myArena->Read("Original.arena", ctree);

  /*
    GtkCTreeNode* Node1 = AddToTree(ctree, NULL, new Gadget("Benoit"));
    AddToTree(ctree, Node1, new Gadget("Clement"));
    AddToTree(ctree, Node1, new Gadget("Cecile"));
    Node1 = AddToTree(ctree, NULL, new Gadget("JeanMarie"));
    Node1 = AddToTree(ctree, Node1, new Gadget("Corinne"));
    Node1 = AddToTree(ctree, Node1, new Gadget("Magalie"));
    AddToTree(ctree, Node1, new Gadget("Carolineeeee"));
  */

  /*GtkCTreeNode* Root = gtk_ctree_insert_node(GTK_CTREE(ctree), NULL, NULL, Feuille1,
			5, NULL, NULL, NULL, NULL, FALSE, FALSE);
  
    gtk_ctree_insert_node(GTK_CTREE(ctree), Root, NULL, Feuille2,
			5, NULL, NULL, NULL, NULL, FALSE, FALSE);
  */
  gtk_box_pack_start(GTK_BOX(Vue), ctree, TRUE, TRUE, 0);
  gtk_widget_show (ctree);

    button = gtk_button_new_with_label ("Espace de travail");
    gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (hello), (gpointer) "bouton 2" );
    gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (Destroy), NULL);
    gtk_box_pack_start(GTK_BOX(Vue), button, TRUE, TRUE, 0);
    gtk_widget_show (button);

  gtk_box_pack_start(GTK_BOX(vbox), Vue, TRUE, TRUE, 0);
  gtk_widget_show(Vue);

  Status = gtk_statusbar_new();
  gtk_widget_set_usize(GTK_WIDGET (Status), 300, 20);

  gtk_box_pack_start(GTK_BOX(vbox), Status, FALSE, TRUE, 0);
  gtk_widget_show (Status);

  gtk_widget_show (vbox);

  return window;
}
  
GtkWidget* init_menu_bar()
{
  GtkWidget* MenuBar = gtk_menu_bar_new();

  GtkWidget* OneMenu = gtk_menu_new();

  GtkWidget* MenuItem = gtk_menu_item_new_with_label("Open");
  gtk_signal_connect (GTK_OBJECT (MenuItem), "activate",
		      GTK_SIGNAL_FUNC (hello),
		      (gpointer) "file.open");
  gtk_menu_append( GTK_MENU(OneMenu), MenuItem);
  gtk_widget_show (MenuItem);

  MenuItem = gtk_menu_item_new_with_label("Save");
  gtk_signal_connect (GTK_OBJECT (MenuItem), "activate",
		      GTK_SIGNAL_FUNC (hello),
		      (gpointer) "file.open");
  gtk_menu_append( GTK_MENU(OneMenu), MenuItem);
  gtk_widget_show (MenuItem);
  
  MenuItem = gtk_menu_item_new_with_label("Close");
  gtk_signal_connect (GTK_OBJECT (MenuItem), "activate",
		      GTK_SIGNAL_FUNC (hello),
		      (gpointer) "file.open");
  gtk_menu_append( GTK_MENU(OneMenu), MenuItem);
  gtk_widget_show (MenuItem);

  MenuItem = gtk_menu_item_new_with_label("Quit");
  gtk_signal_connect (GTK_OBJECT (MenuItem), "activate",
		      GTK_SIGNAL_FUNC (Destroy), NULL);
  gtk_menu_append( GTK_MENU(OneMenu), MenuItem);
  gtk_widget_show (MenuItem);

  GtkWidget* file_item = gtk_menu_item_new_with_label ("File");
  gtk_widget_show (file_item);

  gtk_menu_item_set_submenu (GTK_MENU_ITEM (file_item), OneMenu);
  gtk_menu_bar_append (GTK_MENU_BAR (MenuBar), file_item);

  return MenuBar;
}
