#include "sensor_controller.h"
int TAMANHO_TABELAS =43;
int escalaBufferSDP = 1;

const struct device *dev1;
bool led_is_on = true;
int ret1;

uint8_t tabflow[43] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38,
                                           40, 45, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200};

 float MapFloat(float value, float fromLow, float fromHigh, float toLow, float toHigh) {
    return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow; 
  }

  float FlowCalc(float dpvalue, float tabela_calib[TAMANHO_TABELAS]){
    if(abs(dpvalue) < tabela_calib[1]){
      if(dpvalue <= 0){
        return (MapFloat(abs(dpvalue), 0, tabela_calib[1], tabflow[0], tabflow[1]));
      }
      return (MapFloat(abs(dpvalue), 0, tabela_calib[1], tabflow[0], tabflow[1])*(-1));
    }
    for(int i = 1; i < TAMANHO_TABELAS; i++){
      if(tabela_calib[i] <= abs(dpvalue) && abs(dpvalue) < tabela_calib[i+1]){
        if(dpvalue <= 0){
          return (MapFloat(abs(dpvalue), tabela_calib[i], tabela_calib[i+1], tabflow[i], tabflow[i+1]));
        }
        return (MapFloat(abs(dpvalue), tabela_calib[i], tabela_calib[i+1], tabflow[i], tabflow[i+1])*(-1));
      }
    }
    return -1;
  }

void ConfigurePin(){

		dev1 = device_get_binding(LED0);
		if (dev1 == NULL) {
			return;
		}

		ret1 = gpio_pin_configure(dev1, PIN, GPIO_OUTPUT_ACTIVE | FLAGS);
		if (ret1 < 0) {
			return;
		}

}