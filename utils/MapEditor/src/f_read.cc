#include <stdio.h>
#include <string.h>
#include <iostream.h>
#include "Arena.h"
#include "Gadget.h"
#include "AllGadgets.h"
#include "f_read.hh"
#include "Variable.h"

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
  Gadget *Obj;
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

      if( Obj=createGadget(Value, GadgetName, NULL) /* != NULL */ )
	{
	  Obj->Read(fp);
	  TheGadgets.push_back(Obj);
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
	      if(!strcmp(type, "Define"))//There could be an other definition in it...
		{
		  NbDefine++;
		}

	      if(NbDefine < 1)     //This is the end of the unknown object
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

int WallGadget::Read(FILE *fp)
{
  int n;
  int NbDefine = 0;
  while(read_line(fp)>0)
    {
      if((sscanf(buf, "%s", type)) == 1)        //Skip empty line
	{
	  
	  if(!strcmp(type, "InfoString"))
	    {
	      info_string = read_info_string(fp);
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
int ExplosionGadget::Read(FILE *fp)
{
  cout<<"Reading a Explosion\n";
  int n;
  int NbDefine = 0;
  while(read_line(fp)>0)
    {
      if((sscanf(buf, "%s", type)) == 1)        //Skip empty line
	{
	  if(!strcmp(type, "InfoString"))
	    {
	      info_string = read_info_string(fp);
	    }
	  else if(!strcmp(type, "Define"))
	    {
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
int  WeaponGadget::Read(FILE *fp)
{
  cout<<"Reading a WeaponGadget\n";
  int n;
  int NbDefine = 0;
  while(read_line(fp)>0)
    {
      if((sscanf(buf, "%s", type)) == 1)        //Skip empty line
	{
	  
	  if(!strcmp(type, "InfoString"))
	    {
	      info_string = read_info_string(fp);
	    }
	  else if(!strcmp(type, "Define"))
	    {
	      
	    }
	  else
	    {
	      int i;
	      for(i = 0; i < LAST_WEAPONVAR; i++)
		{
		  if(!strcmp(type, variable_def[i].name))
		    {
		      break;
		    }
		}
	      if(i < LAST_WEAPONVAR)
		{
		  double THEValue;
		  sscanf(buf, "%*s %d", &THEValue);
		  cout<<variable_def[i].name<<" as the value of "<<THEValue<<endl;
		}
	      else
		{
		  cout<<"The variable is not a known variable\n";
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

int  ShotGadget::Read(FILE *fp)
{
  cout<<"Reading a shot gadget\n";
  int n;
  int NbDefine = 0;
  while(read_line(fp)>0)
    {
      if((sscanf(buf, "%s", type)) == 1)        //Skip empty line
	{
	  
	  if(!strcmp(type, "InfoString"))
	    {
	      info_string = read_info_string(fp);
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
      if (*buf != ';' && !blank_line())          
	// Skip comments for the moment
	{
	  return (1);
	}
    }
}

bool blank_line()
{
  for(int i = 0; i < BUF_SIZE; i++)
    {
      if(buf[i] == '\n' || buf[i] == '\0')
	{
	  return true;
	}
      if(buf[i] != ' ' && buf[i] != '\t') //Not a blank line when it find the first char...
	{
	  return false;
	}
    }
  return true;
}

void skip_line(FILE *fp)
{
    char c;
    while ((c=fgetc(fp)) != '\n') {
	if (feof(fp))
	    return;
    }
}

string read_info_string(FILE *fp)
{
  //TODO :
  //   Make sure there is a end...
  //   Accept Multi lines info...
  string To_return;
  string tmp = buf;
  int Start = tmp.find("_(\"");     //  _("
  int End   = tmp.find("\")");      //      ")
  if(End == -1) 
    {
      //The end is not on this line...
    }
  else
    {
      cout<<"  info : "<<Start<<" --> "<<End<<endl;
    }
  To_return = string(tmp, Start+3, End-(Start+3));

  return To_return;
}

