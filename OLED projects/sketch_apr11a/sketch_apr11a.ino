#include <U8glib.h>

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_NO_ACK | U8G_I2C_OPT_FAST);

int p=0;

void setup() {
  u8g.setFont(u8g_font_tpssb);
  u8g.setColorIndex(1);
}

void loop() {
  u8g.firstPage();
  do{
    u8g.drawStr(55,50,"Bar");
    u8g.drawFrame(0,10,128,20);
    u8g.drawBox(10,15,p,10);
  }while(u8g.nextPage());

  if(p<108){
    p++;
  }else{
    p=0;
  }
}
