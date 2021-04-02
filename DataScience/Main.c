#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include "Main.h"

#undef main

SDL_Window* w;
SDL_Renderer* r;

double* data = NULL;
double* data_ma12 = NULL;
double* data_ma26 = NULL;

_Bool quit = 0;

int lines = LINES;
int maxlines = 0;

float pan_x = 0;
float pan_y = 0;

float s = 0.5;

void Init() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		perror(SDL_GetError());
		exit(-1);
	}
	w = SDL_CreateWindow("Data visualization in C", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, NULL);
	if (w == NULL) {
		perror(SDL_GetError());
		exit(-1);
	}
	r = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED);
	if (r == NULL) {
		perror(SDL_GetError());
		exit(-1);
	}
}

void DeInit() {
	SDL_DestroyWindow(w);
	SDL_DestroyRenderer(r);
	SDL_Quit();
	free(data);
	free(data_ma12);
	free(data_ma26);
	w = NULL;
	r = NULL;
}

errno_t GetData() {
	FILE* f;
	f = fopen("EURHUF_complete_rates.txt", "r");
	if (f == NULL) {
		perror("couldn't open data file!\n");
		return -2;
	}
	fseek(f, 0, SEEK_SET);
	fseek(f, 0, SEEK_END);
	size_t sz = ftell(f);
	fseek(f, 0, SEEK_SET);

	char* rawdata = malloc(sz);
	if (rawdata == NULL) {
		perror("couldn't allocate memory for data!\n");
		return -3;
	}

	char c;
	int newlines = 0;
	while (!feof(f)) {
		c = fgetc(f);
		if (c == '\n')
			newlines++;
	}

	printf("%d\n", newlines);
	maxlines = newlines;

	data = malloc(sizeof(double)*newlines*2);

	if (data == NULL) {
		perror("couldn't allocate memory for data!\n");
		return -3;
	}

	fseek(f, 0, SEEK_SET);
	fread(rawdata, 1, sz, f);

	char* tok = strtok(rawdata, "\n");
	int i = 0;
	while (tok != NULL) {
		*(data + i) = atof(tok);
		tok = strtok(NULL, "\n");
		i++;
	}

	free(rawdata);

	fclose(f);



	return 0;
}

errno_t GetMA12() {
	FILE* f;
	f = fopen("EURHUF_complete_ma12.txt", "r");
	if (f == NULL) {
		perror("couldn't open data file!\n");
		return -2;
	}
	fseek(f, 0, SEEK_SET);
	fseek(f, 0, SEEK_END);
	size_t sz = ftell(f);
	fseek(f, 0, SEEK_SET);

	char* rawdata = malloc(sz);
	if (rawdata == NULL) {
		perror("couldn't allocate memory for data!\n");
		return -3;
	}

	char c;
	int newlines = 0;
	while (!feof(f)) {
		c = fgetc(f);
		if (c == '\n')
			newlines++;
	}

	printf("%d\n", newlines);

	data_ma12 = malloc(sizeof(double) * newlines * 2);

	if (data_ma12 == NULL) {
		perror("couldn't allocate memory for data!\n");
		return -3;
	}

	fseek(f, 0, SEEK_SET);
	fread(rawdata, 1, sz, f);

	char* tok = strtok(rawdata, "\n");
	int i = 0;
	while (tok != NULL) {
		*(data_ma12 + i) = atof(tok);
		tok = strtok(NULL, "\n");
		i++;
	}

	free(rawdata);

	fclose(f);

	return 0;
}

errno_t GetMA26() {
	FILE* f;
	f = fopen("EURHUF_complete_ma26.txt", "r");
	if (f == NULL) {
		perror("couldn't open data file!\n");
		return -2;
	}
	fseek(f, 0, SEEK_SET);
	fseek(f, 0, SEEK_END);
	size_t sz = ftell(f);
	fseek(f, 0, SEEK_SET);

	char* rawdata = malloc(sz);
	if (rawdata == NULL) {
		perror("couldn't allocate memory for data!\n");
		return -3;
	}

	char c;
	int newlines = 0;
	while (!feof(f)) {
		c = fgetc(f);
		if (c == '\n')
			newlines++;
	}

	printf("%d\n", newlines);

	data_ma26 = malloc(sizeof(double) * newlines * 2);

	if (data_ma26 == NULL) {
		perror("couldn't allocate memory for data!\n");
		return -3;
	}

	fseek(f, 0, SEEK_SET);
	fread(rawdata, 1, sz, f);

	char* tok = strtok(rawdata, "\n");
	int i = 0;
	while (tok != NULL) {
		*(data_ma26 + i) = atof(tok);
		tok = strtok(NULL, "\n");
		i++;
	}

	free(rawdata);

	fclose(f);

	return 0;
}

void Render() {
	float max1 = 0;
	for (int i = lines-500; i < lines+500; i++) {
		if (data[i] >= max1) {
			max1 = data[i];
		}
	}

	int liness = 1000;

	for (int i = lines-500; i < lines+500; i++) {

		float datapos1 = H/s - data[i]/max1*(700+pan_y) /s;
		float datapos2 = H/s - data[i + 1]/max1*(700+pan_y) /s;
		SDL_SetRenderDrawColor(r, data[i] > data[i + 1] ? 170 : 0, data[i] > data[i + 1] ? 0 : 170, 30, 255);
		SDL_RenderDrawLine(r, i * W / liness + pan_x, datapos1, (i + 1) * W / liness + pan_x, datapos2);
		if (i > 12) {
			SDL_SetRenderDrawColor(r, 150, 140, 30, 255);
			float dataposma121 = H/s - data_ma12[i] / max1 * (700+pan_y)/s;
			float dataposma122 = H/s - data_ma12[i + 1] / max1 * (700+pan_y)/s;
			SDL_RenderDrawLine(r, (i+12) * W / liness + pan_x, dataposma121, (i + 1+12) * W / liness + pan_x, dataposma122);
		}
		if (i > 26) {
			SDL_SetRenderDrawColor(r, 30, 170, 200, 255);
			float dataposma261 = H/s - data_ma26[i] / max1 * (700+pan_y)/s;
			float dataposma262 = H/s - data_ma26[i + 1] / max1 * (700+pan_y)/s;
			SDL_RenderDrawLine(r, (i+26) * W / liness + pan_x, dataposma261, (i + 1+26) * W / liness + pan_x, dataposma262);
			//SDL_RenderDrawLine(r, (i+26) * W / lines, H * 3*(1000-lines/2)/1000*2 - data_ma26[i] * 10*(1000-lines/2)/1000*2, (i + 1+26) * W / lines, H * 3*(1000-lines/2)/1000*2 - data_ma26[i + 1] * 10*(1000-lines/2)/1000*2);
		}

	}
	SDL_SetRenderDrawColor(r, 40, 40, 40, 255);
	SDL_RenderSetScale(r, 10, 10);
	SDL_RenderDrawLine(r, 0, 0, 0, H / 10);
	SDL_RenderDrawLine(r, 0, H/10-10, W/10, H/10-10);
	SDL_RenderSetScale(r, 1, 1);
}

void EventLoop() {
	int drag = 0;
	int zoom = 0;
	while (quit != 1) {
		SDL_Event e;
		
		while (SDL_PollEvent(&e) != NULL) {
			
			switch (e.type) {
			case SDL_QUIT: {
				quit = 1;
				break;
			}
			case SDL_KEYDOWN: {
				if (e.key.keysym.sym == SDLK_LCTRL) {
					zoom = 1;
					break;
				}
			}
			case SDL_KEYUP: {
				if (e.key.keysym.sym == SDLK_LCTRL) {
					zoom = 0;
					break;
				}
			}
			case SDL_MOUSEWHEEL: {
				if (e.wheel.y > 0) {
					if (zoom == 1) {
						s -= 0.05;
					}
					else {
						lines += 25;
						pan_x -= 50;
					}
				}
				else {
					if (zoom == 1) {
						s += 0.05;
					}
					else {
						lines -= 30;
						pan_x += 50;
					}
				}
			}
			case SDL_MOUSEBUTTONDOWN: {
				if (e.button.button == SDL_BUTTON_LEFT) {
					drag = 1;
					
					break;
				}
			}
			case SDL_MOUSEBUTTONUP: {
				if (e.button.button == SDL_BUTTON_LEFT) {
					drag = 0;
					
					break;
				}
			}
			case SDL_MOUSEMOTION: {
				
				if (drag == 1) {
					pan_x += 0.4*e.motion.xrel;
					pan_y -= 0.4*e.motion.yrel;
				}
			}
			}
		}
		SDL_SetRenderDrawColor(r, 10, 10, 10, 255);
		SDL_RenderClear(r);
		Render();
	
		SDL_RenderPresent(r);
	}

	DeInit();
}

void RunScript() {
	system("python main.py");
}


int main(int argc, char** argv) {
#if RERUN_SCRIPT == 1
	RunScript();
#endif
	Init();
	if (GetData() < 0) {
		perror("error in GetData!\n");
		exit(-1);
	}
	if (GetMA12() < 0) {
		perror("error in GetMA12!\n");
		exit(-1);
	}
	if (GetMA26() < 0) {
		perror("error in GetMA26!\n");
		exit(-1);
	}

	EventLoop();

	return 0;
}