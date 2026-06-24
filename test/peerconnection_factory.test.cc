#include "test/gtest.h"

#include "libwebrtc.h"
#include "rtc_audio_device.h"
#include "rtc_mediaconstraints.h"
#include "rtc_peerconnection.h"
#include "rtc_peerconnection_factory.h"

using libwebrtc::LibWebRTC;
using libwebrtc::scoped_refptr;

// Audio device enumeration must work immediately after the factory is
// initialised, WITHOUT first creating an RTCPeerConnection.
//
// Background: on desktop platforms the underlying webrtc::AudioDeviceModule
// returns the "uninitialized" sentinel (-1) from RecordingDevices()/
// PlayoutDevices() until AudioDeviceModule::Init() has been called. That call
// used to be deferred until the voice engine spun up during PeerConnection
// creation, so enumeration silently returned nothing beforehand. The factory
// now calls Init() eagerly when it creates the ADM, so the device count must:
//   1. not be the -1 uninitialized sentinel before any PeerConnection exists,
//      and
//   2. be identical before and after a PeerConnection is created (enumeration
//      must not depend on the PeerConnection lifecycle).
//
// Both checks are robust on headless machines: with no audio hardware the
// count is simply 0 both times, which still satisfies (1) and (2). The
// pre-fix behaviour (-1 before, real count after) fails both.
TEST(AudioDevice, EnumerationWorksBeforePeerConnection) {
  ASSERT_TRUE(LibWebRTC::Initialize());

  scoped_refptr<libwebrtc::RTCPeerConnectionFactory> factory =
      LibWebRTC::CreateRTCPeerConnectionFactory();
  ASSERT_TRUE(factory.get() != nullptr);
  ASSERT_TRUE(factory->Initialize());

  scoped_refptr<libwebrtc::RTCAudioDevice> audio_device =
      factory->GetAudioDevice();
  ASSERT_TRUE(audio_device.get() != nullptr);

  // (1) Enumeration before any PeerConnection has been created.
  const int16_t recording_before = audio_device->RecordingDevices();
  const int16_t playout_before = audio_device->PlayoutDevices();
  EXPECT_GE(recording_before, 0);  // not the -1 uninitialized sentinel
  EXPECT_GE(playout_before, 0);

  // Create a PeerConnection, which is what historically triggered the ADM
  // initialization.
  libwebrtc::RTCConfiguration config{};
  scoped_refptr<libwebrtc::RTCMediaConstraints> constraints =
      libwebrtc::RTCMediaConstraints::Create();
  scoped_refptr<libwebrtc::RTCPeerConnection> pc =
      factory->Create(config, constraints);
  ASSERT_TRUE(pc.get() != nullptr);

  // (2) Enumeration after a PeerConnection exists must match the earlier
  // result — the device set does not depend on the PeerConnection.
  const int16_t recording_after = audio_device->RecordingDevices();
  const int16_t playout_after = audio_device->PlayoutDevices();
  EXPECT_EQ(recording_before, recording_after);
  EXPECT_EQ(playout_before, playout_after);

  factory->Delete(pc);
  pc = nullptr;
  audio_device = nullptr;
  factory->Terminate();
  factory = nullptr;
  LibWebRTC::Terminate();
}
