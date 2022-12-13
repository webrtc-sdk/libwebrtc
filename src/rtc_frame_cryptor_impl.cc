#include "rtc_frame_cryptor_impl.h"

namespace libwebrtc {

scoped_refptr<RTCFrameCryptor> FrameCryptorFactory::frameCryptorFromRtpSender(
    scoped_refptr<RTCRtpSender> sender,
    Algorithm algorithm,
    scoped_refptr<KeyManager> keyManager) {
  return new RefCountedObject<RTCFrameCryptorImpl>(algorithm, keyManager,
                                                   sender);
}

/// Create a frame cyrptor from a [RTCRtpReceiver].
scoped_refptr<RTCFrameCryptor> FrameCryptorFactory::frameCryptorFromRtpReceiver(
    scoped_refptr<RTCRtpReceiver> receiver,
    Algorithm algorithm,
    scoped_refptr<KeyManager> keyManager) {
  return new RefCountedObject<RTCFrameCryptorImpl>(algorithm, keyManager,
                                                   receiver);
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


RTCFrameCryptorImpl::RTCFrameCryptorImpl(Algorithm algorithm,
                                         scoped_refptr<KeyManager> key_manager,
                                         scoped_refptr<RTCRtpSender> sender)
    : enabled_(false),
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
          mediaType, AlgorithmToFrameCryptorAlgorithm(algorithm),
          keyImpl->rtc_key_manager()));

  impl->rtc_rtp_sender()->SetEncoderToPacketizerFrameTransformer(
      e2ee_transformer_);
  e2ee_transformer_->SetEnabled(false);
}

RTCFrameCryptorImpl::RTCFrameCryptorImpl(Algorithm algorithm,
                                         scoped_refptr<KeyManager> key_manager,
                                         scoped_refptr<RTCRtpReceiver> receiver)
    : enabled_(false),
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
          mediaType, AlgorithmToFrameCryptorAlgorithm(algorithm),
          keyImpl->rtc_key_manager()));

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

scoped_refptr<KeyManager> KeyManager::Create() {
  return new RefCountedObject<DefaultKeyManagerImpl>();
}

DefaultKeyManagerImpl::DefaultKeyManagerImpl() {
  impl_ = new rtc::RefCountedObject<DefaultKeyManagerImpl::KeyManagerImpl>(this);
}

DefaultKeyManagerImpl::~DefaultKeyManagerImpl() {}

bool DefaultKeyManagerImpl::SetKey(int index, vector<uint8_t> key) {
  if (index > webrtc::KeyManager::kMaxKeySize) {
    return false;
  }
  webrtc::MutexLock lock(&mutex_);
  if (index + 1 > (int)keys_.size()) {
    keys_.resize(index + 1);
  }
  keys_[index] = key.std_vector();
  return true;
}

bool DefaultKeyManagerImpl::SetKeys(vector<vector<uint8_t>> keys) {
  webrtc::MutexLock lock(&mutex_);
  keys_.clear();
  for (auto key : keys.std_vector()) {
    keys_.push_back(key.std_vector());
  }
  return true;
}

const vector<vector<uint8_t>> DefaultKeyManagerImpl::GetKeys() const {
  webrtc::MutexLock lock(&mutex_);
  return keys_;
}

const std::vector<std::vector<uint8_t>> DefaultKeyManagerImpl::keys() const {
  webrtc::MutexLock lock(&mutex_);
  return keys_;
}

}  // namespace libwebrtc
