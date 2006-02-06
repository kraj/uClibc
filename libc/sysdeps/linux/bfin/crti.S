	.section .init
	.section .fini

.text;
.align 2
.global __init;
.type __init, STT_FUNC;
__init:
	LINK 0;
	SP += -12;

.align 2
.global __fini;
.type __fini, STT_FUNC;
__fini:
	LINK 0;
	SP += -12;
