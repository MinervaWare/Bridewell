/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * */

/*
 * This was implemented very hastely (hence the very ugly unoptimized code) 
 * to get generation up and going. This needs re-written later.
 * */


/*
 "The only marvelous thing about C++ is that anyone manages to get any work done in
	it at all. Fortunately, most good programmers know that they can avoid
	C++ by writing largely in C"
	- The Unix-Haters Handbook
 * */

#include "wfc.h"
#include "game.h"
#include <time.h>
#ifdef WIN32
#include <io.h>
#include <Shlwapi.h>
#define F_OK 0
#define access _access
#else
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#ifndef EEXIST
#define EEXIST      17
#endif
#endif

#define WSPACE "1 1 1"
#define BSPACE "0 0 0"

struct tileData wfc;

int getConSize(int tile, enum sides side) {
	int i;
	for(i=0;i<MAXTILES;i++) {
		if(side==WRIGHT) {
				//log_info("r: %d", wfc.tileSideCons.tileRight[tile][i]);
				if(wfc.tileSideCons.tileRight[tile][i]==0) return i; 
		} else if(side==WLEFT) {
				if(wfc.tileSideCons.tileLeft[tile][i]==0) return i; 
		} else if(side==WUP) {
				if(wfc.tileSideCons.tileUp[tile][i]==0) return i; 
		} else if(side==WDOWN) {
				//log_info("d: %d", wfc.tileSideCons.tileDown[tile][i]);
				if(wfc.tileSideCons.tileDown[tile][i]==0) return i; 
		}
	}
	return i+1;
}


static int outnum=0;
int thrst = 0;
void *mapToFile() {
	thrst = 1;
	char rm[256];
#if defined __WIN32 | __WIN64
	sprintf(rm, "del world\\out%d.map", outnum);
	WinExec(rm, SW_HIDE);
#else
	char _buf[256];
	snprintf(_buf, sizeof(_buf), "world/out%d.map", outnum);
	if(access(_buf,F_OK)==0&&remove(_buf)!=0) {
		perror("remove old ppm file");
		return NULL;
	}
#endif
	char fname[256];
	snprintf(fname, sizeof(fname), "world/out%d.map", outnum);
  //if (access(fname, F_OK) == 0) return NULL;
  //log_info("%d out", outnum);
	FILE *file = fopen(getExecDir(fname), "wb");
	int i,j;
	int size = (MAPSIZE*TILESIZE)*(MAPSIZE*TILESIZE);
	int buffer[size+size];
	for(i=0;i<MAPSIZE*TILESIZE;i++) {
 		for(j=0;j<MAPSIZE*TILESIZE;j++) {
			buffer[(MAPSIZE*TILESIZE)*i+j] = wfc.mapExpanded[i][j];
		}
	}
	for(i=0;i<MAXLIGHTS;i++) buffer[(size)+i] = wfc.lightX[i];
	for(i=0;i<MAXLIGHTS;i++) buffer[(size+MAXLIGHTS)+i] = wfc.lightY[i];
	fwrite(buffer,sizeof(buffer),1,file);
	fclose(file);
	return NULL;
}

mapData *readMap(int level) {
	mapData *data = calloc(1, sizeof(mapData));
	int size = (MAPSIZE*TILESIZE)*(MAPSIZE*TILESIZE);
	data->map = calloc(size, sizeof(int));
	data->lightX = calloc(MAXLIGHTS, sizeof(int));
	data->lightY = calloc(MAXLIGHTS, sizeof(int));
	char buf[256];
	snprintf(buf, sizeof(buf), "world/out%d.map", level);
	FILE *f = fopen(getExecDir(buf), "rb");
	if(!f) return NULL;
	size_t read = fread(data->map, sizeof(int), size, f);
	if(read!=size) return NULL;
	fseek(f, size*sizeof(int), SEEK_SET);
	read = fread(data->lightX, sizeof(int), MAXLIGHTS, f);
	if(read!=MAXLIGHTS) return NULL;
	fseek(f, (size+MAXLIGHTS)*sizeof(int), SEEK_SET);
	read = fread(data->lightY, sizeof(int), MAXLIGHTS, f);
	if(read!=MAXLIGHTS) return NULL;
	fclose(f);
	return data;
}

void mapToPPM() {
  	
	/*char rm[256];
#if defined __WIN32 | __WIN64
 	if(PathFileExists("world")!=1) WinExec("mkdir world", SW_HIDE);
 	sprintf(rm, "del world\\out%d.ppm", outnum);
 	WinExec(rm, SW_HIDE);
#else
	if(mkdir("world", 0755)!=0&&errno!=EEXIST) {
		perror("mkdir world");
		return;
	}
	char _buf[256];
	snprintf(_buf, sizeof(_buf), "world/out%d.ppm", outnum);
	if(access(_buf,F_OK)==0&&remove(_buf)!=0) {
		perror("remove old ppm file");
		return;
	}
#endif

 	if(thrst==0) startThread("mapgen", (BRIDEFUNC)mapToFile);

	char fname[256];	
	sprintf(fname, "world/out%d.ppm", outnum);
	FILE *file = fopen(fname, "w");
	char foutn[256];
	sprintf(foutn, "out%d.ppm", outnum);
	char *base = "P3\n# out.ppm";
	char buf[256];
	snprintf(buf, sizeof(buf), "%s\n%d %d\n1\n", 
			base, (MAPSIZE*TILESIZE), (MAPSIZE*TILESIZE));
	fprintf(file, "%s", buf);
	int i,j;
	for(i=0;i<MAPSIZE*TILESIZE;i++) {
		for(j=0;j<MAPSIZE*TILESIZE;j++) {
			if(wfc.mapExpanded[i][j]==1)
				fprintf(file, "%s ", WSPACE);
			else fprintf(file, "%s ", BSPACE);
		}
		fprintf(file, "\n");
	}
 	fclose(file);
 	if(thrst==1) {
   		thrst=0;
   		finishThread("mapgen");
 	}*/
	mapToFile();
	outnum++;
}

void expandMap() {
	int i,j,k,l;
	for(i=0;i<MAPSIZE;i++) {
		for(j=0;j<MAPSIZE;j++) {
			Vec2 exindex = (Vec2){i*3, j*3};
			for(k=0;k<TILESIZE;k++) {
				for(l=0;l<TILESIZE;l++) {
					wfc.mapExpanded[(int)exindex.x+k][(int)exindex.y+l] = 
						wfc.tileSet[wfc.outputMap[i][j]][k][l];
				}
			}
		}
	}	
	//SetExitKey(0);
}

void populateLights() {
	int i,j,k,l;
	for(i=0;i<MAXLIGHTS;i++) {
		wfc.lightX[i] = -1;
		wfc.lightY[i] = -1;
	}
	for(i=0;i<MAXLIGHTS;i++) {
		int x = rand()%(MAPSIZE*TILESIZE);
		int y = rand()%(MAPSIZE*TILESIZE);
		if(wfc.mapExpanded[x][y]!=0) {
			while(wfc.mapExpanded[x][y]!=0) {x++;y++;}
		}
		wfc.lightX[i] = x;
		wfc.lightY[i] = y;
		for(j=0;j<i-1;j++) 
			if(wfc.lightX[j]==x&&wfc.lightY[j]==y) {i--;}
	}
}

void populateEntities() {
	int count, i,j, k;
	int msize = (MAPSIZE*TILESIZE)*(MAPSIZE*TILESIZE);
	count = rand()%(msize/8)+(msize/12);
	for(k=0;k<count;k++) {
		i = rand()%(MAPSIZE*TILESIZE);
		j = rand()%(MAPSIZE*TILESIZE);
		if(wfc.mapExpanded[i][j]==0) {k--;continue;}
		wfc.mapExpanded[i][j] = 2;
	}
}


void generateMap() {
	// get ranom seed for future randomization
	//time_t t;
	int x,y;
	for(x=0;x<MAPSIZE;x++) {
		for(y=0;y<MAPSIZE;y++) {
			wfc.outputMap[x][y] = 0;
			wfc.entropyMap[x][y] = MAXTILES;
		}
	}
	static int gencycle = 0;
	int clock_;
	clock_t c = clock();
	clock_ = (int)c;
	srand(time(NULL)-clock_);
	wfc.seed = rand();
	srand(wfc.seed);
	
	int ftile = rand() % MAXTILES;

	//Vector2 mapindex = {(float)MAPSIZE/2, (float)MAPSIZE/2};
	wfc.outputMap[0][0] = ftile;
	int frs = getConSize(ftile, WRIGHT);
	int fds = getConSize(ftile, WDOWN);
	//log_info("frs: %d , fds: %d", frs, fds);
	int l,k;
	for(l=0;l<MAPSIZE;l++) {
		for(k=0;k<MAPSIZE;k++) {
			wfc.entropyMap[l][k] = MAXTILES;
		}
	}
	if(frs==0) frs=20;
	if(fds==0) fds=20;
	wfc.entropyMap[0][1] = frs;
	wfc.entropyMap[1][0] = fds;

	wfc.entropyMap[0][0] = 0;
	//log_info("seed: %d", wfc.seed);
	//log_info("ftile: %d", ftile);
	int i,j;
	for(i=0;i<MAPSIZE;i++) {
		for(j=0;j<MAPSIZE;j++) {
			//if(wfc.entropyMap[i][j]!=20)log_info("%d", wfc.entropyMap[i][j]);
			if(wfc.entropyMap[i][j]!=0) {

				if(i-1>=0&&wfc.outputMap[i-1][j]<=20&&wfc.entropyMap[i][j]!=0) {
					if(j-1>=0&&wfc.outputMap[i][j-1]!=0) {
						/*  Dual Entropy */

						//wfc.outputMap[i][j] = 15;
						if(wfc.outputMap[i-1][j]==0&&
								wfc.outputMap[i][j-1]==0) wfc.entropyMap[i][j] = MAXTILES;
						
						int tdownentrop = getConSize(wfc.outputMap[i-1][j], WDOWN);
						int brightentrop = getConSize(wfc.outputMap[i][j-1], WRIGHT);
						int f,g,same=0;
						#if defined _WIN32 || _WIN64 || \
							__gnu_linux__ || defined __linux__ || defined linux
      					int possibilities[MAXTILES];
						#else 
						int *possibilities = calloc(MAXTILES, sizeof(int));
						#endif
						int ltile = wfc.outputMap[i][j-1];
						int utile = wfc.outputMap[i-1][j];
						for(f=0;f<tdownentrop;f++) {
							for(g=0;g<brightentrop;g++) {
								if(wfc.tileSideCons.tileRight[ltile][g]==
										wfc.tileSideCons.tileDown[utile][f]) {
									#if defined _WIN32 || _WIN64 || \
										__gnu_linux__ || defined __linux__ || defined linux 
         							possibilities[same] = wfc.tileSideCons.tileRight[ltile][g];
									#else 
									memcpy(&possibilities[same], &wfc.tileSideCons.tileRight[ltile][g], 
											sizeof(wfc.tileSideCons.tileRight[ltile][g]));
									#endif
									same++;
									//log_info("%d", wfc.tileSideCons.tileRight[ltile][g]);
								}
							}
						}
						if(same!=0) {
							int collapse = rand() % same;
							wfc.outputMap[i][j] = possibilities[collapse];
							//log_info("%d, %d", same, possibilities[0]);
							frs = getConSize(wfc.outputMap[i][j], WRIGHT);
							fds = getConSize(wfc.outputMap[i][j], WDOWN);
							if(frs==0) frs = 20;
							if(fds==0) fds = 20;
							if(i+1<MAPSIZE) wfc.entropyMap[i+1][j] = fds;
							if(j+1<MAPSIZE) wfc.entropyMap[i][j+1] = frs;
							wfc.entropyMap[i][j] = 0;
						} else {
							// pick random side to connect to based on entropy
							int side = rand() % 1;
							if(side==0) {
								if(wfc.outputMap[i-1][j]==0) wfc.entropyMap[i][j]=MAXTILES;
								int collapse = rand() % wfc.entropyMap[i][j];
								if(wfc.entropyMap[i][j]==20) {
									wfc.outputMap[i][j] = collapse;
								} else {
									wfc.outputMap[i][j] = 
										wfc.tileSideCons.tileDown[wfc.outputMap[i-1][j]][collapse];
								}
								frs = getConSize(wfc.outputMap[i][j], WRIGHT);
								fds = getConSize(wfc.outputMap[i][j], WDOWN);
								if(frs==0) frs=20;
								if(fds==0) fds=20;
								if(i+1<MAPSIZE) wfc.entropyMap[i+1][j] = fds;
								if(j+1<MAPSIZE) wfc.entropyMap[i][j+1] = frs;
								wfc.entropyMap[i][j]=0;
							} else {
								if(wfc.outputMap[i][j-1]==0) wfc.entropyMap[i][j]=MAXTILES;
								int collapse = rand() % wfc.entropyMap[i][j];
								if(wfc.entropyMap[i][j]==20) {
									wfc.outputMap[i][j] = collapse;
								} else {
									wfc.outputMap[i][j] = 
										wfc.tileSideCons.tileRight[wfc.outputMap[i][j-1]][collapse];
								}
								frs = getConSize(wfc.outputMap[i][j], WRIGHT);
								fds = getConSize(wfc.outputMap[i][j], WDOWN);
								if(frs==0) frs=20;
								if(fds==0) fds=20;
								if(i+1<MAPSIZE) wfc.entropyMap[i+1][j] = fds;
								if(j+1<MAPSIZE) wfc.entropyMap[i][j+1] = frs;
								wfc.entropyMap[i][j]=0;
							}
							
						}
						#if defined __APPLE__
      					free(possibilities);
      					#endif
						// Dual entropy TODO
					} else {
						if(i==0) continue;
						if(wfc.outputMap[i-1][j]==0) wfc.entropyMap[i][j]=MAXTILES;
						int collapse = rand() % wfc.entropyMap[i][j];
						if(wfc.entropyMap[i][j]==20) {
							wfc.outputMap[i][j] = collapse;
						} else {
							wfc.outputMap[i][j] = 
								wfc.tileSideCons.tileDown[wfc.outputMap[i-1][j]][collapse];
						}
						frs = getConSize(wfc.outputMap[i][j], WRIGHT);
						fds = getConSize(wfc.outputMap[i][j], WDOWN);
						if(frs==0) frs=20;
						if(fds==0) fds=20;
						if(i+1<MAPSIZE) wfc.entropyMap[i+1][j] = fds;
						if(j+1<MAPSIZE) wfc.entropyMap[i][j+1] = frs;
						wfc.entropyMap[i][j]=0;
					}
				} else {
					if(j==0) continue;
					if(j-1>=0&&wfc.outputMap[i][j-1]<=20&&wfc.entropyMap[i][j]!=0) {
						if(wfc.outputMap[i][j-1]==0) wfc.entropyMap[i][j]=MAXTILES;
						int collapse = rand() % wfc.entropyMap[i][j];
						if(wfc.entropyMap[i][j]==20) {
							wfc.outputMap[i][j] = collapse;
						} else {
							wfc.outputMap[i][j] = 
								wfc.tileSideCons.tileRight[wfc.outputMap[i][j-1]][collapse];
						}
						//log_info("rand: %d , ent: %d , tile: %d", 
						//		collapse, wfc.entropyMap[i][j], wfc.outputMap[i][j]);
						frs = getConSize(wfc.outputMap[i][j], WRIGHT);
						fds = getConSize(wfc.outputMap[i][j], WDOWN);
						if(frs==0) frs=20;
						if(fds==0) fds=20;
						if(i+1<MAPSIZE) wfc.entropyMap[i+1][j] = fds;
						if(j+1<MAPSIZE) wfc.entropyMap[i][j+1] = frs;
						//log_info("%d", frs);
						wfc.entropyMap[i][j]=0;
					}
				}

			} 	
		}
	}
	//for(i=0;i<MAPSIZE;i++) log_info("%d", wfc.outputMap[0][i]);
	expandMap();
	populateEntities();
	populateLights();
	
	mapToPPM();
	gencycle++;
}

Vec2 getOppositex(Vec2 pos) {
	Vec2 ret;
	switch((int)pos.y) {
		case 2: ret.y = pos.y-pos.y;
				break;
		case 1: ret.y = pos.y;
				break;
		case 0: ret.y = pos.y+2;
	}
	ret.x = pos.x;
	return ret;
}

Vec2 getOppositey(Vec2 pos) {
	Vec2 ret;
	switch((int)pos.x) {
		case 2: ret.x = pos.x-pos.x;
				break;
		case 1: ret.x = pos.x;
				break;
		case 0: ret.x = pos.x+2;
	}
	ret.y = pos.y;
	return ret;
}


void addConnection(int tile, int connection) {
	int i;
	for(i=0;i<MAXTILES&&wfc.tileConnections[tile][i]!=0;i++);
	wfc.tileConnections[tile][i] = connection;
}

void addSideCon(int tile, int connection, enum sides side) {
	int i;
	for(i=0;i<MAXTILES;i++) { 
		if(side==WRIGHT) {
			if(wfc.tileSideCons.tileRight[tile][i]==connection) break;
			if(wfc.tileSideCons.tileRight[tile][i]==0) {
				wfc.tileSideCons.tileRight[tile][i] = connection;
				break;
			}
		} else if(side==WLEFT) {
			if(wfc.tileSideCons.tileLeft[tile][i]==connection) break;
			if(wfc.tileSideCons.tileLeft[tile][i]==0) {
				wfc.tileSideCons.tileLeft[tile][i] = connection;
				break;
			}
		} else if(side==WUP) {
			if(wfc.tileSideCons.tileUp[tile][i]==connection) break;
			if(wfc.tileSideCons.tileUp[tile][i]==0) {
				wfc.tileSideCons.tileUp[tile][i] = connection;
				break;
			}
		} else if(side==WDOWN) {
			if(wfc.tileSideCons.tileDown[tile][i]==connection) break;
			if(wfc.tileSideCons.tileDown[tile][i]==0) {
				wfc.tileSideCons.tileDown[tile][i] = connection;
				//log_info("%d : %d", tile, wfc.tileSideCons.tileDown[tile][i]);
				break;
			}
		}
	}
}

void getSideConnects() {

	// right
	int i,j,k,l;
	for(i=0;i<MAXTILES;i++) {
		int right = 2;
		int left = 0;
		int up = 0;
		int down = 2;
		for(k=0;k<TILESIZE;k++) {
			// right
			if(wfc.tileSet[i][k][right]==1) {
				Vec2 oppx = getOppositex((Vec2){k,right});
				for(j=0;j<MAXTILES;j++) {
					//if(wfc.tileConnections[i][j]==0) break;
					if(wfc.tileSet[j][(int)oppx.x][(int)oppx.y]==1) {
						addSideCon(i,j,WRIGHT);
					}
				}
			}
			//left
			if(wfc.tileSet[i][k][left]==1) {
				Vec2 oppx = getOppositex((Vec2){k,left});
				for(j=0;j<MAXTILES;j++) {
					if(wfc.tileConnections[i][j]==0) break;
					if(wfc.tileSet[j][(int)oppx.x][(int)oppx.y]==1) {
						addSideCon(i,j,WLEFT);
					}
				}
			}	
			//up
			if(wfc.tileSet[i][up][k]==1) {
				Vec2 oppx = getOppositey((Vec2){up,k});
				for(j=0;j<MAXTILES;j++) {
					//if(wfc.tileConnections[i][j]==0) break;
					if(wfc.tileSet[j][(int)oppx.x][(int)oppx.y]==1) {
						addSideCon(i,j,WUP);
					}
				}
			}	
			//down
			if(wfc.tileSet[i][down][k]==1) {
				Vec2 oppx = getOppositey((Vec2){down,k});
				for(j=0;j<MAXTILES;j++) {
					//if(wfc.tileConnections[i][j]==0) break;
					if(wfc.tileSet[j][(int)oppx.x][(int)oppx.y]==1) {
						addSideCon(i,j,WDOWN);
					}
				}
			}

		}	
	}

}

void getTileConnections() {
	/* 20 x 20 wfc.tileConnections
		each array  holds all the indexes for combinations*/
	int i,j,k;
	for(i=0;i<MAXTILES;i++) {
		for(j=0;j<TILESIZE;j++) {
			for(k=0;k<TILESIZE;k++) {
				if(j==1&&k==1) continue;
				if(wfc.tileSet[i][j][k]==1) {
					Vec2 oppx = getOppositex((Vec2){j, k});
					int l,g,f;
					for(l=0;l<MAXTILES;l++) {
						if(l==i) continue;
						if(wfc.tileSet[l][(int)oppx.x][(int)oppx.y]==1) {
							//log_info("%d : %d", i, l);
							addConnection(i, l);
						}
					}
				}
			}
		}
	}
	getSideConnects();
}

void *initWFC() {
	wfc.state = STARTED;
	int i,l,f;
	for(i=0;i<MAXTILES;i++) {
		for(l=0;l<TILESIZE;l++) {
			for(f=0;f<TILESIZE;f++) {
				memcpy(&wfc.tileSet[i][l][f], &TileSet1[i][l][f], sizeof(TileSet1[i][l][f]));
			}
		}
	}	

	getTileConnections();
	for(int i=0;i<GENWORLDS;i++) generateMap();
 
	wfc.state = FINISHED;
	return NULL;	
}

enum WFCState getWFCState() {
	return wfc.state;
}

void setState(enum WFCState st) {
	wfc.state = st;
}
