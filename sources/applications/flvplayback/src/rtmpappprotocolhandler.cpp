/*
 *  Copyright (c) 2010,
 *  Gavriloaie Eugen-Andrei (shiretu@gmail.com)
 *
 *  This file is part of crtmpserver.
 *  crtmpserver is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  crtmpserver is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with crtmpserver.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifdef HAS_PROTOCOL_RTMP
#include "rtmpappprotocolhandler.h"
#include "protocols/rtmp/basertmpprotocol.h"
#include "protocols/rtmp/messagefactories/messagefactories.h"
#include "application/baseclientapplication.h"
#include "streaming/baseinnetstream.h"
#include "streaming/streamstypes.h"
#include "protocols/protocolmanager.h"

using namespace app_flvplayback;

RTMPAppProtocolHandler::RTMPAppProtocolHandler(Variant &configuration)
: BaseRTMPAppProtocolHandler(configuration) {

}

RTMPAppProtocolHandler::~RTMPAppProtocolHandler() {

}

bool RTMPAppProtocolHandler::ProcessInvokeGeneric(BaseRTMPProtocol *pFrom,
		Variant &request) {


	string functionName = M_INVOKE_FUNCTION(request);
    if (functionName == "toggleAudioFlag") {
        return ProcessToggleAudioFlag(pFrom, request);
    } else if (functionName == "getAvailableFlvs") {
		return ProcessGetAvailableFlvs(pFrom, request);
	} else if (functionName == "insertMetadata") {
		return ProcessInsertMetadata(pFrom, request);
	} else {
		return BaseRTMPAppProtocolHandler::ProcessInvokeGeneric(pFrom, request);
	}
}

bool RTMPAppProtocolHandler::ProcessToggleAudioFlag(BaseRTMPProtocol *pFrom, Variant &request)
{
    if (M_INVOKE_PARAM(request, 1) != V_STRING || M_INVOKE_PARAM(request, 2) != V_BOOL) {
        FATAL("Invalid request:\n%s", STR(request.ToString()));
        return false;
    }

    std::string streamName = M_INVOKE_PARAM(request, 1);
    bool enableFlag = M_INVOKE_PARAM(request, 2);

    // Get the streams manager
    StreamsManager *pStreamsManager = GetApplication()->GetStreamsManager();

    // Find the stream
    map<uint32_t, BaseStream *> streams = pStreamsManager->FindByTypeByName(
            ST_IN, streamName, true, true);
    if (streams.size() == 0) {
        FATAL("Stream %s not found", STR(streamName));
        return false;
    }

    BaseInStream* pInStream = (BaseInStream *) MAP_VAL(streams.begin());
    pInStream->SetStreamFlag(BaseInStream::FLAG_MUTE, enableFlag);

    return true;
}

bool RTMPAppProtocolHandler::ProcessGetAvailableFlvs(BaseRTMPProtocol *pFrom, Variant &request) {
	Variant parameters;
	parameters.PushToArray(Variant());
	parameters.PushToArray(Variant());

	map<uint32_t, BaseStream *> allInboundStreams =
			GetApplication()->GetStreamsManager()->FindByType(ST_IN_NET, true);

	FOR_MAP(allInboundStreams, uint32_t, BaseStream *, i) {
		parameters[(uint32_t) 1].PushToArray(MAP_VAL(i)->GetName());
	}

	Variant message = GenericMessageFactory::GetInvoke(3, 0, 0, false, 0,
			"SetAvailableFlvs", parameters);

	return SendRTMPMessage(pFrom, message);
}

bool RTMPAppProtocolHandler::ProcessInsertMetadata(BaseRTMPProtocol *pFrom, Variant &request) {
	NYIR;
}
#endif /* HAS_PROTOCOL_RTMP */

