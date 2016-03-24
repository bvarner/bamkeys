#ifndef MESSAGE_PORT_H
#define MESSAGE_PORT_H

#include <malloc.h>
#include <Handler.h>
#include <Message.h>
#include <OS.h>

/**
 * Magical class that wraps the functions of sending / receiving BMessages over ports!
 * 
 * This class has two purposes (err, modes... really) one for sending messages to the
 * port, one for receiving.
 */
class MessagePort {
	public:
		MessagePort(const char* portName, bool create = true, int32 size = 16, BHandler *target = NULL);
		~MessagePort();
		
		bool InitCheck();
		
		status_t WriteMessage(BMessage* msg);
		bool HasMessage();
		BMessage* NextMessage(bool block = false);
		
	private:
		bool HasMessage(ssize_t *size, bool block = false);
		
		static int32 KnockKnock(void** arg);
		int32 DeliveryBoy();
		
		thread_id fDeliver_id;
		
		bool fServerMode;
		bool fPortOwner;
		BHandler *fTarget;
		port_id fPort;
};

#endif /* MESSAGE_PORT_H */