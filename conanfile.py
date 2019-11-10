import os
from conans import ConanFile, tools, CMake
from conans.errors import ConanInvalidConfiguration


class KFRConan(ConanFile):
    name = "kfr"
    version = "3.0.9"
    description = "Fast, modern C++ DSP framework, FFT, Sample Rate Conversion, FIR/IIR/Biquad Filters"
    topics = ("conan", "kfr", "avx", "fft", "audio", "dsp")
    url = "https://github.com/bincrafters/conan-kfr"
    homepage = "https://github.com/kfrlib/kfr"
    license = "GPL-2.0"
    settings = "build_type", "os", "compiler", "arch"
    options = {"dft": [True, False], "header_only": [True, False], "shared": [True, False], "fPIC": [True, False]}
    default_options = {"dft": False, "header_only": True, "shared": False, "fPIC": True}
    exports = ["LICENSE.md"]
    exports_sources = ["CMakeLists.txt", "0001-lib.patch"]
    generators = "cmake"

    @property
    def _source_subfolder(self):
        return "source_subfolder"

    @property
    def _build_subfolder(self):
        return "build_subfolder"

    def configure(self):
        if self.options.dft and self.options.header_only:
            raise ConanInvalidConfiguration("DFT can not be built with header-only")
        if self.options.dft and (self.settings.compiler != "clang" and self.settings.compiler != "apple-clang"):
            raise ConanInvalidConfiguration("DFT option is only compatible with Clang")

        if self.options.header_only:
            self.settings.clear()
            del self.options.shared
            del self.options.fPIC
            del self.options.dft
        elif not self.options.header_only and self.settings.os == "Windows":
            del self.options.fPIC

    def source(self):
        sha256 = "71ff6b62db268d76acd5ebf720b3f4d0786de59fea02fcabb30bb7121ce82d15"
        tools.get("{0}/archive/{1}.tar.gz".format(self.homepage, self.version), sha256=sha256)
        extracted_dir = self.name + "-" + self.version
        os.rename(extracted_dir, self._source_subfolder)

    def _configure_cmake(self):
        cmake = CMake(self)
        if not self.options.header_only:
            cmake.definitions["ENABLE_DFT"] = self.options.dft
        cmake.configure(build_folder=self._build_subfolder)
        return cmake

    def build(self):
        if not self.options.header_only:
            tools.patch(base_path=self._source_subfolder, patch_file="0001-lib.patch")
            cmake = self._configure_cmake()
            cmake.build()

    def package(self):
        self.copy(pattern="LICENSE.txt", dst="licenses", src=self._source_subfolder)
        cmake = self._configure_cmake()
        cmake.install()
        os.remove(os.path.join(self.package_folder, "include", "kfr", "io", "dr", "README.txt"))
        if self.options.header_only:
            tools.rmdir(os.path.join(self.package_folder, "lib"))

    def package_id(self):
        if self.options.header_only:
            self.info.header_only()

    def package_info(self):
        if not self.options.header_only:
            self.cpp_info.libs = tools.collect_libs(self)
            if self.options.dft:
                self.cpp_info.defines.append("KFR_ENABLE_FLAC=1")
            else:
                self.cpp_info.defines.append("KFR_NO_DFT")
        else:
            self.cpp_info.defines.append("KFR_NO_DFT")

        if tools.os_info.is_linux:
            self.cpp_info.libs.extend(["pthread", "m"])


