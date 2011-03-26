#ifndef SUBSCRIPTIONMANAGER_H_
#define SUBSCRIPTIONMANAGER_H_

#include "../gidaivel-arduino-base/Avieul.h"

#define DEBUG_SUBMGR


class SubscriptionManager {
public:
	SubscriptionManager(AvieulSender *sender);

	void add(XBeeAddress from);
	void remove(XBeeAddress from);
	void publish(uint8_t* data, uint8_t data_length);
	bool isSubscribed(XBeeAddress from);

private:
	AvieulSender *_sender;
	XBeeAddress *_subscribers;
	uint8_t _subscriber_count;
};

#endif /* SUBSCRIPTIONMANAGER_H_ */
