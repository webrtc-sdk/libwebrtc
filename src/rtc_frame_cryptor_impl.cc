#include "rtc_frame_cryptor_impl.h"

#include "rtc_peerconnection_factory_impl.h"

namespace libwebrtc {

scoped_refptr<RTCFrameCryptor> FrameCryptorFactory::frameCryptorFromRtpSender(
    scoped_refptr<RTCPeerConnectionFactory> factory,
    const string participant_id, scoped_refptr<RTCRtpSender> sender,
    Algorithm algorithm, scoped_refptr<KeyProvider> key_provider) {
  return new RefCountedObject<RTCFrameCryptorImpl>(
      factory, participant_id, algorithm, key_provider, sender);
}

/// Create a frame cyrptor from a [RTCRtpReceiver].
scoped_refptr<RTCFrameCryptor> FrameCryptorFactory::frameCryptorFromRtpReceiver(
    scoped_refptr<RTCPeerConnectionFactory> factory,
    const string participant_id, scoped_refptr<RTCRtpReceiver> receiver,
    Algorithm algorithm, scoped_refptr<KeyProvider> key_provider) {
  return new RefCountedObject<RTCFrameCryptorImpl>(
      factory, participant_id, algorithm, key_provider, receiver);
}

webrtc::FrameCryptorTransformer::Algorithm AlgorithmToFrameCryptorAlgorithm(
    Algorithm algorithm) {
  switch (algorithm) {
    case Algorithm::kAesGcm:
      return webrtc::FrameCryptorTransformer::Algorithm::kAesGcm;
    case Algorithm::kAesCbc:
      return webrtc::FrameCryptorTransformer::Algorithm::kAesCbc;
    default:
      return webrtc::FrameCryptorTransformer::Algorithm::kAesGcm;
  }
}

RTCFrameCryptorImpl::RTCFrameCryptorImpl(
    scoped_refptr<RTCPeerConnectionFactory> factory,
    const string participant_id, Algorithm algorithm,
    scoped_refptr<KeyProvider> key_provider, scoped_refptr<RTCRtpSender> sender)
    : participant_id_(participant_id),
      enabled_(false),
      key_index_(0),
      key_provider_(key_provider),
      sender_(sender),
      observer_(rtc::make_ref_counted<RTCFrameCryptorObserverAdapter>()) {
  auto factoryImpl = static_cast<RTCPeerConnectionFactoryImpl*>(factory.get());
  auto keyImpl = static_cast<DefaultKeyProviderImpl*>(key_provider.get());
  RTCRtpSenderImpl* impl = static_cast<RTCRtpSenderImpl*>(sender.get());
  auto mediaType =
      impl->rtc_rtp_sender()->track()->kind() == "audio"
          ? webrtc::FrameCryptorTransformer::MediaType::kAudioFrame
          : webrtc::FrameCryptorTransformer::MediaType::kVideoFrame;
  e2ee_transformer_ = rtc::scoped_refptr<webrtc::FrameCryptorTransformer>(
      new webrtc::FrameCryptorTransformer(
          factoryImpl->signaling_thread(), to_std_string(participant_id_),
          mediaType, AlgorithmToFrameCryptorAlgorithm(algorithm),
          keyImpl->rtc_key_provider()));
  e2ee_transformer_->RegisterFrameCryptorTransformerObserver(observer_);
  impl->rtc_rtp_sender()->SetEncoderToPacketizerFrameTransformer(
      e2ee_transformer_);
  e2ee_transformer_->SetEnabled(false);
}

RTCFrameCryptorImpl::RTCFrameCryptorImpl(
    scoped_refptr<RTCPeerConnectionFactory> factory,
    const string participant_id, Algorithm algorithm,
    scoped_refptr<KeyProvider> key_provider,
    scoped_refptr<RTCRtpReceiver> receiver)
    : participant_id_(participant_id),
      enabled_(false),
      key_index_(0),
      key_provider_(key_provider),
      receiver_(receiver),
      observer_(
          rtc::make_ref_counted<libwebrtc::RTCFrameCryptorObserverAdapter>()) {
  auto factoryImpl = static_cast<RTCPeerConnectionFactoryImpl*>(factory.get());
  auto keyImpl = static_cast<DefaultKeyProviderImpl*>(key_provider.get());
  RTCRtpReceiverImpl* impl = static_cast<RTCRtpReceiverImpl*>(receiver.get());
  auto mediaType =
      impl->rtp_receiver()->track()->kind() == "audio"
          ? webrtc::FrameCryptorTransformer::MediaType::kAudioFrame
          : webrtc::FrameCryptorTransformer::MediaType::kVideoFrame;
  e2ee_transformer_ = rtc::scoped_refptr<webrtc::FrameCryptorTransformer>(
      new webrtc::FrameCryptorTransformer(
          factoryImpl->signaling_thread(), to_std_string(participant_id_),
          mediaType, AlgorithmToFrameCryptorAlgorithm(algorithm),
          keyImpl->rtc_key_provider()));
  e2ee_transformer_->RegisterFrameCryptorTransformerObserver(observer_);
  impl->rtp_receiver()->SetDepacketizerToDecoderFrameTransformer(
      e2ee_transformer_);
  e2ee_transformer_->SetEnabled(false);
}

RTCFrameCryptorImpl::~RTCFrameCryptorImpl() {}

bool RTCFrameCryptorImpl::SetEnabled(bool enabled) {
  webrtc::MutexLock lock(&mutex_);
  enabled_ = enabled;
  e2ee_transformer_->SetEnabled(enabled_);
  return true;
}

void RTCFrameCryptorImpl::RegisterRTCFrameCryptorObserver(
    scoped_refptr<RTCFrameCryptorObserver> observer) {
  webrtc::MutexLock lock(&mutex_);
  observer_->RegisterObserver(observer);
}

void RTCFrameCryptorImpl::DeRegisterRTCFrameCryptorObserver() {
  webrtc::MutexLock lock(&mutex_);
  observer_->DeRegisterObserver();
  e2ee_transformer_->UnRegisterFrameCryptorTransformerObserver();
}

void RTCFrameCryptorObserverAdapter::OnFrameCryptionStateChanged(
    const std::string participant_id, webrtc::FrameCryptionState error) {
  {
    RTCFrameCryptionState state = RTCFrameCryptionState::kNew;
    switch (error) {
      case webrtc::FrameCryptionState::kNew:
        state = RTCFrameCryptionState::kNew;
        break;
      case webrtc::FrameCryptionState::kOk:
        state = RTCFrameCryptionState::kOk;
        break;
      case webrtc::FrameCryptionState::kDecryptionFailed:
        state = RTCFrameCryptionState::kDecryptionFailed;
        break;
      case webrtc::FrameCryptionState::kEncryptionFailed:
        state = RTCFrameCryptionState::kEncryptionFailed;
        break;
      case webrtc::FrameCryptionState::kMissingKey:
        state = RTCFrameCryptionState::kMissingKey;
        break;
      case webrtc::FrameCryptionState::kKeyRatcheted:
        state = RTCFrameCryptionState::kKeyRatcheted;
        break;
      case webrtc::FrameCryptionState::kInternalError:
        state = RTCFrameCryptionState::kInternalError;
        break;
    }
    webrtc::MutexLock lock(&mutex_);
    if (observer_) {
      observer_->OnFrameCryptionStateChanged(participant_id, state);
    }
  }
}

bool RTCFrameCryptorImpl::enabled() const {
  webrtc::MutexLock lock(&mutex_);
  return enabled_;
}

bool RTCFrameCryptorImpl::SetKeyIndex(int index) {
  webrtc::MutexLock lock(&mutex_);
  key_index_ = index;
  e2ee_transformer_->SetKeyIndex(key_index_);
  return true;
}

int RTCFrameCryptorImpl::key_index() const {
  webrtc::MutexLock lock(&mutex_);
  return key_index_;
}

scoped_refptr<KeyProvider> KeyProvider::Create(KeyProviderOptions* options) {
  return new RefCountedObject<DefaultKeyProviderImpl>(options);
}

}  // namespace libwebrtc
