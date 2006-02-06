	.section .init
	.section .fini

.text;
.align 2
.global __init;
.type __init, STT_FUNC;
	SP += 12;
	UNLINK;
	rts;
	.size	__init, .-__init
.align 2
.global __fini;
.type __fini, STT_FUNC;
	SP += 12;
	UNLINK;
	rts;
	.size	__fini, .-__fini
