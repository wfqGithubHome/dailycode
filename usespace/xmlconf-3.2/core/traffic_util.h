#ifndef _TRAFFIC_UTIL_
#define _TRAFFIC_UTIL_

#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <sys/types.h>
#include <opt.h>
#include <asm/types.h>


namespace maxnet{

	class TrafficUtil
    {
		typedef struct RATE_SUFFIX
		{
			const char *name;
			double scale;
		}_RATE_SUFFIX;
		public:
			// Construction and Destruction.
			TrafficUtil();
			~TrafficUtil();
			static int		getInteger(int *val, const char *arg, int base);
			static int		getUnsigned(unsigned *val, const char *arg, int base);
			static int		getU64(__u64 *val, const char *arg, int base);
			static int		getU32(__u32 *val, const char *arg, int base);
			static int		getU16(__u16 *val, const char *arg, int base);
			static int		getU8(__u8 *val, const char *arg, int base);
			static int		getU16(__s16 *val, const char *arg, int base);
			static int		getS8(__s8 *val, const char *arg, int base);

			static int		getRate(unsigned int *rate, const char *str);
			static int		getRateAndCell(unsigned int  *rate, int *cell_log, char *str);
			static int		getSize(unsigned int  *size, const char *str);
			static int		getSizeAndCell(unsigned int  *size, int *cell_log, char *str);

			static std::string		printRate(unsigned int  rate);
			static std::string		printBurst(unsigned int  rate);
		private:
			/* See http://physics.nist.gov/cuu/Units/binary.html */
			static const RATE_SUFFIX suffixes[];
	};
}


#endif
