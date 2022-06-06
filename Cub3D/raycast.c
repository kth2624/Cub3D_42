#include "cub3d.h"

extern int worldMap[mapWidth][mapHeight];

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
				//int color = info->texture[texNum][texHeight * texY + texX];
				int color;
				if (side == 1 && stepY == 1)
					color = info->texture[3][texHeight * texY + texX];
				else if (side == 1 && stepY == -1)
					color = info->texture[4][texHeight * texY + texX];
				else if (side == 0 && stepX == 1)
					color = info->texture[5][texHeight * texY + texX];
				else if (side == 0 && stepX == -1)
					color = info->texture[6][texHeight * texY + texX];
				else
					color = (color >> 1)&8315711;
				info->buf[y][x] = color;
			}
			else if(y < drawStart)//천장
				info->buf[y][x] = 0xb640ff;
			else//바닥
				info->buf[y][x] = 0x000000;
		}
		x++;
	}
}