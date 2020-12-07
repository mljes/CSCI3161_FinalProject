#pragma once
/*

CSCI3161 FINAL PROJECT
MARIA JESSEN B00743170

Functions to aid drawing text

*/

// strings for different weather conditions
char weatherTextSnow[] = "SNOW ";
char weatherTextClear[] = "SUN  ";
char weatherTextRain[] = "RAIN ";

/// <summary>
/// Set the current weather condiions indicator based on weather mode enabled
/// </summary>
/// <param name="line"></param>
/// <param name="showSnow"></param>
/// <param name="showRain"></param>
void setWeatherText(char line[18], GLboolean showSnow, GLboolean showRain) {
	int i;
	for (i = 0; i < 5; i++) {
		if (showSnow) {
			line[i + 10] = weatherTextSnow[i];
		}
		else if (showRain) {
			line[i + 10] = weatherTextRain[i];
		}
		else {
			line[i + 10] = weatherTextClear[i];
		}
	}
}

/// <summary>
/// Set altimeter or speedometer text
/// </summary>
/// <param name="line"></param>
/// <param name="height"></param>
/// <param name="factor"></param>
void setNumericalText(char line[18], GLfloat height, GLfloat factor) {
	GLint result = height * factor; // get phoney speed/height
	result = result > 9999 ? 9999 : result; // don't try to print beyond 4 digits

	char insertedText[10];

	sprintf_s(&insertedText, sizeof(insertedText), "%d", result); // convert int to string

	int i;
	for (i = 0; i < 4 - (4 - sizeof(result)); i++) { // put number string into info text string
		line[i + 10 + (4 - sizeof(result))] = insertedText[i];
	}
}