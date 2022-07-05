/*************************************************
 * Author : Zhenjiang.Zhang
 * Email : zjzhang@126.com
 * Date : 2022-07-05
 * Description : libwebrtc example
 * License : MIT license.
 **************************************************/

#include <stdio.h>
#include <sys/time.h>
#include <condition_variable>
#include <mutex>
#include "base/scoped_ref_ptr.h"
#include "libwebrtc.h"
#include "rtc_audio_device.h"
#include "rtc_media_stream.h"
#include "rtc_media_track.h"
#include "rtc_mediaconstraints.h"
#include "rtc_peerconnection.h"
#include "rtc_peerconnection_factory.h"
#include "rtc_types.h"
#include "rtc_video_device.h"
#include "rtc_video_frame.h"
#include "rtc_video_renderer.h"

using namespace libwebrtc;


class RTCVideoRendererImpl : public RTCVideoRenderer<scoped_refptr<RTCVideoFrame>> {
 public:
  RTCVideoRendererImpl(string tag)
      : seq_(0), tag_(tag) {}
  virtual ~RTCVideoRendererImpl() {}
  virtual void OnFrame(scoped_refptr<RTCVideoFrame> frame) {
    seq_++;
    if (seq_ % 10 == 0) {
      struct timeval tv;
      gettimeofday(&tv, NULL);
      ulong now = tv.tv_sec * 1000 + tv.tv_usec / 1000;
      ulong diff = now - tv_;
      tv_ = now;
      printf("[%s] Frame %dx%d %lu %f\r\n", tag_.c_string(), frame->width(), frame->height(), diff, 10000.0 / diff);
      fflush(stdout);
    }
  }
 private:
  ulong tv_;
  int seq_;
  string tag_;
};

class RTCPeerConnectionObserverImpl : public RTCPeerConnectionObserver {
  scoped_refptr<RTCPeerConnection> pc_;
  string tags_;
 public:
  RTCPeerConnectionObserverImpl(string tags, scoped_refptr<RTCPeerConnection> pc)
      : pc_(pc), tags_(tags){};
  virtual void OnSignalingState(RTCSignalingState state) {
    printf("==%s==>OnSignalingState %d\r\n", tags_.c_string(), state);
    fflush(stdout);
  };

  virtual void OnPeerConnectionState(RTCPeerConnectionState state) {
    printf("==%s==>OnPeerConnectionState %d\r\n", tags_.c_string(), state);
    fflush(stdout);
  };

  virtual void OnIceGatheringState(RTCIceGatheringState state) {
    printf("==%s==>OnIceGatheringState %d\r\n", tags_.c_string(), state);
    fflush(stdout);
  };

  virtual void OnIceConnectionState(RTCIceConnectionState state) {
    printf("==%s==>OnIceConnectionState %d\r\n", tags_.c_string(), state);
    fflush(stdout);
  };

  virtual void OnIceCandidate(scoped_refptr<RTCIceCandidate> candidate) {
    printf("==%s==>OnIceCandidate %s\r\n", tags_.c_string(), candidate->candidate().c_string());
    pc_->AddCandidate(candidate->sdp_mid(), candidate->sdp_mline_index(), candidate->candidate());
    fflush(stdout);
  };

  virtual void OnAddStream(scoped_refptr<RTCMediaStream> stream) {
    auto video = stream->video_tracks();
    auto audio = stream->audio_tracks();
    printf("==%s==>OnAddStream Video %lu\r\n", tags_.c_string(), video.size());
    printf("==%s==>OnAddStream Audio %lu\r\n", tags_.c_string(), audio.size());
    for (scoped_refptr<RTCVideoTrack> track : video.std_vector()) {
      printf("==%s==>OnAddStream RTCVideoTrack %s\r\n", tags_.c_string(), track->id().c_string());
      track->AddRenderer(new RTCVideoRendererImpl(tags_));
    }
    for (scoped_refptr<RTCAudioTrack> track : audio.std_vector()) {
      printf("==%s==>OnAddStream RTCAudioTrack %s\r\n", tags_.c_string(), track->id().c_string());
    }
    fflush(stdout);
  };

  virtual void OnRemoveStream(scoped_refptr<RTCMediaStream> stream) {
    printf("==%s==>OnRemoveStream\r\n", tags_.c_string());
    fflush(stdout);
  };

  virtual void OnDataChannel(scoped_refptr<RTCDataChannel> data_channel) {
    printf("==%s==>OnDataChannel\r\n", tags_.c_string());
    fflush(stdout);
  };

  virtual void OnRenegotiationNeeded() {
    printf("==%s==>OnRenegotiationNeeded\r\n", tags_.c_string());
    fflush(stdout);
  };

  virtual void OnTrack(scoped_refptr<RTCRtpTransceiver> transceiver) {
    printf("==%s==>OnTrack\r\n", tags_.c_string());
    fflush(stdout);
  };

  virtual void OnAddTrack(vector<scoped_refptr<RTCMediaStream>> streams,
                          scoped_refptr<RTCRtpReceiver> receiver) {
    printf("==%s==>OnAddTrack MediaType %d %s\r\n", tags_.c_string(), receiver->media_type(), receiver->track()->id().c_string());
    fflush(stdout);
  };

  virtual void OnRemoveTrack(scoped_refptr<RTCRtpReceiver> receiver) {
    printf("==%s==>OnRemoveTrack\r\n", tags_.c_string());
    fflush(stdout);
  };
};

const RTCConfiguration config_;

typedef void (RTCPeerConnection::*CreateFn) (
    OnSdpCreateSuccess,
    OnSdpCreateFailure,
    scoped_refptr<RTCMediaConstraints>);

int waitCreateDescription(scoped_refptr<RTCPeerConnection> pc,CreateFn fn, scoped_refptr<RTCMediaConstraints> mc, string &sdp,string &type,string &error) {
  int ret = -1;
  bool resultReady = false;
  std::mutex m;
  std::unique_lock<std::mutex> k(m);
  std::condition_variable c;
  (pc->*fn)(
      [&](const libwebrtc::string sdp_, const libwebrtc::string type_) {
        ret=0;
        sdp=sdp_;
        type=type_;
        resultReady = true;
        c.notify_one(); },
      [&](const char* error_) {
        error = string(error_);
        resultReady = true;
        c.notify_one();
      },
      mc);
  c.wait(k, [&] { return resultReady; });
  return ret;
}

typedef void (RTCPeerConnection::*SetLocalFn) (
    string,
    string,
    OnSetSdpSuccess,
    OnSetSdpFailure);
int waitSetDescription(scoped_refptr<RTCPeerConnection> pc,SetLocalFn fn,string sdp,string type,string &error) {
  int  ret= -1;
  bool resultReady = false;
  std::mutex m;
  std::unique_lock<std::mutex> k(m);
  std::condition_variable c;
  (pc->*fn)(sdp,type,
      [&]() {
        ret=0;
        resultReady = true;
        c.notify_one(); },
      [&](const char* error_) {
        error = string(error_);
        resultReady = true;
        c.notify_one();
      });
  c.wait(k, [&] { return resultReady; });
  return ret;
}
typedef void (RTCPeerConnection::*GetLocalFn) (
  OnGetSdpSuccess,
  OnGetSdpFailure);

int waitGetDescription(scoped_refptr<RTCPeerConnection> pc,GetLocalFn fn,string &sdp,string &type,string &error) {
  int  ret= -1;
  bool resultReady = false;
  std::mutex m;
  std::unique_lock<std::mutex> k(m);
  std::condition_variable c;
  (pc->*fn)(
      [&](const char* sdp_, const char* type_) {
        ret=0;
        sdp=string(sdp_);
        type=string(type_);
        resultReady = true;
        c.notify_one(); },
      [&](const char* error) {
        resultReady = true;
        c.notify_one();
      });
  c.wait(k, [&] { return resultReady; });
  return ret;
}


int main() {
  LibWebRTC::Initialize();
  scoped_refptr<RTCPeerConnectionFactory> pcFactory = LibWebRTC::CreateRTCPeerConnectionFactory();
  pcFactory->Initialize();
  //屏幕设备测试
  auto screen_device_ = pcFactory->GetDesktopDevice();
  auto video_screen_ = screen_device_->CreateScreenCapturer();
  auto video_window_ = screen_device_->CreateWindowCapturer();

  //摄像设备测试
  auto video_device_ = pcFactory->GetVideoDevice();
  printf(" Number Of Video Devices %d \r\n", video_device_->NumberOfDevices());
  char deviceNameUTF8[255];
  char deviceUniqueIdUTF8[255];
  video_device_->GetDeviceName(0, deviceNameUTF8, 254, deviceUniqueIdUTF8, 254);
  printf(" Name Of Video Devices [%s] [%s]\r\n", deviceNameUTF8, deviceUniqueIdUTF8);

  //摄像设备捕获
  auto video_caputer_ = video_device_->Create(deviceNameUTF8, 0, 1280, 720, 30);
  auto constraints = RTCMediaConstraints::Create();
  auto video_source_ = pcFactory->CreateVideoSource(video_caputer_, "Test", constraints);
  auto video_track_ = pcFactory->CreateVideoTrack(video_source_, "Video_Test");

  video_track_->AddRenderer(new RTCVideoRendererImpl("Local Renderer"));

  //音频设备测试
  auto audio_device_ = pcFactory->GetAudioDevice();
  printf(" Number Of Audio Recording Devices %d \r\n", audio_device_->RecordingDevices());
  printf(" Number Of Audio Playout Devices %d \r\n", audio_device_->PlayoutDevices());
  char name[RTCAudioDevice::kAdmMaxDeviceNameSize];
  char guid[RTCAudioDevice::kAdmMaxGuidSize];
  audio_device_->RecordingDeviceName(0, name, guid);
  printf(" Name Of Audio Recording Devices [%s] [%s] \r\n", name, guid);
  audio_device_->PlayoutDeviceName(0, name, guid);
  printf(" Name Of Audio Playout Devices [%s] [%s] \r\n", name, guid);
  audio_device_->SetPlayoutDevice(0);
  audio_device_->SetRecordingDevice(0);
  //音频设备捕获
  auto audio_source_ = pcFactory->CreateAudioSource("Test");
  auto audio_track_ = pcFactory->CreateAudioTrack(audio_source_, "Audio_Test");

  auto pc_sender = pcFactory->Create(config_, RTCMediaConstraints::Create());
  auto pc_receiver = pcFactory->Create(config_, RTCMediaConstraints::Create());
  // TODO 后续使用AddTransceiver代理AddStream

  auto stream_ = pcFactory->CreateStream("Test");
  //流里面添加Track
  stream_->AddTrack(pcFactory->CreateAudioTrack(audio_source_, "Audio_Test1"));
  stream_->AddTrack(pcFactory->CreateAudioTrack(audio_source_, "Audio_Test2"));
  stream_->AddTrack(pcFactory->CreateAudioTrack(audio_source_, "Audio_Test3"));
  stream_->AddTrack(pcFactory->CreateAudioTrack(audio_source_, "Audio_Test4"));
  stream_->AddTrack(video_track_);
  // pc中添加流
  pc_sender->AddStream(stream_);
  // pc_receiver->AddStream(stream_);
  //设置状态回调
  pc_receiver->RegisterRTCPeerConnectionObserver(new RTCPeerConnectionObserverImpl("answer", pc_sender));
  pc_sender->RegisterRTCPeerConnectionObserver(new RTCPeerConnectionObserverImpl("offer", pc_receiver));
  string sdp,type,error;
  auto pc_mc = RTCMediaConstraints::Create();
  int ret=0;
  ret=waitCreateDescription(pc_sender,&RTCPeerConnection::CreateOffer, pc_mc, sdp,type,error);
  if (ret<0){
    printf("=== %s ===\r\n", error.c_string());
  }
  ret=waitSetDescription(pc_sender,&RTCPeerConnection::SetLocalDescription,sdp,type,error);
  if (ret<0){
    printf("=== %s ===\r\n", error.c_string());
  }
  ret=waitGetDescription(pc_sender,&RTCPeerConnection::GetLocalDescription,sdp,type,error);
  if (ret<0){
    printf("=== %s ===\r\n", error.c_string());
  }
  ret=waitSetDescription(pc_receiver,&RTCPeerConnection::SetRemoteDescription,sdp,type,error);
  if (ret<0){
    printf("=== %s ===\r\n", error.c_string());
  }
  ret=waitCreateDescription(pc_receiver,&RTCPeerConnection::CreateAnswer, pc_mc, sdp,type,error);
  if (ret<0){
    printf("=== %s ===\r\n", error.c_string());
  }
  ret=waitSetDescription(pc_receiver,&RTCPeerConnection::SetLocalDescription,sdp,type,error);  
  if (ret<0){
    printf("=== %s ===\r\n", error.c_string());
  }
  ret=waitGetDescription(pc_receiver,&RTCPeerConnection::GetLocalDescription,sdp,type,error);  
  if (ret<0){
    printf("=== %s ===\r\n", error.c_string());
  }
  ret=waitSetDescription(pc_sender,&RTCPeerConnection::SetRemoteDescription,sdp,type,error);
  if (ret<0){
    printf("=== %s ===\r\n", error.c_string());
  }
  getchar();
  pc_sender->Close();
  pc_receiver->Close();
  printf("Fininsh\r\n");
  audio_device_ = nullptr;
  video_device_ = nullptr;
  video_screen_ = nullptr;
  video_window_ = nullptr;
  pcFactory->Terminate();
}