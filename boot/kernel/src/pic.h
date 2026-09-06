// kernel/src/pic.h
#ifndef PIC_H
#define PIC_H

void pic_remap(void);
void pic_unmask(unsigned char irq);
void pic_mask(unsigned char irq);

#endif