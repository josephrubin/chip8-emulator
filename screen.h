#ifndef CHIP8_SCREEN_H
#define CHIP8_SCREEN_H

/* Initialize the screen, allocate memory for the display. */
uint8_t Screen_init(void);

/* Paint a pixel by XORing `value` onto the display at the position (`x`,`y`).
 * Return true if a pixel was cleared this way,
 * which is the system's version of collision detection. */
uint8_t Screen_paint(uint8_t x, uint8_t y, uint8_t value);

/* Clear the display, turning every pixel off. */
void Screen_clear(void);

/* Print the display in 2D form to stdout, using '#' for on pixels,
 * and ' ' for off pixels. */
void Screen_print(void);

/* Free associated resources and disable the component
 * until it is initialized again. */
void Screen_uninit(void);

#endif /* CHIP8_SCREEN_H */
