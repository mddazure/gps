#include "serializer.h"

BEGIN_NAMESPACE(RaspiNamespace);

DECLARE_STRUCT(GsvStructType,
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

END_NAMESPACE(RaspiNamespace);
