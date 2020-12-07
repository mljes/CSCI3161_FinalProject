#pragma once
/*

CSCI3161 FINAL PROJECT
MARIA JESSEN B00743170

Functions and arrays for loading and storing textures from PPM files

*/

// Texture dimensions

#define SKY_WIDTH 896
#define SKY_HEIGHT 385

#define ALT_SKY_WIDTH 318
#define ALT_SKY_HEIGHT 159

#define SEA_WIDTH 1600
#define SEA_HEIGHT 1200

#define MOUNT_WIDTH 1280 
#define MOUNT_HEIGHT 1104

// Texture filenames
#define SKY_TEXTURE_FILENAME "sky08.ppm"
#define SEA_TEXTURE_FILENAME "sea02.ppm"
#define MOUNT_TEXTURE_FILENAME "mount03.ppm"
#define ALT_SKY_TEXTURE_FILENAME "alt_sky.ppm"

// Arrays to store texels
GLubyte skyTexture[SKY_HEIGHT][SKY_WIDTH][4];
GLubyte seaTexture[SEA_HEIGHT][SEA_WIDTH][4];
GLubyte mountTexture[MOUNT_HEIGHT][MOUNT_WIDTH][4];
GLubyte altSkyTexture[ALT_SKY_HEIGHT][ALT_SKY_WIDTH][4];

/// <summary>
/// Save current colors to sky texture
/// </summary>
/// <param name="row"></param>
/// <param name="column"></param>
/// <param name="red"></param>
/// <param name="green"></param>
/// <param name="blue"></param>
void addPixelToSkyTexture(int row, int column, int red, int green, int blue) {
	skyTexture[column][row][0] = (GLubyte)red;
	skyTexture[column][row][1] = (GLubyte)green;
	skyTexture[column][row][2] = (GLubyte)blue;
	skyTexture[column][row][3] = (GLubyte)1.0;
}

/// <summary>
/// Save current colors to alternate sky texture
/// </summary>
/// <param name="row"></param>
/// <param name="column"></param>
/// <param name="red"></param>
/// <param name="green"></param>
/// <param name="blue"></param>
void addPixelToAltSkyTexture(int row, int column, int red, int green, int blue) {
	altSkyTexture[column][row][0] = (GLubyte)red;
	altSkyTexture[column][row][1] = (GLubyte)green;
	altSkyTexture[column][row][2] = (GLubyte)blue;
	altSkyTexture[column][row][3] = (GLubyte)1.0;
}

/// <summary>
/// Save current colors to sea texture
/// </summary>
/// <param name="row"></param>
/// <param name="column"></param>
/// <param name="red"></param>
/// <param name="green"></param>
/// <param name="blue"></param>
void addPixelToSeaTexture(int row, int column, int red, int green, int blue) {
	seaTexture[column][row][0] = (GLubyte)red;
	seaTexture[column][row][1] = (GLubyte)green;
	seaTexture[column][row][2] = (GLubyte)blue;
	seaTexture[column][row][3] = (GLubyte)1.0;
}

/// <summary>
/// Save current colors to mountain texture
/// </summary>
/// <param name="row"></param>
/// <param name="column"></param>
/// <param name="red"></param>
/// <param name="green"></param>
/// <param name="blue"></param>
void addPixelToMountTexture(int row, int column, int red, int green, int blue) {
	mountTexture[column][row][0] = (GLubyte)red;
	mountTexture[column][row][1] = (GLubyte)green;
	mountTexture[column][row][2] = (GLubyte)blue;
	mountTexture[column][row][3] = (GLubyte)1.0;
}

/// <summary>
/// Load an image from a file into a texture array. Based on code supplied in lectures.
/// </summary>
/// <param name="filename"></param>
void loadImage(char* filename) {
	int imageRows = 0;
	int imageCols = 0;

	FILE* fileID;

	errno_t err = fopen_s(&fileID, filename, "r"); // open the file to read

	if (fileID == 0) { // handle read errors
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

	// check if file is a PPM 
	if ((headerLine[0] != 'P') || (headerLine[1] != '3')) {
		printf("\"%s\" IS NOT A PPM FILE\n", filename);
		return;
	}

	fgetc(fileID); // get rid of \n

	fscanf_s(fileID, "%c", &tempChar, 1); // read comment character #

	while (tempChar == '#') { // read comments
		char toDiscard = 0;
		
		while (toDiscard != '\n') { // print comments
			fscanf_s(fileID, "%c", &toDiscard, 1);
			printf("%c", toDiscard);
		}

		printf("\n");
		
		fscanf_s(fileID, "%c", &tempChar, 1);
	}

	ungetc(tempChar, fileID); // put back the last character (no longer comments)

	fscanf_s(fileID, "%d %d %d", &imageRows, &imageCols, &maxValue); // read the file and color dimensions

	totalPixels = imageRows * imageCols;

	int row = imageRows - 1;
	int column = imageCols - 1;

	// read and save the color values
	for (i = 0; i < totalPixels; i++) {
		fscanf_s(fileID, "%d %d %d", &red, &green, &blue);

		// save the color to the appropriate array based on the name of the texture file we're processing
		if (strcmp(filename, SKY_TEXTURE_FILENAME) == 0) addPixelToSkyTexture(row, column, red, green, blue);
		if (strcmp(filename, ALT_SKY_TEXTURE_FILENAME) == 0) addPixelToAltSkyTexture(row, column, red, green, blue);
		if (strcmp(filename, SEA_TEXTURE_FILENAME) == 0) addPixelToSeaTexture(row, column, red, green, blue);
		if (strcmp(filename, MOUNT_TEXTURE_FILENAME) == 0) addPixelToMountTexture(row, column, red, green, blue);

		// to avoid having to rotate the texture later, just fill array backwards
		if (row > 0) {
			row--; // move back a row in the array
		}
		else {
			row = imageRows - 1; // go back to first row in array
			column--; // go backwards a column in the array
		}
	}

	fclose(fileID); // close the file
}

