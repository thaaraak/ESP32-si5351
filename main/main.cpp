#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "si5351.h"

#define I2C_MASTER_NUM	0
#define I2C_MASTER_SDA_IO 26
#define I2C_MASTER_SCL_IO 27

extern "C" void app_main();

Si5351 synth;


static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 100000;
    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
}

int lastMult = -1;

void changeFrequency( int currentFrequency )
{
	  int mult = 0;

	  if ( currentFrequency < 8000000 )
		  mult = 100;
	  else if ( currentFrequency < 11000000 )
		  mult = 80;
	  else if ( currentFrequency < 15000000 )
		  mult = 50;
	  else if ( currentFrequency < 22000000 )
		  mult = 40;
	  else if ( currentFrequency < 30000000 )
		  mult = 30;

	  uint64_t freq = currentFrequency * 100ULL;
	  uint64_t pllFreq = freq * mult;

	  synth.set_freq_manual(freq, pllFreq, SI5351_CLK0);
	  synth.set_freq_manual(freq, pllFreq, SI5351_CLK2);

	  if ( mult != lastMult )
	  {
		  synth.set_phase(SI5351_CLK0, 0);
		  synth.set_phase(SI5351_CLK2, mult);
		  synth.pll_reset(SI5351_PLLA);
		  lastMult = mult;
	  }
}

void app_main()
{

	i2c_master_init();

	synth.init( I2C_MASTER_NUM, SI5351_CRYSTAL_LOAD_8PF, 25000000, 0 );

	int freq = 7000000;
	changeFrequency(freq);

    while(1)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);

    }
}
