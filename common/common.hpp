#ifndef FDAS_COMMON_COMMON_HPP_
#define FDAS_COMMON_COMMON_HPP_

/**
 * Common infrastructure for the FDAS.
 */


#include <cstdint>
#include <fstream>
#include <list>
#include <memory>

#include <boost/program_options.hpp>


namespace fdas {
  
/** Data identifier. */
class DataId {
 protected:
  const char *strid; /**< Unique data identifier string. */
  const char *description = "";
  const char *units = "";
  
 public:
  const char* Description() const {return description;}
  const char* Units() const {return units;}
  const char* StrId() const {return strid;}
  
  DataId(const char *strid, const char *description, const char *units)
      : strid(strid), description(description), units(units) {}
  DataId(const char *strid)
      : strid(strid) {}
};

/** Data point. */
template<typename DataType> class Datum {
 public:
  const DataId * const id;
  DataType data;
  uint64_t timestamp;
  
  Datum(const DataId *id, DataType data, uint64_t timestamp)
      : id(id), data(data), timestamp(timestamp) {}
};


class DataSink {
 public:
  virtual void Take(Datum<int8_t> datum) = 0;
  virtual void Take(Datum<int16_t> datum) = 0;
  virtual void Take(Datum<int32_t> datum) = 0;
  virtual void Take(Datum<int64_t> datum) = 0;
  virtual void Take(Datum<uint8_t> datum) = 0;
  virtual void Take(Datum<uint16_t> datum) = 0;
  virtual void Take(Datum<uint32_t> datum) = 0;
  virtual void Take(Datum<uint64_t> datum) = 0;
  virtual void Take(Datum<double> datum) = 0;
  virtual void Take(Datum<float> datum) = 0;
};

class TextFileDataSink : public DataSink {
  std::ofstream ostream;
  
 public:
  virtual void Take(Datum<int8_t> datum);
  virtual void Take(Datum<int16_t> datum);
  virtual void Take(Datum<int32_t> datum);
  virtual void Take(Datum<int64_t> datum);
  virtual void Take(Datum<uint8_t> datum);
  virtual void Take(Datum<uint16_t> datum);
  virtual void Take(Datum<uint32_t> datum);
  virtual void Take(Datum<uint64_t> datum);
  virtual void Take(Datum<double> datum);
  virtual void Take(Datum<float> datum);  
  
  explicit TextFileDataSink(const char *filename) : ostream(filename) {}
  explicit TextFileDataSink(const std::string &filename) : ostream(filename) {}
};

typedef std::shared_ptr<DataSink> DataSinkPtr;
typedef std::list<DataSinkPtr> DataSinkPtrList;

//** General program options like help and logging. */
boost::program_options::options_description GeneralOptions();

//** Data sinking program options. */
boost::program_options::options_description DataSinkOptions();

//** Create the list of data sinks specified in program options. */
DataSinkPtrList BuildDataSinks(const boost::program_options::variables_map &vm);

}// namespace fdas

#endif//FDAS_COMMON_COMMON_HPP_
