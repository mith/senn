#include "utils.hpp"
#include <boost/iostreams/device/mapped_file.hpp>

std::string file_to_str(std::string filename)
{
    using namespace boost::iostreams;
    mapped_file_source txt_file(filename);
    std::string src(txt_file.data(), txt_file.size());
    txt_file.close();
    return src;
}
