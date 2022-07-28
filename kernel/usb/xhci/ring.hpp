// Event Ring, Command Ring, Transfer Ring のクラスや関連機能

#pragma once

#include <cstdint>
#include <vector>

#include "error.hpp"
#include "usb/memory.hpp"
#include "usb/xhci/registers.hpp"
#include "usb/xhci/trb.hpp"

namespace usb::xhci {
// Command/Transfer Ring を表すクラス
class Ring {
 public:
  Ring() = default;
  Ring(const Ring &) = delete;
  ~Ring();
  Ring &operator=(const Ring &) = delete;

  Error Initialize(size_t buf_size);

  template <typename TRBType>
  TRB *Push(const TRBType &trb) {
    return Push(trb.data);
  }

  TRB *Buffer() const { return buf_; }

 private:
  TRB *buf_ = nullptr;
  size_t buf_size_ = 0;

  bool cycle_bit_;
  size_t write_index_;

  // TRB に cycle bit を設定した上でリング末尾に書き込む
  // write_index_ は変化させない
  void CopyToLast(const std::array<uint32_t, 4> &data);

  // TRB に cycle bit を設定した上でリング末尾に書き込む
  // write_index_ をインクリメントする。その結果 write_index_ が末尾に達したら
  // LinkTRB を適切に配置して write_index_ を 0 に戻し cycle bit を反転させる
  TRB *Push(const std::array<uint32_t, 4> &data);
};

union EventRingSegmentTableEntry {
  std::array<uint32_t, 4> data;
  struct {
    uint64_t ring_segment_base_address;

    uint32_t ring_segment_size : 16;
    uint32_t : 16;

    uint32_t : 32;
  } __attribute__((packed)) bits;
};

class EventRing {
 public:
  Error Initialize(size_t buf_size, InterrupterRegisterSet *interrupter);

  TRB *ReadDequeuePointer() const {
    return reinterpret_cast<TRB *>(interrupter_->ERDP.Read().Pointer());
  }

  void WriteDequeuePointer(TRB *p);

  bool HasFront() const { return Front()->bits.cycle_bit == cycle_bit_; }

  TRB *Front() const { return ReadDequeuePointer(); }

  void Pop();

 private:
  TRB *buf_;
  size_t buf_size_;

  bool cycle_bit_;
  EventRingSegmentTableEntry *erst_;
  InterrupterRegisterSet *interrupter_;
};
}  // namespace usb::xhci
