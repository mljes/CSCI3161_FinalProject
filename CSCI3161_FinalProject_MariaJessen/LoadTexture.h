#pragma once
#define TEXTURE_WIDTH 64
#define TEXTURE_HEIGHT 64

#define SKY_WIDTH 896
#define SKY_HEIGHT 385

#define SEA_WIDTH 1600
#define SEA_HEIGHT 1200

#define MOUNT_WIDTH 1280 
#define MOUNT_HEIGHT 1104

GLubyte* imageData;
GLubyte skyTexture[SKY_HEIGHT][SKY_WIDTH][4];
GLubyte seaTexture[SEA_HEIGHT][SEA_WIDTH][4];
GLubyte mountTexture[MOUNT_WIDTH][MOUNT_HEIGHT][4];

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

void loadImage(char* filename)
{
	int imageRows = 0;
	int imageCols = 0;
	// the ID of the image file
	FILE* fileID;
	// open the image file for reading
	errno_t err = fopen_s(&fileID, filename, "r");

	if (fileID == 0) {
		printf("AN ERROR OCCURRED WHEN TRYING TO READ %s\n", filename);
		return;
	}

	// maxValue
	int  maxValue;

	// total number of pixels in the image
	int  totalPixels;

	// temporary character
	char tempChar;

	// counter variable for the current pixel in the image
	int i;

	// array for reading in header information
	char headerLine[100];

	// if the original values are larger than 255
	float RGBScaling;

	// temporary variables for reading in the red, green and blue data of each pixel
	int red, green, blue;

	// read in the first header line (original didn't work, switched to just getting first 2 chars)
	headerLine[0] = fgetc(fileID);
	headerLine[1] = fgetc(fileID);

	// make sure that the image begins with 'P3', which signifies a PPM file
	if ((headerLine[0] != 'P') || (headerLine[1] != '3'))
	{
		printf("This is not a PPM file!\n");
		exit(0);
	}

	// read in the first character of the next line
	fscanf_s(fileID, "%c", &tempChar);

	// while we still have comment lines (which begin with #)
	while (tempChar == '#')
	{
		// read in the comment
		fscanf_s(fileID, "%[^\n] ", &headerLine);

		// print the comment
		printf("%s\n", headerLine);

		// read in the first character of the next line
		fscanf_s(fileID, "%c", &tempChar);
	}

	// the last one was not a comment character '#', so we need to put it back into the file stream (undo)
	ungetc(tempChar, fileID);

	// read in the image height, width and the maximum value
	fscanf_s(fileID, "%d %d %d", &imageRows, &imageCols, &maxValue);

	// compute the total number of pixels in the image
	totalPixels = imageRows * imageCols;

	int row =  imageRows - 1;
	int column = imageCols - 1;
	for (i = 0; i < totalPixels; i++)
	{
		// read in the current pixel from the file
		fscanf_s(fileID, "%d %d %d", &red, &green, &blue);

		if (strcmp(filename, "sky08.ppm") == 0) addPixelToSkyTexture(row, column, red, green, blue);
		if (strcmp(filename, "sea02.ppm") == 0) addPixelToSeaTexture(row, column, red, green, blue);
		if (strcmp(filename, "mount03.ppm") == 0) addPixelToMountTexture(row, column, red, green, blue);

		if (row > 0)
			row--;
		else {
			row = imageRows - 1;
			column--;
		}
	}

	// close the image file
	fclose(fileID);
}