#include "3dEngine.h"

#include <stdio.h>
#include <stdlib.h>

#include "debug/debug.h"

#define SCR_HEIGHT 30
#define BL_SHIFT 6
#define BLOCK_SIZE 64

#define FOV 70.0f // degrees
#define FOV_RAD (FOV * (M_PI / 180.0f))

float dist(float ax, float ay, float bx, float by, float ang) {
    return (sqrt((bx - ax) * (bx - ax) + (by - ay) * (by - ay)));
}


float cast_ray(int (*map)[10], Player_info* player, float angle, int col, int rows)
{
    int mapX, mapY, depthOfField;
    float rayX, rayY, yOffset, xOffset, disT;
    int colour = 1;


    float rayAngle = angle * (M_PI / 180.0f);
    if (rayAngle > M_PI) rayAngle -= 2 * M_PI;
    if (rayAngle <= -M_PI) rayAngle += 2 * M_PI;
    //int rayStartX = (player->x >> 6) << 6;
    //int rayStartY = player->y;
    //rayStartX >>= 6;

    //Start detecting horizontal lines

    depthOfField = 0;
    float disH = 50000, hx = player->x, hy = player->y;
    float aTan = -1/tan(rayAngle);
    if (rayAngle < 0 && rayAngle > -M_PI) {
	rayY = (((int)player->y >> 6) << 6) - 0.0001;
	rayX = (player->y - rayY) * aTan + player->x;
	yOffset = -64;
	xOffset = -yOffset * aTan;
    } else if (rayAngle > 0 && rayAngle < M_PI) {
	rayY = (((int)player->y >> 6) << 6) + 64;
	rayX = (player->y - rayY) * aTan + player->x;
	yOffset = 64;
	xOffset = -yOffset * aTan;
    } else if (rayAngle == 0 || rayAngle == M_PI || rayAngle == M_PI) {
	rayY = player->y;
	rayX = player->x;
	depthOfField = 8;
    }

    while (depthOfField < 8) {
	mapX = (int)(rayX) >> 6;
	mapY = (int)(rayY) >> 6;
	if (mapX >= 0 && mapX <= 9 && mapY >= 0 && mapY <= 9 && map[mapY][mapX] > 0) {
	    hx = rayX;
	    hy = rayY;
	    disH = dist(player->x, player->y, hx, hy, rayAngle);
	    depthOfField = 8;
	    colour = map[mapY][mapX];
	    //printf("disH: %f\n", disH);
	} else {
	    rayX += xOffset;
	    rayY += yOffset;
	    depthOfField++;
	}
    }


    //Start detecting vertical lines
    depthOfField = 0;
    float disV = 50000, vx = player->x, vy = player->y;
    float nTan = -tan(rayAngle);
    if (rayAngle < -M_PI / 2 || rayAngle > M_PI / 2) {
	rayX = (((int)player->x >> 6) << 6) - 0.0001;
	rayY = (player->x - rayX) * nTan + player->y;
	xOffset = -64;
	yOffset = -xOffset * nTan;
    } else if (rayAngle < M_PI / 2 || rayAngle > -M_PI / 2) {
	rayX = (((int)player->x >> 6) << 6) + 64;
	rayY = (player->x - rayX) * nTan + player->y;
	xOffset = 64;
	yOffset = -xOffset * nTan;
    } else if (rayAngle == M_PI / 2 || rayAngle == -M_PI / 2) {
	rayY = player->y;
	rayX = player->x;
	depthOfField = 8;
    }

    while (depthOfField < 8) {
	mapX = (int)(rayX) >> 6;
	mapY = (int)(rayY) >> 6;
	if (mapX >= 0 && mapX <= 9 && mapY >= 0 && mapY <= 9 && map[mapY][mapX] > 0) {
	    vx = rayX;
	    vy = rayY;
	    disV = dist(player->x, player->y, vx, vy, rayAngle);
	    depthOfField = 8;
	    colour = map[mapY][mapX];
	    //printf("disV: %f\n", disV);
	} else {
	    rayX += xOffset;
	    rayY += yOffset;
	    depthOfField++;
	}
    }
    char thing = 'Z';

    if (disV < disH) {
	rayX = vx;
	rayY = vy;
	disT = disV;
	thing = 'X';
    } else if (disH < disV) {
    //} else {
	rayX = hx;
	rayY = hy;
	disT = disH;
	thing = 'O';
    } else {
	return 0; //NEED TO FIX
    }
	//printf("%f, %f\n", disH, disV);

    float radPAngle = player->angle * (M_PI / 180);
    float cAngle = radPAngle - rayAngle;

    if (cAngle > M_PI) cAngle -= 2 * M_PI;
    if (cAngle < -M_PI) cAngle += 2 * M_PI;

    disT *= cos(cAngle);
    int lineH = abs((64 * rows) / disT); //REPLACE 44 WITH SCREEN HEIGHT
    if (lineH > rows) {
	lineH = rows;
    }
    
    //DRAW THE LINE, STARTING AT CORRECT POINT
    //printf("map thing: %d\n", map[0][3]);
    //int drawX = 40 + angle - player->angle;
//    if (drawX > 180) drawX -= 360;
//    if (drawX < -180) drawX += 360;
    int drawX = col;
    //printf("disT = %f, lineH = %d\n", disT, lineH);
    //printf("%d\n", drawX);
    int lineOffset = (rows / 2) - (lineH >> 1);
    render_line(drawX, lineOffset, lineH, thing, colour);
    //printf("%f\n", disT);
    return disT;
}

float draw_all_stuff(int (*map)[10], Player_info* player, int cols, int rows, Sprite** sprites)
{
    float zBuffer[cols];

    for (int col = 0; col < cols; col++) {
	float rayAngle = (player->angle - (FOV / 2.0f)) + ((float)col / cols) * FOV;
	zBuffer[col] = cast_ray(map, player, rayAngle, col, rows);  // Pass exact screen column
    }
    for (int spriteNum = 0; spriteNum < 1; spriteNum++) { //NEED TO CHANGE 1 TO THE NUMBER OF SPRITES
	(*sprites)[spriteNum].distanceToPlayer = 
	    dist((float)player->x, (float)player->y, (float)(*sprites)[spriteNum].x, (float)(*sprites)[spriteNum].x, 0);
    }
    //NEED TO QSORT ARRAY BASED ON DISTANCE FROM PLAYER, FURTHEST AWAY SHOULD BE FIRST
    debug_init();
    for (int spriteNum = 0; spriteNum < 1; spriteNum++) { //ALSO NEED TO MAKE 1 INTO NUMBER OF SPRITES
	int xDiff = sprites[spriteNum]->x - player->x;
	int yDiff = sprites[spriteNum]->y - player->y;
	float angFromPlayer = atan2(-yDiff, xDiff) * (180 / M_PI);
	if (angFromPlayer < -180) {
	    angFromPlayer += 360;
	} else if (angFromPlayer > 180) {
	    angFromPlayer -= 360;
	}
	int q = player->angle + (FOV / 2) - angFromPlayer;
	if (player->angle > 90 && q < -90) {
	    //q += 360;
	}
	if (player->angle < -90 && q > 90) {
	    //q -= 360;
	}
	int spriteScreenX = abs((cols / FOV) * q);
	int spriteScreenY = rows / 4;
	int spriteDist = sqrt((xDiff * xDiff) + (yDiff * yDiff));
	int spriteHeight = abs((rows * 64) / spriteDist);
	int spriteWidth = spriteHeight;
	int startCol = spriteScreenX - spriteWidth / 2;
	if (startCol < 0) {
	    startCol = 0;
	}
	int endCol = spriteScreenX + spriteWidth / 2;
	if (endCol > cols) {
	    endCol = cols;
	}
	debug_print("ssX: %d, sW/2: %d\n", spriteScreenX, spriteWidth / 2);
	//debug_print("startCol: %d, endCol: %d\n", startCol, endCol);
	for (int col = startCol; col < endCol; col++) {
	    if (zBuffer[col] > spriteDist) {
		debug_print("col: %d, y thing: %d, size: %d\n", col, spriteScreenY - spriteWidth / 2, spriteWidth);
		render_line(col, spriteScreenY - spriteWidth / 2, spriteWidth, 'B', 1);
	    }
	}
    }
    debug_close();
}
