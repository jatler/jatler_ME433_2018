

#ifndef I2C_HW5_H__   /* Guard against multiple inclusion */
#define I2C_HW5_H__

void initExpander(void);
void setGP0(void);
void setExpander(char pin, char level);
char getExpander(void);

#endif