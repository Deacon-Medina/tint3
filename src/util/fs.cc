#include <pwd.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <fstream>
#include <sstream>
#include <utility>

#include "util/common.hh"
#include "util/environment.hh"
#include "util/fs.hh"

namespace util {
namespace fs {

DirectoryContents::DirectoryContents(const std::string& path)
    : dir_(opendir(path.c_str())) {}

DirectoryContents::~DirectoryContents() {
  if (dir_ != nullptr) {
    closedir(dir_);
  }
}

DirectoryContents::iterator const DirectoryContents::begin() const {
  return DirectoryContents::iterator(dir_);
}

DirectoryContents::iterator const DirectoryContents::end() const {
  return DirectoryContents::iterator();
}

DirectoryContents::iterator::iterator()
    : dir_(nullptr), entry_(nullptr), pos_(-1) {}

DirectoryContents::iterator::iterator(DIR* dir)
    : dir_(dir), entry_(nullptr), pos_(-1) {
  if (dir_ != nullptr) {
    pos_ = telldir(dir_);
  }
}

DirectoryContents::iterator& DirectoryContents::iterator::operator++() {
  if (dir_ != nullptr) {
    entry_ = readdir(dir_);

    if (entry_ != nullptr) {
      pos_ = telldir(dir_);
    } else {
      dir_ = nullptr;
      pos_ = -1;
    }
  }

  return (*this);
}

const std::string DirectoryContents::iterator::operator*() const {
  if (entry_ != nullptr) {
    return entry_->d_name;
  }

  return std::string();
}

bool DirectoryContents::iterator::operator!=(
    DirectoryContents::iterator const& other) const {
  return (dir_ != other.dir_ || pos_ != other.pos_);
}

namespace {

std::string StripLeadingSlash(std::string path) {
  // Remove the leading '/' if this is not a path to the filesystem root.
  if (path.length() > 1 && path[0] == '/') {
    return path.substr(1);
  }

  return path;
}

std::string StripTrailingSlash(std::string path) {
  // Remove the trailing '/' if this is not a path to the filesystem root.
  if (path.length() > 1 && path[path.length() - 1] == '/') {
    return path.substr(0, path.length() - 1);
  }

  return path;
}

}  // namespace

Path::Path(Path const& other) : path_(other.path_) {}

Path::Path(Path&& other) : path_(std::move(other.path_)) {}

Path::Path(std::string const& path) : path_(StripTrailingSlash(path)) {}

Path::Path(const char* path) : path_(StripTrailingSlash(path)) {}

Path& Path::operator=(Path other) {
  std::swap(path_, other.path_);
  return (*this);
}

Path Path::operator/(std::string const& component) {
  return Path{*this} /= component;
}

Path& Path::operator/=(std::string const& component) {
  if (!path_.empty() && path_ != "/") {
    path_.append("/");
  }
  path_.append(StripLeadingSlash(component));
  return (*this);
}

Path::operator std::string() const { return path_; }

std::ostream& operator<<(std::ostream& os, Path const& path) {
  return os << path.path_;
}

std::string BuildPath(std::initializer_list<std::string> parts) {
  Path path;
  for (auto const& p : parts) {
    path /= p;
  }
  return path;
}

bool CopyFile(std::string const& from_path, std::string const& to_path) {
  std::ifstream from(from_path);

  if (!from) {
    return false;
  }

  std::ofstream to(to_path);

  if (!to) {
    return false;
  }

  std::istreambuf_iterator<char> begin_from(from);
  std::istreambuf_iterator<char> end_from;
  std::ostreambuf_iterator<char> begin_to(to);

  std::copy(begin_from, end_from, begin_to);
  return true;
}

bool CreateDirectory(std::string const& path, mode_t mode) {
  auto components = util::string::Split(path, '/');
  Path partial_path{"/"};

  for (std::string const& component : components) {
    if (component.empty()) {
      continue;
    }
    partial_path /= component;
    if (DirectoryExists(partial_path)) {
      continue;
    }
    std::string missing_directory{partial_path};
    if (mkdir(missing_directory.c_str(), mode) != 0) {
      return false;
    }
  }
  return true;
}

bool DirectoryExists(std::string const& path) {
  struct stat info;

  if (stat(path.c_str(), &info) != 0) {
    return false;
  }

  return S_ISDIR(info.st_mode);
}

bool FileExists(std::string const& path) {
  struct stat info;

  if (stat(path.c_str(), &info) != 0) {
    return false;
  }

  return S_ISREG(info.st_mode);
}

bool FileExists(std::initializer_list<std::string> parts) {
  return FileExists(BuildPath(parts));
}

Path HomeDirectory() {
  std::string home = environment::Get("HOME");

  if (!home.empty()) {
    return home;
  }

  struct passwd* pwd = getpwuid(getuid());

  if (pwd != nullptr) {
    return pwd->pw_dir;
  }

  // if we got here both getenv() and getpwuid() failed, so let's just return
  // an empty string to indicate failure
  return std::string();
}

bool IsAbsolutePath(std::string const& path) {
  char* real_path = realpath(path.c_str(), nullptr);

  if (real_path == nullptr) {
    return false;
  }

  bool is_absolute = (std::strcmp(path.c_str(), real_path) == 0);
  std::free(real_path);
  return is_absolute;
}

bool ReadFile(std::string const& path,
              std::function<bool(std::string const&)> const& fn) {
  static std::streamsize kMaxBytesToRead = (1 << 20);

  std::ifstream is(path);
  std::string contents;

  if (!is.good()) {
    return false;
  }

  is.seekg(0, is.end);
  std::streamsize num_bytes_to_read = is.tellg();
  is.seekg(0, is.beg);

  while (!is.eof() && num_bytes_to_read != 0) {
    std::streamsize buf_size = std::min(num_bytes_to_read, kMaxBytesToRead);
    char buf[buf_size + 1];

    if (is.read(buf, buf_size).bad()) {
      return false;
    }

    std::streamsize num_bytes_read = is.gcount();
    num_bytes_to_read -= num_bytes_read;
    buf[num_bytes_read] = '\0';
    contents.append(buf);
  }

  return fn(contents);
}

bool ReadFileByLine(std::string const& path,
                    std::function<bool(std::string const&)> const& fn) {
  std::ifstream is(path);

  if (!is.good()) {
    return false;
  }

  while (!is.eof()) {
    std::string line;
    std::getline(is, line);
    if (!fn(line)) {
      return false;
    }
  }

  return true;
}

}  // namespace fs
}  // namespace util
