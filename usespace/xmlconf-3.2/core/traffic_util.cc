#include "traffic_util.h"

namespace maxnet
{
	// See http://physics.nist.gov/cuu/Units/binary.html 

	const TrafficUtil::RATE_SUFFIX TrafficUtil::suffixes[] = {
			{ "bit",	1. },
			{ "Kibit",	1024. },
			{ "kbit",	1000. },
			{ "mibit",	1024.*1024. },
			{ "mbit",	1000000. },
			{ "gibit",	1024.*1024.*1024. },
			{ "gbit",	1000000000. },
			{ "tibit",	1024.*1024.*1024.*1024. },
			{ "tbit",	1000000000000. },
			{ "Bps",	8. },
			{ "bps",	1. },
			{ "Kbps",	1000. },
			{ "Mbps",	1000000. },
			{ "KiBps",	8.*1024. },
			{ "KBps",	8000. },
			{ "MiBps",	8.*1024*1024. },
			{ "MBps",	8000000. },
			{ "GiBps",	8.*1024.*1024.*1024. },
			{ "GBps",	8000000000. },
			{ "TiBps",	8.*1024.*1024.*1024.*1024. },
			{ "TBps",	8000000000000. },
			{ NULL }
	};

	TrafficUtil::TrafficUtil(){

	}

	TrafficUtil::~TrafficUtil(){

	}

	int TrafficUtil::getInteger(int *val, const char *arg, int base)
	{
		long res;
		char *ptr;

		if (!arg || !*arg)
			return -1;
		res = strtol(arg, &ptr, base);
		if (!ptr || ptr == arg || *ptr || res > INT_MAX || res < INT_MIN)
			return -1;
		*val = res;
		return 0;
	}

	int TrafficUtil::getUnsigned(unsigned *val, const char *arg, int base)
	{
		unsigned long res;
		char *ptr;

		if (!arg || !*arg)
			return -1;
		res = strtoul(arg, &ptr, base);
		if (!ptr || ptr == arg || *ptr || res > UINT_MAX)
			return -1;
		*val = res;
		return 0;
	}

	int TrafficUtil::getU64(__u64 *val, const char *arg, int base)
	{
		unsigned long long res;
		char *ptr;

		if (!arg || !*arg)
			return -1;
		res = strtoull(arg, &ptr, base);
		if (!ptr || ptr == arg || *ptr || res == 0xFFFFFFFFULL)
			return -1;
		*val = res;
		return 0;
	}

	int TrafficUtil::getU32(__u32 *val, const char *arg, int base)
	{
		unsigned long res;
		char *ptr;

		if (!arg || !*arg)
			return -1;
		res = strtoul(arg, &ptr, base);
		if (!ptr || ptr == arg || *ptr || res > 0xFFFFFFFFUL)
			return -1;
		*val = res;
		return 0;
	}

	int TrafficUtil::getU16(__u16 *val, const char *arg, int base)
	{
		unsigned long res;
		char *ptr;

		if (!arg || !*arg)
			return -1;
		res = strtoul(arg, &ptr, base);
		if (!ptr || ptr == arg || *ptr || res > 0xFFFF)
			return -1;
		*val = res;
		return 0;
	}

	int TrafficUtil::getU8(__u8 *val, const char *arg, int base)
	{
		unsigned long res;
		char *ptr;

		if (!arg || !*arg)
			return -1;
		res = strtoul(arg, &ptr, base);
		if (!ptr || ptr == arg || *ptr || res > 0xFF)
			return -1;
		*val = res;
		return 0;
	}

	int TrafficUtil::getU16(__s16 *val, const char *arg, int base)
	{
		long res;
		char *ptr;

		if (!arg || !*arg)
			return -1;
		res = strtol(arg, &ptr, base);
		if (!ptr || ptr == arg || *ptr || res > 0x7FFF || res < -0x8000)
			return -1;
		*val = res;
		return 0;
	}

	int TrafficUtil::getS8(__s8 *val, const char *arg, int base)
	{
		long res;
		char *ptr;

		if (!arg || !*arg)
			return -1;
		res = strtol(arg, &ptr, base);
		if (!ptr || ptr == arg || *ptr || res > 0x7F || res < -0x80)
			return -1;
		*val = res;
		return 0;
	}

	int TrafficUtil::getRate(unsigned int *rate, const char *str)
	{
		char *p;
		unsigned int bps = strtol(str, &p, 0);
		//const RATE_SUFFIX *s;  delete by pengyunsheng 2009/01/04
		unsigned int value;


		if (p == str)
		{
			return -1;
		}
		if (*p == '\0') {
			*rate = bps / 8;	/* assume bytes/sec */
			return 0;
		}
		value = bps;
//		for (s = suffixes; s->name; ++s) {
			if(strcasecmp(p, "Kbps") == 0)
				{

					//*rate = value * 1000 / 8;
					*rate = value * 1000 ;
					return 0;
				}
			else if(strcasecmp(p, "Mbps") == 0)
				{
					//*rate = value * 1000000 / 8;	
					*rate = value * 1000000 ;
					return 0;
				}
			else{
					*rate = value ;
					return 0;
			}
#if 0		
			if (strcasecmp(s->name, p) == 0) {
				*rate = (bps * s->scale) / 8.;
				return 0;
			}
#endif			
//		}

		return -1;
	}

	int TrafficUtil::getRateAndCell(unsigned int  *rate, int *cell_log, char *str)
	{
		char * slash = strchr(str, '/');

		if (slash)
			*slash = 0;
		if (getRate(rate, str))
			return -1;

		if (slash) {
			int cell;
			int i;

			if (getInteger(&cell, slash+1, 0)){
				return -1;
			}
			*slash = '/';

			for (i=0; i<32; i++) {
				if ((1<<i) == cell) {
					*cell_log = i;
					return 0;
				}
			}
			return -1;
		}
		return 0;
	}


	int TrafficUtil::getSize(unsigned int *size, const char *str)
	{
		unsigned int sz;
		char *p;

		sz = strtol(str, &p, 0);
		if (p == str)
			return -1;

		if (*p) {
			if (strcasecmp(p, "kb") == 0 || strcasecmp(p, "k")==0)
				sz *= 1024;
			else if (strcasecmp(p, "gb") == 0 || strcasecmp(p, "g")==0)
				sz *= 1024*1024*1024;
			else if (strcasecmp(p, "gbit") == 0)
				sz *= 1024*1024*1024/8;
			else if (strcasecmp(p, "mb") == 0 || strcasecmp(p, "m")==0)
				sz *= 1024*1024;
			else if (strcasecmp(p, "mbit") == 0)
				sz *= 1024*1024/8;
			else if (strcasecmp(p, "kbit") == 0)
				sz *= 1024/8;
			else if (strcasecmp(p, "b") != 0)
				return -1;
		}

		*size = sz;
		return 0;
	}

	int TrafficUtil::getSizeAndCell(unsigned int *size, int *cell_log, char *str)
	{
		char * slash = strchr(str, '/');

		if (slash)
			*slash = 0;

		if (getSize(size, str))
			return -1;

		if (slash) {
			int cell;
			int i;

			if (getInteger(&cell, slash+1, 0))
				return -1;
			*slash = '/';

			for (i=0; i<32; i++) {
				if ((1<<i) == cell) {
					*cell_log = i;
					return 0;
				}
			}
			return -1;
		}
		return 0;
	}

	std::string TrafficUtil::printBurst(unsigned int  rate)
	{
		unsigned int  tmp = rate*8;
		char buf[128];
		int len = sizeof(buf);

		//             1600000
		//int rateInt = (int) (rate / 1.5);
//		int rateInt = (int) rate;

		if (tmp > 1000000)
			snprintf(buf, len, "64k");
		else if (tmp > 5000)
			snprintf(buf, len, "12k");
		else if (tmp > 1000)
			snprintf(buf, len, "6k");
		else
			snprintf(buf, len, "1k");
		
		std::string retstr;
		retstr = buf;
		return retstr;
	}

	std::string TrafficUtil::printRate(unsigned int  rate)
	{
		//unsigned int  tmp = rate*8;
		unsigned int  tmp = rate;
		char buf[128];
		int len = sizeof(buf);

		//             1600000
		//int rateInt = (int) (rate / 1.5);
		int rateInt = (int) rate;

		if (tmp >= 1000000 && (rateInt % 1000000) == 0)
			snprintf(buf, len, "%uMbps", tmp/1000000);
		else if (tmp >= 1000 && (rateInt % 1000) == 0)
			snprintf(buf, len, "%uKbps", tmp/1000);
		else
			snprintf(buf, len, "%ubps",  tmp);
		
		std::string retstr;
		retstr = buf;
		return retstr;
	}
}
