#ifndef ZERO_ROUTE_PPPD_AUTH_H
#define ZERO_ROUTE_PPPD_AUTH_H

#include <iostream>
#include <string>
#include <vector>

#define MAX_MTU 5000

namespace maxnet{


	class RoutePPPDAuthenticate {

		public:

			RoutePPPDAuthenticate();
			~RoutePPPDAuthenticate();
			bool isVaild();
			void output();

	
			void setRequirePAP(bool require_pap);
			void setRequireCHAP(bool require_chap);
			void setRequireMSCHAP(bool require_mschap);
			void setRequireMSCHAPV2(bool require_mschapv2);
			void setRequireEAP(bool require_eap);
			
			bool getRequirePAP();
			bool getRequireCHAP();
			bool getRequireMSCHAP();
			bool getRequireMSCHAPV2();
			bool getRequireEAP();


		private:
			bool require_pap;
			bool require_chap;
			bool require_mschap;
			bool require_mschapv2;
			bool require_eap;
	};

}

#endif // _ZERO_SYSTEM_H
