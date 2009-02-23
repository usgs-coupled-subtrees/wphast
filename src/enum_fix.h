
// BC.bc_solution_type
#define ST_UNDEFINED   2 // UNDEFINED
#define ST_FIXED      12 // FIXED
#define ST_ASSOCIATED 13 // ASSOCIATED

// zone.defined
#define ZD_UNDEFINED   2 // UNDEFINED

// time.type
#define TT_UNDEFINED   2 // UNDEFINED
#define TT_STEP       16 // STEP
#define TT_UNITS      17 // UNITS

// grid.uniform
#define GU_UNDEFINED   2 // UNDEFINED

// Head_ic.ic_type
#define HIC_UNDEFINED   2 // UNDEFINED
#define HIC_ZONE        4 // ZONE
#define HIC_WATER_TABLE 5 // WATER_TABLE

// BC.bc_solution_type
// zone.defined
// time.type
// grid.uniform
// Head_ic.ic_type
#undef UNDEFINED

// BC.bc_solution_type
#undef FIXED
#undef ASSOCIATED

// time.type
#undef STEP
#undef UNITS

// Head_ic.ic_type
#undef ZONE
#undef WATER_TABLE


#ifdef __SAVE_ENUMS__

enum TIME_TYPE {
	TT_UNDEFINED =  2,
	TT_STEP      = 16,
	TT_UNITS     = 17,
};

enum HEAD_IC_TYPE {
	HIC_UNDEFINED   = 2,
	HIC_ZONE        = 4,
	HIC_WATER_TABLE = 5,
};

#endif
