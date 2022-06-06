#ifndef CUB3D_H
# define CUB3D_H
#include "mlx/mlx.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define X_EVENT_KEY_PRESS	2
#define X_EVENT_KEY_EXIT	17
#define texWidth 64
#define texHeight 64
#define mapWidth 24
#define mapHeight 24
#define width 1920
#define height 1080
# define K_A 0
# define K_D 2
# define K_S 1
# define K_W 13
# define K_AR_L 123
# define K_AR_R 124
# define K_ESC 53



typedef struct s_img
{
	void *img;
	int *data;
	int sizeLine;
	int bpp;
	int endian;
	int img_width;
	int img_height;
}	t_img;

typedef struct s_info
{
	double posX;
	double posY;
	double dirX;
	double dirY;
	double planeX;
	double planeY;
	void *mlx;
	void *win;
	t_img img;
	int **buf;
	int **texture;
	double moveSpeed;
	double rotSpeed;
} t_info;


void	calc(t_info *info);

int	key_exit(int key, t_info *info);
int	key_press(int key, t_info *info);


#endif