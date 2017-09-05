# Copyright 2015-2017 RVJ Callanan.
# Released under the GNU General Public License (Version 3).

# build.mak: portable makefile for scdu project
# requires companion build.sh script

################################################################################

ifneq ($(SCDU_CONFIGURED),1)
  $(error this makefile must be invoked by build.sh)
endif

.DELETE_ON_ERROR:

################################################################################

.PHONY: check
check:
	@:; scdu_check

################################################################################

.PHONY: clean
clean:
	@:; scdu_clean

################################################################################

.PHONY: sanitise
sanitise:
	@:; scdu_sanitise

################################################################################

.PHONY: pks
pks: $(SCDU_PKS_RPATH)

$(SCDU_PKS_RPATH): $(SCDU_APP_BIN_RPATH)
	@:; scdu_pks

################################################################################

.PHONY: pkb
pkb: $(SCDU_PKB_RPATH)

$(SCDU_PKB_RPATH): $(SCDU_APP_BIN_RPATH)
	@:; scdu_pkb

################################################################################

.PHONY: pkd
pkd: $(SCDU_PKD_RPATH)

$(SCDU_PKD_RPATH): doc
	@:; scdu_pkd

################################################################################

.PHONY: app
app: $(SCDU_APP_BIN_RPATH)

$(SCDU_APP_BIN_RPATH): $(SCDU_APP_BIN_DEPS)
	@:; scdu_link

$(SCDU_APP_OBJ_RDIR)/%.o: $(SCDU_APP_SRC_RDIR)/%.cpp
	@:; scdu_compile $@ $<

-include $(SCDU_APP_OBJ_RDIR)/*.d

################################################################################

.PHONY: libs
libs: core ffs alg

################################################################################

.PHONY: core
core: $(SCDU_COR_LIB_RPATH)

$(SCDU_COR_LIB_RPATH): $(SCDU_COR_LIB_DEPS)
	@:; scdu_archive $@ $(SCDU_COR_OBJ_RDIR)

$(SCDU_COR_OBJ_RDIR)/%.o: $(SCDU_COR_SRC_RDIR)/%.cpp
	@:; scdu_compile $@ $<

-include $(SCDU_COR_OBJ_RDIR)/*.d

################################################################################

.PHONY: ffs
ffs: $(SCDU_FFS_LIB_RPATH)

$(SCDU_FFS_LIB_RPATH): $(SCDU_FFS_LIB_DEPS)
	@:; scdu_archive $@ $(SCDU_FFS_OBJ_RDIR)

$(SCDU_FFS_OBJ_RDIR)/%.o: $(SCDU_FFS_SRC_RDIR)/%.cpp
	@:; scdu_compile $@ $<

-include $(SCDU_FFS_OBJ_RDIR)/*.d

################################################################################

.PHONY: alg
alg: $(SCDU_ALG_LIB_RPATH)

$(SCDU_ALG_LIB_RPATH): $(SCDU_ALG_LIB_DEPS)
	@:; scdu_archive $@ $(SCDU_ALG_OBJ_RDIR)

$(SCDU_ALG_OBJ_RDIR)/%.o: $(SCDU_ALG_SRC_RDIR)/%.cpp
	@:; scdu_compile $@ $<

-include $(SCDU_ALG_OBJ_RDIR)/*.d

################################################################################

.PHONY: doc
doc: ugs dgs

################################################################################

.PHONY: ugs
ugs: ugh ugm ugt

################################################################################

.PHONY: ugh
ugh: $(SCDU_DOC_UGH_RPATH)
$(SCDU_DOC_UGH_RPATH): $(SCDU_DOC_UGH_DEPS)
	@:; scdu_ugh

################################################################################

.PHONY: ugm
ugm: $(SCDU_DOC_UGM_RPATH)
$(SCDU_DOC_UGM_RPATH): $(SCDU_DOC_UGM_DEPS)
	@:; scdu_ugm

################################################################################

.PHONY: ugt
ugt: $(SCDU_DOC_UGT_RPATH)
$(SCDU_DOC_UGT_RPATH): $(SCDU_DOC_UGT_DEPS)
	@:; scdu_ugt

################################################################################

.PHONY: dgs
dgs: dgh dgm dgt

################################################################################

.PHONY: dgh
dgh: $(SCDU_DOC_DGH_RPATH)
$(SCDU_DOC_DGH_RPATH): $(SCDU_DOC_DGH_DEPS)
	@:; scdu_dgh

################################################################################

.PHONY: dgm
dgm: $(SCDU_DOC_DGM_RPATH)
$(SCDU_DOC_DGM_RPATH): $(SCDU_DOC_DGM_DEPS)
	@:; scdu_dgm

################################################################################

.PHONY: dgt
dgt: $(SCDU_DOC_DGT_RPATH)
$(SCDU_DOC_DGT_RPATH): $(SCDU_DOC_DGT_DEPS)
	@:; scdu_dgt

################################################################################