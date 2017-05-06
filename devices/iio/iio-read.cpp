/**
 * FDAS device module for reading iio devices.
 */

#include <boost/log/trivial.hpp>
#include <iio.h>

#include "common/common.hpp"

namespace po = boost::program_options;

using namespace fdas;
using std::string;



void ReadBuffer(struct iio_device *dev) {
  
}

int main (int argc, char *argv[]) {
  string device_name;
  po::options_description iio_options_desc("IIO options");
  iio_options_desc.add_options()
      ("device,d", po::value<string>(&device_name), "iio device to read from");
  
  po::options_description desc("Read from iio device.");
  desc.add(GeneralOptions()).add(DataSinkOptions()).add(iio_options_desc);
  
  po::variables_map vm;

  // Build libiio context
  struct iio_context *ctx = iio_create_local_context();
  if (!ctx) {
    BOOST_LOG_TRIVIAL(error) << "Could not create libiio context";
    return EXIT_FAILURE;
  }

  // Get the iio device
  struct iio_device *dev = iio_context_find_device(ctx, device_name.c_str());
  if (!dev) {
    BOOST_LOG_TRIVIAL(error) << "Could not find iio device `" << device_name
                             << '`';
    return EXIT_FAILURE;
  }

  // Read from the device
  ReadBuffer(dev);

  // Cleanup and exit
  iio_context_destroy(ctx);
  return EXIT_SUCCESS;
}
