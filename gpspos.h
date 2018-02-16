#include "serializer.h"

BEGIN_NAMESPACE(RaspiNamespace);

DECLARE_STRUCT(GgaStructType,
	ascii_char_ptr, GSVHeader,
	float, UTCTime,
	float, Lat,
	ascii_char_ptr, NorthSouth,
	float, Lon,
	ascii_char_ptr, EastWest,
	float, Hdop,
	float, Alt
);

DECLARE_MODEL(Gp735Type,
	WITH_DATA(GgaStructType,gga)
);

END_NAMESPACE(RaspiNamespace);
