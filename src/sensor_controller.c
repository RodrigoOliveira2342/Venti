#include "sensor_controller.h"

const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

static int readSensorI2C( uint8_t *buf, uint16_t addr, uint8_t len){
    return i2c_read(i2c_dev ,buf ,len ,addr);
}


 float mapFloat(float value, float fromLow, float fromHigh, float toLow, float toHigh) {
    return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow; 
  }

  int flowCalc(int dpvalue, int tabela_calib[TAMANHO_TABELAS]){
    if(abs(dpvalue) < tabela_calib[1]){
      if(dpvalue <= 0){
        return (mapFloat(abs(dpvalue), 0, tabela_calib[1], tabflow[0], tabflow[1])*100);
      }
      return (mapFloat(abs(dpvalue), 0, tabela_calib[1], tabflow[0], tabflow[1])*(-1)*100);
    }
    for(int i = 1; i < TAMANHO_TABELAS; i++){
      if(tabela_calib[i] <= abs(dpvalue) && abs(dpvalue) < tabela_calib[i+1]){
        if(dpvalue <= 0){
          return (mapFloat(abs(dpvalue), tabela_calib[i], tabela_calib[i+1], tabflow[i], tabflow[i+1])*100);
        }
        return (mapFloat(abs(dpvalue), tabela_calib[i], tabela_calib[i+1], tabflow[i], tabflow[i+1])*(-1)*100);
      }
    }
    return -1;
  }
