// PCI パス制御のプログラム

#pragma once

#include <array>
#include <cstdint>

#include "error.hpp"

namespace pci {
// CONFIG_ADDRESS レジスタの IO ポートアドレス
const uint16_t kConfigAddress = 0x0cf8;
// CONFIG_DATA レジスタ の IO ポートアドレス
const uint16_t kConfigData = 0x0cfc;

// PCI デバイスのクラスコード
struct ClassCode {
  uint8_t base, sub, interface;

  bool Match(uint8_t b) { return b == base; }
  bool Match(uint8_t b, uint8_t s) { return Match(b) && s == sub; }
  bool Match(uint8_t b, uint8_t s, uint8_t i) {
    return Match(b, s) && i == interface;
  }
};

// PCIデバイスを操作するためのデータを格納する
struct Device {
  uint8_t bus, device, function, header_type;
  ClassCode class_code;
};

// CONFIG_ADDRESS に指定された整数を書き込む
void WriteAddress(uint32_t address);
// CONFIG_DATA に指定された整数を書き込む
void WriteData(uint32_t value);
// CONFIG_DATA から 32 ビット整数を読み込む
uint32_t ReadData();

// ベンダ ID レジスタを読み取る(全へっだタイプ共通)
uint16_t ReadVendorId(uint8_t bus, uint8_t device, uint8_t function);
// デバイス ID レジスタを読み取る(全へっだタイプ共通)
uint16_t ReadDeviceId(uint8_t bus, uint8_t device, uint8_t function);
// ヘッダタイプレジスタを読み取る(全へっだタイプ共通)
uint8_t ReadHeaderType(uint8_t bus, uint8_t device, uint8_t function);
// クラスコードレジスタを読み取る
ClassCode ReadClassCode(uint8_t bus, uint8_t device, uint8_t function);

inline uint16_t ReadVendorId(const Device& dev) {
  return ReadVendorId(dev.bus, dev.device, dev.function);
}

// 指定された PCI デバイスの 32 ビットレジスタを読み取る
uint32_t ReadConfReg(const Device& dev, uint8_t reg_addr);

void WriteConfReg(const Device& dev, uint8_t reg_addr, uint32_t value);

// バス番号レジスタを読み取る
// - 23:16 : サブオーディネイトバス番号
// - 15:8  : セカンダリバス番号
// - 7:0   : リビジョン番号
uint32_t ReadBusNumbers(uint8_t bus, uint8_t device, uint8_t function);

// 単一ファンクションの場合に真を返す
bool IsSingleFunctionDevice(uint8_t header_type);

// ScanAllBus()により発見された PCI デバイス一覧
inline std::array<Device, 32> devices;
// devices　の有効な要素の数
inline int num_device;
// PCI デバイスを全て探索し devices　に格納する
Error ScanAllBus();

constexpr uint8_t CalcBarAddress(unsigned int bar_index) {
  return 0x10 + 4 * bar_index;
}

WithError<uint64_t> ReadBar(Device& device, unsigned int bar_index);
}  // namespace pci