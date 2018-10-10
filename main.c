#include "./drivers/inc/vga.h"
#include "./drivers/inc/ISRs.h"
#include "./drivers/inc/LEDs.h"
#include "./drivers/inc/audio.h"
#include "./drivers/inc/HPS_TIM.h"
#include "./drivers/inc/int_setup.h"
#include "./drivers/inc/wavetable.h"
#include "./drivers/inc/pushbuttons.h"
#include "./drivers/inc/ps2_keyboard.h"
#include "./drivers/inc/HEX_displays.h"
#include "./drivers/inc/slider_switches.h"

double superSignal = 0.0;
int volume=0x000000FF;
int amplitude = 2;
int sampleCount = 0;
double maxSignal = 21478363.0;

int x = 0; //xLocation on display
double y[320]; //yLocation mappings on display (think of as discretized function)

double mod(double a, double b){
	double floor = (double)((int)(a/b));
	return (a - b * (floor));
}

double floor(double a){
	return (double)((int)a);
}

double calculateSignal(double frequency, int t){
	double index = frequency * t;
	double waveTableVal;

	double sample = mod(sample, 48000.0);
	double sampleFloor = floor(sample);
	if(sample == sampleFloor){
		waveTableVal = (double) sine[(int)sample];
	}else{
		double diff = sample - sampleFloor;
		waveTableVal = (1 - diff) * sine[(int)sampleFloor] + diff * sine[(int)(sampleFloor + 1)];
	}
	return  volume * waveTableVal;
}

	int main() {
		//Set up interups & timer
    	enable_ps2_int_ASM();
    	int_setup(2, (int[]){79, 199});
	
		HPS_TIM_config_t hps_tim;
		hps_tim.tim=TIM0;
		hps_tim.timeout=10;
		hps_tim.LD_en=1;
		hps_tim.INT_en=1;
		hps_tim.enable=1;

		HPS_TIM_config_ASM(&hps_tim);
		
		//Run Application
    	while(1) {

		//VGA_clear_pixelbuff_ASM(); //reset display

		if(hps_tim0_int_flag){ //check if sampling rate fulfilled
			hps_tim0_int_flag = 0; //reset interrupt

					//DISPLAY
					/*int key = 0;
					for(key; key < 8; key++){
						if(keysPressed[key]){
							superSignal += (calculateSignal(frequencies[key], sampleCount));
						}
					}

					if(x >= 320){ //If drawn signal across entire screen
						x = 0; // Draw black pixels fo all drawn points
					}

					VGA_draw_point_ASM(x, y[x],(0x000000)); //clear point
					y[x] = 119 + (superSignal/maxSignal)*120; //center signal in y on screen
					VGA_draw_point_ASM(x, y[x], (0xFFFFFF)); //draw point in white on screen
					x++; //increment x to draw next point*/
				//

			
			//PLAY SOUND
			if(sampleCount >= 48000){ //reset sample count if waveTable exhausted
				sampleCount = 0;
			}

			int i;
			for(i = 0; i < 144000; i++){
				int key, index;
				superSignal = 0;

				for(key=0; key < 8; key++){
					if(keysPressed[key]){
						superSignal += calculateSignal(frequencies[key], sampleCount);
					}
				} //end key for loop
			
				if(audio_write_data_ASM((int)superSignal,(int)superSignal)){
					sampleCount++;
				}//Attempt to save audio
		  }//End outer for loop
		}//End timer dependent code
		}// End while loop
	return 0;
  } //end main



