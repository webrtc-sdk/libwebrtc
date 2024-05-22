#ifndef LIB_WEBRTC_MEDIA_STREAM_IMPL_HXX
#define LIB_WEBRTC_MEDIA_STREAM_IMPL_HXX

#include "rtc_media_stream.h"
#include "rtc_peerconnection.h"

#include "api/media_stream_interface.h"
#include "api/peer_connection_interface.h"

namespace libwebrtc {

class WebRTCStatsCollectorCallback : public webrtc::RTCStatsCollectorCallback {
 public:
  WebRTCStatsCollectorCallback(OnStatsCollectorSuccess success,
                               OnStatsCollectorFailure failure)
      : success_(success), failure_(failure) {}
  ~WebRTCStatsCollectorCallback() {}

  static rtc::scoped_refptr<WebRTCStatsCollectorCallback> Create(
      OnStatsCollectorSuccess success, OnStatsCollectorFailure failure) {
    rtc::scoped_refptr<WebRTCStatsCollectorCallback> rtc_stats_observer =
        rtc::scoped_refptr<WebRTCStatsCollectorCallback>(
            new rtc::RefCountedObject<WebRTCStatsCollectorCallback>(success,
                                                                    failure));
    rtc_stats_observer->AddRef();
    return rtc_stats_observer;
  }

  virtual void OnStatsDelivered(
      const rtc::scoped_refptr<const webrtc::RTCStatsReport>& report) override;

 private:
  OnStatsCollectorSuccess success_;
  OnStatsCollectorFailure failure_;
};

class RTCStatsMemberImpl : public RTCStatsMember {
 public:
  RTCStatsMemberImpl(const webrtc::Attribute* member)
      : attr_(member) {}
  ~RTCStatsMemberImpl() {}

  string GetName() const override { return string(attr_->name()); }

  Type GetType() const override {
    if (attr_->holds_alternative<bool>()) {
      return Type::kBool;
    } else if (attr_->holds_alternative<int32_t>()) {
       return Type::kInt32;
    } else if (attr_->holds_alternative<uint32_t>()) {
       return Type::kUint32;
    } else if (attr_->holds_alternative<int64_t>()) {
        return Type::kInt64;
    } else if (attr_->holds_alternative<uint64_t>()) {
        return Type::kUint64;
    } else if (attr_->holds_alternative<double>()) {
        return Type::kDouble;
    } else if (attr_->holds_alternative<std::string>()) {
        return Type::kString;
    } else if (attr_->holds_alternative<std::vector<bool>>()) {
        return Type::kSequenceBool;
    } else if (attr_->holds_alternative<std::vector<int32_t>>()) {
        return Type::kSequenceInt32;
    } else if (attr_->holds_alternative<std::vector<uint32_t>>()) { 
        return Type::kSequenceUint32;
    } else if (attr_->holds_alternative<std::vector<int64_t>>()) {
        return Type::kSequenceInt64;
    } else if (attr_->holds_alternative<std::vector<uint64_t>>()) {
        return Type::kSequenceUint64;
    } else if (attr_->holds_alternative<std::vector<double>>()) {
        return Type::kSequenceDouble;
    } else if (attr_->holds_alternative<std::vector<std::string>>()) {
        return Type::kSequenceString;
    }
    
    return Type::kString;
  }

  bool IsDefined() const override { 
      return attr_->has_value();
  }

  bool ValueBool() const override {
    return attr_->get<bool>();
  }

  int32_t ValueInt32() const override {
    return attr_->get<int32_t>();
  }

  uint32_t ValueUint32() const override {
    return attr_->get<uint32_t>();
  }

  int64_t ValueInt64() const override {
    return attr_->get<int64_t>();
  }

  uint64_t ValueUint64() const override { 
    return attr_->get<uint64_t>();
  }

  double ValueDouble() const override { 
    return attr_->get<double>();
  }

  string ValueString() const override { 
      return attr_->get<std::string>();
  }

  vector<bool> ValueSequenceBool() const override {
    return attr_->get<std::vector<bool>>();
  }

  vector<int32_t> ValueSequenceInt32() const override {
    return attr_->get<std::vector<int32_t>>();
  }

  vector<uint32_t> ValueSequenceUint32() const override {
    return attr_->get<std::vector<uint32_t>>();
  }

  vector<int64_t> ValueSequenceInt64() const override {
    return attr_->get<std::vector<int64_t>>();
  }

  vector<uint64_t> ValueSequenceUint64() const override {
    return attr_->get<std::vector<uint64_t>>();
  }

  vector<double> ValueSequenceDouble() const override {
    return attr_->get<std::vector<double>>();
  }

  vector<string> ValueSequenceString() const override {
    return attr_->get<std::vector<std::string>>();
  }

  map<string, uint64_t> ValueMapStringUint64() const override {
    // const std::map<std::string, uint64_t>& mapout = *member_->cast_to<
    //    webrtc::RTCStatsMember<std::map<std::string, uint64_t>>>();
    return map<string, uint64_t>();
  }

  map<string, double> ValueMapStringDouble() const override {
    // const std::map<std::string, double>& mapout =
    //    *member_
    //         ->cast_to<webrtc::RTCStatsMember<std::map<std::string,
    //         double>>>();
    return map<string, double>();
  }

 private:
  const webrtc::Attribute* attr_;
};

class MediaRTCStatsImpl : public MediaRTCStats {
 public:
  MediaRTCStatsImpl(std::unique_ptr<webrtc::RTCStats> stats);

  virtual const string id() override;

  virtual const string type() override;

  virtual int64_t timestamp_us() override;

  virtual const string ToJson() override;

  virtual const vector<scoped_refptr<RTCStatsMember>> Members() override {
    members_.clear();
    for (const webrtc::Attribute attr : stats_->Attributes()) {
      members_.push_back(new RefCountedObject<RTCStatsMemberImpl>(&attr));
    }
    return members_;
  }

 private:
  std::unique_ptr<webrtc::RTCStats> stats_;
  std::vector<scoped_refptr<RTCStatsMember>> members_;
};

class MediaStreamImpl : public RTCMediaStream,
                        public webrtc::ObserverInterface {
 public:
  MediaStreamImpl(
      rtc::scoped_refptr<webrtc::MediaStreamInterface> rtc_media_stream);

  ~MediaStreamImpl();

  virtual bool AddTrack(scoped_refptr<RTCAudioTrack> track) override;

  virtual bool AddTrack(scoped_refptr<RTCVideoTrack> track) override;

  virtual bool RemoveTrack(scoped_refptr<RTCAudioTrack> track) override;

  virtual bool RemoveTrack(scoped_refptr<RTCVideoTrack> track) override;

  virtual vector<scoped_refptr<RTCAudioTrack>> audio_tracks() override;

  virtual vector<scoped_refptr<RTCVideoTrack>> video_tracks() override;

  virtual vector<scoped_refptr<RTCMediaTrack>> tracks() override;

  virtual scoped_refptr<RTCAudioTrack> FindAudioTrack(
      const string track_id) override;

  virtual scoped_refptr<RTCVideoTrack> FindVideoTrack(
      const string track_id) override;

  virtual const string label() override { return label_; }

  virtual const string id() override { return id_; }

  virtual void OnChanged() override;

 public:
  rtc::scoped_refptr<webrtc::MediaStreamInterface> rtc_media_stream() {
    return rtc_media_stream_;
  }

  void RegisterRTCPeerConnectionObserver(RTCPeerConnectionObserver* observer) {
    observer_ = observer;
  }

 private:
  rtc::scoped_refptr<webrtc::MediaStreamInterface> rtc_media_stream_;
  rtc::scoped_refptr<webrtc::PeerConnectionInterface> rtc_peerconnection_;
  std::vector<scoped_refptr<RTCAudioTrack>> audio_tracks_;
  std::vector<scoped_refptr<RTCVideoTrack>> video_tracks_;
  RTCPeerConnectionObserver* observer_ = nullptr;
  string label_, id_;
};

}  // namespace libwebrtc
#endif  //! LIB_WEBRTC_MEDIA_STREAM_IMPL_HXX