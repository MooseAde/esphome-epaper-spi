#include "epaper_waveshare.h"

namespace esphome::epaper_spi {

static const char *const TAG = "epaper_spi.waveshare";

bool EpaperWaveshare::initialise(bool partial) {

  if (partial) {
    // Send partial init sequence, as defined in waveharemodel.initsequence property
    // EPaperBase::initialise(partial);
    this->cmd_data(0x32, this->partial_lut_, this->partial_lut_length_);
    this->cmd_data(0x37, {0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00});
    this->cmd_data(0x11, {0x03});  // Set data entry mode to Y increment, X increment
    this->cmd_data(0x44, {0x01, (uint8_t) ((this->width_ % 8 == 0)? (this->width_ / 8 ): (this->width_ / 8 + 1))}); // Set RAM X start/end
    this->cmd_data(0x45, {(uint8_t) 0, (uint8_t) 0, (uint8_t) (this->height_ & 0xFF), (uint8_t) ((this->height_ & 0x100) >> 8)});  // Set RAM Y start/end 
    this->cmd_data(0x3C, {0x80});
    this->cmd_data(0x22, {0xCf});
    this->command(0x20);
    this->next_delay_ = 100;
  } else {
    // Send Full init sequence as defined in waveharemodel.initsequencefull property
    // SEE commented code below from 2.66in example
    if (EPaperBase::reset()) {
      this->command(0x12);
    }
    this->cmd_data(0x11, {0x03});  // Set data entry mode to Y increment, X increment
    this->cmd_data(0x44, {0x01, (uint8_t) ((this->width_ % 8 == 0)? (this->width_ / 8 ): (this->width_ / 8 + 1))}); // Set RAM X start/end
    this->cmd_data(0x45, {(uint8_t) 0, (uint8_t) 0, (uint8_t) (this->height_ & 0xFF), (uint8_t) ((this->height_ & 0x100) >> 8)});  // Set RAM Y start/end
    this->cmd_data(0x3C, {0x01});
  }
  this->send_red_ = true;
  return true;
}

void EpaperWaveshare::set_window() {
  this->x_low_ &= ~7;
  this->x_high_ += 7;
  this->x_high_ &= ~7;
  uint16_t x_start = this->x_low_ / 8;
  uint16_t x_end = (this->x_high_ - 1) / 8;
  this->cmd_data(0x44, {(uint8_t) x_start, (uint8_t) (x_end)});
  this->cmd_data(0x4E, {(uint8_t) x_start});
  this->cmd_data(0x45, {(uint8_t) this->y_low_, (uint8_t) (this->y_low_ / 256), (uint8_t) (this->y_high_ - 1),
                        (uint8_t) ((this->y_high_ - 1) / 256)});
  this->cmd_data(0x4F, {(uint8_t) this->y_low_, (uint8_t) (this->y_low_ / 256)});
  ESP_LOGV(TAG, "Set window X: %u-%u, Y: %u-%u", this->x_low_, this->x_high_, this->y_low_, this->y_high_);
}

void EpaperWaveshare::refresh_screen(bool partial) {
  if (partial) {
    this->cmd_data(0x22, {0x0F});
  } else {
    this->cmd_data(0x22, {0xC7});
  }
  this->command(0x20);
  this->next_delay_ = partial ? 100 : 3000;
}

void EpaperWaveshare::deep_sleep() { this->cmd_data(0x10, {0x01}); }
}  // namespace esphome::epaper_spi



// void EPD_2IN66_Init(void)
// {
//     EPD_2IN66_Reset();
//     EPD_2IN66_ReadBusy();
//     EPD_2IN66_SendCommand(0x12);//soft  reset
//     EPD_2IN66_ReadBusy();
	
// 	/* Y increment, X increment */
//     EPD_2IN66_SendCommand(0x11);
//     EPD_2IN66_SendData(0x03);
//     /*	Set RamX-address Start/End position	*/
//     EPD_2IN66_SendCommand(0x44);
//     EPD_2IN66_SendData(0x01); 
//     EPD_2IN66_SendData((EPD_2IN66_WIDTH % 8 == 0)? (EPD_2IN66_WIDTH / 8 ): (EPD_2IN66_WIDTH / 8 + 1) );
//     /*	Set RamY-address Start/End position	*/
//     EPD_2IN66_SendCommand(0x45);
//     EPD_2IN66_SendData(0);
//     EPD_2IN66_SendData(0);
//     EPD_2IN66_SendData((EPD_2IN66_HEIGHT&0xff));
//     EPD_2IN66_SendData((EPD_2IN66_HEIGHT&0x100)>>8);

//     EPD_2IN66_SendCommand(0x3C);        // Border  设置  黑白一般设置为跟随白波形即 0x01        Border setting 
//     EPD_2IN66_SendData(0x01);
	
// }

// /******************************************************************************
// function :	Initialize the e-Paper register (Partial display)
// parameter:
// ******************************************************************************/
// void EPD_2IN66_Init_partial(void)
// {
//     EPD_2IN66_Reset();
//     EPD_2IN66_ReadBusy();
//     EPD_2IN66_SendCommand(0x12);//soft  reset
//     EPD_2IN66_ReadBusy();
   
//     EPD_2IN66_SetLUA();
//     EPD_2IN66_SendCommand(0x37); 
//     EPD_2IN66_SendData(0x00);  
//     EPD_2IN66_SendData(0x00);  
//     EPD_2IN66_SendData(0x00);  
//     EPD_2IN66_SendData(0x00); 
//     EPD_2IN66_SendData(0x00);  	
//     EPD_2IN66_SendData(0x40);  
//     EPD_2IN66_SendData(0x00);  
//     EPD_2IN66_SendData(0x00);   
//     EPD_2IN66_SendData(0x00);  
//     EPD_2IN66_SendData(0x00);

// 	/* Y increment, X increment */
//     EPD_2IN66_SendCommand(0x11);
//     EPD_2IN66_SendData(0x03);
//     /*	Set RamX-address Start/End position	*/
//     EPD_2IN66_SendCommand(0x44);
//     EPD_2IN66_SendData(0x01); 
//     EPD_2IN66_SendData((EPD_2IN66_WIDTH % 8 == 0)? (EPD_2IN66_WIDTH / 8 ): (EPD_2IN66_WIDTH / 8 + 1) );
//     /*	Set RamY-address Start/End position	*/
//     EPD_2IN66_SendCommand(0x45);
//     EPD_2IN66_SendData(0);
//     EPD_2IN66_SendData(0);
//     EPD_2IN66_SendData((EPD_2IN66_HEIGHT&0xff));
//     EPD_2IN66_SendData((EPD_2IN66_HEIGHT&0x100)>>8);

//     EPD_2IN66_SendCommand(0x3C); 
//     EPD_2IN66_SendData(0x80);   

//     EPD_2IN66_SendCommand(0x22); 
//     EPD_2IN66_SendData(0xcf); 
//     EPD_2IN66_SendCommand(0x20); 
//     EPD_2IN66_ReadBusy();
// }
// */
