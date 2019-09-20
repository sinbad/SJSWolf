// Header file for the raycasting and texturing side of the engine

// Typedefs
typedef long fixed; 
typedef int map_type[16][16];
enum boolean {false, true};

// Constants
const double PI=3.141592654;
const IMAGE_WIDTH=64;
const IMAGE_HEIGHT=64;
const CIRCLE_DIVISIONS=(2048);
const WALL_HEIGHT=64;           // Height of wall in pixels
const VIEWER_DISTANCE=160;      // Viewer distance from screen
const VIEWPORT_LEFT=0;          // Dimensions of viewport
const VIEWPORT_RIGHT=319;
const VIEWPORT_TOP=0;
const VIEWPORT_BOT=199;
const VIEWPORT_HEIGHT=VIEWPORT_BOT-VIEWPORT_TOP;
const VIEWPORT_CENTER=VIEWPORT_TOP+VIEWPORT_HEIGHT/2;



// Function prototypes
void draw_maze(map_type map,char *screen,fixed xview,fixed yview,
                       int viewing_angle,int viewer_height,
                       unsigned char *textmaps,
                       int do_walls,int do_floor,int do_ciel);
void trigtables(void);
extern "C" scalecol(int height, unsigned char *textmaps,
                       char *buffer, int dheight, unsigned char *lightptr);
extern "C" floorline(fixed x_start, fixed y_start, fixed x_increment,
                      fixed y_increment, unsigned char *textmaps,
                      char *buffer, unsigned char *lightptr, int wall_bots[], int row);


void setlighting(unsigned char *palette);

