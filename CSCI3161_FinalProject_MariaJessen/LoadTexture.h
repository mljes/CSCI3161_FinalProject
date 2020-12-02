#pragma once

#define SKY_WIDTH 896
#define SKY_HEIGHT 385

#define SEA_WIDTH 1600
#define SEA_HEIGHT 1200

#define MOUNT_WIDTH 1280 
#define MOUNT_HEIGHT 1104

GLubyte* imageData;
GLubyte skyTexture[SKY_HEIGHT][SKY_WIDTH][4];
GLubyte seaTexture[SEA_HEIGHT][SEA_WIDTH][4];
GLubyte mountTexture[MOUNT_HEIGHT][MOUNT_WIDTH][4];

void addPixelToSkyTexture(int row, int column, int red, int green, int blue) {
	skyTexture[column][row][0] = (GLubyte)red;
	skyTexture[column][row][1] = (GLubyte)green;
	skyTexture[column][row][2] = (GLubyte)blue;
	skyTexture[column][row][3] = (GLubyte)1.0;
}

void addPixelToSeaTexture(int row, int column, int red, int green, int blue) {
	seaTexture[column][row][0] = (GLubyte)red;
	seaTexture[column][row][1] = (GLubyte)green;
	seaTexture[column][row][2] = (GLubyte)blue;
	seaTexture[column][row][3] = (GLubyte)1.0;
}

void addPixelToMountTexture(int row, int column, int red, int green, int blue) {
	mountTexture[column][row][0] = (GLubyte)red;
	mountTexture[column][row][1] = (GLubyte)green;
	mountTexture[column][row][2] = (GLubyte)blue;
	mountTexture[column][row][3] = (GLubyte)1.0;
}

void loadImage(char* filename) {
	int imageRows = 0;
	int imageCols = 0;

	FILE* fileID;

	errno_t err = fopen_s(&fileID, filename, "r");

	if (fileID == 0) {
		printf("AN ERROR OCCURRED WHEN TRYING TO READ %s\n", filename);
		return;
	}

	int maxValue;
	int totalPixels;
	char tempChar;

	int i;

	char headerLine[1000] = {0};

	int red, green, blue;

	headerLine[0] = fgetc(fileID);
	headerLine[1] = fgetc(fileID);

	if ((headerLine[0] != 'P') || (headerLine[1] != '3')) {
		printf("\"%s\" IS NOT A PPM FILE\n", filename);
		return;
	}

	fgetc(fileID); // get rid of \n

	fscanf_s(fileID, "%c", &tempChar, 1);

	while (tempChar == '#') {
		char toDiscard = 0;
		
		while (toDiscard != '\n') {
			fscanf_s(fileID, "%c", &toDiscard, 1);
			printf("%c", toDiscard);
		}

		printf("\n");
		
		//tempChar = fgetc(fileID);
		fscanf_s(fileID, "%c", &tempChar, 1);
	}

	ungetc(tempChar, fileID);

	fscanf_s(fileID, "%d %d %d", &imageRows, &imageCols, &maxValue);

	totalPixels = imageRows * imageCols;

	int row = imageRows - 1;
	int column = imageCols - 1;

	for (i = 0; i < totalPixels; i++) {
		fscanf_s(fileID, "%d %d %d", &red, &green, &blue);

		if (strcmp(filename, "sky08.ppm") == 0) addPixelToSkyTexture(row, column, red, green, blue);
		if (strcmp(filename, "sea02.ppm") == 0) addPixelToSeaTexture(row, column, red, green, blue);
		if (strcmp(filename, "mount03.ppm") == 0) addPixelToMountTexture(row, column, red, green, blue);

		if (row > 0) {
			row--;
		}
		else {
			row = imageRows - 1;
			column--;
		}
	}

	fclose(fileID);
}

