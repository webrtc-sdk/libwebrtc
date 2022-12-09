#include "rtc_frame_cryptor_impl.h"

namespace libwebrtc {

scoped_refptr<RTCFrameCryptor> FrameCyrptorFactory::frameCyrptorFromRtpSender(
    scoped_refptr<RTCRtpSender> sender,
    Algorithm algorithm,
    scoped_refptr<KeyManager> keyManager) {
  return new RefCountedObject<RTCFrameCryptorImpl>(algorithm, keyManager,
                                                   sender);
}

/// Create a frame cyrptor from a [RTCRtpReceiver].
scoped_refptr<RTCFrameCryptor> FrameCyrptorFactory::frameCyrptorFromRtpReceiver(
    scoped_refptr<RTCRtpReceiver> receiver,
    Algorithm algorithm,
    scoped_refptr<KeyManager> keyManager) {
  return new RefCountedObject<RTCFrameCryptorImpl>(algorithm, keyManager,
                                                   receiver);
}

RTCFrameCryptorImpl::RTCFrameCryptorImpl(Algorithm algorithm,
                                         scoped_refptr<KeyManager> key_manager,
                                         scoped_refptr<RTCRtpSender> sender)
    : enabled_(false),
      key_index_(0),
      key_manager_(key_manager),
      sender_(sender) {
  std::shared_ptr<webrtc::KeyManager> keyImpl(
      static_cast<DefaultKeyManagerImpl*>(key_manager.get()));
  RTCRtpSenderImpl* impl = static_cast<RTCRtpSenderImpl*>(sender.get());
  auto mediaType =
      impl->rtc_rtp_sender()->track()->kind() == "audio"
          ? webrtc::FrameCryptorTransformer::MediaType::kAudioFrame
          : webrtc::FrameCryptorTransformer::MediaType::kVideoFrame;
  e2ee_transformer_ = rtc::scoped_refptr<webrtc::FrameCryptorTransformer>(
      new webrtc::FrameCryptorTransformer(
          mediaType, webrtc::FrameCryptorTransformer::Algorithm::kAesGcm,
          keyImpl));

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
  std::shared_ptr<webrtc::KeyManager> keyImpl(
      static_cast<DefaultKeyManagerImpl*>(key_manager.get()));
  RTCRtpReceiverImpl* impl = static_cast<RTCRtpReceiverImpl*>(receiver.get());
  auto mediaType =
      impl->rtp_receiver()->track()->kind() == "audio"
          ? webrtc::FrameCryptorTransformer::MediaType::kAudioFrame
          : webrtc::FrameCryptorTransformer::MediaType::kVideoFrame;
  e2ee_transformer_ = rtc::scoped_refptr<webrtc::FrameCryptorTransformer>(
      new webrtc::FrameCryptorTransformer(
          mediaType, webrtc::FrameCryptorTransformer::Algorithm::kAesGcm,
          keyImpl));

  impl->rtp_receiver()->SetDepacketizerToDecoderFrameTransformer(
      e2ee_transformer_);
  e2ee_transformer_->SetEnabled(false);
}

RTCFrameCryptorImpl::~RTCFrameCryptorImpl() {}

bool RTCFrameCryptorImpl::setEnabled(bool enabled) {
  enabled_ = enabled;
  e2ee_transformer_->SetEnabled(enabled_);
  return true;
}

bool RTCFrameCryptorImpl::enabled() const {
  return enabled_;
}

bool RTCFrameCryptorImpl::SetKeyIndex(int index) {
  key_index_ = index;
  e2ee_transformer_->SetKeyIndex(key_index_);
  return true;
}

int RTCFrameCryptorImpl::key_index() const {
  return key_index_;
}

scoped_refptr<KeyManager> KeyManager::Create() {
  return new RefCountedObject<DefaultKeyManagerImpl>();
}

DefaultKeyManagerImpl::DefaultKeyManagerImpl() {}

DefaultKeyManagerImpl::~DefaultKeyManagerImpl() {}

bool DefaultKeyManagerImpl::SetKey(int index, vector<uint8_t> key) {
  if (index > kMaxKeySize) {
    return false;
  }
  webrtc::MutexLock lock(&mutex_);
  if (index > (int)keys_.size()) {
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
