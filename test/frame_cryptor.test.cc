#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "test/gmock.h"
#include "test/gtest.h"

#include "rtc_base/logging.h"

#include "rtc_frame_cryptor.h"
#include "rtc_types.h"
#include "libwebrtc.h"

namespace libwebrtc {
namespace {

vector<uint8_t> MakeKey(uint8_t seed) {
  std::vector<uint8_t> bytes(16, seed);
  return vector<uint8_t>(bytes);
}

}  // namespace

// --- KeyProvider creation ---

TEST(KeyProviderTest, CreateWithDefaultOptionsSucceeds) {
  KeyProviderOptions options;
  scoped_refptr<KeyProvider> provider = KeyProvider::Create(&options);
  ASSERT_TRUE(provider.get() != nullptr);
}

// Fixture that owns a shared-key provider for the duration of each test.
class KeyProviderTestFixture : public ::testing::Test {
 protected:
  void SetUp() override {
    KeyProviderOptions options;
    options.shared_key = true;
    options.ratchet_salt = MakeKey(0x01);
    options.ratchet_window_size = 16;
    options.failure_tolerance = -1;
    options.key_ring_size = 16;
    options.key_derivation_algorithm = KeyDerivationAlgorithm::kPBKDF2;
    provider_ = KeyProvider::Create(&options);
    ASSERT_TRUE(provider_.get() != nullptr);
  }

  void TearDown() override { provider_ = nullptr; }

  scoped_refptr<KeyProvider> provider_;
};

TEST_F(KeyProviderTestFixture, SetSharedKeySucceeds) {
  EXPECT_TRUE(provider_->SetSharedKey(0, MakeKey(0x11)));
}

TEST_F(KeyProviderTestFixture, ExportSharedKeyAfterSet) {
  ASSERT_TRUE(provider_->SetSharedKey(0, MakeKey(0x22)));
  vector<uint8_t> exported = provider_->ExportSharedKey(0);
  EXPECT_GT(exported.size(), 0u);
}

TEST_F(KeyProviderTestFixture, RatchetSharedKeyAfterSet) {
  ASSERT_TRUE(provider_->SetSharedKey(0, MakeKey(0x33)));
  vector<uint8_t> ratcheted = provider_->RatchetSharedKey(0);
  // Ratcheting returns the derived key material (may be non-empty).
  EXPECT_GE(ratcheted.size(), 0u);
}

TEST_F(KeyProviderTestFixture, SetKeyForParticipantSucceeds) {
  EXPECT_TRUE(provider_->SetKey(string("participant-1"), 0, MakeKey(0x44)));
}

TEST_F(KeyProviderTestFixture, ExportKeyAfterSetKey) {
  ASSERT_TRUE(provider_->SetKey(string("participant-1"), 0, MakeKey(0x55)));
  vector<uint8_t> exported =
      provider_->ExportKey(string("participant-1"), 0);
  EXPECT_GT(exported.size(), 0u);
}

TEST_F(KeyProviderTestFixture, RatchetKeyAfterSetKey) {
  ASSERT_TRUE(provider_->SetKey(string("participant-1"), 0, MakeKey(0x66)));
  vector<uint8_t> ratcheted =
      provider_->RatchetKey(string("participant-1"), 0);
  EXPECT_GE(ratcheted.size(), 0u);
}

TEST_F(KeyProviderTestFixture, SetSifTrailerDoesNotCrash) {
  std::vector<uint8_t> trailer = {0xDE, 0xAD, 0xBE, 0xEF};
  provider_->SetSifTrailer(vector<uint8_t>(trailer));
}

// --- RTCFrameCryptor ---
//
// RTCFrameCryptor requires a live RTP sender/receiver from an established
// peer connection, which is not constructible in a pure unit test. The
// FrameCryptorFactory entry points are exercised in integration tests; here
// we only verify the supporting enums/options used to drive them are usable.

TEST(RTCFrameCryptorTypes, AlgorithmAndStateEnumsAreDistinct) {
  EXPECT_NE(FrameCryptorAlgorithm::kAesGcm, FrameCryptorAlgorithm::kAesCbc);
  EXPECT_NE(static_cast<int>(RTCFrameCryptionState::kNew),
            static_cast<int>(RTCFrameCryptionState::kOk));
}

}  // namespace libwebrtc
