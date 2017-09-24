#ifndef LCD_H
#define LCD_H
#endif

#ifdef __cplusplus
extern "C" {
#endif
    
void lcd_cmd(char command);
void lcd_init(void);
void lcd_setCursor(char x, char y);
void lcd_printChar(char myChar);
void lcd_printStr(const char* s, int line);
void lcd_animate(const char* s, int length);
void lcd_setup(void);

#ifdef __cplusplus 
}
#endif