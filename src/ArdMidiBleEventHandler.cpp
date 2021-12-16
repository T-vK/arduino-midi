#include "ArdMidiBleEventHandler.h"
#ifdef ARDMIDIBLEEVENTHANDLER_H

namespace midi {

const char * APP_EVENT_HDLR = "ArdMidiBleEventHandler";

ArdMidiBleEventHandler::ArdMidiBleEventHandler(MidiVoicer *p_MidiVoicer, int *p_channel)
: MidiEventHandler(p_MidiVoicer, p_channel) {
};

ArdMidiBleEventHandler::~ArdMidiBleEventHandler(){
}

/**
 * @brief Callback function to support a read request.
 * @param [in] pCharacteristic The characteristic that is the source of the event.
 */
void ArdMidiBleEventHandler::onRead(BLECharacteristic* pCharacteristic) {
	ESP_LOGD(APP_EVENT_HDLR, "onRead");
} // onRead


/**
 * @brief Callback function to support a write request.
 * @param [in] pCharacteristic The characteristic that is the source of the event.
 */
void ArdMidiBleEventHandler::onWrite(BLECharacteristic* pCharacteristic) {
	ESP_LOGD(APP_EVENT_HDLR, "onWrite");
  const char* str = pCharacteristic->getValue().c_str();
  int len = pCharacteristic->getValue().length();
  parse((uint8_t*)str, len);
} 

} // namespace

#endif