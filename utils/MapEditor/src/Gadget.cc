#include <iostream.h>
#include "Gadget.h"


int Gadget::last_id_used = 0;

Gadget::~Gadget()
{
  /*
    if( variables != NULL )
    delete [] variables;
    
    if( functions != NULL )
    delete [] functions;
  */
}

void Gadget::Print()
{
  cout<<"Here is an object\n";
}


GtkCTreeNode* AddToTree(GtkWidget* tree, GtkCTreeNode* parent, Gadget* G)
{
  GtkCTreeNode* Root = gtk_ctree_insert_node(GTK_CTREE(tree), parent, NULL, 
					     NULL, 5, NULL, NULL, NULL, 
					     NULL, FALSE, FALSE);
  gtk_ctree_node_set_text(GTK_CTREE(tree), Root, 0, G->name.c_str());
  gtk_ctree_node_set_row_data(GTK_CTREE(tree), Root, G);
  return Root;
}
