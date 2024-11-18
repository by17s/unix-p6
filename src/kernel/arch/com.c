#include <x86.h>  // Include x86 hardware access functions

int com_is_transmit_empty()  // Check if the COM port's transmit buffer is empty
{
   return inb(COM1_PORT + 5) & 0x20;  // Read status register and return the empty status
}

void com_write(char c)  // Write a character to the COM port
{
   while (com_is_transmit_empty() == 0);  // Wait until the transmit buffer is ready

   if(c == '\n')  // If the character is a newline
      outb(COM1_PORT, '\r');  // Send a carriage return first

   outb(COM1_PORT, c);  // Send the character to the COM port
}