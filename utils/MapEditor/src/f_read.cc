#include <stdio.h>
#include <string.h>
#include <iostream.h>
#include "Arena.h"
#include "Gadget.h"
#include "f_read.hh"

#define	BUF_SIZE	1024
char		 buf[BUF_SIZE];		// input buffer
char            type[BUF_SIZE];

int line_no = 0;

void Arena::Read(char * Name)
{
  FILE *fp;
  if((fp = fopen(Name, "r")) != NULL)
    {
      cout<<"Parsing the file...\n";
      ReadHead(fp);
      Read(fp);
      cout<<"Parsing done!\n";
      Write();
      fclose(fp);
    }
  else
    {
      cout<<"Problem with the file...\n";
    }
}

int Arena::Read(FILE *fp)
{
  Gadget *O;
  char GadgetName[50];
  char Value[50];
  int num_object = 0;

  while (read_line(fp) > 0) 
    {
    
      printf("%s", buf);
      if(strncmp(buf, "Define ", 7))
	{
	  printf("Invalid format at line %d.\n", line_no);
	}
      else if (sscanf(buf, "%*s %s %s", Value, GadgetName) != 2) 
	{
	  printf("Incorrect definition at line %d.\n", line_no);
	  return (num_object != 0? 0: -1);     // ok if any objects have been read 
	}
      
      if(!strcmp(Value, "Wall"))    //Maybe we shoud find it in a table
	{
	  O = new Wall(GadgetName);
	  O->Next = TheGadgets;
	  TheGadgets = O;
	  TheGadgets->Read(fp);
	  printf("Add a Wall to the list\n");
	}
      else
	{
	  printf("Unknown object... skip it\n");
	  int NbDefine = 1;
	  while(read_line(fp)>0)
	    {
	      sscanf(buf, "%s", type);
	      if(!strcmp(type, "EndDefine"))  //This is the end of a definition
		{
		  NbDefine--;
		}
	      if(!strcmp(type, "Define"))     //There could be an other definition in it...
		{
		  NbDefine++;
		}

	      if(NbDefine < 1)                       //This is the end of the unknown object
		break;
	    }
	  num_object ++;
	}
    }
  printf("There were %d objects in this file...\n", num_object);
}

int Arena::ReadHead(FILE *fp)
{
  if(read_line(fp) < 0)
    {
      printf("Erreur... Empty File\n");
      return -1;
    }
  else
    {
      if(strncmp(buf, "! Arenafile version 2 !", 23))
	{
	  printf("Version declaration should be the first line of the Arena\n");
	  return -1;
	}
    }
}

int Wall::Read(FILE *fp)
{
  int n;
  int NbDefine = 0;
  while(read_line(fp)>0)
    {
      if((sscanf(buf, "%s", type)) == 1)        //Skip empty line
	{
	  if(!strcmp(type, "Size"))
	    {
	      n = sscanf(buf, "%*s %f", &mySize);
	      if(n != 1)
		{
		  printf("%d\n", n);
		  printf("There is a little problem : no size specified in line %d.\n", line_no); 
		}
	    }
	  
	  //Skip a couple of things...
	  if(!strcmp(type, "Define"))     //There could be an other definition in it...
	    NbDefine++;
	  if(!strcmp(type, "EndDefine"))  //This is the end of a definition
	    NbDefine--;              
	  
	  if(NbDefine < 0)                       //This is the end of the unknown object
	    break;
	}
    }
}


int read_line(FILE *fp)
{
  while (1) 
    {
      if (NULL == fgets(buf, BUF_SIZE, fp)) {
	return (-1);
      }
      line_no++;
      if (*buf != ';' && *buf != '\n')          // Skip comments for the moment
	{	
	  return (1);
	}
    }
}

void skip_line(FILE *fp)
{
    char c;
    while ((c=fgetc(fp)) != '\n') {
	if (feof(fp))
	    return;
    }
}







