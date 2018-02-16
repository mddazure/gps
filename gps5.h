#include "serializer.h"

BEGIN_NAMESPACE(RaspiNamespace);

DECLARE_STRUCT(SatStruct,
	int, SatID,
	int, El,
	int, Az,
	int, Snr
);

DECLARE_MODEL(SatModel,
	WITH_DATA(SatStruct, Sat1)
/*	WITH_DATA(SatStruct, Sat2),
	WITH_DATA(SatStruct, Sat3),
	WITH_DATA(SatStruct, Sat4)*/
);

END_NAMESPACE(RaspiNamespace);
