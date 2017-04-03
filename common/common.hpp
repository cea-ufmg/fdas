#ifndef COMMON_HPP
#define COMMON_HPP

/**
 * Common infrastructure for the FDAS.
 */


#include <cstdint>

namespace fdas {
  template<typename DataType> class Datum {
  public:
    uint16_t id;
    DataType data;
    uint64_t timestamp;

    Datum(uint16_t id, DataType data, uint64_t timestamp)
      : id(id), data(data), timestamp(timestamp) {}
  };

  class DataSink {
  public:
    virtual void take(Datum<int8_t> datum) = 0;
    virtual void take(Datum<int16_t> datum) = 0;
    virtual void take(Datum<int32_t> datum) = 0;
    virtual void take(Datum<int64_t> datum) = 0;
    virtual void take(Datum<uint8_t> datum) = 0;
    virtual void take(Datum<uint16_t> datum) = 0;
    virtual void take(Datum<uint32_t> datum) = 0;
    virtual void take(Datum<uint64_t> datum) = 0;
    virtual void take(Datum<double> datum) = 0;
    virtual void take(Datum<float> datum) = 0;
  };
}

#endif//COMMON_HPP
