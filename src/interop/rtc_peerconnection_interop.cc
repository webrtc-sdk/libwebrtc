#include "rtc_peerconnection.h"
#include "src/rtc_peerconnection_impl.h"
#include "interop_api.h"

using namespace libwebrtc;

int LIB_WEBRTC_CALL
RTCMediaRTCStatsList_GetCount (
    rtcMediaRTCStatsListHandle handle
) noexcept
{
    DECLARE_LIST_GET_COUNT(handle, MediaRTCStatsList);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCMediaRTCStatsList_GetItem (
    rtcMediaRTCStatsListHandle handle,
    int index,
    rtcMediaRTCStatsHandle* pOutRetVal
) noexcept
{
    DECLARE_LIST_GET_ITEM(handle, index, pOutRetVal, rtcMediaRTCStatsHandle, MediaRTCStatsList, MediaRTCStats);
}


rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_IsInitialized (
    rtcPeerConnectionHandle handle,
    rtcBool32* pOutRetVal
) noexcept
{
  DECLARE_GET_VALUE(handle, pOutRetVal, rtcBool32, RTCPeerConnection, IsInitialized);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_AddStream (
    rtcPeerConnectionHandle handle,
    rtcMediaStreamHandle stream,
    int* pOutRetVal
) noexcept
{
  if (pOutRetVal) {
    *pOutRetVal = -1;
  }
  CHECK_POINTER_EX(stream, rtcResultU4::kInvalidParameter);
  CHECK_NATIVE_HANDLE(handle);
  
  libwebrtc::scoped_refptr<RTCPeerConnection> p = static_cast<RTCPeerConnection*>(handle);
  libwebrtc::scoped_refptr<RTCMediaStream> pStream = static_cast<RTCMediaStream*>(stream);
  int retVal = p->AddStream(pStream);
  if (pOutRetVal) {
    *pOutRetVal = retVal;
  }
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_RemoveStream (
    rtcPeerConnectionHandle handle,
    rtcMediaStreamHandle stream,
    int* pOutRetVal
) noexcept
{
  if (pOutRetVal) {
    *pOutRetVal = -1;
  }
  CHECK_POINTER_EX(stream, rtcResultU4::kInvalidParameter);
  CHECK_NATIVE_HANDLE(handle);
  
  libwebrtc::scoped_refptr<RTCPeerConnection> p = static_cast<RTCPeerConnection*>(handle);
  libwebrtc::scoped_refptr<RTCMediaStream> pStream = static_cast<RTCMediaStream*>(stream);
  int retVal = p->RemoveStream(pStream);
  if (pOutRetVal) {
    *pOutRetVal = retVal;
  }
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_CreateLocalMediaStream (
    rtcPeerConnectionHandle handle,
    const char* stream_id,
    rtcMediaStreamHandle* pOutRetVal
) noexcept
{
  CHECK_OUT_POINTER(pOutRetVal);
  CHECK_NATIVE_HANDLE(handle);
  
  libwebrtc::scoped_refptr<RTCPeerConnection> p = static_cast<RTCPeerConnection*>(handle);
  libwebrtc::scoped_refptr<RTCMediaStream> pStream = p->CreateLocalMediaStream(string(stream_id));
  *pOutRetVal = static_cast<rtcMediaStreamHandle>(pStream.release());
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_CreateDataChannel (
    rtcPeerConnectionHandle handle,
    const char* label,
    /* [optional, in] */ rtcDataChannelInit* dataChannelDict,
    rtcDataChannelHandle* pOutRetVal
) noexcept
{
  CHECK_OUT_POINTER(pOutRetVal);
  CHECK_NATIVE_HANDLE(handle);

  RTCDataChannelInit initDict{};
  if (dataChannelDict) {
    initDict.ordered = static_cast<bool>(dataChannelDict->ordered);
    initDict.reliable = static_cast<bool>(dataChannelDict->reliable);
    initDict.maxRetransmitTime = dataChannelDict->maxRetransmitTime;
    initDict.maxRetransmits = dataChannelDict->maxRetransmits;
    initDict.protocol = string(dataChannelDict->protocol != rtcDataChannelProtocol::kSCTP ? "quic" : "sctp");
    initDict.negotiated = static_cast<bool>(dataChannelDict->negotiated);
    initDict.id = dataChannelDict->id;
  }

  libwebrtc::scoped_refptr<RTCPeerConnection> p = static_cast<RTCPeerConnection*>(handle);
  libwebrtc::scoped_refptr<RTCDataChannel> pDataChannel = p->CreateDataChannel(string(label), &initDict);
  *pOutRetVal = static_cast<rtcDataChannelHandle>(pDataChannel.release());
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_CreateOffer (
    rtcPeerConnectionHandle handle,
    rtcObjectHandle user_data,
    rtcOnGetSdpSuccessDelegate success,
    rtcOnFailureDelegate failure,
    rtcMediaConstraintsHandle constraints
) noexcept
{
  CHECK_NATIVE_HANDLE(handle);
  CHECK_POINTER_EX(success, rtcResultU4::kInvalidParameter);
  CHECK_POINTER_EX(failure, rtcResultU4::kInvalidParameter);
  CHECK_POINTER_EX(constraints, rtcResultU4::kInvalidParameter);

  libwebrtc::scoped_refptr<RTCPeerConnection> p = static_cast<RTCPeerConnection*>(handle);
  OnSdpCreateSuccess fn_success = [success, user_data](const string sdp, const string type) {
    success(user_data, sdp.c_string(), type.c_string());
  };
  OnSdpCreateFailure fn_failure = [failure, user_data](const char* error) {
    failure(user_data, error);
  };
  libwebrtc::scoped_refptr<RTCMediaConstraints> pConstraints= static_cast<RTCMediaConstraints*>(constraints);
  p->CreateOffer(fn_success, fn_failure, pConstraints);
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_CreateAnswer (
    rtcPeerConnectionHandle handle,
    rtcObjectHandle user_data,
    rtcOnGetSdpSuccessDelegate success,
    rtcOnFailureDelegate failure,
    rtcMediaConstraintsHandle constraints
) noexcept
{
  CHECK_NATIVE_HANDLE(handle);
  CHECK_POINTER_EX(success, rtcResultU4::kInvalidParameter);
  CHECK_POINTER_EX(failure, rtcResultU4::kInvalidParameter);
  CHECK_POINTER_EX(constraints, rtcResultU4::kInvalidParameter);

  libwebrtc::scoped_refptr<RTCPeerConnection> p = static_cast<RTCPeerConnection*>(handle);
  OnSdpCreateSuccess fn_success = [success, user_data](const string sdp, const string type) {
    success(user_data, sdp.c_string(), type.c_string());
  };
  OnSdpCreateFailure fn_failure = [failure, user_data](const char* error) {
    failure(user_data, error);
  };
  libwebrtc::scoped_refptr<RTCMediaConstraints> pConstraints= static_cast<RTCMediaConstraints*>(constraints);
  p->CreateAnswer(fn_success, fn_failure, pConstraints);
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_RestartIce (
    rtcPeerConnectionHandle handle
) noexcept
{
  CHECK_NATIVE_HANDLE(handle);
  libwebrtc::scoped_refptr<RTCPeerConnection> p = static_cast<RTCPeerConnection*>(handle);
  p->RestartIce();
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_Close (
    rtcPeerConnectionHandle handle
) noexcept
{
  CHECK_NATIVE_HANDLE(handle);

  // unregistered
  RTCPeerConnection_UnregisterObserver(handle);

  libwebrtc::scoped_refptr<RTCPeerConnection> p = static_cast<RTCPeerConnection*>(handle);
  p->Close();
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_SetLocalDescription (
    rtcPeerConnectionHandle handle,
    const char* sdp,
    const char* type,
    rtcObjectHandle user_data,
    rtcOnSetSdpSuccessDelegate success,
    rtcOnFailureDelegate failure
) noexcept
{
  CHECK_NATIVE_HANDLE(handle);
  CHECK_POINTER_EX(success, rtcResultU4::kInvalidParameter);
  CHECK_POINTER_EX(failure, rtcResultU4::kInvalidParameter);

  OnSetSdpSuccess fn_success = [success, user_data]() {
    success(user_data);
  };
  OnSetSdpFailure fn_failure = [failure, user_data](const char* error) {
    failure(user_data, error);
  };
  libwebrtc::scoped_refptr<RTCPeerConnection> p = static_cast<RTCPeerConnection*>(handle);
  p->SetLocalDescription(string(sdp), string(type), fn_success, fn_failure);
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_SetRemoteDescription (
    rtcPeerConnectionHandle handle,
    const char* sdp,
    const char* type,
    rtcObjectHandle user_data,
    rtcOnSetSdpSuccessDelegate success,
    rtcOnFailureDelegate failure
) noexcept
{
  CHECK_NATIVE_HANDLE(handle);
  CHECK_POINTER_EX(success, rtcResultU4::kInvalidParameter);
  CHECK_POINTER_EX(failure, rtcResultU4::kInvalidParameter);

  OnSetSdpSuccess fn_success = [success, user_data]() {
    success(user_data);
  };
  OnSetSdpFailure fn_failure = [failure, user_data](const char* error) {
    failure(user_data, error);
  };
  libwebrtc::scoped_refptr<RTCPeerConnection> p = static_cast<RTCPeerConnection*>(handle);
  p->SetRemoteDescription(string(sdp), string(type), fn_success, fn_failure);
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetLocalDescription (
    rtcPeerConnectionHandle handle,
    rtcObjectHandle user_data,
    rtcOnGetSdpSuccessDelegate success,
    rtcOnFailureDelegate failure
) noexcept
{
  CHECK_NATIVE_HANDLE(handle);
  CHECK_POINTER_EX(success, rtcResultU4::kInvalidParameter);
  CHECK_POINTER_EX(failure, rtcResultU4::kInvalidParameter);

  OnGetSdpSuccess fn_success = [success, user_data](const char* sdp, const char* type) {
    success(user_data, sdp, type);
  };
  OnGetSdpFailure fn_failure = [failure, user_data](const char* error) {
    failure(user_data, error);
  };
  libwebrtc::scoped_refptr<RTCPeerConnection> p = static_cast<RTCPeerConnection*>(handle);
  p->GetLocalDescription(fn_success, fn_failure);
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetRemoteDescription (
    rtcPeerConnectionHandle handle,
    rtcObjectHandle user_data,
    rtcOnGetSdpSuccessDelegate success,
    rtcOnFailureDelegate failure
) noexcept
{
  CHECK_NATIVE_HANDLE(handle);
  CHECK_POINTER_EX(success, rtcResultU4::kInvalidParameter);
  CHECK_POINTER_EX(failure, rtcResultU4::kInvalidParameter);

  OnGetSdpSuccess fn_success = [success, user_data](const char* sdp, const char* type) {
    success(user_data, sdp, type);
  };
  OnGetSdpFailure fn_failure = [failure, user_data](const char* error) {
    failure(user_data, error);
  };
  libwebrtc::scoped_refptr<RTCPeerConnection> p = static_cast<RTCPeerConnection*>(handle);
  p->GetRemoteDescription(fn_success, fn_failure);
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_AddCandidate (
    rtcPeerConnectionHandle handle,
    const char* mid, 
    int mid_mline_index,
    const char* candiate
) noexcept
{
  CHECK_NATIVE_HANDLE(handle);
  
  libwebrtc::scoped_refptr<RTCPeerConnection> p = static_cast<RTCPeerConnection*>(handle);
  p->AddCandidate(
    string(mid),
    mid_mline_index,
    string(candiate)
  );
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_RegisterObserver (
    rtcPeerConnectionHandle handle,
    rtcPeerConnectionObserverCallbacks* callbacks
) noexcept
{
  CHECK_NATIVE_HANDLE(handle);
  CHECK_POINTER_EX(callbacks, rtcResultU4::kInvalidParameter);

  // unregistered
  RTCPeerConnection_UnregisterObserver(handle);
  
  libwebrtc::scoped_refptr<RTCPeerConnection> p = static_cast<RTCPeerConnection*>(handle);
  RTCPeerConnectionObserver* pObserver = static_cast<RTCPeerConnectionObserver*>(
    new RTCPeerConnectionObserverImpl(static_cast<void*>(callbacks))
  );
  p->RegisterRTCPeerConnectionObserver(pObserver);
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_UnregisterObserver (
    rtcPeerConnectionHandle handle
) noexcept
{
  CHECK_NATIVE_HANDLE(handle);

  libwebrtc::scoped_refptr<RTCPeerConnection> p = static_cast<RTCPeerConnection*>(handle);
  RTCPeerConnectionObserverImpl* pObserverImpl = static_cast<RTCPeerConnectionObserverImpl*>(p->GetObserver());
  p->DeRegisterRTCPeerConnectionObserver();    
  if (pObserverImpl) {
    delete pObserverImpl;
  }
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetLocalStreams (
    rtcPeerConnectionHandle handle,
    rtcMediaStreamListHandle* pOutRetVal
) noexcept
{
  DECLARE_GET_LIST_HANDLE(handle, pOutRetVal, rtcMediaStreamListHandle, RTCPeerConnection, RTCMediaStreamList, local_streams);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetRemoteStreams (
    rtcPeerConnectionHandle handle,
    rtcMediaStreamListHandle* pOutRetVal
) noexcept
{
  DECLARE_GET_LIST_HANDLE(handle, pOutRetVal, rtcMediaStreamListHandle, RTCPeerConnection, RTCMediaStreamList, remote_streams);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetSenderStats (
    rtcPeerConnectionHandle handle,
    rtcRtpSenderHandle sender,
    rtcObjectHandle user_data,
    rtcOnStatsCollectorSuccessDelegate success,
    rtcOnFailureDelegate failure,
    rtcBool32* pOutRetVal
) noexcept
{
  if (pOutRetVal) {
    *pOutRetVal = rtcBool32::kFalse;
  }
  CHECK_NATIVE_HANDLE(handle);
  CHECK_POINTER_EX(sender, rtcResultU4::kInvalidParameter);
  CHECK_POINTER_EX(success, rtcResultU4::kInvalidParameter);
  CHECK_POINTER_EX(failure, rtcResultU4::kInvalidParameter);

  OnStatsCollectorSuccess fn_success = [success, user_data](const vector<libwebrtc::scoped_refptr<MediaRTCStats>> reports) {
    libwebrtc::scoped_refptr<MediaRTCStatsList> pList = MediaRTCStatsList::Create(reports);
    success(
      user_data,
      static_cast<rtcMediaRTCStatsListHandle>(pList.release())
    );
  };
  OnStatsCollectorFailure fn_failure = [failure, user_data](const char* error) {
    failure(user_data, error);
  };
  libwebrtc::scoped_refptr<RTCPeerConnection> p = static_cast<RTCPeerConnection*>(handle);
  libwebrtc::scoped_refptr<RTCRtpSender> pSender = static_cast<RTCRtpSender*>(sender);
  bool retVal = p->GetStats(pSender, fn_success, fn_failure);
  if (pOutRetVal) {
    *pOutRetVal = static_cast<rtcBool32>(retVal);
  }
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetReceiverStats (
    rtcPeerConnectionHandle handle,
    rtcRtpReceiverHandle receiver,
    rtcObjectHandle user_data,
    rtcOnStatsCollectorSuccessDelegate success,
    rtcOnFailureDelegate failure,
    rtcBool32* pOutRetVal
) noexcept
{
  if (pOutRetVal) {
    *pOutRetVal = rtcBool32::kFalse;
  }
  CHECK_NATIVE_HANDLE(handle);
  CHECK_POINTER_EX(receiver, rtcResultU4::kInvalidParameter);
  CHECK_POINTER_EX(success, rtcResultU4::kInvalidParameter);
  CHECK_POINTER_EX(failure, rtcResultU4::kInvalidParameter);

  OnStatsCollectorSuccess fn_success = [success, user_data](const vector<libwebrtc::scoped_refptr<MediaRTCStats>> reports) {
    libwebrtc::scoped_refptr<MediaRTCStatsList> pList = MediaRTCStatsList::Create(reports);
    success(
      user_data,
      static_cast<rtcMediaRTCStatsListHandle>(pList.release())
    );
  };
  OnStatsCollectorFailure fn_failure = [failure, user_data](const char* error) {
    failure(user_data, error);
  };
  libwebrtc::scoped_refptr<RTCPeerConnection> p = static_cast<RTCPeerConnection*>(handle);
  libwebrtc::scoped_refptr<RTCRtpReceiver> pReceiver = static_cast<RTCRtpReceiver*>(receiver);
  bool retVal = p->GetStats(pReceiver, fn_success, fn_failure);
  if (pOutRetVal) {
    *pOutRetVal = static_cast<rtcBool32>(retVal);
  }
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetStats (
    rtcPeerConnectionHandle handle,
    rtcObjectHandle user_data,
    rtcOnStatsCollectorSuccessDelegate success,
    rtcOnFailureDelegate failure
) noexcept
{
  CHECK_NATIVE_HANDLE(handle);
  CHECK_POINTER_EX(success, rtcResultU4::kInvalidParameter);
  CHECK_POINTER_EX(failure, rtcResultU4::kInvalidParameter);

  OnStatsCollectorSuccess fn_success = [success, user_data](const vector<libwebrtc::scoped_refptr<MediaRTCStats>> reports) {
    libwebrtc::scoped_refptr<MediaRTCStatsList> pList = MediaRTCStatsList::Create(reports);
    success(
      user_data,
      static_cast<rtcMediaRTCStatsListHandle>(pList.release())
    );
  };
  OnStatsCollectorFailure fn_failure = [failure, user_data](const char* error) {
    failure(user_data, error);
  };
  libwebrtc::scoped_refptr<RTCPeerConnection> p = static_cast<RTCPeerConnection*>(handle);
  p->GetStats(fn_success, fn_failure);
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_AddTransceiver1 (
    rtcPeerConnectionHandle handle,
    rtcMediaType media_type,
    rtcRtpTransceiverHandle* pOutRetVal
) noexcept
{
  CHECK_OUT_POINTER(pOutRetVal);
  CHECK_NATIVE_HANDLE(handle);
  
  libwebrtc::scoped_refptr<RTCPeerConnection> p = static_cast<RTCPeerConnection*>(handle);
  libwebrtc::scoped_refptr<RTCRtpTransceiver> pTransceiver = p->AddTransceiver(static_cast<RTCMediaType>(media_type));
  *pOutRetVal = static_cast<rtcRtpTransceiverHandle>(pTransceiver.release());
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_AddTransceiver2 (
    rtcPeerConnectionHandle handle,
    rtcMediaTrackHandle track,
    rtcRtpTransceiverHandle* pOutRetVal
) noexcept
{
  CHECK_OUT_POINTER(pOutRetVal);
  CHECK_NATIVE_HANDLE(handle);
  CHECK_POINTER_EX(track, rtcResultU4::kInvalidParameter);
  
  libwebrtc::scoped_refptr<RTCPeerConnection> p = static_cast<RTCPeerConnection*>(handle);
  libwebrtc::scoped_refptr<RTCMediaTrack> pTrack = static_cast<RTCMediaTrack*>(track);
  libwebrtc::scoped_refptr<RTCRtpTransceiver> pTransceiver = p->AddTransceiver(pTrack);
  *pOutRetVal = static_cast<rtcRtpTransceiverHandle>(pTransceiver.release());
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_AddTransceiver3 (
    rtcPeerConnectionHandle handle,
    rtcMediaTrackHandle track,
    rtcRtpTransceiverInitHandle init,
    rtcRtpTransceiverHandle* pOutRetVal
) noexcept
{
  CHECK_OUT_POINTER(pOutRetVal);
  CHECK_NATIVE_HANDLE(handle);
  CHECK_POINTER_EX(track, rtcResultU4::kInvalidParameter);
  CHECK_POINTER_EX(init, rtcResultU4::kInvalidParameter);
  
  libwebrtc::scoped_refptr<RTCPeerConnection> p = static_cast<RTCPeerConnection*>(handle);
  libwebrtc::scoped_refptr<RTCMediaTrack> pTrack = static_cast<RTCMediaTrack*>(track);
  libwebrtc::scoped_refptr<RTCRtpTransceiverInit> pInit = static_cast<RTCRtpTransceiverInit*>(init);
  libwebrtc::scoped_refptr<RTCRtpTransceiver> pTransceiver = p->AddTransceiver(pTrack, pInit);
  *pOutRetVal = static_cast<rtcRtpTransceiverHandle>(pTransceiver.release());
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_AddTrack (
    rtcPeerConnectionHandle handle,
    rtcMediaTrackHandle track,
    const char* stream_ids,
    rtcRtpSenderHandle* pOutRetVal
) noexcept
{
  CHECK_OUT_POINTER(pOutRetVal);
  CHECK_NATIVE_HANDLE(handle);
  CHECK_POINTER_EX(track, rtcResultU4::kInvalidParameter);

  vector<string> streamIds = string(stream_ids).split("\n");
  libwebrtc::scoped_refptr<RTCPeerConnection> p = static_cast<RTCPeerConnection*>(handle);
  libwebrtc::scoped_refptr<RTCMediaTrack> pTrack = static_cast<RTCMediaTrack*>(track);
  libwebrtc::scoped_refptr<RTCRtpSender> pSender = p->AddTrack(pTrack, streamIds);
  *pOutRetVal = static_cast<rtcRtpSenderHandle>(pSender.release());
  return rtcResultU4::kSuccess;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_RemoveTrack (
    rtcPeerConnectionHandle handle,
    rtcRtpSenderHandle sender,
    rtcBool32* pOutRetVal
) noexcept
{
  if (pOutRetVal) {
    *pOutRetVal = rtcBool32::kFalse;
  }
  CHECK_NATIVE_HANDLE(handle);
  CHECK_POINTER_EX(sender, rtcResultU4::kInvalidParameter);
  
  libwebrtc::scoped_refptr<RTCPeerConnection> p = static_cast<RTCPeerConnection*>(handle);
  libwebrtc::scoped_refptr<RTCRtpSender> pSender = static_cast<RTCRtpSender*>(sender);
  bool retVal = p->RemoveTrack(pSender);
  if (pOutRetVal) {
    *pOutRetVal = static_cast<rtcBool32>(retVal);
  }
  return rtcResultU4::kInvalidOperation;
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetSenders (
    rtcPeerConnectionHandle handle,
    rtcRtpSenderListHandle* pOutRetVal
) noexcept
{
  DECLARE_GET_LIST_HANDLE(handle, pOutRetVal, rtcRtpSenderListHandle, RTCPeerConnection, RTCRtpSenderList, senders);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetTransceivers (
    rtcPeerConnectionHandle handle,
    rtcRtpTransceiverListHandle* pOutRetVal
) noexcept
{
  DECLARE_GET_LIST_HANDLE(handle, pOutRetVal, rtcRtpTransceiverListHandle, RTCPeerConnection, RTCRtpTransceiverList, transceivers);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetReceivers (
    rtcPeerConnectionHandle handle,
    rtcRtpReceiverListHandle* pOutRetVal
) noexcept
{
  DECLARE_GET_LIST_HANDLE(handle, pOutRetVal, rtcRtpReceiverListHandle, RTCPeerConnection, RTCRtpReceiverList, receivers);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetSignalingState (
    rtcPeerConnectionHandle handle,
    rtcSignalingState* pOutRetVal
) noexcept
{
  DECLARE_GET_VALUE(handle, pOutRetVal, rtcSignalingState, RTCPeerConnection, signaling_state);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetIceConnectionState (
    rtcPeerConnectionHandle handle,
    rtcIceConnectionState* pOutRetVal
) noexcept
{
  DECLARE_GET_VALUE(handle, pOutRetVal, rtcIceConnectionState, RTCPeerConnection, ice_connection_state);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetStandardizedIceConnectionState (
    rtcPeerConnectionHandle handle,
    rtcIceConnectionState* pOutRetVal
) noexcept
{
  DECLARE_GET_VALUE(handle, pOutRetVal, rtcIceConnectionState, RTCPeerConnection, standardized_ice_connection_state);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetPeerConnectionState (
    rtcPeerConnectionHandle handle,
    rtcPeerConnectionState* pOutRetVal
) noexcept
{
  DECLARE_GET_VALUE(handle, pOutRetVal, rtcPeerConnectionState, RTCPeerConnection, peer_connection_state);
}

rtcResultU4 LIB_WEBRTC_CALL
RTCPeerConnection_GetIceGatheringState (
    rtcPeerConnectionHandle handle,
    rtcIceGatheringState* pOutRetVal
) noexcept
{
  DECLARE_GET_VALUE(handle, pOutRetVal, rtcIceGatheringState, RTCPeerConnection, ice_gathering_state);
}
