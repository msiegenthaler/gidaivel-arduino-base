#include "SubscriptionManager.h"


#ifdef DEBUG_SUBMGR
inline void printHex(uint8_t *data, uint8_t len, bool seperate) {
	for (int i=0; i<len; i++) {
		if (i>0 && seperate) Serial.print(" ");
		if (data[i] < 0x10) Serial.print("0");
		Serial.print(data[i], HEX);
	}
}
inline void printAddress(XBeeAddress address) {
	printHex((uint8_t*)&address, 8, false);
}
#endif


inline XBeeAddress addressAtIndex(XBeeAddress *addresses, uint8_t index) {
	return *(addresses + sizeof(XBeeAddress)*index);
}


SubscriptionManager::SubscriptionManager(AvieulSender *sender) {
	_sender = sender;
	_subscriber_count = 0;
	_subscribers = (XBeeAddress*) malloc(0);
}

bool SubscriptionManager::isSubscribed(XBeeAddress from) {
	for (uint8_t i=0; i<_subscriber_count; i++) {
		if (addressAtIndex(_subscribers, i) == from) return true;
	}
	return false;
}

void SubscriptionManager::add(XBeeAddress from) {
	if (isSubscribed(from)) {
#ifdef DEBUG_SUBMGR
		Serial.print("Request for already subscribed subscriber ");
		printAddress(from);
		Serial.println();
#endif
		return; //already subscribed
	}

#ifdef DEBUG_SUBMGR
	Serial.print("Adding subscription ");
	printAddress(from);
	Serial.println();
#endif
	XBeeAddress* old = _subscribers;
	XBeeAddress* na = (XBeeAddress*) malloc(sizeof(XBeeAddress)*(_subscriber_count+1));

	memcpy(na, _subscribers, sizeof(XBeeAddress)*_subscriber_count);
	*(na+sizeof(XBeeAddress)*_subscriber_count) = from;

	free(old);
	_subscriber_count++;
	_subscribers = na;

	Serial.print("Now subscribed: ");
	for (uint8_t i = 0; i<_subscriber_count; i++) {
		printAddress(addressAtIndex(_subscribers, i));
		Serial.print(" ");
	}
	Serial.println();
}

void SubscriptionManager::remove(XBeeAddress from) {
	if (!isSubscribed(from)) {
#ifdef DEBUG_SUBMGR
		Serial.print("Ignoring subscription-remove from not subscribed ");
		printAddress(from);
		Serial.println();
#endif
		return; //not subscribed
	}

#ifdef DEBUG_SUBMGR
	Serial.print("Removing subscription ");
	printAddress(from);
	Serial.println();
#endif

	uint8_t c=0;
	XBeeAddress *na = (XBeeAddress*) malloc(sizeof(XBeeAddress)*(_subscriber_count-1));
	for (uint8_t i=0; i<_subscriber_count; i++) {
		XBeeAddress adr = addressAtIndex(_subscribers, i);
		if (adr==from) continue;
		*(na+sizeof(XBeeAddress)*c) = from;
		c++;
	}
	free(_subscribers);
	_subscribers = na;
	_subscriber_count--;
}

void SubscriptionManager::publish(uint8_t* data, uint8_t data_length) {
#ifdef DEBUG_SUBMGR
	Serial.println("Notifying subscribers");
#endif
	for (uint8_t i=0; i < _subscriber_count; i++) {
		XBeeAddress sub = addressAtIndex(_subscribers, i);
#ifdef DEBUG_SUBMGR
		Serial.print(" * Notifying: ");
		printAddress(sub);
		Serial.println();
#endif
		_sender->send(sub, data, data_length);
	}
}
