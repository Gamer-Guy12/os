#include <gdt.h>
#include <interrupts.h>

// Interrupt handlers
extern void idt_handler_0(void);
extern void idt_handler_1(void);
extern void idt_handler_2(void);
extern void idt_handler_3(void);
extern void idt_handler_4(void);
extern void idt_handler_5(void);
extern void idt_handler_6(void);
extern void idt_handler_7(void);
extern void idt_handler_8(void);
extern void idt_handler_9(void);
extern void idt_handler_10(void);
extern void idt_handler_11(void);
extern void idt_handler_12(void);
extern void idt_handler_13(void);
extern void idt_handler_14(void);
extern void idt_handler_15(void);
extern void idt_handler_16(void);
extern void idt_handler_17(void);
extern void idt_handler_18(void);
extern void idt_handler_19(void);
extern void idt_handler_20(void);
extern void idt_handler_21(void);
extern void idt_handler_22(void);
extern void idt_handler_23(void);
extern void idt_handler_24(void);
extern void idt_handler_25(void);
extern void idt_handler_26(void);
extern void idt_handler_27(void);
extern void idt_handler_28(void);
extern void idt_handler_29(void);
extern void idt_handler_30(void);
extern void idt_handler_31(void);
extern void idt_handler_32(void);
extern void idt_handler_33(void);
extern void idt_handler_34(void);
extern void idt_handler_35(void);
extern void idt_handler_36(void);
extern void idt_handler_37(void);
extern void idt_handler_38(void);
extern void idt_handler_39(void);
extern void idt_handler_40(void);
extern void idt_handler_41(void);
extern void idt_handler_42(void);
extern void idt_handler_43(void);
extern void idt_handler_44(void);
extern void idt_handler_45(void);
extern void idt_handler_46(void);
extern void idt_handler_47(void);
extern void idt_handler_48(void);
extern void idt_handler_49(void);
extern void idt_handler_50(void);
extern void idt_handler_51(void);
extern void idt_handler_52(void);
extern void idt_handler_53(void);
extern void idt_handler_54(void);
extern void idt_handler_55(void);
extern void idt_handler_56(void);
extern void idt_handler_57(void);
extern void idt_handler_58(void);
extern void idt_handler_59(void);
extern void idt_handler_60(void);
extern void idt_handler_61(void);
extern void idt_handler_62(void);
extern void idt_handler_63(void);
extern void idt_handler_64(void);
extern void idt_handler_65(void);
extern void idt_handler_66(void);
extern void idt_handler_67(void);
extern void idt_handler_68(void);
extern void idt_handler_69(void);
extern void idt_handler_70(void);
extern void idt_handler_71(void);
extern void idt_handler_72(void);
extern void idt_handler_73(void);
extern void idt_handler_74(void);
extern void idt_handler_75(void);
extern void idt_handler_76(void);
extern void idt_handler_77(void);
extern void idt_handler_78(void);
extern void idt_handler_79(void);
extern void idt_handler_80(void);
extern void idt_handler_81(void);
extern void idt_handler_82(void);
extern void idt_handler_83(void);
extern void idt_handler_84(void);
extern void idt_handler_85(void);
extern void idt_handler_86(void);
extern void idt_handler_87(void);
extern void idt_handler_88(void);
extern void idt_handler_89(void);
extern void idt_handler_90(void);
extern void idt_handler_91(void);
extern void idt_handler_92(void);
extern void idt_handler_93(void);
extern void idt_handler_94(void);
extern void idt_handler_95(void);
extern void idt_handler_96(void);
extern void idt_handler_97(void);
extern void idt_handler_98(void);
extern void idt_handler_99(void);
extern void idt_handler_100(void);
extern void idt_handler_101(void);
extern void idt_handler_102(void);
extern void idt_handler_103(void);
extern void idt_handler_104(void);
extern void idt_handler_105(void);
extern void idt_handler_106(void);
extern void idt_handler_107(void);
extern void idt_handler_108(void);
extern void idt_handler_109(void);
extern void idt_handler_110(void);
extern void idt_handler_111(void);
extern void idt_handler_112(void);
extern void idt_handler_113(void);
extern void idt_handler_114(void);
extern void idt_handler_115(void);
extern void idt_handler_116(void);
extern void idt_handler_117(void);
extern void idt_handler_118(void);
extern void idt_handler_119(void);
extern void idt_handler_120(void);
extern void idt_handler_121(void);
extern void idt_handler_122(void);
extern void idt_handler_123(void);
extern void idt_handler_124(void);
extern void idt_handler_125(void);
extern void idt_handler_126(void);
extern void idt_handler_127(void);
extern void idt_handler_128(void);
extern void idt_handler_129(void);
extern void idt_handler_130(void);
extern void idt_handler_131(void);
extern void idt_handler_132(void);
extern void idt_handler_133(void);
extern void idt_handler_134(void);
extern void idt_handler_135(void);
extern void idt_handler_136(void);
extern void idt_handler_137(void);
extern void idt_handler_138(void);
extern void idt_handler_139(void);
extern void idt_handler_140(void);
extern void idt_handler_141(void);
extern void idt_handler_142(void);
extern void idt_handler_143(void);
extern void idt_handler_144(void);
extern void idt_handler_145(void);
extern void idt_handler_146(void);
extern void idt_handler_147(void);
extern void idt_handler_148(void);
extern void idt_handler_149(void);
extern void idt_handler_150(void);
extern void idt_handler_151(void);
extern void idt_handler_152(void);
extern void idt_handler_153(void);
extern void idt_handler_154(void);
extern void idt_handler_155(void);
extern void idt_handler_156(void);
extern void idt_handler_157(void);
extern void idt_handler_158(void);
extern void idt_handler_159(void);
extern void idt_handler_160(void);
extern void idt_handler_161(void);
extern void idt_handler_162(void);
extern void idt_handler_163(void);
extern void idt_handler_164(void);
extern void idt_handler_165(void);
extern void idt_handler_166(void);
extern void idt_handler_167(void);
extern void idt_handler_168(void);
extern void idt_handler_169(void);
extern void idt_handler_170(void);
extern void idt_handler_171(void);
extern void idt_handler_172(void);
extern void idt_handler_173(void);
extern void idt_handler_174(void);
extern void idt_handler_175(void);
extern void idt_handler_176(void);
extern void idt_handler_177(void);
extern void idt_handler_178(void);
extern void idt_handler_179(void);
extern void idt_handler_180(void);
extern void idt_handler_181(void);
extern void idt_handler_182(void);
extern void idt_handler_183(void);
extern void idt_handler_184(void);
extern void idt_handler_185(void);
extern void idt_handler_186(void);
extern void idt_handler_187(void);
extern void idt_handler_188(void);
extern void idt_handler_189(void);
extern void idt_handler_190(void);
extern void idt_handler_191(void);
extern void idt_handler_192(void);
extern void idt_handler_193(void);
extern void idt_handler_194(void);
extern void idt_handler_195(void);
extern void idt_handler_196(void);
extern void idt_handler_197(void);
extern void idt_handler_198(void);
extern void idt_handler_199(void);
extern void idt_handler_200(void);
extern void idt_handler_201(void);
extern void idt_handler_202(void);
extern void idt_handler_203(void);
extern void idt_handler_204(void);
extern void idt_handler_205(void);
extern void idt_handler_206(void);
extern void idt_handler_207(void);
extern void idt_handler_208(void);
extern void idt_handler_209(void);
extern void idt_handler_210(void);
extern void idt_handler_211(void);
extern void idt_handler_212(void);
extern void idt_handler_213(void);
extern void idt_handler_214(void);
extern void idt_handler_215(void);
extern void idt_handler_216(void);
extern void idt_handler_217(void);
extern void idt_handler_218(void);
extern void idt_handler_219(void);
extern void idt_handler_220(void);
extern void idt_handler_221(void);
extern void idt_handler_222(void);
extern void idt_handler_223(void);
extern void idt_handler_224(void);
extern void idt_handler_225(void);
extern void idt_handler_226(void);
extern void idt_handler_227(void);
extern void idt_handler_228(void);
extern void idt_handler_229(void);
extern void idt_handler_230(void);
extern void idt_handler_231(void);
extern void idt_handler_232(void);
extern void idt_handler_233(void);
extern void idt_handler_234(void);
extern void idt_handler_235(void);
extern void idt_handler_236(void);
extern void idt_handler_237(void);
extern void idt_handler_238(void);
extern void idt_handler_239(void);
extern void idt_handler_240(void);
extern void idt_handler_241(void);
extern void idt_handler_242(void);
extern void idt_handler_243(void);
extern void idt_handler_244(void);
extern void idt_handler_245(void);
extern void idt_handler_246(void);
extern void idt_handler_247(void);
extern void idt_handler_248(void);
extern void idt_handler_249(void);
extern void idt_handler_250(void);
extern void idt_handler_251(void);
extern void idt_handler_252(void);
extern void idt_handler_253(void);
extern void idt_handler_254(void);
extern void idt_handler_255(void);

void register_handlers(void) {
  set_idt_gate(0, &idt_handler_0, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(1, &idt_handler_1, KERNEL_CODE_SELECTOR, IDT_TRAP_GATE, 0,
               0, 0);
  set_idt_gate(2, &idt_handler_2, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(3, &idt_handler_3, KERNEL_CODE_SELECTOR, IDT_TRAP_GATE, 0,
               0, 0);
  set_idt_gate(4, &idt_handler_4, KERNEL_CODE_SELECTOR, IDT_TRAP_GATE, 0,
               0, 0);
  set_idt_gate(5, &idt_handler_5, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(6, &idt_handler_6, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(7, &idt_handler_7, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(8, &idt_handler_8, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(9, &idt_handler_9, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(10, &idt_handler_10, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(11, &idt_handler_11, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(12, &idt_handler_12, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(13, &idt_handler_13, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(14, &idt_handler_14, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(15, &idt_handler_15, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(16, &idt_handler_16, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(17, &idt_handler_17, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(18, &idt_handler_18, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(19, &idt_handler_19, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(20, &idt_handler_20, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(21, &idt_handler_21, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(22, &idt_handler_22, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(23, &idt_handler_23, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(24, &idt_handler_24, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(25, &idt_handler_25, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(26, &idt_handler_26, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(27, &idt_handler_27, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(28, &idt_handler_28, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(29, &idt_handler_29, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(30, &idt_handler_30, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(31, &idt_handler_31, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(32, &idt_handler_32, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(33, &idt_handler_33, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(34, &idt_handler_34, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(35, &idt_handler_35, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(36, &idt_handler_36, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(37, &idt_handler_37, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(38, &idt_handler_38, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(39, &idt_handler_39, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(40, &idt_handler_40, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(41, &idt_handler_41, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(42, &idt_handler_42, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(43, &idt_handler_43, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(44, &idt_handler_44, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(45, &idt_handler_45, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(46, &idt_handler_46, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(47, &idt_handler_47, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(48, &idt_handler_48, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(49, &idt_handler_49, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(50, &idt_handler_50, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(51, &idt_handler_51, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(52, &idt_handler_52, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(53, &idt_handler_53, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(54, &idt_handler_54, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(55, &idt_handler_55, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(56, &idt_handler_56, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(57, &idt_handler_57, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(58, &idt_handler_58, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(59, &idt_handler_59, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(60, &idt_handler_60, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(61, &idt_handler_61, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(62, &idt_handler_62, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(63, &idt_handler_63, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(64, &idt_handler_64, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(65, &idt_handler_65, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(66, &idt_handler_66, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(67, &idt_handler_67, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(68, &idt_handler_68, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(69, &idt_handler_69, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(70, &idt_handler_70, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(71, &idt_handler_71, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(72, &idt_handler_72, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(73, &idt_handler_73, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(74, &idt_handler_74, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(75, &idt_handler_75, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(76, &idt_handler_76, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(77, &idt_handler_77, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(78, &idt_handler_78, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(79, &idt_handler_79, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(80, &idt_handler_80, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(81, &idt_handler_81, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(82, &idt_handler_82, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(83, &idt_handler_83, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(84, &idt_handler_84, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(85, &idt_handler_85, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(86, &idt_handler_86, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(87, &idt_handler_87, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(88, &idt_handler_88, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(89, &idt_handler_89, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(90, &idt_handler_90, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(91, &idt_handler_91, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(92, &idt_handler_92, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(93, &idt_handler_93, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(94, &idt_handler_94, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(95, &idt_handler_95, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(96, &idt_handler_96, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(97, &idt_handler_97, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(98, &idt_handler_98, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(99, &idt_handler_99, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE, 0,
               0, 0);
  set_idt_gate(100, &idt_handler_100, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(101, &idt_handler_101, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(102, &idt_handler_102, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(103, &idt_handler_103, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(104, &idt_handler_104, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(105, &idt_handler_105, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(106, &idt_handler_106, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(107, &idt_handler_107, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(108, &idt_handler_108, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(109, &idt_handler_109, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(110, &idt_handler_110, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(111, &idt_handler_111, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(112, &idt_handler_112, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(113, &idt_handler_113, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(114, &idt_handler_114, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(115, &idt_handler_115, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(116, &idt_handler_116, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(117, &idt_handler_117, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(118, &idt_handler_118, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(119, &idt_handler_119, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(120, &idt_handler_120, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(121, &idt_handler_121, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(122, &idt_handler_122, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(123, &idt_handler_123, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(124, &idt_handler_124, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(125, &idt_handler_125, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(126, &idt_handler_126, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(127, &idt_handler_127, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(128, &idt_handler_128, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(129, &idt_handler_129, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(130, &idt_handler_130, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(131, &idt_handler_131, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(132, &idt_handler_132, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(133, &idt_handler_133, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(134, &idt_handler_134, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(135, &idt_handler_135, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(136, &idt_handler_136, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(137, &idt_handler_137, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(138, &idt_handler_138, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(139, &idt_handler_139, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(140, &idt_handler_140, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(141, &idt_handler_141, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(142, &idt_handler_142, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(143, &idt_handler_143, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(144, &idt_handler_144, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(145, &idt_handler_145, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(146, &idt_handler_146, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(147, &idt_handler_147, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(148, &idt_handler_148, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(149, &idt_handler_149, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(150, &idt_handler_150, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(151, &idt_handler_151, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(152, &idt_handler_152, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(153, &idt_handler_153, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(154, &idt_handler_154, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(155, &idt_handler_155, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(156, &idt_handler_156, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(157, &idt_handler_157, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(158, &idt_handler_158, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(159, &idt_handler_159, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(160, &idt_handler_160, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(161, &idt_handler_161, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(162, &idt_handler_162, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(163, &idt_handler_163, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(164, &idt_handler_164, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(165, &idt_handler_165, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(166, &idt_handler_166, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(167, &idt_handler_167, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(168, &idt_handler_168, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(169, &idt_handler_169, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(170, &idt_handler_170, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(171, &idt_handler_171, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(172, &idt_handler_172, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(173, &idt_handler_173, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(174, &idt_handler_174, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(175, &idt_handler_175, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(176, &idt_handler_176, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(177, &idt_handler_177, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(178, &idt_handler_178, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(179, &idt_handler_179, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(180, &idt_handler_180, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(181, &idt_handler_181, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(182, &idt_handler_182, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(183, &idt_handler_183, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(184, &idt_handler_184, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(185, &idt_handler_185, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(186, &idt_handler_186, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(187, &idt_handler_187, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(188, &idt_handler_188, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(189, &idt_handler_189, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(190, &idt_handler_190, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(191, &idt_handler_191, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(192, &idt_handler_192, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(193, &idt_handler_193, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(194, &idt_handler_194, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(195, &idt_handler_195, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(196, &idt_handler_196, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(197, &idt_handler_197, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(198, &idt_handler_198, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(199, &idt_handler_199, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(200, &idt_handler_200, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(201, &idt_handler_201, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(202, &idt_handler_202, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(203, &idt_handler_203, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(204, &idt_handler_204, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(205, &idt_handler_205, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(206, &idt_handler_206, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(207, &idt_handler_207, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(208, &idt_handler_208, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(209, &idt_handler_209, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(210, &idt_handler_210, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(211, &idt_handler_211, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(212, &idt_handler_212, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(213, &idt_handler_213, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(214, &idt_handler_214, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(215, &idt_handler_215, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(216, &idt_handler_216, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(217, &idt_handler_217, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(218, &idt_handler_218, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(219, &idt_handler_219, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(220, &idt_handler_220, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(221, &idt_handler_221, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(222, &idt_handler_222, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(223, &idt_handler_223, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(224, &idt_handler_224, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(225, &idt_handler_225, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(226, &idt_handler_226, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(227, &idt_handler_227, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(228, &idt_handler_228, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(229, &idt_handler_229, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(230, &idt_handler_230, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(231, &idt_handler_231, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(232, &idt_handler_232, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(233, &idt_handler_233, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(234, &idt_handler_234, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(235, &idt_handler_235, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(236, &idt_handler_236, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(237, &idt_handler_237, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(238, &idt_handler_238, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(239, &idt_handler_239, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(240, &idt_handler_240, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(241, &idt_handler_241, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(242, &idt_handler_242, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(243, &idt_handler_243, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(244, &idt_handler_244, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(245, &idt_handler_245, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(246, &idt_handler_246, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(247, &idt_handler_247, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(248, &idt_handler_248, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(249, &idt_handler_249, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(250, &idt_handler_250, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(251, &idt_handler_251, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(252, &idt_handler_252, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(253, &idt_handler_253, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(254, &idt_handler_254, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
  set_idt_gate(255, &idt_handler_255, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE,
               0, 0, 0);
}
