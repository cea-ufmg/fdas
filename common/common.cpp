/**
 * Common infrastructure for the FDAS.
 */

#include "common.hpp"
#include <string>
#include <vector>

namespace po = boost::program_options;
using std::vector;
using std::string;

namespace fdas {

void TextFileDataSink::Take(Datum<int8_t> datum) {
  this->ostream << datum.id->NumId() << '\t';
  this->ostream << datum.data << '\t';
  this->ostream << datum.timestamp << std::endl;
}

void TextFileDataSink::Take(Datum<int16_t> datum) {
  this->ostream << datum.id->NumId() << '\t';
  this->ostream << datum.data << '\t';
  this->ostream << datum.timestamp << std::endl;
}

void TextFileDataSink::Take(Datum<int32_t> datum) {
  this->ostream << datum.id->NumId() << '\t';
  this->ostream << datum.data << '\t';
  this->ostream << datum.timestamp << std::endl;
}

void TextFileDataSink::Take(Datum<int64_t> datum) {
  this->ostream << datum.id->NumId() << '\t';
  this->ostream << datum.data << '\t';
  this->ostream << datum.timestamp << std::endl;
}

void TextFileDataSink::Take(Datum<uint8_t> datum) {
  this->ostream << datum.id->NumId() << '\t';
  this->ostream << datum.data << '\t';
  this->ostream << datum.timestamp << std::endl;
}

void TextFileDataSink::Take(Datum<uint16_t> datum) {
  this->ostream << datum.id->NumId() << '\t';
  this->ostream << datum.data << '\t';
  this->ostream << datum.timestamp << std::endl;
}

void TextFileDataSink::Take(Datum<uint32_t> datum) {
  this->ostream << datum.id->NumId() << '\t';
  this->ostream << datum.data << '\t';
  this->ostream << datum.timestamp << std::endl;
}

void TextFileDataSink::Take(Datum<uint64_t> datum) {
  this->ostream << datum.id->NumId() << '\t';
  this->ostream << datum.data << '\t';
  this->ostream << datum.timestamp << std::endl;
}

void TextFileDataSink::Take(Datum<double> datum) {
  this->ostream << datum.id->NumId() << '\t';
  this->ostream << datum.data << '\t';
  this->ostream << datum.timestamp << std::endl;
}

void TextFileDataSink::Take(Datum<float> datum) {
  this->ostream << datum.id->NumId() << '\t';
  this->ostream << datum.data << '\t';
  this->ostream << datum.timestamp << std::endl;
}

po::options_description DataSinkOptions() {
  po::options_description desc("Common FDAS data sinking options");
  desc.add_options()
      ("log-data-text-file", po::value< vector<string> >(), 
       "Log data into text file");

  return desc;
}

po::options_description GeneralOptions() {
  po::options_description desc("General program options, help and logging");
  desc.add_options()
      ("help,h", "Print help and argument usage");
  
  return desc;
}

}
