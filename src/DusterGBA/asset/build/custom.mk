a_pawn_0.img.bin: a_pawn_0.png
	$(call atlpng2grit,1)

turn_tp.img.bin: ../rawmap/turn_tp.png
	$(call rawmap2grit,1,0,0)