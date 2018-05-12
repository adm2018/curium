package=native_ccache
<<<<<<< HEAD
$(package)_version=3.3.3
$(package)_download_path=https://samba.org/ftp/ccache
$(package)_file_name=ccache-$($(package)_version).tar.bz2
$(package)_sha256_hash=2985bc5e32ebe38d2958d508eb54ddcad39eed909489c0c2988035214597ca54
=======
$(package)_version=3.2.4
$(package)_download_path=https://samba.org/ftp/ccache
$(package)_file_name=ccache-$($(package)_version).tar.bz2
$(package)_sha256_hash=ffeb967edb549e67da0bd5f44f729a2022de9fdde65dfd80d2a7204d7f75332e
>>>>>>> dev-1.12.1.0

define $(package)_set_vars
$(package)_config_opts=
endef

define $(package)_config_cmds
  $($(package)_autoconf)
endef

define $(package)_build_cmds
  $(MAKE)
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) install
endef

define $(package)_postprocess_cmds
  rm -rf lib include
endef
