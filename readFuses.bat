avrdude -Cavrdude.conf -v -pattiny13 -cstk500v1 -PCOM6 -b19200 -U lfuse:r:low_fuse_val.hex:h -U hfuse:r:high_fuse_val.hex:h