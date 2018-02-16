#include "serializer.h"

BEGIN_NAMESPACE(RaspiNamespace);

/*
DECLARE_STRUCT(GsvStructType,
	ascii_char_ptr, UTCTime,
	ascii_char_ptr, GSVHeader,
	int, NumberOfMessages,
	int, MessageNumber,
	int, SatsInView,
	int, Sat1ID,
	int, Sat1El,
	int, Sat1Az,
	int, Sat1SNR,
        int, Sat2ID,
        int, Sat2El,
        int, Sat2Az,
        int, Sat2SNR,
        int, Sat3ID,
        int, Sat3El,
        int, Sat3Az,
        int, Sat3SNR,
        int, Sat4ID,
        int, Sat4El,
        int, Sat4Az,
        int, Sat4SNR,
	int, ChSum
);

DECLARE_MODEL(Gp735Type,
	WITH_DATA(GsvStructType,gsv)
);
*/

DECLARE_STRUCT(SatStruct,
	int, SatID,
	ascii_char_ptr, UTCTime,
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
