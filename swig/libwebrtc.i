/* libwebrtc.i */
%module libwebrtc

%include <std_string.i>
%include <std_vector.i>

%{
    #include "../include/base/atomicops.h"
    #include "../include/base/refcount.h"
    #include "../include/base/refcountedobject.h"
    #include "../include/base/scoped_ref_ptr.h"

    #include "../include/rtc_media_track.h"
    #include "../include/rtc_audio_frame.h"
    #include "../include/rtc_audio_source.h"
    #include "../include/rtc_audio_track.h"
    #include "../include/rtc_data_channel.h"
    #include "../include/rtc_dtmf_sender.h"
    #include "../include/rtc_dtls_transport.h"
    #include "../include/rtc_frame_cryptor.h"
    #include "../include/rtc_media_stream.h"
    #include "../include/rtc_ice_candidate.h"
    #include "../include/rtc_mediaconstraints.h"
    #include "../include/rtc_peerconnection_factory.h"
    #include "../include/rtc_peerconnection.h"
    #include "../include/rtc_rtp_capabilities.h"
    #include "../include/rtc_rtp_parameters.h"
    #include "../include/rtc_rtp_receiver.h"
    #include "../include/rtc_rtp_sender.h"
    #include "../include/rtc_rtp_transceiver.h"
    #include "../include/rtc_session_description.h"
    #include "../include/rtc_types.h"
    #include "../include/rtc_video_frame.h"
    #include "../include/rtc_video_renderer.h"
    #include "../include/rtc_video_source.h"
    #include "../include/rtc_video_track.h"
    #include "../include/libwebrtc.h"
    #include "../include/helper.h"
%}

%include ../include/base/atomicops.h
%include ../include/base/refcount.h
%include ../include/base/refcountedobject.h
%include ../include/base/scoped_ref_ptr.h
%include ../include/rtc_config.h
%include ../include/rtc_types.h
%include ../include/rtc_media_track.h
%include ../include/rtc_audio_frame.h
%include ../include/rtc_audio_source.h
%include ../include/rtc_audio_track.h
%include ../include/rtc_data_channel.h
%include ../include/rtc_dtmf_sender.h
%include ../include/rtc_dtls_transport.h
%include ../include/rtc_video_frame.h
%include ../include/rtc_video_renderer.h
%include ../include/rtc_video_source.h
%include ../include/rtc_video_track.h
%include ../include/rtc_media_stream.h
%include ../include/rtc_rtp_capabilities.h
%include ../include/rtc_rtp_parameters.h
%include ../include/rtc_rtp_receiver.h
%include ../include/rtc_rtp_sender.h
%include ../include/rtc_rtp_transceiver.h
%include ../include/rtc_session_description.h
%include ../include/rtc_ice_candidate.h
%include ../include/rtc_mediaconstraints.h
%include ../include/rtc_peerconnection.h
%include ../include/rtc_peerconnection_factory.h
%include ../include/rtc_frame_cryptor.h
%include ../include/libwebrtc.h
%include ../include/helper.h

%inline %{
  using namespace libwebrtc;
  RTCPeerConnectionFactory* factoryFromPtr(scoped_refptr<RTCPeerConnectionFactory> ptr) {
    return ptr.get();
  }

   RTCMediaConstraints* constraintsFromPtr(scoped_refptr<RTCMediaConstraints> ptr) {
    return ptr.get();
  }
%}