// Travis Lane
// Heartbleed Message

#ifndef HEARTBLEED_MESSAGE_H
#define HEARTBLEED_MESSAGE_H

#include <stdint.h>

/* This isn't unncommon.
 * It also, isn't horribly unsafe.
 * However, there are better ways.
 */
typedef struct buffer_over_read_message_t {
	uint32_t hm_len;
	char     hm_data[0];
} HeartbleedMessage;

#endif /* HEARTBLEED_MESSAGE_H */