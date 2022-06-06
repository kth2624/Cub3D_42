#include "../mlx/mlx.h"
#include "key_macos.h"
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

int	worldMap[mapWidth][mapHeight] =
{
	{4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,7,7,7,7,7,7,7,7},
	{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7},
	{4,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7},
	{4,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7},
	{4,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7},
	{4,0,4,0,0,0,0,5,5,5,5,5,5,5,5,5,7,7,0,7,7,7,7,7},
	{4,0,5,0,0,0,0,5,0,5,0,5,0,5,0,5,7,0,0,0,7,7,7,1},
	{4,0,6,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,0,0,0,8},
	{4,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,7,7,1},
	{4,0,8,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,0,0,0,8},
	{4,0,0,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,7,7,7,1},
	{4,0,0,0,0,0,0,5,5,5,5,0,5,5,5,5,7,7,7,7,7,7,7,1},
	{6,6,6,6,6,6,6,6,6,6,6,0,6,6,6,6,6,6,6,6,6,6,6,6},
	{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
	{6,6,6,6,6,6,0,6,6,6,6,0,6,6,6,6,6,6,6,6,6,6,6,6},
	{4,4,4,4,4,4,0,4,4,4,6,0,6,2,2,2,2,2,2,2,3,3,3,3},
	{4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,0,0,0,2},
	{4,0,0,0,0,0,0,0,0,0,0,0,6,2,0,0,5,0,0,2,0,0,0,2},
	{4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,2,0,2,2},
	{4,0,6,0,6,0,0,0,0,4,6,0,0,0,0,0,5,0,0,0,0,0,0,2},
	{4,0,0,5,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,2,0,2,2},
	{4,0,6,0,6,0,0,0,0,4,6,0,6,2,0,0,5,0,0,2,0,0,0,2},
	{4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,0,0,0,2},
	{4,4,4,4,4,4,4,4,4,4,1,1,1,2,2,2,2,2,2,3,3,3,3,3}
};

void	calc(t_info *info)
{
	int	x;

	x = 0;
	while (x < width)
	{
		double cameraX = 2 * x / (double)width - 1;//스크린 왼쪽 -1, 중앙 0, 오른쪽 1
		double rayDirX = info->dirX + info->planeX * cameraX;
		double rayDirY = info->dirY + info->planeY * cameraX;
		//rayDir = 광선의 방향벡터
		//광선의 방향은 (방향벡터) + (카메라평면 * 배수)로 구할 수 있음

		//mapX,Y는 현재 광선의 위치, 광선이 있는 한 칸
		int mapX = (int)info->posX;
		int mapY = (int)info->posY;

		//length of ray from current position to next x or y-side
		//시작위치에서 처음 만나는 x, y의 면
		double sideDistX;
		double sideDistY;

		//length of ray from one x or y-side to next x or y-side
		//첫번째 x,y면에서 바로 다음 x,y면 (1만큼 이동)
		double deltaDistX = fabs(1 / rayDirX);
		double deltaDistY = fabs(1 / rayDirY);
		double perpWallDist;//나중에 광선의 이동거리를 계산하는데 사용
		//어안효과죽일때 사용함

		//DDA알고리즘은 반복문 실행할 때마다 x, y방향으로 딱 한 칸씩 점프

		//what direction to step in x or y-direction (either +1 or -1)
		//광선의 방향에 따라 어느 방향으로 건너뛰는지 달라지는데 그 정보는 stepX,Y에 담김
		int stepX;
		int stepY;

		int hit = 0; //was there a wall hit?
		int side; //was a NS or a EW wall hit?

		if (rayDirX < 0)
		{
			stepX = -1;
			sideDistX = (info->posX - mapX) * deltaDistX;
		}
		else
		{
			stepX = 1;
			sideDistX = (mapX + 1.0 - info->posX) * deltaDistX;
		}
		if (rayDirY < 0)
		{
			stepY = -1;
			sideDistY = (info->posY - mapY) * deltaDistY;
		}
		else
		{
			stepY = 1;
			sideDistY = (mapY + 1.0 - info->posY) * deltaDistY;
		}
		//hit 종료조건 벽의 x,y면과 부딪쳤는지 여부
		while (hit == 0)
		{
			//jump to next map square, OR in x-direction, OR in y-direction
			//x에 부딪혔
			if (sideDistX < sideDistY)
			{
				sideDistX += deltaDistX;
				mapX += stepX;
				side = 0;
			}
			else
			{
				sideDistY += deltaDistY;
				mapY += stepY;
				side = 1;
			}
			//Check if ray has hit a wall
			if (worldMap[mapX][mapY] > 0) hit = 1;
		}
		//어안효과 죽이기
		if (side == 0)
			perpWallDist = (mapX - info->posX + (1 - stepX) / 2) / rayDirX;
		else
			perpWallDist = (mapY - info->posY + (1 - stepY) / 2) / rayDirY;

		//Calculate height of line to draw on screen
		int lineHeight = (int)(height / perpWallDist);

		//calculate lowest and highest pixel to fill in current stripe
		int drawStart = -lineHeight / 2 + height / 2;
		if(drawStart < 0)
			drawStart = 0;
		int drawEnd = lineHeight / 2 + height / 2;
		if(drawEnd >= height)
			drawEnd = height - 1;

		//texturing calculations
		int texNum = worldMap[mapX][mapY];

		//calculate value of wallX
		double wallX;
		if(side == 0)
			wallX = info->posY + perpWallDist * rayDirY;
		else
			wallX = info->posX + perpWallDist * rayDirX;
		wallX -= floor(wallX);

		//x coordinate on the texture
		int texX = (int)(wallX * (double)texWidth);
		if(side == 0 && rayDirX > 0)
			texX = texWidth - texX - 1;
		if(side == 1 && rayDirY < 0)
			texX = texWidth - texX - 1;

		double step = 1.0 * texHeight / lineHeight;

		double texPos = (drawStart - height / 2 + lineHeight / 2) * step;


		for(int y = 0; y < height; y++)
		{
			if(y >= drawStart && y < drawEnd)
			{
				int texY = (int)texPos & (texHeight - 1);
				texPos += step;
				int color = info->texture[texNum][texHeight * texY + texX];
				if(side == 1)
					color = (color >> 1)&8355711;
				info->buf[y][x] = color;
			}
			else if(y < drawStart)
				info->buf[y][x] = 0xb640ff;
			else
				info->buf[y][x] = 0x000000;
		}

		x++;
	}
}

void draw(t_info *info)
{
	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			info->img.data[y * width + x] = info->buf[y][x];
		}
	}
	mlx_put_image_to_window(info->mlx, info->win, info->img.img, 0, 0);
}



int	main_loop(t_info *info)
{
	calc(info);
	draw(info);
	return (0);
}

int	key_press(int key, t_info *info)
{
	if (key == K_W)
	{
		if (!worldMap[(int)(info->posX + info->dirX * info->moveSpeed)][(int)(info->posY)])
			info->posX += info->dirX * info->moveSpeed;
		if (!worldMap[(int)(info->posX)][(int)(info->posY + info->dirY * info->moveSpeed)])
			info->posY += info->dirY * info->moveSpeed;
	}
	//move backwards if no wall behind you
	if (key == K_S)
	{
		if (!worldMap[(int)(info->posX - info->dirX * info->moveSpeed)][(int)(info->posY)])
			info->posX -= info->dirX * info->moveSpeed;
		if (!worldMap[(int)(info->posX)][(int)(info->posY - info->dirY * info->moveSpeed)])
			info->posY -= info->dirY * info->moveSpeed;
	}
	//rotate to the right
	if (key == K_D)
	{
		//both camera direction and camera plane must be rotated
		double oldDirX = info->dirX;
		info->dirX = info->dirX * cos(-info->rotSpeed) - info->dirY * sin(-info->rotSpeed);
		info->dirY = oldDirX * sin(-info->rotSpeed) + info->dirY * cos(-info->rotSpeed);
		double oldPlaneX = info->planeX;
		info->planeX = info->planeX * cos(-info->rotSpeed) - info->planeY * sin(-info->rotSpeed);
		info->planeY = oldPlaneX * sin(-info->rotSpeed) + info->planeY * cos(-info->rotSpeed);
	}
	//rotate to the left
	if (key == K_A)
	{
		//both camera direction and camera plane must be rotated
		double oldDirX = info->dirX;
		info->dirX = info->dirX * cos(info->rotSpeed) - info->dirY * sin(info->rotSpeed);
		info->dirY = oldDirX * sin(info->rotSpeed) + info->dirY * cos(info->rotSpeed);
		double oldPlaneX = info->planeX;
		info->planeX = info->planeX * cos(info->rotSpeed) - info->planeY * sin(info->rotSpeed);
		info->planeY = oldPlaneX * sin(info->rotSpeed) + info->planeY * cos(info->rotSpeed);
	}
	if (key == K_ESC)
		exit(0);
	return (0);
}

void load_image(t_info *info, int*texture, char *path, t_img *img)
{
	img->img = mlx_xpm_file_to_image(info->mlx, path, &img->img_width, &img->img_height);
	img->data = (int *)mlx_get_data_addr(img->img, &img->bpp, &img->sizeLine, &img->endian);
	for(int y = 0; y < img->img_height; y++)
	{
		for(int x = 0; x < img->img_width; x++)
		{
			texture[img->img_width * y + x] = img->data[img->img_width * y + x];
		}
	}
	mlx_destroy_image(info->mlx, img->img);
}

void load_texture(t_info *info)
{
	t_img img;

	load_image(info, info->texture[0], "textures/eagle.xpm", &img);
	load_image(info, info->texture[1], "textures/redbrick.xpm", &img);
	load_image(info, info->texture[2], "textures/purplestone.xpm", &img);
	load_image(info, info->texture[3], "textures/greystone.xpm", &img);
	load_image(info, info->texture[4], "textures/bluestone.xpm", &img);
	load_image(info, info->texture[5], "textures/mossy.xpm", &img);
	load_image(info, info->texture[6], "textures/wood.xpm", &img);
	load_image(info, info->texture[7], "textures/colorstone.xpm", &img);

}
int	main(void)
{
	t_info info;
	info.mlx = mlx_init();

	info.posX = 22.0;
	info.posY = 11.5;
	info.dirX = -1.0;
	info.dirY = 0.0;
	info.planeX = 0.0;
	info.planeY = 0.66;
	info.moveSpeed = 0.05;
	info.rotSpeed = 0.05;


	info.buf = (int **)malloc(sizeof(int *) * height);
	for (int i = 0; i < height; i++)
	{
		info.buf[i] = (int *)malloc(sizeof(int) * width);
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			info.buf[i][j] = 0;
		}
	}
	
	if(!(info.texture = (int **)malloc(sizeof(int *) * 8)))
		return -1;
	for(int i = 0; i < 8; i++)
	{
		if(!(info.texture[i] = (int *)malloc(sizeof(int) * (texHeight * texWidth))))
			return -1;
	}
	

	for(int i = 0; i < 8; i++)
	{
		for(int j = 0; j < texHeight * texWidth; j++)
		{
			info.texture[i][j] = 0;
		}
	}
	load_texture(&info);
	
	printf("04\n");

	info.win = mlx_new_window(info.mlx, width, height, "mlx");

	info.img.img = mlx_new_image(info.mlx, width, height);
	info.img.data = (int *)mlx_get_data_addr(info.img.img, &info.img.bpp, &info.img.sizeLine, &info.img.endian);
	printf("05\n");

	mlx_loop_hook(info.mlx, &main_loop, &info);
	printf("06\n");

	mlx_hook(info.win, X_EVENT_KEY_PRESS, 0, &key_press, &info);
	printf("07\n");

	mlx_loop(info.mlx);
}
