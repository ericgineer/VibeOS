#ifndef PIC_H
#define PIC_H

#include <stdint.h>

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

void pic_remap(int offset1, int offset2);
void pic_send_eoi(unsigned char irq);
void pic_disable(void);
void pic_clear_masks(void);

#endif
