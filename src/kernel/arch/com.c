#include <x86.h>

int com_is_transmit_empty()
{
   return inb(COM1_PORT + 5) & 0x20;
}

void com_write(char c)
{
   while (com_is_transmit_empty() == 0);

   if(c == '\n')
      outb(COM1_PORT, '\r');

   outb(COM1_PORT, c);
}