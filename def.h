#define dim(array) (sizeof(array)/sizeof(*array))

#define SCREENW 640
#define SCREENH 480

#define FPS 30 /* frame per seconds */
#define MSPF (1000/FPS) /* milliseconds per frame */

#define BLOCKW 8
#define BLOCKH 8
#define SCREENROW (SCREENW/BLOCKW)
#define SCREENCOL (SCREENH/BLOCKH)