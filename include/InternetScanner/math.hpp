#define roundToNext8(number) ((number + 7 & (-8)))
#define roundToPrevious8(number) ((number >> 3) << 3)
