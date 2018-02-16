#include "serializer.h"

BEGIN_NAMESPACE(RaspiNamespace);

DECLARE_STRUCT(SatStructType,
        ascii_char_ptr, UTCTime,
        int, SatID,
        int, El,
        int, Az,
        int, Snr
);

DECLARE_MODEL(Gp735Type,
        WITH_DATA(SatStructType,sat)
);

END_NAMESPACE(RaspiNamespace);
