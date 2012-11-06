#ifndef DATAPACKET_HPP_
#define DATAPACKET_HPP_

#include <cstdlib>
#include <iostream>
#include <cstring>
#include "DataPacketStructs.hpp"

typedef unsigned char byte;

class DataPacket
{
public:

	//copy constructor
	DataPacket(const DataPacket& pk);

	header_t header;
	byte * data;
	//size_t datalen;

	DataPacket();
	~DataPacket();

	void clear();
};

std::ostream& operator<<(std::ostream& output, DataPacket& pk);
std::ostream& operator<<(std::ostream& output, header_t header);
std::ostream& operator<<(std::ostream& output, encoder_reply_t data);
std::ostream& operator<<(std::ostream& os, const new_encoder_pk_t& rv);
std::ostream& operator<<(std::ostream& os, const new_encoder_single_pk_t& rv);

#endif //DATAPACKET_HPP_
