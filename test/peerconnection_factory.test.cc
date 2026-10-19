#include "test/gtest.h"

#include "libwebrtc.h"
#include "rtc_audio_device.h"
#include "rtc_mediaconstraints.h"
#include "rtc_peerconnection.h"
#include "rtc_peerconnection_factory.h"
#include "rtc_rtp_sender.h"

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

TEST(AudioDevice, ExternalOnlyFactoryRejectsMicrophoneAndAllowsCustomSources) {
  ASSERT_TRUE(LibWebRTC::Initialize());
  libwebrtc::RTCPeerConnectionFactoryOptions options;
  options.external_audio_input_only = true;
  auto factory = LibWebRTC::CreateRTCPeerConnectionFactory(options);
  ASSERT_TRUE(factory->Initialize());
  EXPECT_EQ(factory->CreateAudioSource("mic").get(), nullptr);
  auto source = factory->CreateAudioSource(
      "external", libwebrtc::RTCAudioSource::SourceType::kCustom);
  ASSERT_NE(source.get(), nullptr);
  auto track = factory->CreateAudioTrack(source, "external-track");
  ASSERT_NE(track.get(), nullptr);

  libwebrtc::RTCConfiguration config{};
  auto constraints = libwebrtc::RTCMediaConstraints::Create();
  auto first = factory->Create(config, constraints);
  auto second = factory->Create(config, constraints);
  ASSERT_NE(first.get(), nullptr);
  ASSERT_NE(second.get(), nullptr);
  auto first_sender = first->AddTrack(track, {});
  auto second_sender = second->AddTrack(track, {});
  ASSERT_NE(first_sender.get(), nullptr);
  ASSERT_NE(second_sender.get(), nullptr);
  EXPECT_TRUE(first->RemoveTrack(first_sender));
  EXPECT_TRUE(second->RemoveTrack(second_sender));
  first_sender = nullptr;
  second_sender = nullptr;
  first->Close();
  second->Close();
  factory->Delete(first);
  factory->Delete(second);
  first = nullptr;
  second = nullptr;
  track = nullptr;
  source = nullptr;
  factory->Terminate();
  factory = nullptr;
  LibWebRTC::Terminate();
}

TEST(AudioDevice, ExternalOnlyPolicyIsCopiedAndDefaultFactoryStaysIndependent) {
  ASSERT_TRUE(LibWebRTC::Initialize());
  libwebrtc::RTCPeerConnectionFactoryOptions options;
  options.external_audio_input_only = true;
  auto external = LibWebRTC::CreateRTCPeerConnectionFactory(options);
  options.external_audio_input_only = false;
  auto ordinary = LibWebRTC::CreateRTCPeerConnectionFactory();
  ASSERT_TRUE(external->Initialize());
  ASSERT_TRUE(ordinary->Initialize());
  EXPECT_EQ(external->CreateAudioSource("external-mic").get(), nullptr);
  auto mic = ordinary->CreateAudioSource("ordinary-mic");
  auto custom = ordinary->CreateAudioSource(
      "ordinary-custom", libwebrtc::RTCAudioSource::SourceType::kCustom);
  ASSERT_NE(mic.get(), nullptr);
  ASSERT_NE(custom.get(), nullptr);
  EXPECT_EQ(external->CreateAudioTrack(mic, "foreign-mic").get(), nullptr);

  libwebrtc::RTCConfiguration config{};
  auto constraints = libwebrtc::RTCMediaConstraints::Create();
  auto external_pc = external->Create(config, constraints);
  auto ordinary_pc = ordinary->Create(config, constraints);
  ASSERT_NE(external_pc.get(), nullptr);
  ASSERT_NE(ordinary_pc.get(), nullptr);
  external_pc->Close();
  ordinary_pc->Close();
  external->Delete(external_pc);
  ordinary->Delete(ordinary_pc);
  external_pc = nullptr;
  ordinary_pc = nullptr;
  custom = nullptr;
  mic = nullptr;
  external->Terminate();
  ordinary->Terminate();
  external = nullptr;
  ordinary = nullptr;
  LibWebRTC::Terminate();
}

TEST(AudioDevice, DefaultOptionsAllowMicrophoneAndRejectNullSource) {
  ASSERT_TRUE(LibWebRTC::Initialize());
  libwebrtc::RTCPeerConnectionFactoryOptions options;
  EXPECT_FALSE(options.external_audio_input_only);
  auto factory = LibWebRTC::CreateRTCPeerConnectionFactory(options);
  ASSERT_TRUE(factory->Initialize());
  auto source = factory->CreateAudioSource("ordinary-mic");
  ASSERT_NE(source.get(), nullptr);
  auto track = factory->CreateAudioTrack(source, "ordinary-track");
  EXPECT_NE(track.get(), nullptr);
  EXPECT_EQ(factory->CreateAudioTrack(nullptr, "null-source").get(), nullptr);
  track = nullptr;
  source = nullptr;
  factory->Terminate();
  factory = nullptr;
  LibWebRTC::Terminate();
}
