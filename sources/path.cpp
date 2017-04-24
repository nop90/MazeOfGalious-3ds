#ifdef _WIN32
#include "windows.h"
#else
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NAME_MAX_LEN	256

/* Paths: */ 

extern char **g_paths;
extern int n_g_paths,act_g_path;
extern char *g_path;
extern char **s_paths;
extern int n_s_paths,act_s_path;
extern char *s_path;


#ifdef _WIN32
void init_paths(void)
{

	if (g_paths==NULL) {

		WIN32_FIND_DATA finfo;
		HANDLE h;
		int i,j;
		char **tmp_g_paths; 

		n_g_paths=0;
		h=FindFirstFile("graphics\\*.*",&finfo);
		if (h!=INVALID_HANDLE_VALUE) {
			if (strcmp(finfo.cFileName,".")!=0 &&
				strcmp(finfo.cFileName,"..")!=0) {
				i=0;
				n_g_paths=1;
				g_paths=new char *[1];
				g_paths[i]=new char[NAME_MAX_LEN];
				sprintf(g_paths[i++],"graphics\\%s\\",strupr(finfo.cFileName));
			} /* if */ 

			while(FindNextFile(h,&finfo)==TRUE) {

				if (strcmp(finfo.cFileName,".")!=0 &&
					strcmp(finfo.cFileName,"..")!=0) {
					if (n_g_paths==0) {
						i=0;
						n_g_paths=1;
						g_paths=new char *[1];
						g_paths[i]=new char[NAME_MAX_LEN];
						sprintf(g_paths[i++],"graphics\\%s\\",strupr(finfo.cFileName));
					} else {
						tmp_g_paths=g_paths;
						n_g_paths++;
						g_paths=new char *[i+1];
						for(j=0;j<i;j++) {
							g_paths[j]=tmp_g_paths[j];
						} /* for */ 
						delete tmp_g_paths;
						g_paths[i]=new char[NAME_MAX_LEN];
						sprintf(g_paths[i++],"graphics\\%s\\",strupr(finfo.cFileName));
					} /* if */ 
				} /* if */ 
			} /* while */ 
		} else {
			g_paths=new char *[2];
			g_paths[0]=new char[NAME_MAX_LEN];
			g_paths[1]=new char[NAME_MAX_LEN];
			strcpy(g_paths[0],"graphics\\original\\");
			strcpy(g_paths[1],"graphics\\alternate\\");
		} /* if */ 

		act_g_path=0;
		g_path=g_paths[act_g_path];

	} /* if */ 


	if (s_paths==NULL) {

		WIN32_FIND_DATA finfo;
		HANDLE h;
		int i,j;
		char **tmp_s_paths; 

		n_s_paths=0;
		h=FindFirstFile("sound\\*.*",&finfo);
		if (h!=INVALID_HANDLE_VALUE) {
			if (strcmp(finfo.cFileName,".")!=0 &&
				strcmp(finfo.cFileName,"..")!=0) {
				i=0;
				n_s_paths=1;
				s_paths=new char *[1];
				s_paths[i]=new char[NAME_MAX_LEN];
				sprintf(s_paths[i++],"sound\\%s\\",strupr(finfo.cFileName));
			} /* if */ 

			while(FindNextFile(h,&finfo)==TRUE) {

				if (strcmp(finfo.cFileName,".")!=0 &&
					strcmp(finfo.cFileName,"..")!=0) {
					if (n_s_paths==0) {
						i=0;
						n_s_paths=1;
						s_paths=new char *[1];
						s_paths[i]=new char[NAME_MAX_LEN];
						sprintf(s_paths[i++],"sound\\%s\\",strupr(finfo.cFileName));
					} else {
						tmp_s_paths=s_paths;
						n_s_paths++;
						s_paths=new char *[i+1];
						for(j=0;j<i;j++) {
							s_paths[j]=tmp_s_paths[j];
						} /* for */ 
						delete tmp_s_paths;
						s_paths[i]=new char[NAME_MAX_LEN];
						sprintf(s_paths[i++],"sound\\%s\\",strupr(finfo.cFileName));
					} /* if */ 
				} /* if */ 
			} /* while */ 
		} else {
			s_paths=new char *[2];
			s_paths[0]=new char[NAME_MAX_LEN];
			s_paths[1]=new char[NAME_MAX_LEN];
			strcpy(s_paths[0],"sound\\original\\");
			strcpy(s_paths[1],"sound\\alternate\\");
		} /* if */ 

		act_s_path=0;
		s_path=s_paths[act_s_path];
	} /* if */ 
} /* init_paths */ 
#elif defined(_3DS)
void init_paths(void) {

	n_g_paths=7;
	n_s_paths=7;
	s_paths=new char *[7];
	g_paths=new char *[7];
		
	for (int i=0; i<7;i++) {
		s_paths[i]=new char[NAME_MAX_LEN];
		g_paths[i]=new char[NAME_MAX_LEN];
	} 

	strcpy(g_paths[0],"romfs:/graphics/naramura/");
	strcpy(g_paths[1],"romfs:/graphics/Zan/");
	strcpy(g_paths[2],"romfs:/graphics/original/");
	strcpy(g_paths[3],"romfs:/graphics/alternate/");
	strcpy(g_paths[4],"romfs:/graphics/alfonso/");
	strcpy(g_paths[5],"romfs:/graphics/boltian/");
	strcpy(g_paths[6],"romfs:/graphics/hinox/");
	strcpy(s_paths[0],"romfs:/sound/jorito/");
	strcpy(s_paths[1],"romfs:/sound/Zan/");
	strcpy(s_paths[2],"romfs:/sound/original/");
	strcpy(s_paths[3],"romfs:/sound/alternate/");
	strcpy(s_paths[4],"romfs:/sound/Bart/");
	strcpy(s_paths[5],"romfs:/sound/Rob/");
	strcpy(s_paths[6],"romfs:/sound/wolf/");

	act_g_path=0;
	act_s_path=0;
	g_path=g_paths[act_g_path];
	s_path=s_paths[act_s_path];

} /* init_paths */ 

#else
void init_paths(void) {

  int i,j;
  DIR *dd;
  struct dirent *direntry;
  
  if (g_paths==NULL) {	  
	  char **tmp_g_paths; 

	  n_g_paths=0;
	  
	  if((dd=opendir("graphics"))==NULL) {
	    perror("ERROR: Could not open directory \"graphics\"");
	    exit(1);
	  }
	  if ((direntry=readdir(dd))==NULL) {
	    perror("ERROR: Could not read directory \"graphics\"");
	    exit(1);
	  }
	  
	  while(direntry != NULL) {

	    // fprintf(stderr,"DIR: %s\n",direntry->d_name);
	    
	    if (strcmp(direntry->d_name,".")!=0 &&
		strcmp(direntry->d_name,"..")!=0) {
	      if (n_g_paths==0) {
		i=0;
		n_g_paths=1;
		g_paths=new char *[1];
		g_paths[i]=new char[NAME_MAX_LEN];
		sprintf(g_paths[i++],"graphics/%s/",direntry->d_name);
	      } else {
		tmp_g_paths=g_paths;
		n_g_paths++;
		g_paths=new char *[i+1];
		for(j=0;j<i;j++) {
		  g_paths[j]=tmp_g_paths[j];
		} /* for */ 
		delete tmp_g_paths;
		g_paths[i]=new char[NAME_MAX_LEN];
		sprintf(g_paths[i++],"graphics/%s/",direntry->d_name);
	      } /* if */ 
	    } /* if */ 
	    
	    direntry=readdir(dd);
	  } /* while */ 
	  closedir(dd);
	  
	  if (n_g_paths==0) {
	    fprintf(stderr,"ERROR: No directories for graphics found in graphics/* \n");
	    exit(1);
	  }
  } /* if */ 


  if (s_paths==NULL) {	  
	  char **tmp_s_paths; 

	  n_s_paths=0;
	  
	  if((dd=opendir("sound"))==NULL) {
	    perror("ERROR: Could not open directory \"sound\"");
	    exit(1);
	  }
	  if ((direntry=readdir(dd))==NULL) {
	    perror("ERROR: Could not read directory \"sound\"");
	    exit(1);
	  }
	  
	  while(direntry != NULL) {

	    // fprintf(stderr,"DIR: %s\n",direntry->d_name);
	    
	    if (strcmp(direntry->d_name,".")!=0 &&
		strcmp(direntry->d_name,"..")!=0) {
	      if (n_s_paths==0) {
		i=0;
		n_s_paths=1;
		s_paths=new char *[1];
		s_paths[i]=new char[NAME_MAX_LEN];
		sprintf(s_paths[i++],"sound/%s/",direntry->d_name);
	      } else {
		tmp_s_paths=s_paths;
		n_s_paths++;
		s_paths=new char *[i+1];
		for(j=0;j<i;j++) {
		  s_paths[j]=tmp_s_paths[j];
		} /* for */ 
		delete tmp_s_paths;
		s_paths[i]=new char[NAME_MAX_LEN];
		sprintf(s_paths[i++],"sound/%s/",direntry->d_name);
	      } /* if */ 
	    } /* if */ 
	    
	    direntry=readdir(dd);
	  } /* while */ 
	  closedir(dd);
	  
	  if (n_s_paths==0) {
	    fprintf(stderr,"ERROR: No directories for sound found in sound/* \n");
	    exit(1);
	  }
  } /* if */ 

} /* init_paths */ 

#endif
