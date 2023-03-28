#include "rtc_frame_cryptor_impl.h"

namespace libwebrtc {

scoped_refptr<RTCFrameCryptor> FrameCryptorFactory::frameCryptorFromRtpSender(
    const string participant_id,
    scoped_refptr<RTCRtpSender> sender,
    Algorithm algorithm,
    scoped_refptr<KeyManager> keyManager) {
  return new RefCountedObject<RTCFrameCryptorImpl>(participant_id, algorithm,
                                                   keyManager, sender);
}

/// Create a frame cyrptor from a [RTCRtpReceiver].
scoped_refptr<RTCFrameCryptor> FrameCryptorFactory::frameCryptorFromRtpReceiver(
    const string participant_id,
    scoped_refptr<RTCRtpReceiver> receiver,
    Algorithm algorithm,
    scoped_refptr<KeyManager> keyManager) {
  return new RefCountedObject<RTCFrameCryptorImpl>(participant_id, algorithm,
                                                   keyManager, receiver);
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

RTCFrameCryptorImpl::RTCFrameCryptorImpl(const string participant_id,
                                         Algorithm algorithm,
                                         scoped_refptr<KeyManager> key_manager,
                                         scoped_refptr<RTCRtpSender> sender)
    : participant_id_(participant_id),
      enabled_(false),
      key_index_(0),
      key_manager_(key_manager),
      sender_(sender) {
  auto keyImpl = static_cast<DefaultKeyManagerImpl*>(key_manager.get());
  RTCRtpSenderImpl* impl = static_cast<RTCRtpSenderImpl*>(sender.get());
  auto mediaType =
      impl->rtc_rtp_sender()->track()->kind() == "audio"
          ? webrtc::FrameCryptorTransformer::MediaType::kAudioFrame
          : webrtc::FrameCryptorTransformer::MediaType::kVideoFrame;
  e2ee_transformer_ = rtc::scoped_refptr<webrtc::FrameCryptorTransformer>(
      new webrtc::FrameCryptorTransformer(
          participant_id_.std_string(), mediaType,
          AlgorithmToFrameCryptorAlgorithm(algorithm),
          keyImpl->rtc_key_manager()));
  e2ee_transformer_->SetFrameCryptorTransformerObserver(this);
  impl->rtc_rtp_sender()->SetEncoderToPacketizerFrameTransformer(
      e2ee_transformer_);
  e2ee_transformer_->SetEnabled(false);
}

RTCFrameCryptorImpl::RTCFrameCryptorImpl(const string participant_id,
                                         Algorithm algorithm,
                                         scoped_refptr<KeyManager> key_manager,
                                         scoped_refptr<RTCRtpReceiver> receiver)
    : participant_id_(participant_id),
      enabled_(false),
      key_index_(0),
      key_manager_(key_manager),
      receiver_(receiver) {
  auto keyImpl = static_cast<DefaultKeyManagerImpl*>(key_manager.get());
  RTCRtpReceiverImpl* impl = static_cast<RTCRtpReceiverImpl*>(receiver.get());
  auto mediaType =
      impl->rtp_receiver()->track()->kind() == "audio"
          ? webrtc::FrameCryptorTransformer::MediaType::kAudioFrame
          : webrtc::FrameCryptorTransformer::MediaType::kVideoFrame;
  e2ee_transformer_ = rtc::scoped_refptr<webrtc::FrameCryptorTransformer>(
      new webrtc::FrameCryptorTransformer(
          participant_id_.std_string(), mediaType,
          AlgorithmToFrameCryptorAlgorithm(algorithm),
          keyImpl->rtc_key_manager()));
  e2ee_transformer_->SetFrameCryptorTransformerObserver(this);
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
    RTCFrameCryptorObserver* observer) {
  webrtc::MutexLock lock(&mutex_);
  observer_ = observer;
}

void RTCFrameCryptorImpl::DeRegisterRTCFrameCryptorObserver() {
  webrtc::MutexLock lock(&mutex_);
  observer_ = nullptr;
  e2ee_transformer_->SetFrameCryptorTransformerObserver(nullptr);
}

void RTCFrameCryptorImpl::OnFrameCryptionStateChanged(
    const std::string participant_id,
    webrtc::FrameCryptionState error) {
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
      observer_->OnFrameCryptionStateChanged(participant_id_, state);
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

scoped_refptr<KeyManager> KeyManager::Create(KeyProviderOptions *options) {
  return new RefCountedObject<DefaultKeyManagerImpl>(options);
}

}  // namespace libwebrtc
