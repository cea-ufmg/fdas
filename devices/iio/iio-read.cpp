/**
 * FDAS device module for reading iio devices.
 */

#include <common/common.hpp>

namespace po = boost::program_options;

using namespace fdas;
using std::string;

int main (int argc, char *argv[]) {
  po::options_description iio_options_desc("IIO options");
  iio_options_desc.add_options()
      ("device,d", po::value<string>(), "iio device to read from");
  
  po::options_description desc("Read from iio device.");
  desc.add(GeneralOptions()).add(DataSinkOptions()).add(iio_options_desc);
  
  po::variables_map vm;
  
  return 0;
}
