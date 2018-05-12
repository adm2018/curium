package=zeromq
<<<<<<< HEAD
$(package)_version=4.1.5
$(package)_download_path=https://github.com/zeromq/zeromq4-1/releases/download/v$($(package)_version)/
$(package)_file_name=$(package)-$($(package)_version).tar.gz
$(package)_sha256_hash=04aac57f081ffa3a2ee5ed04887be9e205df3a7ddade0027460b8042432bdbcf
$(package)_patches=9114d3957725acd34aa8b8d011585812f3369411.patch 9e6745c12e0b100cd38acecc16ce7db02905e27c.patch

define $(package)_set_vars
  $(package)_config_opts=--without-documentation --disable-shared --without-libsodium --disable-curve
  $(package)_config_opts_linux=--with-pic
  $(package)_cxxflags=-std=c++11
endef

define $(package)_preprocess_cmds
  patch -p1 < $($(package)_patch_dir)/9114d3957725acd34aa8b8d011585812f3369411.patch && \
  patch -p1 < $($(package)_patch_dir)/9e6745c12e0b100cd38acecc16ce7db02905e27c.patch && \
  ./autogen.sh
=======
$(package)_version=4.0.7
$(package)_download_path=http://download.zeromq.org
$(package)_file_name=$(package)-$($(package)_version).tar.gz
$(package)_sha256_hash=e00b2967e074990d0538361cc79084a0a92892df2c6e7585da34e4c61ee47b03

define $(package)_set_vars
  $(package)_config_opts=--without-documentation --disable-shared
  $(package)_config_opts_linux=--with-pic
>>>>>>> dev-1.12.1.0
endef

define $(package)_config_cmds
  $($(package)_autoconf)
endef

define $(package)_build_cmds
<<<<<<< HEAD
  $(MAKE) libzmq.la
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) install-libLTLIBRARIES install-includeHEADERS install-pkgconfigDATA
=======
  $(MAKE) -C src
endef

define $(package)_stage_cmds
  $(MAKE) -C src DESTDIR=$($(package)_staging_dir) install
>>>>>>> dev-1.12.1.0
endef

define $(package)_postprocess_cmds
  rm -rf bin share
endef
