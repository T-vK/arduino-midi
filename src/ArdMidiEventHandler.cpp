#include "ArdMidiEventHandler.h"
#ifdef ARDMIDIEVENTHANDLER_H

#include "arduino.h"

namespace midi {

const char* APP = "MidiEventHandler";

//MidiEventHandler *self_MidiEventHandler;

MidiEventHandler::MidiEventHandler(MidiVoicer *p_MidiVoicer, int *p_channel){
  this->p_MidiVoicer = p_MidiVoicer;
  this->p_channel = p_channel;
//  self_MidiEventHandler = this;
};

MidiEventHandler::~MidiEventHandler(){
}

/**
 * @brief Parse byte stream for ble midi messages
 * @param [in] msg byte stream
 * @param [in] len length of the msg
 */

void MidiEventHandler::parse(uint8_t* msg, uint8_t len){
  ESP_LOGD(APP, "parse: len: %d", len);

  int pos = 0;
  uint8_t status=0;
  uint8_t channel=0;
  uint8_t p1=0;
  uint8_t p2=0;

  while (pos<len){
    // find status ingnoring headers and timestamps
    while (msg[pos]>>7 == 1) {
      // if next is data we have a status record
      if (pos+1<len && msg[pos+1]>>7 == 0){
        // status: 0b1001 << 4 | channel;
        status = msg[pos] >> 4;  // high 4 bits
        channel = msg[pos] & 0x0F; // lower 4 bits
        p1 = 0;
        p2 = 0;
      }
      pos++;
    }
     
    // process data bytes
    if (msg[pos]>>7 == 0) { // data
      p1 = msg[pos];
      // check if we have 2 data bytes
      if (pos+1<len && msg[pos+1]>>7 == 0) {
        pos++;
        p2 = msg[pos];
        onCommand(channel, status, p1, p2);
      } else {
        onCommand(channel, status, p1, 0);
      }    
    }
    pos++;
  }
}

void MidiEventHandler::onCommand(uint8_t channel, uint8_t status, uint8_t p1,uint8_t p2 ){
  ESP_LOGD(APP, "onCommand channel:%d, status:%d, p1:%d,  p2:%d", (int)channel, (int)status, (int)p1, (int)p2);
  ESP_LOGD(APP, "onCommand filtered channel: %d ", *p_channel);
  if (p_channel==nullptr || *p_channel < 0 || *p_channel == channel) {
    switch (status) {
      case 0b1001:
        onNoteOn(channel, p1, p2);
        break;
      case 0b1000:
        onNoteOff(channel, p1, p2);
        break;
      case 0b1110:
        onPitchBend(channel, p1);
        break;
      case 0b1011:
        onControlChange(channel, p1, p2);
        break;
      default:
        //ESP_LOGI(APP, "Unsupported MIDI status: %d",status);
        break;
    }
  }
}

void MidiEventHandler::onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity){
    ESP_LOGD(APP, "noteOn note:%d, velocity:%d, channel:%d", (int)note, (int)velocity, (int)channel);
    p_MidiVoicer->noteOn(note, velocity, channel);
};

void MidiEventHandler::onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity){
    ESP_LOGD(APP, "noteOff note:%d, velocity:%d, channel:%d", (int)note, (int)velocity, (int)channel);
    p_MidiVoicer->noteOff(note, velocity, channel);
};
void MidiEventHandler::onControlChange(uint8_t channel, uint8_t controller, uint8_t value){
    p_MidiVoicer->controlChange(controller,(StkFloat) value, channel);
};

void MidiEventHandler::onPitchBend(uint8_t channel, uint8_t value){
    p_MidiVoicer->pitchBend(value, channel);
};


} // namespace

#endif