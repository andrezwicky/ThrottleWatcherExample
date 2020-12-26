#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include <boost/format.hpp>
#include <SimConnect.h>

using namespace std;
using boost::format;

/*SimConnect Setup:
*/
int     quit = 0;
HANDLE  hSimConnect = NULL;

static enum GROUP_ID {
    GROUP_THROTTLE
};

struct structThrottleControl {
    double throttlePercentL;
    double throttlePercentR;
};

structThrottleControl        tc = { 0, 0};

static enum EVENT_ID {
    EVENT_THROTTLE,
};

static enum DATA_DEFINE_ID {
    DEFINITION_THROTTLE,
};

static enum DATA_REQUEST_ID {
    REQUEST_THROTTLE,
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*SimConnect Function Prototypes:
*/
void SimConnectOpen();
void CALLBACK MyDispatchProcRD(SIMCONNECT_RECV* pData, DWORD cbData, void* pContext);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main() {

    SimConnectOpen();

    HRESULT hr;
    hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_THROTTLE, "GENERAL ENG THROTTLE LEVER POSITION:1", "percent");
    hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_THROTTLE, "GENERAL ENG THROTTLE LEVER POSITION:2", "percent");
    
    hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_THROTTLE, DEFINITION_THROTTLE, SIMCONNECT_SIMOBJECT_TYPE_USER, SIMCONNECT_PERIOD_SIM_FRAME, SIMCONNECT_DATA_REQUEST_FLAG_CHANGED);

    while (quit==0) {
        SimConnect_CallDispatch(hSimConnect, MyDispatchProcRD, NULL);
    }

	return 0;
}




/*SimConnect Functions;
*/
void SimConnectOpen() {

    if (SUCCEEDED(SimConnect_Open(&hSimConnect, "Client Event", NULL, NULL, NULL, NULL))) {

        std::cout << "Sim Connected" << std::endl;

    }
    else {
        std::cout << "Sim Connection failed!" << std::endl;
    }
}

void CALLBACK MyDispatchProcRD(SIMCONNECT_RECV* pData, DWORD cbData, void* pContext)
{
    HRESULT hr;


    switch (pData->dwID)
    {

    case SIMCONNECT_RECV_ID_SIMOBJECT_DATA:
    {
        SIMCONNECT_RECV_SIMOBJECT_DATA* pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA*)pData;

        switch (pObjData->dwRequestID)
        {

        case REQUEST_THROTTLE:
        {
            DWORD ObjectID = pObjData->dwObjectID;
            structThrottleControl* pS = (structThrottleControl*)&pObjData->dwData;

            tc.throttlePercentL = pS->throttlePercentL;
            tc.throttlePercentR = pS->throttlePercentR;

            cout << format("\nthrottleL%.0f\t throttleR%.0f") % pS->throttlePercentL % pS->throttlePercentR;
            /* you could replace cout with your code here*/
            break;
        }

        default:
            break;
        }
        break;
    }

    case SIMCONNECT_RECV_ID_QUIT:
    {
        quit = 1;
        break;
    }
    
    case SIMCONNECT_RECV_ID_OPEN:
    {
        // enter code to handle SimConnect version information received in a SIMCONNECT_RECV_OPEN structure.
        SIMCONNECT_RECV_OPEN* openData = (SIMCONNECT_RECV_OPEN*)pData;
        cout << format("SimConnect Version#:\t%d.%d") % openData->dwSimConnectVersionMajor % openData->dwSimConnectVersionMinor << endl;
        cout << format("SimConnect Build#:\t%d.%d") % openData->dwSimConnectBuildMajor % openData->dwSimConnectBuildMinor << endl;
        break;

        cout << (SIMCONNECT_RECV_OPEN*)pData << endl;

        break;
    }

    default:
        cout << format("\nReceived:%d") % pData->dwID << endl;
        break;
    }
}
