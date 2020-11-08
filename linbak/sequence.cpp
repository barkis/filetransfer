#include <cstdint>
#include "sync.h"

unsigned int Sync::Sequence(unsigned char *aiSeq)	{	
	uint16_t i16Seq{};
	//Get LH byte and shift it into position
	i16Seq |= aiSeq[0];
	i16Seq <<= 8;
	//add the RH byte
	i16Seq |= aiSeq[1];
	return (unsigned int)i16Seq;
}
