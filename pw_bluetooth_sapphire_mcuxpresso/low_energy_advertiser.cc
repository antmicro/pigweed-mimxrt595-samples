#include "pw_bluetooth_sapphire_mcuxpresso/low_energy_advertiser.h"

#include "bluetooth/addr.h"
#include "bluetooth/bluetooth.h"

namespace bt::hci {

void LowEnergyAdvertiserMimxrt595::StartAdvertising(
    const DeviceAddress& address,
    const AdvertisingData& data,
    const AdvertisingData& scan_rsp,
    const AdvertisingOptions& options,
    ConnectionCallback connect_callback,
    ResultFunction<> result_callback) {
  bt_addr_t addr;
  int i = 0;
  for (auto it = address.value().bytes().cbegin();
       it != address.value().bytes().cend();
       ++it) {
    addr.val[i++] = *it;
  }
  bt_addr_le_t peer{.type = (uint8_t)address.type(), .a = addr};
  struct bt_le_adv_param param{.id = 0,
                               .sid = 0,
                               .secondary_max_skip = 100,
                               .options = options.flags,
                               .interval_min = options.interval.min(),
                               .interval_max = options.interval.max(),
                               .peer = &peer};
  // TODO: for now hardcode advertising data
  // but this should be taken from parameters instead
  uint8_t data2[] = {0x4};
  uint8_t data3[] = {0x25, 0x00, 0xBC, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                     0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                     0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc8};
  struct bt_data ad[] = {
      {
          .type = BT_DATA_FLAGS,
          .data_len = 1,
          .data = (uint8_t*)&data2,
      },
      {// TODO: convert string values from
       // https://www.bluetooth.com/specifications/assigned-numbers/
       // to defines from: gap.h
       // data.uris()
       .type = BT_DATA_MANUFACTURER_DATA,
       .data_len = sizeof(data3),
       .data = (uint8_t*)&data3}};
  char name[] = {'t', 'e', 's', 't'};
  struct bt_data sd{.type = BT_DATA_NAME_COMPLETE,
                    .data_len = sizeof(name),
                    .data = (uint8_t*)(&name)};
  // bt_le_adv_start(&param, &ad, 1, &sd, 1);
  struct bt_le_adv_param params{.id = BT_ID_DEFAULT,
                                .sid = 0,
                                .secondary_max_skip = 0,
                                .options = (BT_LE_ADV_OPT_USE_IDENTITY),
                                .interval_min = (BT_GAP_ADV_FAST_INT_MIN_2),
                                .interval_max = (BT_GAP_ADV_FAST_INT_MAX_2),
                                .peer = (NULL)};
  auto err = bt_le_adv_start(&params, &ad[0], 2, &sd, 1);
  if (err) {
    PW_LOG_CRITICAL("Advertising failed to start (err %d)", err);
  }

  // TODO: Handle connect callback, result callback
}

// Stops advertisement on all currently advertising addresses. Idempotent and
// asynchronous.
void LowEnergyAdvertiserMimxrt595::StopAdvertising() { bt_le_adv_stop(); }

// Stops any advertisement currently active on |address|. Idempotent and
// asynchronous.
void LowEnergyAdvertiserMimxrt595::StopAdvertising(const DeviceAddress& address,
                                                   bool extended_pdu) {
  PW_LOG_CRITICAL("UNIMPLEMENTED: StopAdvertising");
}

// Callback for an incoming LE connection. This function should be called in
// reaction to any connection that was not initiated locally. This object will
// determine if it was a result of an active advertisement and route the
// connection accordingly.
void LowEnergyAdvertiserMimxrt595::OnIncomingConnection(
    hci_spec::ConnectionHandle handle,
    pw::bluetooth::emboss::ConnectionRole role,
    const DeviceAddress& peer_address,
    const hci_spec::LEConnectionParameters& conn_params) {
  PW_LOG_CRITICAL("UNIMPLEMENTED: OnIncomingConnection");
}

// Returns the maximum number of advertisements that can be supported
size_t LowEnergyAdvertiserMimxrt595::MaxAdvertisements() const {
  PW_LOG_CRITICAL("UNIMPLEMENTED: MaxAdvertisements");
  return 100;
}

// Build the HCI command packet to enable advertising for the flavor of low
// energy advertising being implemented.
EmbossCommandPacket LowEnergyAdvertiserMimxrt595::BuildEnablePacket(
    const DeviceAddress& address,
    pw::bluetooth::emboss::GenericEnableParam enable,
    bool extended_pdu) {
  PW_LOG_CRITICAL("UNIMPLEMENTED: BuildEnablePacket");
  return EmbossCommandPacket::New<
      pw::bluetooth::emboss::DisconnectCommandWriter>(hci_spec::kDisconnect);
}

// Build the HCI command packet to set the advertising parameters for the
// flavor of low energy advertising being implemented.
std::optional<EmbossCommandPacket>
LowEnergyAdvertiserMimxrt595::BuildSetAdvertisingParams(
    const DeviceAddress& address,
    const AdvertisingEventProperties& properties,
    pw::bluetooth::emboss::LEOwnAddressType own_address_type,
    const AdvertisingIntervalRange& interval,
    bool extended_pdu) {
  PW_LOG_CRITICAL("UNIMPLEMENTED: BuildSetAdvertisingParams");
  return std::nullopt;
}

// Build the HCI command packet to set the advertising data for the flavor of
// low energy advertising being implemented.
std::vector<EmbossCommandPacket>
LowEnergyAdvertiserMimxrt595::BuildSetAdvertisingData(
    const DeviceAddress& address,
    const AdvertisingData& data,
    AdvFlags flags,
    bool extended_pdu) {
  PW_LOG_CRITICAL("UNIMPLEMENTED: BuildSetAdvertisingData");
  return std::vector<EmbossCommandPacket>();
}

// Build the HCI command packet to delete the advertising parameters from the
// controller for the flavor of low energy advertising being implemented. This
// method is used when stopping an advertisement.
EmbossCommandPacket LowEnergyAdvertiserMimxrt595::BuildUnsetAdvertisingData(
    const DeviceAddress& address, bool extended_pdu) {
  PW_LOG_CRITICAL("UNIMPLEMENTED: BuildUnsetAdvertisingData");
  return EmbossCommandPacket::New<
      pw::bluetooth::emboss::DisconnectCommandWriter>(hci_spec::kDisconnect);
}

// Build the HCI command packet to set the data sent in a scan response (if
// requested) for the flavor of low energy advertising being implemented.
std::vector<EmbossCommandPacket>
LowEnergyAdvertiserMimxrt595::BuildSetScanResponse(
    const DeviceAddress& address,
    const AdvertisingData& scan_rsp,
    bool extended_pdu) {
  PW_LOG_CRITICAL("UNIMPLEMENTED: BuildSetScanResponse");
  std::vector<EmbossCommandPacket> packets;
  packets.push_back(
      EmbossCommandPacket::New<pw::bluetooth::emboss::DisconnectCommandWriter>(
          hci_spec::kDisconnect));
  return packets;
}

// Build the HCI command packet to delete the advertising parameters from the
// controller for the flavor of low energy advertising being implemented.
EmbossCommandPacket LowEnergyAdvertiserMimxrt595::BuildUnsetScanResponse(
    const DeviceAddress& address, bool extended_pdu) {
  PW_LOG_CRITICAL("UNIMPLEMENTED: BuildUnsetScanResponse");
  return EmbossCommandPacket::New<
      pw::bluetooth::emboss::DisconnectCommandWriter>(hci_spec::kDisconnect);
}

// Build the HCI command packet to remove the advertising set entirely from
// the controller's memory for the flavor of low energy advertising being
// implemented.
EmbossCommandPacket LowEnergyAdvertiserMimxrt595::BuildRemoveAdvertisingSet(
    const DeviceAddress& address, bool extended_pdu) {
  PW_LOG_CRITICAL("UNIMPLEMENTED: BuildRemoveAdvertisingSet");
  return EmbossCommandPacket::New<
      pw::bluetooth::emboss::DisconnectCommandWriter>(hci_spec::kDisconnect);
}
bool LowEnergyAdvertiserMimxrt595::AllowsRandomAddressChange() const {
  PW_LOG_CRITICAL("UNIMPLEMENTED: AllowsRandomAddressChange");
  return false;
}
};  // namespace bt::hci
