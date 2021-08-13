
#include "dinc.h"

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    systick_init();
    uart_init();
    
    printf("start...\r\n");
    
    while(1)
    {
        char ch;
        if(uart_getChar(UART_COM1, &ch) == 1)
        {
            uart_sendChar(UART_COM1, ch);
        }
        if(uart_getChar(UART_COM2, &ch) == 1)
        {
            uart_sendChar(UART_COM2, ch);
        }
        if(uart_getChar(UART_COM3, &ch) == 1)
        {
            uart_sendChar(UART_COM3, ch);
        }
    }
}
