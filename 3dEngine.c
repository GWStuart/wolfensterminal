#include "3dEngine.h"

#include <stdio.h>
#include <stdlib.h>

#define SCR_HEIGHT 30

float dist(float ax, float ay, float bx, float by, float ang) {
    return (sqrt((bx - ax) * (bx - ax) + (by - ay) * (by - ay)));
}


void cast_ray(int (*map)[10], struct Player* player, int angle)
{
    int mapX, mapY, depthOfField;
    float rayX, rayY, yOffset, xOffset, disT;



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
	    //printf("disV: %f\n", disV);
	} else {
	    rayX += xOffset;
	    rayY += yOffset;
	    depthOfField++;
	}
    }
    char thing = '0';

    if (disV < disH) {
	rayX = vx;
	rayY = vy;
	disT = disV;
	thing = '#';
    } else if (disH < disV) {
    //} else {
	rayX = hx;
	rayY = hy;
	disT = disH;
	thing = '@';
    } else {
    }
	//printf("%f, %f\n", disH, disV);

    float radPAngle = player->angle * (M_PI / 180);
    float cAngle = radPAngle - rayAngle;

    if (cAngle > M_PI) cAngle -= 2 * M_PI;
    if (cAngle < -M_PI) cAngle += 2 * M_PI;

    disT *= cos(cAngle);
    int lineH = abs((64 * SCR_HEIGHT) / disT); //REPLACE 44 WITH SCREEN HEIGHT
    if (lineH > SCR_HEIGHT) {
	lineH = SCR_HEIGHT;
    }
    
    //DRAW THE LINE, STARTING AT CORRECT POINT
    //printf("map thing: %d\n", map[0][3]);
    int drawX = 40 + angle - player->angle;
    if (drawX > 180) drawX -= 360;
    if (drawX < -180) drawX += 360;
    //printf("disT = %f, lineH = %d\n", disT, lineH);
    //printf("%d\n", drawX);
    render_line(drawX, 0, lineH, thing);
    //printf("%f\n", disT);


}
