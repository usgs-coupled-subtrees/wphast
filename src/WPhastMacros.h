///////////////////////////////////////////////////////////////////////////////
// HDF Macros
///////////////////////////////////////////////////////////////////////////////

#define DECL_SZ_MACRO(name) \
	static const char sz_##name[] = #name

#define DECL_SZ_DEFINED_MACRO(name) \
	DECL_SZ_MACRO(name); \
	DECL_SZ_MACRO(name##_defined)

#define HDF_GETSET_MACRO(name, h5_type) \
	do { \
		DECL_SZ_MACRO(name); \
		VERIFY(0 <= CGlobal::HDFSerialize(bStoring, loc_id, sz_##name, h5_type, 1, &this->name)); \
	} while(0)

#define HDF_GETSET_DEFINED_MACRO(name, h5_type) \
	do { \
		DECL_SZ_DEFINED_MACRO(name); \
		HDF_GETSET_MACRO(name##_defined, H5T_NATIVE_INT); \
		HDF_GETSET_MACRO(name, h5_type); \
	} while (0)

#define HDF_GET_CHAR_STAR_MACRO(name) \
	do { \
		DECL_SZ_MACRO(name); \
		herr_t status = CGlobal::HDFSerializeStringOrNull(bStoring, loc_id, sz_##name, &this->name); \
		ASSERT(status >= 0 || this->name == NULL); \
	} while(0)

#define HDF_STD_STRING_MACRO(name) \
	do { \
		DECL_SZ_MACRO(name); \
		CGlobal::HDFSerializeString(bStoring, loc_id, sz_##name, this->name); \
	} while(0)


///////////////////////////////////////////////////////////////////////////////
// CArchive Macros
///////////////////////////////////////////////////////////////////////////////

#define ARC_GETSET_MACRO(archive, name) \
	do { \
		if (archive.IsStoring()) archive << name; \
		else archive >> name; \
	} while (0)

#define ARC_GETSET_DEFINED_MACRO(archive, name) \
	do { \
		ARC_GETSET_MACRO(archive, name##_defined); \
		ARC_GETSET_MACRO(archive, name); \
	} while (0)

#define ARC_GET_CHAR_STAR_MACRO(archive, name) \
	do { \
		CString str(""); \
		if (archive.IsStoring()) \
		{ \
			if (name) \
			{ \
				str = name; \
			} \
			ar << str; \
		} \
		else \
		{ \
			delete[] name; \
			ar >> str; \
			if (str.IsEmpty()) \
			{ \
				name = 0; \
			} \
			else \
			{ \
				name = new char[str.GetLength() + 1]; \
				::strcpy(name, str); \
			} \
		} \
	} while(0)

#define ARC_STD_STRING_MACRO(archive, stdstr) \
	do { \
		CString str(""); \
		if (archive.IsStoring()) \
		{ \
			str = stdstr.c_str(); \
			ar << str; \
		} \
		else \
		{ \
			ar >> str; \
			stdstr = str; \
		} \
	} while(0)

#define ARC_VERSION(archive, name, nVersion) \
	do { \
		CString type; \
		if (archive.IsStoring()) \
		{ \
			/* store type as string*/ \
			type = #name; \
			archive << type; \
			/* store version in case changes need to be made */ \
			archive << nVersion; \
		} \
		else \
		{ \
			/* read type as string */ \
			archive >> type; \
			ASSERT(type.Compare(#name) == 0); \
			/* read version in case changes need to be made */ \
			archive >> nVersion; \
		} \
	} while(0)

#define ARC_GETSET_COORD_SYS_MACRO(archive, name) \
	do { \
		int n = name; \
		if (archive.IsStoring()) archive << n; \
		else archive >> n; \
		if (!archive.IsStoring()) { \
			switch (n) { \
			case PHAST_Transform::MAP: \
				name = PHAST_Transform::MAP; \
				break; \
			default: \
				name = PHAST_Transform::GRID; \
				break; \
			} \
		} \
	} while (0)
