#include "MessagePort.h"

/**
 * Constructs the MessagePort for client mode. 
 * Automatically delivers all messages to the specified BHandler.
 */
MessagePort::MessagePort(const char* portName, bool create = true, int32 size = 16, BHandler *target = NULL) {
	fPortOwner = false;
	fPort = find_port(portName);
	if (fPort == B_NAME_NOT_FOUND && create) {
		fPort = create_port(size, portName);
		fPortOwner = true;
	}
	
	fTarget = target;
	if (fTarget != NULL) {
		fDeliver_id = spawn_thread(KnockKnock, "delivery_boy", B_LOW_PRIORITY, this);
		resume_thread(fDeliver_id);
	}
	
	fServerMode = (fTarget == NULL);
}

/**
 * Destroys the MessagePort.
 * Kills the thread, deletes the port (if we're the server)
 * and deallocates the BDataIO buffer.
 */
MessagePort::~MessagePort() {
	kill_thread(fDeliver_id);
	
	if (fPortOwner && InitCheck()) {
		delete_port(fPort);
	}
}

/**
 * Verifies we can talk to the port.
 */
bool MessagePort::InitCheck() {
	return fPort != B_NAME_NOT_FOUND && 
	       fPort != B_BAD_VALUE && 
	       fPort != B_NO_MORE_PORTS;
}

/**
 * Writes the given BMessage out to the port.
 *
 * @return B_OK if everything works, 
 * @return B_ERROR if we're a failure.
 */
status_t MessagePort::WriteMessage(BMessage *msg) {
	status_t ret = B_ERROR;
	if (fServerMode) {
		ssize_t size = msg->FlattenedSize();
		char* buf = (char*)malloc(size);
		ret = msg->Flatten(buf, size);
		if (ret == B_OK) {
			ret = write_port(fPort, msg->what, buf, size);
		}
		free(buf);
	}
	return ret;
}

bool MessagePort::HasMessage() {
	ssize_t size;
	return HasMessage(&size, false);
}

bool MessagePort::HasMessage(ssize_t *size, bool block = false) {
	bool ret = false;
	if (!fServerMode) {
		if (block) {
			*size = port_buffer_size(fPort);
		} else {
			*size = port_buffer_size_etc(fPort, B_TIMEOUT, 0);
		}
		ret = (*size != B_WOULD_BLOCK && 
			   *size != B_BAD_PORT_ID && 
			   *size != B_TIMED_OUT);
	}
	return ret;
}

BMessage* MessagePort::NextMessage(bool block = false) {
	BMessage* msg = NULL;
	if (!fServerMode) {
		ssize_t size;
		if (HasMessage(&size, block)) {
			char* buf = (char *)malloc(size);
			int32 what;
			
			status_t ret;
			if (block) {
				ret = read_port(fPort, &what, (void *)buf, size);
			} else {
				ret = read_port_etc(fPort, &what, (void *)buf, size, B_TIMEOUT, 0);
			}
			
			if (! (ret < B_OK)) {
				msg = new BMessage((uint32)what);
				if (msg->Unflatten(buf) != B_OK) {
					delete msg;
					msg = NULL;
				}
			}
			free(buf);
		}
	}
	
	return msg;
}

/* AUTOMATIC BHANDLER DELIVERY (SEPARATE THREAD) */
int32 MessagePort::KnockKnock(void **arg) {
	MessagePort *obj = (MessagePort*)arg;
	return obj->DeliveryBoy();
}

int32 MessagePort::DeliveryBoy() {
	BMessage *msg = NULL;
	do {
		msg = NextMessage(true);
		if (msg != NULL) {
			fTarget->MessageReceived(msg);
			delete msg;
		}
		snooze(1000);
	} while (true);
}
