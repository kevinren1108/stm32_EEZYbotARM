#include "stm32f10x.h"
#include "util.h"

int main (void){
	clockInit();
	led_IO_init();
	tim3_IO_init();
	tim3_enable();

	
	int i = 5;
	while(1){
		
		if(i < 26){
			update(4);
			delay(2400000);
			update(i);
			delay(2400000);
			i ++;
		}else{
			i = 4;
		}
		
		
		
	}
	
	
	
	
	
}
