#include <arch.h>

int com_is_transmit_empty()
{
   return inb(COM1_PORT + 5) & 0x20;
}

void com_write(char data)
{
   while (com_is_transmit_empty() == 0);

   outb(COM1_PORT, data);
}