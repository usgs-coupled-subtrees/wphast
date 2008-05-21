#pragma once
#include "srcinput/Cube.h"
#include "ISerializable.h"

class SerializableCube : public Cube, public ISerializable
{
public:
	SerializableCube(void);
	SerializableCube(Cube c);
	virtual ~SerializableCube(void);

	virtual void Serialize(bool bStoring, hid_t loc_id);

};
