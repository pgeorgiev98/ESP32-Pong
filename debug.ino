#include "debug.h"
#include "display.h"

void halt() {
	for (;;) delay(10000);
}

void die(const char *errorMessage, const char *file, int line) {
	lcd.fillScreen(255 >> 3); // Blue
	lcd.setColor(255, 255, 255);
	lcd.setBackColor(0, 0, 255);

	lcd.print(errorMessage, CENTER, 100);
	char buf[255];
	sprintf(buf, "%s:%d", file, line);
	lcd.print(buf, CENTER, 200);

	halt();
}
