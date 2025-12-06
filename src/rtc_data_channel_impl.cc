#include "rtc_data_channel_impl.h"
#include "interop_api.h"

namespace libwebrtc {

/**
 * class RTCDataChannelObserverImpl 
 */

RTCDataChannelObserverImpl::RTCDataChannelObserverImpl(void* callbacks /* rtcDataChannelObserverCallbacks* */)
  : callbacks_(nullptr)
{
  if (callbacks) {
    size_t nSize = sizeof(rtcDataChannelObserverCallbacks);
    callbacks_ = malloc(nSize);
    memcpy(callbacks_, (const void*)callbacks, nSize);
  }
}

RTCDataChannelObserverImpl::~RTCDataChannelObserverImpl()
{
  if (callbacks_) {
    free(callbacks_);
  }
  callbacks_ = nullptr;
}

void RTCDataChannelObserverImpl::OnStateChange(RTCDataChannelState state)
{
  if (callbacks_) {
    rtcDataChannelObserverCallbacks* pCallbacks = reinterpret_cast<rtcDataChannelObserverCallbacks*>(callbacks_);
    pCallbacks->StateChanged(
      pCallbacks->UserData,
      static_cast<rtcDataChannelState>(state));
  }
}

void RTCDataChannelObserverImpl::OnMessage(const char* buffer, int length, bool binary)
{
  if (callbacks_) {
    rtcDataChannelObserverCallbacks* pCallbacks = reinterpret_cast<rtcDataChannelObserverCallbacks*>(callbacks_);
    pCallbacks->MessageReceived(
      pCallbacks->UserData,
      buffer,
      length,
      binary ? rtcBool32::kTrue : rtcBool32::kFalse);
  }
}

/**
 * class RTCDataChannelImpl 
 */

RTCDataChannelImpl::RTCDataChannelImpl(
    webrtc::scoped_refptr<webrtc::DataChannelInterface> rtc_data_channel)
    : rtc_data_channel_(rtc_data_channel), crit_sect_(new webrtc::Mutex()) {
  rtc_data_channel_->RegisterObserver(this);
  label_ = rtc_data_channel_->label();
}

RTCDataChannelImpl::~RTCDataChannelImpl() {
  rtc_data_channel_->UnregisterObserver();
}

void RTCDataChannelImpl::Send(const uint8_t* data, uint32_t size,
                              bool binary /*= false*/) {
  webrtc::CopyOnWriteBuffer copyOnWriteBuffer(data, size);
  webrtc::DataBuffer buffer(copyOnWriteBuffer, binary);
  rtc_data_channel_->Send(buffer);
}

void RTCDataChannelImpl::Close() {
  rtc_data_channel_->UnregisterObserver();
  rtc_data_channel_->Close();
}

void RTCDataChannelImpl::RegisterObserver(RTCDataChannelObserver* observer) {
  webrtc::MutexLock(crit_sect_.get());
  observer_ = observer;
}

void RTCDataChannelImpl::UnregisterObserver() {
  webrtc::MutexLock(crit_sect_.get());
  observer_ = nullptr;
}

const string RTCDataChannelImpl::label() const { return label_; }

int RTCDataChannelImpl::id() const { return rtc_data_channel_->id(); }

uint64_t RTCDataChannelImpl::buffered_amount() const { return rtc_data_channel_->buffered_amount(); }

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
  webrtc::MutexLock(crit_sect_.get());
  if (observer_) observer_->OnStateChange(state_);
}

RTCDataChannelState RTCDataChannelImpl::state() { return state_; }

void RTCDataChannelImpl::OnMessage(const webrtc::DataBuffer& buffer) {
  if (observer_)
    observer_->OnMessage(buffer.data.data<char>(), buffer.data.size(),
                         buffer.binary);
}

}  // namespace libwebrtc
