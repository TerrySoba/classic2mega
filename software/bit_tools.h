/* Some bit manipulation macros */
#define SET_BIT(X,Y)  X |= (1 << (Y))
#define CLR_BIT(X,Y)  X &= (~(1 << (Y)))
#define GET_BIT(X,Y)  (((X) >> (Y)) & 1)
#define SET_BIT_VALUE(X,Y,V) { if (V & 1) SET_BIT(X,Y); else CLR_BIT(X,Y); }
#define TOGGLE_BIT(X,Y) { SET_BIT_VALUE(X,Y,~(GET_BIT(X,Y))); }
