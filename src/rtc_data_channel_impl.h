#ifndef LIB_WEBRTC_RTC_DATA_CHANNEL_IMPL_HXX
#define LIB_WEBRTC_RTC_DATA_CHANNEL_IMPL_HXX

#include "rtc_data_channel.h"
#include "rtc_types.h"
#include "api/data_channel_interface.h"
#include "rtc_base/synchronization/mutex.h"

namespace libwebrtc {

class RTCDataChannelImpl : public RTCDataChannel,
                           public webrtc::DataChannelObserver {
 public:
  RTCDataChannelImpl(
      rtc::scoped_refptr<webrtc::DataChannelInterface> rtc_data_channel);

  virtual void Send(const uint8_t* data, uint32_t size, bool binary = false) override;

  virtual void Close() override;

  virtual void RegisterObserver(RTCDataChannelObserver* observer) override;

  virtual void UnregisterObserver() override;

  virtual const string label() const override;

  virtual int id() const override;

  virtual RTCDataChannelState state() override;

  rtc::scoped_refptr<webrtc::DataChannelInterface> rtc_data_channel() {
    return rtc_data_channel_;
  }

 protected:
  virtual void OnStateChange() override;

  virtual void OnMessage(const webrtc::DataBuffer& buffer) override;

 private:
  rtc::scoped_refptr<webrtc::DataChannelInterface> rtc_data_channel_;
  RTCDataChannelObserver* observer_ = nullptr;
  std::unique_ptr<webrtc::Mutex> crit_sect_;
  RTCDataChannelState state_;
  string label_;
};

}  // namespace libwebrtc

#endif  // !LIB_WEBRTC_RTC_DATA_CHANNEL_IMPL_HXX
