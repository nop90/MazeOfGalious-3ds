#ifdef _WIN32
#include <windows.h>
#include <windowsx.h>
#else
#include <sys/time.h>
#include <time.h>
#endif

#ifdef _3DS
#include "3ds.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include "SDL.h"
#include "SDL_mixer.h"
#include "sound.h"

#include "bitmaps.h"
#include "sprites.h"
#include "tiles.h"

#include "mog.h"

#include "debug.h"

extern void DebugReport(void);

int SCREEN_X = 640;
int SCREEN_Y = 400;
int COLOUR_DEPTH = 8;

#define TRANSPARANT_COLOR (0)

bool fullscreen = true;

// Redrawing constant
int REDRAWING_PERIOD = 40;

// Frames per second counter
float frames_per_sec = 0.0;
int frames_per_sec_tmp = 0;
int init_time = 0;

// Paths
#ifdef _3DS
char *default_g_path = "romfs:/graphics/original/";
char *default_s_path = "romfs:/sound/original/";
extern int STATE;
#else
char *default_g_path = "graphics/original/";
char *default_s_path = "sound/original/";
#endif
char **g_paths = 0;
int n_g_paths = 0, act_g_path = 0;
char *g_path = 0;
char **s_paths = 0;
int n_s_paths = 0, act_s_path = 0;
char *s_path = 0;

extern int music_volume, sfx_volume;
extern int fighting_demon;


void pause(unsigned int time)
{
	unsigned int initt = SDL_GetTicks();

	while((SDL_GetTicks() - initt) < time);
} 


/* Pantalla: */ 

SDL_Surface *screen;

void Render(SDL_Surface *surface);
SDL_Surface* initializeSDL(int flags);
void finalizeSDL();

#ifdef _WIN32
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nCmdShow)
{
#else
int main(int argc, char** argv)
{
#endif

	int time, act_time;
	SDL_Event event;
    bool quit = false;
#ifdef _3DS
	romfsInit();
	screen = initializeSDL((fullscreen ? SDL_FULLSCREEN : 0)|SDL_CONSOLEBOTTOM|SDL_HWSURFACE);
	SDL_N3DSKeyBind(KEY_CPAD_UP|KEY_CSTICK_UP, SDLK_UP);
	SDL_N3DSKeyBind(KEY_CPAD_DOWN|KEY_CSTICK_DOWN, SDLK_DOWN);
	SDL_N3DSKeyBind(KEY_CPAD_LEFT|KEY_CSTICK_LEFT, SDLK_LEFT);
	SDL_N3DSKeyBind(KEY_CPAD_RIGHT|KEY_CSTICK_RIGHT, SDLK_RIGHT);
	SDL_N3DSKeyBind(KEY_A, SDLK_SPACE);
	SDL_N3DSKeyBind(KEY_B, SDLK_TAB);
	SDL_N3DSKeyBind(KEY_X, SDLK_BACKSPACE);
	SDL_N3DSKeyBind(KEY_Y, SDLK_HASH);
	SDL_N3DSKeyBind(KEY_L, SDLK_LCTRL);
	SDL_N3DSKeyBind(KEY_R, SDLK_RCTRL);
#else
	screen = initializeSDL((fullscreen ? SDL_FULLSCREEN : 0));
#endif
	if (screen == 0) {
		return 0;
	}

	time= init_time = SDL_GetTicks();

	init_paths();
	if (!cargar_configuracion("MoG.cfg")) {
		configuracion_por_defecto();
		guardar_configuracion("MoG.cfg");
	} /* if */ 

	GameInit(SCREEN_X, SCREEN_Y);
	SDL_FillRect(screen, NULL, 0);
	while (!quit) {
	
		while(SDL_PollEvent(&event)) {
            switch(event.type) {
			
                // grab keyboard events 
                case SDL_KEYDOWN:
#ifdef __APPLE__
                    // different quit shortcut on OSX: apple+Q
                    if (event.key.keysym.sym == SDLK_q) {
                        SDLMod modifiers;
                        modifiers = SDL_GetModState();
                        if ((modifiers&KMOD_META) != 0) {
                            quit = true;
                        }
                    }
#endif
#ifdef _WIN32
                    // different quit shortcut on WIN32: ALT+F4
                    if (event.key.keysym.sym == SDLK_F4) {
                        SDLMod modifiers;
                        modifiers = SDL_GetModState();
                        if ((modifiers&KMOD_ALT) != 0) {
                            quit = true;
                        }
                    }
#endif
#ifdef _3DS
					if (event.key.keysym.sym == SDLK_ESCAPE && STATE == 2) {
#else
                    // default quit: F12
                    if (event.key.keysym.sym == SDLK_F12) {
#endif
                        quit = true;
                    } /* if */

/*
FIXME: the code below is a big copy/paste; it should be in a separate function in stead
*/

#ifdef __APPLE__
                    if (event.key.keysym.sym == SDLK_f) {
                        SDLMod modifiers;
                        modifiers = SDL_GetModState();
                        if ((modifiers&KMOD_META) != 0) {
							fullscreen = (fullscreen ? false : true);
							SDL_QuitSubSystem(SDL_INIT_VIDEO);
							SDL_InitSubSystem(SDL_INIT_VIDEO);
							
							if (SDL_WasInit(SDL_INIT_VIDEO)) {
								screen = SDL_SetVideoMode(SCREEN_X, SCREEN_Y, COLOUR_DEPTH, SDL_HWPALETTE|(fullscreen ? SDL_FULLSCREEN : 0));
								
								if (screen == NULL) {
									output_debug_message("Couldn't set %ix%ix%i", SCREEN_X, SCREEN_Y, COLOUR_DEPTH);
									if (fullscreen) {
										output_debug_message( ",fullscreen,");
									}
									output_debug_message(" video mode: %s\n", SDL_GetError ());
									quit = true;
								} else {
									output_debug_message( "Set the video resolution to: %ix%ix%i",
										SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h,
										SDL_GetVideoSurface()->format->BitsPerPixel);
									if (fullscreen) {
										output_debug_message( ",fullscreen");
									}
									output_debug_message("\n");
								}
												
								SDL_WM_SetCaption("Maze of Galious v0.63", 0);
								get_palette();
							} else {
								quit = true;
							}
                        } // if
                    } // if
#else
#ifndef _3DS
                    if (event.key.keysym.sym == SDLK_RETURN)
                    {
						if (IsAltPressed2()) {
							fullscreen = (fullscreen ? false : true);
							SDL_QuitSubSystem(SDL_INIT_VIDEO);
							SDL_InitSubSystem(SDL_INIT_VIDEO);
							
							if (SDL_WasInit(SDL_INIT_VIDEO)) {
								screen = SDL_SetVideoMode(SCREEN_X, SCREEN_Y, COLOUR_DEPTH, SDL_HWPALETTE|(fullscreen ? SDL_FULLSCREEN : 0));
								if (screen == NULL) {
									output_debug_message( "Couldn't set %ix%ix%i", SCREEN_X, SCREEN_Y, COLOUR_DEPTH);
									if (fullscreen) {
										output_debug_message( ",fullscreen,");
									}
									output_debug_message(" video mode: %s\n", SDL_GetError ());
									quit = true;
								} else {
									output_debug_message( "Set the video resolution to: %ix%ix%i",
										SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h,
										SDL_GetVideoSurface()->format->BitsPerPixel);
									if (fullscreen) {
										output_debug_message( ",fullscreen");
									}
									output_debug_message("\n");
								}
												
								SDL_WM_SetCaption("Maze of Galious v0.63", 0);
								get_palette();
							} else {
								quit = true;
							}
                        }
                    }
#endif
#endif
				
					// Change graphic set with either F10 or 9 (F10 is already used in OSX)

#ifdef _3DS						
					if (event.key.keysym.sym == SDLK_LCTRL && STATE ==2) {
#else
					if (event.key.keysym.sym == SDLK_F10 || event.key.keysym.sym == SDLK_9) {
#endif
						act_g_path++;
						if (act_g_path >= n_g_paths) {
							act_g_path = 0;
						}
						
						g_path = g_paths[act_g_path];
						ReleaseGraphics();
						ReloadGraphics(SCREEN_X, SCREEN_Y);
						guardar_configuracion("MoG.cfg");
						if (fighting_demon != 0) {
							redo_demonintro(fighting_demon, 0, SCREEN_X, SCREEN_Y);
						}
					} 

					// Change sound set with either F11 or 0 (F10 is already used in OSX)
#ifdef _3DS						
					if (event.key.keysym.sym == SDLK_RCTRL && STATE ==2) {
#else
					if (event.key.keysym.sym == SDLK_F11 || event.key.keysym.sym == SDLK_0) {
#endif
						act_s_path++;
						if (act_s_path >= n_s_paths) {
							act_s_path = 0;
						}
						
						s_path = s_paths[act_s_path];
						ReleaseSound(false);
						ReloadSound();
						music_recovery();
						guardar_configuracion("MoG.cfg");
						Mix_VolumeMusic(music_volume);
						SetSFXVolume(sfx_volume);
					}

					if (event.key.keysym.sym == SDLK_d) {
						write_debug_report("debug-report.txt");
					} 
                    break;

                // quit game and close window 
                case SDL_QUIT:
                    quit = true;
                    break;
            }
        }
		
		// game main loop
		act_time = SDL_GetTicks();
        if (!quit && act_time - time >= REDRAWING_PERIOD) {
            int max_frame_step = 10;
            do {
                time += REDRAWING_PERIOD;
                if ((act_time - time) > 10 * REDRAWING_PERIOD) {
                    time = act_time;
				}

	
				// render graphics
				Render(screen);
				SDL_Flip(screen);
				
                act_time = SDL_GetTicks();
                max_frame_step--;
            } while (act_time - time >= REDRAWING_PERIOD && max_frame_step > 0);

        }
        if ((act_time - init_time) >= 1000) {
            frames_per_sec = frames_per_sec_tmp;
            frames_per_sec_tmp = 0;
            init_time = act_time;
        }
		
		SDL_Delay(1);		
	}

	finalizeSDL();
#ifdef _3DS
	romfsExit();
#endif
	return 0;
}


SDL_Surface* initializeSDL(int moreflags)
{
	char VideoName[256];
	SDL_Surface *screen;

#ifdef _3DS
	int flags = moreflags|SDL_FULLSCREEN|SDL_CONSOLEBOTTOM;
#else
	int flags = SDL_HWPALETTE|moreflags;
#endif
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0) {
		return 0;
	}

	output_debug_message( "Initializing SDL video subsystem.\n");

	if ((SDL_Init(SDL_INIT_VIDEO)) == -1) {
		output_debug_message( "Couldn't initialize video subsystem: %s\n", SDL_GetError());
		exit(-1);
	}

	SDL_VideoDriverName(VideoName, sizeof (VideoName));

    output_debug_message( "SDL driver used: %s\n", VideoName);
    // Set the environment variable SDL_VIDEODRIVER to override
    // For Linux: x11 (default), dga, fbcon, directfb, svgalib,
    //            ggi, aalib
    // For Windows: directx (default), windib
	output_debug_message( "SDL video subsystem initialized.\n");

	output_debug_message( "Initializing SDL audio subsystem.\n");

	if ((SDL_InitSubSystem(SDL_INIT_AUDIO)) == -1) {
	  output_debug_message( "Couldn't initialize audio subsystem: %s\n", SDL_GetError());
	  exit(-1);
	}
	output_debug_message( "SDL audio subsystem initialized.\n");


	atexit(SDL_Quit);
	SDL_WM_SetCaption("Maze of Galious v0.63", 0);
	
	if (fullscreen) {
		SDL_ShowCursor(SDL_DISABLE);
	}
	
	Sound_initialization();

	pause(1000);
	screen = SDL_SetVideoMode(SCREEN_X, SCREEN_Y, COLOUR_DEPTH, flags);
	if (screen == NULL) {
		output_debug_message( "Couldn't set %ix%ix%i", SCREEN_X, SCREEN_Y, COLOUR_DEPTH);
	    if (fullscreen) {
			output_debug_message( ",fullscreen,");
	    }
		output_debug_message(" video mode: %s\n", SDL_GetError ());
	    exit(-1);
	} else {
	    output_debug_message( "Set the video resolution to: %ix%ix%i",
				 SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h,
				 SDL_GetVideoSurface()->format->BitsPerPixel);
	    if (fullscreen) {
			output_debug_message( ",fullscreen");
		}
	    output_debug_message("\n");
    }
	
	SDL_EnableUNICODE(1);
	
	return screen;
}

void finalizeSDL()
{
	ReleaseSound(true);
	ReleaseGraphics();
	Sound_release();
	SDL_Quit();
}

