#ifndef AVIEUL_H_
#define AVIEUL_H_

#include "WConstants.h"
#import "../arduino-xbee/Series1XBee.h"


#define DEBUG_AVIEUL  //define for serial debug messages


class AvieulService;

class AvieulSender {
public:
	virtual void send(XBeeAddress to, uint8_t* data, uint8_t data_length);
};


/**
 * An Avieul-service.
 * Responsible for processing incoming commands and executing the desired functions.
 */
class AvieulService {
public:
	void initialize(uint8_t index, AvieulSender *sender);

	uint32_t getType();
	uint8_t getVersion();
	uint8_t getIndex();

	/**
	 * Process a call/request/subscription for this service. Called by Avieul.
	 */
	void process(XBeeAddress from, uint8_t* request, uint8_t request_length);

protected:
	/**
	 * Process a call.
	 */
	virtual void processCall(uint16_t callType, XBeeAddress from, uint8_t* payload, uint8_t payload_length);
	/**
	 * Process a request by 'from'.
	 * @return true if successfully processed and answered with a reply message
	 * 		   false if the request type is unknown. This class will respond with a service-request-unknown-message
	 */
	virtual bool processRequest(uint16_t requestType, XBeeAddress from, uint8_t* payload, uint8_t payload_length);
	/**
	 * Add a subscription for a subscription type.
	 * @return true if successful: this class will send a subscription-confirm-message to 'from'
	 * 		   false if unknown type: this class will send a subscription-unknown-message to 'from'
	 */
	virtual bool addSubscription(XBeeAddress from, uint16_t subscriptionType);
	/**
	 * Remove the subscription. Ignore unknown or not registered subscriptions.
	 */
	virtual void removeSubscription(XBeeAddress from, uint16_t subscriptionType);

	/**
	 * Fills the first 4 bytes of the response
	 */
	void fillResponseHeader(uint8_t *buffer, uint16_t requestType);

protected:
	uint32_t _type;
	uint8_t _version;
	uint8_t _index;
	AvieulSender *_sender;
};



/**
 * Takes care of announcing and managing the services and forwards the incoming requests
 * to the services.
 */
class Avieul : public AvieulSender {
public:
	Avieul(Series1XBee *xbee, AvieulService **services, uint8_t service_count);
	void process();

	void send(XBeeAddress to, uint8_t* data, uint8_t data_length);

protected:
	void handle(XBeeAddress from, uint8_t* data, uint8_t data_length);

private:
	void announce(XBeeAddress to);
private:
	Series1XBee *_xbee;
	AvieulService **_services;
	uint8_t _services_count;
};


#endif /* AVIEUL_H_ */
