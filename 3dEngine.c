#include "3dEngine.h"

#include <stdio.h>
#include <stdlib.h>

#include "debug/debug.h"

#include <unistd.h>

#define SCR_HEIGHT 30
#define BL_SHIFT 6
#define BLOCK_SIZE 64

#define FOV 70.0f // degrees
#define FOV_RAD (FOV * (M_PI / 180.0f))

int compar(const void* a, const void* b)
{
    Sprite* aS = (Sprite*) a;
    Sprite* bS = (Sprite*) b;
    if (aS->distanceToPlayer > bS->distanceToPlayer) {
	return -1;
    }
    else if (aS->distanceToPlayer < bS->distanceToPlayer) {
	return 1;
    } else {
	return 0;
    }
}

float dist(float ax, float ay, float bx, float by, float ang) {
    return (sqrt((bx - ax) * (bx - ax) + (by - ay) * (by - ay)));
}

float cast_ray(int (*map)[20], Player_info* player, float angle, int col, int rows)
{
    const int MAP_W = 20, MAP_H = 20;
    const float TILE = 64.0f;
    const float EPS = 0.0001f;

    int mapX, mapY;
    float rayX, rayY, xOffset, yOffset;
    int colourH = 0, colourV = 0;

    // Start with "no hit" distances very large
    float disH = 1e30f, hx = player->x, hy = player->y;
    float disV = 1e30f, vx = player->x, vy = player->y;

    // Angle in radians, normalized
    float rayAngle = angle * (M_PI / 180.0f);
    while (rayAngle >  M_PI) rayAngle -= 2.0f * M_PI;
    while (rayAngle <= -M_PI) rayAngle += 2.0f * M_PI;

    // -------- HORIZONTAL grid intersections --------
    float aTan = -1.0f / tanf(rayAngle == 0.0f ? 1e-6f : rayAngle);
    if (rayAngle < 0.0f) { // looking up
        rayY = (((int)player->y >> 6) << 6) - EPS;
        rayX = (player->y - rayY) * aTan + player->x;
        yOffset = -TILE;
        xOffset = -yOffset * aTan;
    } else if (rayAngle > 0.0f) { // looking down
        rayY = (((int)player->y >> 6) << 6) + TILE;
        rayX = (player->y - rayY) * aTan + player->x;
        yOffset = TILE;
        xOffset = -yOffset * aTan;
    } else { // exactly horizontal (no horizontal crossings)
        xOffset = yOffset = 0.0f;
    }

    for (int d = 0; d < 100; ++d) {
        mapX = (int)floorf(rayX / TILE);
        mapY = (int)floorf(rayY / TILE);
        if (mapX < 0 || mapX >= MAP_W || mapY < 0 || mapY >= MAP_H) break;
        if (map[mapY][mapX] > 0) {
            hx = rayX; hy = rayY;
            disH = dist(player->x, player->y, hx, hy, 0.0f);
            colourH = map[mapY][mapX];
            break;
        }
        rayX += xOffset; rayY += yOffset;
    }

    // -------- VERTICAL grid intersections --------
    float nTan = -tanf(rayAngle);
    if (rayAngle > -M_PI/2 && rayAngle < M_PI/2) { // looking right
        rayX = (((int)player->x >> 6) << 6) + TILE;
        rayY = (player->x - rayX) * nTan + player->y;
        xOffset = TILE;
        yOffset = -xOffset * nTan;
    } else if (rayAngle < -M_PI/2 || rayAngle > M_PI/2) { // looking left
        rayX = (((int)player->x >> 6) << 6) - EPS;
        rayY = (player->x - rayX) * nTan + player->y;
        xOffset = -TILE;
        yOffset = -xOffset * nTan;
    } else { // exactly vertical (no vertical crossings)
        xOffset = yOffset = 0.0f;
    }

    for (int d = 0; d < 100; ++d) {
        mapX = (int)floorf(rayX / TILE);
        mapY = (int)floorf(rayY / TILE);
        if (mapX < 0 || mapX >= MAP_W || mapY < 0 || mapY >= MAP_H) break;
        if (map[mapY][mapX] > 0) {
            vx = rayX; vy = rayY;
            disV = dist(player->x, player->y, vx, vy, 0.0f);
            colourV = map[mapY][mapX];
            break;
        }
        rayX += xOffset; rayY += yOffset;
    }

    // Pick the nearer hit AND its colour
    float disT, disX, disY;
    int texOffset;
    bool darker;
    if (disV < disH) {
	disX = vx;
	disY = vy;
        disT = disV;
	texOffset = ((int)disY % 64) / 2;
	darker = true;
    } else {
	disX = hx;
	disY = hy;
        disT = disH;
	texOffset = ((int)disX % 64) / 2;
	darker = false;
    }
    debug_print("texOffset %d\n", texOffset);

    // Fish-eye correction
    float radPAngle = player->angle * (M_PI / 180.0f);
    float cAngle = radPAngle - rayAngle;
    while (cAngle >  M_PI) cAngle -= 2.0f * M_PI;
    while (cAngle < -M_PI) cAngle += 2.0f * M_PI;
    disT *= cosf(cAngle);

    // Project and draw
    float denom = (disT > 1e-6f) ? disT : 1e-6f;
    int lineH = (int)fabsf((TILE * rows) / denom);
    //float scaleFactor = (float)lineH / 32.0f;
    float scaleFactor = 32.0f / (float)lineH;
    float scaleOffset = 0;
    if (lineH > rows) {
	scaleOffset = (lineH - rows) / 2.0;
	lineH = rows;
    }

    int drawX = col;
    int lineOffset = (rows / 2) - (lineH >> 1);

    //render_line(drawX, lineOffset, lineH, '#', texOffset % 5 + 1);
    render_line_texture(drawX, lineOffset, lineH, texOffset, 1, darker, scaleFactor, scaleOffset);



    return disT;
}


float draw_all_stuff(int (*map)[20], Player_info* player, int cols, int rows, Sprite** sprites, int numSprites)
{
    // 1) Build z-buffer by casting rays per column
    float zBuffer[cols];
    for (int col = 0; col < cols; col++) {
        float rayAngle = (player->angle - (FOV / 2.0f)) + ((float)col / (float)cols) * FOV;
        zBuffer[col] = cast_ray(map, player, rayAngle, col, rows);
        // Cast may return 0 on tie — treat as “very far” so sprites in front can still draw
        if (zBuffer[col] <= 0.0f) zBuffer[col] = 1e9f;
    }

    // 2) Compute sprite distances (ALWAYS use (*sprites)[i])
    for (int i = 0; i < numSprites; i++) {
        Sprite* s = &(*sprites)[i];
        s->distanceToPlayer = dist((float)player->x, (float)player->y, (float)s->x, (float)s->y, 0.0f);
    }

    // TODO: sort sprites back-to-front by distanceToPlayer if you want proper overlap.

    // 3) Project and draw sprites with clipping & depth test

    qsort(*sprites, numSprites, sizeof(Sprite), compar);

    for (int i = 0; i < numSprites; i++) {
        Sprite* s = &(*sprites)[i];
	if (!s->isExist) {
	    continue;
	}

        // Vector from player to sprite
        float dx = (float)s->x - (float)player->x;
        float dy = (float)s->y - (float)player->y;

        // Angle to sprite in degrees
        float spriteAngle = atan2f(dy, dx) * (180.0f / M_PI);

        // Difference from player facing
        float angleDiff = spriteAngle - player->angle;
        while (angleDiff > 180.0f)  angleDiff -= 360.0f;
        while (angleDiff < -180.0f) angleDiff += 360.0f;

        // Cull if outside FOV
        if (fabsf(angleDiff) > (FOV * 0.5f)) continue;

        // Distance (avoid zero to prevent div-by-zero)
        float spriteDist = sqrtf(dx*dx + dy*dy);
	// correct fish-eye
	float playerRad = player->angle * (M_PI / 180.0f);
	float spriteAngleRad = atan2f(dy, dx);
	float angleDiffRad = spriteAngleRad - playerRad;

	while (angleDiffRad > M_PI)  angleDiffRad -= 2*M_PI;
	while (angleDiffRad < -M_PI) angleDiffRad += 2*M_PI;

	spriteDist *= cosf(angleDiffRad);
        if (spriteDist < 1e-3f) spriteDist = 1e-3f;

        // Simple projection to screen X
        float screenXRatio = (angleDiff + (FOV * 0.5f)) / FOV;   // 0..1 across FOV
        int spriteScreenX = (int)(screenXRatio * cols);
        int spriteScreenY = rows / 2;  // center vertically for now

        // Apparent sprite size (scaled like your wall height calc)
        int spriteHeight = (int)((rows * 64.0f) / spriteDist);
        int spriteWidth  = spriteHeight;

        // Clip width/height to sane range
        if (spriteWidth  < 1) spriteWidth  = 1;
        if (spriteHeight < 1) spriteHeight = 1;
        if (spriteHeight > rows) spriteHeight = rows;  // vertical clamp

        // Horizontal span (clipped)
        int startCol = spriteScreenX - (spriteWidth / 2);
        int endCol   = spriteScreenX + (spriteWidth / 2);
        if (startCol < 0)    startCol = 0;
        if (endCol   > cols) endCol   = cols;

        // Vertical start for render_line (clipped so it doesn’t go negative)
        int startRow = spriteScreenY - (spriteHeight / 2);
        if (startRow < 0) startRow = 0;
        if (startRow + spriteHeight > rows) {
            spriteHeight = rows - startRow;
            if (spriteHeight <= 0) continue;
        }

        // Draw columns that pass the z-test
	int totalCols = endCol - startCol;
        for (int col = startCol; col < endCol; col++) {
            // Depth test: sprite must be in front of wall slice
            if (spriteDist < zBuffer[col]) {
                // Render a vertical slice of the sprite. Here we just reuse render_line as a solid column.
                // If you later texture sprites, you’ll sample the correct column instead of drawing a solid.
                //render_line(col, startRow, spriteHeight, 'B', 1);
		//render_player(col, startRow, col*(14.0f / (float)totalCols), spriteHeight);
		int texW;                      // your sprite texture width in columns (was hard-coded 14.0)
		switch(s->spriteType) {
		    case S_GUY:
			texW = 14;
			break;
		    case S_SGUN:
			texW = 49;
			break;
		}
		int rel = col - startCol;           // 0 .. totalCols-1
		// Map 0..totalCols-1 -> 0..texW-1 with rounding & clamping
		int texX = (int)roundf(rel * (texW - 1) / (float)(totalCols - 1));
		if (texX < 0) texX = 0;
		if (texX >= texW) texX = texW - 1;

		//switch(s->spriteType) {
		//    case S_GUY:
		//	render_player(col, startRow, texX, spriteHeight);
		//	break;
		//    case S_SGUN:
		//	render_shotgun(col, startRow, texX, spriteHeight / 4);
		//	break;
		//}

		switch(s->spriteType) {
		   case S_GUY: {
			// normal projected size
			int spriteHeight = (int)((rows * 64.0f) / spriteDist);
			int spriteWidth  = spriteHeight;
			if (spriteWidth  < 1) spriteWidth  = 1;
			if (spriteHeight < 1) spriteHeight = 1;
			if (spriteHeight > rows) spriteHeight = rows;

			int startRow = rows/2 - spriteHeight/2;
			if (startRow < 0) startRow = 0;

			int rel = col - startCol;
			int texX = (int)roundf(rel * (texW - 1) / (float)(totalCols - 1));
			if (texX < 0) texX = 0;
			if (texX >= texW) texX = texW - 1;

			render_player(col, startRow, texX, spriteHeight);
			break;
		   }

		    case S_SGUN: {
		        // Softer scaling so it doesn’t explode at close range
		        int spriteHeight = (int)((rows * 32.0f) / (spriteDist + 32));
		        int spriteWidth  = spriteHeight * 2;  // shotgun shape
		        if (spriteWidth  < 1) spriteWidth  = 1;
		        if (spriteHeight < 1) spriteHeight = 1;

		        // Compute floor anchor per column (use your wall slice height here!)
		        int spriteLineH = (int)(rows / spriteDist);
		        int wallBottom = (rows / 2) + (spriteLineH / 2);
		        if (wallBottom > rows) wallBottom = rows;

		        if (wallBottom > rows) wallBottom = rows;

		        // Anchor shotgun to the floor
		        int startRow = wallBottom - spriteHeight;

		        // Texture mapping
		        int rel = col - startCol;
		        int texW = 49; // shotgun texture width
		        int texX = (int)roundf(rel * (texW - 1) / (float)(totalCols - 1));
		        if (texX < 0) texX = 0;
		        if (texX >= texW) texX = texW - 1;

		        render_shotgun(col, startRow, texX, spriteHeight);
		        break;
		    }


		}


            }
        }
    }

    draw_hand_gun(player, rows, cols);

    return 0.0f; // function is declared float; return something (unused)
}

