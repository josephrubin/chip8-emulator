#ifndef CHIP8_SCREEN_H
#define CHIP8_SCREEN_H

/* Initialize the screen, allocate memory for the display. Return TRUE on
 * success and FALSE on error. */
enum bool Screen_init(void);

/* Paint a pixel by XORing `value` onto the display at the position (`x`,`y`).
 * Return TRUE if a pixel was cleared this way and FALSE otherwise
 * which is the system's version of collision detection. */
enum bool Screen_paint(uint8_t x, uint8_t y, uint8_t value);

/* Clear the display, turning every pixel off. */
void Screen_clear(void);

/*todo:*/
void Screen_display(void);

/* Free associated resources and disable the component
 * until it is initialized again. */
void Screen_uninit(void);

#endif /* CHIP8_SCREEN_H */
