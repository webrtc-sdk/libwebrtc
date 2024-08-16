
from libwebrtc import *
import sys

ret  = LibWebRTC.Initialize()

print(ret)

ptr = LibWebRTC.CreateRTCPeerConnectionFactory()

pcFactory = factoryFromPtr(ptr)

ret = pcFactory.Initialize()

print(ret)

cfg = RTCConfiguration()
cfg.sdp_semantics = SdpSemantics_kUnifiedPlan
cfg.bundle_policy = kBundlePolicyBalanced
cfg.rtcp_mux_policy = RtcpMuxPolicy_kRtcpMuxPolicyNegotiate
cfg.ice_candidate_pool_size = 1
cfg.disable_ipv6 = False
cfg.disable_ipv6_on_wifi = False
cfg.disable_link_local_networks = False
cfg.max_ipv6_networks = 5
cfg.srtp_type = MediaSecurityType_kDTLS_SRTP
cfg.screencast_min_bitrate = 0
cfg.use_rtp_mux = True
cfg.tcp_candidate_policy = TcpCandidatePolicy_kTcpCandidatePolicyDisabled

constraintsPtr = RTCMediaConstraints.Create()

constraints = constraintsFromPtr(constraintsPtr)

constraints.AddMandatoryConstraint("OfferToReceiveAudio", "true")
constraints.AddMandatoryConstraint("OfferToReceiveVideo", "true")

#pc = pcFactory.Create(None, constraintsPtr)

for line in sys.stdin:
    if 'q' == line.rstrip():
        break
    print(f'Input : {line}')

constraintsPtr.disown()
ptr.disown()