# platform: portable64 ref avx2 avx512
platform := portable64
subdirs :=\
  qruov1q7L10v740m100  qruov1q31L3v165m60  qruov1q31L10v600m70   qruov1q127L3v156m54  \
  qruov3q7L10v1100m140 qruov3q31L3v246m87  qruov3q31L10v890m100  qruov3q127L3v228m78  \
  qruov5q7L10v1490m190 qruov5q31L3v324m114 qruov5q31L10v1120m120 qruov5q127L3v306m105

.PHONY: all clean $(subdirs)

all: $(subdirs)

$(subdirs): qruov_config.src
	# aes
	mkdir -p $@/$(platform)a
	grep $@ qruov_config.src > $@/$(platform)a/qruov_config.txt
	sh -c "cd $@/$(platform)a ; ln -s ../../$(platform)/* . || true"
	$(MAKE) -C $@/$(platform)a
	# shake
	mkdir -p $@/$(platform)s
	echo -n " -DQRUOV_PRG_SHAKE " > $@/$(platform)s/qruov_config.txt
	grep $@ qruov_config.src >> $@/$(platform)s/qruov_config.txt
	sh -c "cd $@/$(platform)s ; ln -s ../../$(platform)/* . || true"
	$(MAKE) -C $@/$(platform)s

clean:
	rm -rf $(subdirs)
