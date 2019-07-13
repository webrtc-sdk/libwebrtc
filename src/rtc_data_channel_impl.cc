#include "rtc_data_channel_impl.h"

namespace libwebrtc {

RTCDataChannelImpl::RTCDataChannelImpl(
    rtc::scoped_refptr<webrtc::DataChannelInterface> rtc_data_channel)
    : rtc_data_channel_(rtc_data_channel),
      crit_sect_(new rtc::CriticalSection()) {
  snprintf(label_, kMaxStringLength, rtc_data_channel_->label().data());
}

void RTCDataChannelImpl::Send(const char* data,
	int length,
                              bool binary /*= false*/) {
  if (binary) {
    rtc::CopyOnWriteBuffer binary(data);
    webrtc::DataBuffer buffer(binary, true);
    rtc_data_channel_->Send(buffer);
  } else {
    webrtc::DataBuffer buffer(data);
    rtc_data_channel_->Send(buffer);
  }
}

void RTCDataChannelImpl::Close() {
  rtc_data_channel_->Close();
}

void RTCDataChannelImpl::RegisterObserver(RTCDataChannelObserver* observer) {
  rtc::CritScope(crit_sect_.get());
  observer_ = observer;
}

void RTCDataChannelImpl::UnregisterObserver() {
  rtc::CritScope(crit_sect_.get());
  observer_ = nullptr;
}

const char* RTCDataChannelImpl::label() const {
  return label_;
}

int RTCDataChannelImpl::id() const {
  return rtc_data_channel_->id();
}

void RTCDataChannelImpl::OnStateChange() {
  webrtc::DataChannelInterface::DataState state = rtc_data_channel_->state();
  switch (state) {
    case webrtc::DataChannelInterface::kConnecting:
      state_ = RTCDataChannelConnecting;
      break;
    case webrtc::DataChannelInterface::kOpen:
      state_ = RTCDataChannelOpen;
      break;
    case webrtc::DataChannelInterface::kClosing:
      state_ = RTCDataChannelClosing;
      break;
    case webrtc::DataChannelInterface::kClosed:
      state_ = RTCDataChannelClosed;
      break;
    default:
      break;
  }
  rtc::CritScope(crit_sect_.get());
  if (observer_)
    observer_->OnStateChange(state_);
}

RTCDataChannelState RTCDataChannelImpl::state() {
  return state_;
}

void RTCDataChannelImpl::OnMessage(const webrtc::DataBuffer& buffer) {
  if (observer_)
    observer_->OnMessage(buffer.data.data<char>(), buffer.data.size(), buffer.binary);
 }

} // namespace libwebrtc
