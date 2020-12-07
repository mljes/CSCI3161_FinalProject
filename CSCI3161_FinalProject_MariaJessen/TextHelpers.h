#pragma once
char weatherTextSnow[] = "SNOW ";
char weatherTextClear[] = "SUN  ";
char weatherTextRain[] = "RAIN ";

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

void setNumericalText(char line[18], GLfloat height, GLfloat factor) {
	GLint result = height * factor;
	result = result > 9999 ? 9999 : result;

	char insertedText[10];

	sprintf_s(&insertedText, sizeof(insertedText), "%d", result);

	int i;
	for (i = 0; i < 4 - (4 - sizeof(result)); i++) {
		line[i + 10 + (4 - sizeof(result))] = insertedText[i];
	}
}