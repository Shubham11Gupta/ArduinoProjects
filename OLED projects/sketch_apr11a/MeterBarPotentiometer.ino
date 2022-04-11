#include <U8glib.h>

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_NO_ACK | U8G_I2C_OPT_FAST);

int p=0;

void setup() {
  Serial.begin(115200);
  pinMode(A0, INPUT);
  u8g.setFont(u8g_font_tpssb);
  u8g.setColorIndex(1);
}

void loop() {
  u8g.firstPage();
  int value = analogRead(A0);
  value=value/10;
  do{
    u8g.drawStr(55,50,"Bar");
    u8g.drawFrame(0,10,128,20);
    u8g.drawBox(10,15,value,10);
  }while(u8g.nextPage());
}
//https://wokwi.com/projects/328663315274269266
