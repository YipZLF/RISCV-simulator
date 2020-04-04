
add:     file format elf64-littleriscv


Disassembly of section .text:

00000000000100b0 <register_fini>:
   100b0:	00000793          	li	a5,0
   100b4:	00078863          	beqz	a5,100c4 <register_fini+0x14>
   100b8:	00010537          	lui	a0,0x10
   100bc:	56050513          	addi	a0,a0,1376 # 10560 <__libc_fini_array>
   100c0:	4fc0006f          	j	105bc <atexit>
   100c4:	00008067          	ret

00000000000100c8 <_start>:
   100c8:	00002197          	auipc	gp,0x2
   100cc:	e1818193          	addi	gp,gp,-488 # 11ee0 <__global_pointer$>
   100d0:	f8818513          	addi	a0,gp,-120 # 11e68 <completed.5525>
   100d4:	fc018613          	addi	a2,gp,-64 # 11ea0 <__BSS_END__>
   100d8:	40a60633          	sub	a2,a2,a0
   100dc:	00000593          	li	a1,0
   100e0:	27c000ef          	jal	ra,1035c <memset>
   100e4:	00000517          	auipc	a0,0x0
   100e8:	4d850513          	addi	a0,a0,1240 # 105bc <atexit>
   100ec:	00050863          	beqz	a0,100fc <_start+0x34>
   100f0:	00000517          	auipc	a0,0x0
   100f4:	47050513          	addi	a0,a0,1136 # 10560 <__libc_fini_array>
   100f8:	4c4000ef          	jal	ra,105bc <atexit>
   100fc:	1c4000ef          	jal	ra,102c0 <__libc_init_array>
   10100:	00012503          	lw	a0,0(sp)
   10104:	00810593          	addi	a1,sp,8
   10108:	00000613          	li	a2,0
   1010c:	098000ef          	jal	ra,101a4 <main>
   10110:	1800006f          	j	10290 <exit>

0000000000010114 <__do_global_dtors_aux>:
   10114:	f881c703          	lbu	a4,-120(gp) # 11e68 <completed.5525>
   10118:	04071263          	bnez	a4,1015c <__do_global_dtors_aux+0x48>
   1011c:	ff010113          	addi	sp,sp,-16
   10120:	00813023          	sd	s0,0(sp)
   10124:	00078413          	mv	s0,a5
   10128:	00113423          	sd	ra,8(sp)
   1012c:	00000793          	li	a5,0
   10130:	00078a63          	beqz	a5,10144 <__do_global_dtors_aux+0x30>
   10134:	00011537          	lui	a0,0x11
   10138:	6c450513          	addi	a0,a0,1732 # 116c4 <__FRAME_END__>
   1013c:	00000097          	auipc	ra,0x0
   10140:	000000e7          	jalr	zero # 0 <register_fini-0x100b0>
   10144:	00100793          	li	a5,1
   10148:	00813083          	ld	ra,8(sp)
   1014c:	f8f18423          	sb	a5,-120(gp) # 11e68 <completed.5525>
   10150:	00013403          	ld	s0,0(sp)
   10154:	01010113          	addi	sp,sp,16
   10158:	00008067          	ret
   1015c:	00008067          	ret

0000000000010160 <frame_dummy>:
   10160:	00000793          	li	a5,0
   10164:	00078c63          	beqz	a5,1017c <frame_dummy+0x1c>
   10168:	00011537          	lui	a0,0x11
   1016c:	f9018593          	addi	a1,gp,-112 # 11e70 <object.5530>
   10170:	6c450513          	addi	a0,a0,1732 # 116c4 <__FRAME_END__>
   10174:	00000317          	auipc	t1,0x0
   10178:	00000067          	jr	zero # 0 <register_fini-0x100b0>
   1017c:	00008067          	ret

0000000000010180 <Exit>:
   10180:	ff010113          	addi	sp,sp,-16
   10184:	00813423          	sd	s0,8(sp)
   10188:	01010413          	addi	s0,sp,16
   1018c:	00100893          	li	a7,1
   10190:	00000073          	ecall
   10194:	00000013          	nop
   10198:	00813403          	ld	s0,8(sp)
   1019c:	01010113          	addi	sp,sp,16
   101a0:	00008067          	ret

00000000000101a4 <main>:
   101a4:	fe010113          	addi	sp,sp,-32
   101a8:	00113c23          	sd	ra,24(sp)
   101ac:	00813823          	sd	s0,16(sp)
   101b0:	02010413          	addi	s0,sp,32
   101b4:	fe042623          	sw	zero,-20(s0)
   101b8:	fe042623          	sw	zero,-20(s0)
   101bc:	0480006f          	j	10204 <main+0x60>
   101c0:	000117b7          	lui	a5,0x11
   101c4:	6e078713          	addi	a4,a5,1760 # 116e0 <result>
   101c8:	fec42783          	lw	a5,-20(s0)
   101cc:	00279793          	slli	a5,a5,0x2
   101d0:	00f707b3          	add	a5,a4,a5
   101d4:	0007a783          	lw	a5,0(a5)
   101d8:	00a7879b          	addiw	a5,a5,10
   101dc:	0007871b          	sext.w	a4,a5
   101e0:	000117b7          	lui	a5,0x11
   101e4:	6e078693          	addi	a3,a5,1760 # 116e0 <result>
   101e8:	fec42783          	lw	a5,-20(s0)
   101ec:	00279793          	slli	a5,a5,0x2
   101f0:	00f687b3          	add	a5,a3,a5
   101f4:	00e7a023          	sw	a4,0(a5)
   101f8:	fec42783          	lw	a5,-20(s0)
   101fc:	0017879b          	addiw	a5,a5,1
   10200:	fef42623          	sw	a5,-20(s0)
   10204:	fec42783          	lw	a5,-20(s0)
   10208:	0007871b          	sext.w	a4,a5
   1020c:	00400793          	li	a5,4
   10210:	fae7d8e3          	bge	a5,a4,101c0 <main+0x1c>
   10214:	00500793          	li	a5,5
   10218:	fef42623          	sw	a5,-20(s0)
   1021c:	0480006f          	j	10264 <main+0xc0>
   10220:	000117b7          	lui	a5,0x11
   10224:	6e078713          	addi	a4,a5,1760 # 116e0 <result>
   10228:	fec42783          	lw	a5,-20(s0)
   1022c:	00279793          	slli	a5,a5,0x2
   10230:	00f707b3          	add	a5,a4,a5
   10234:	0007a783          	lw	a5,0(a5)
   10238:	ffb7879b          	addiw	a5,a5,-5
   1023c:	0007871b          	sext.w	a4,a5
   10240:	000117b7          	lui	a5,0x11
   10244:	6e078693          	addi	a3,a5,1760 # 116e0 <result>
   10248:	fec42783          	lw	a5,-20(s0)
   1024c:	00279793          	slli	a5,a5,0x2
   10250:	00f687b3          	add	a5,a3,a5
   10254:	00e7a023          	sw	a4,0(a5)
   10258:	fec42783          	lw	a5,-20(s0)
   1025c:	0017879b          	addiw	a5,a5,1
   10260:	fef42623          	sw	a5,-20(s0)
   10264:	fec42783          	lw	a5,-20(s0)
   10268:	0007871b          	sext.w	a4,a5
   1026c:	00900793          	li	a5,9
   10270:	fae7d8e3          	bge	a5,a4,10220 <main+0x7c>
   10274:	f0dff0ef          	jal	ra,10180 <Exit>
   10278:	00000793          	li	a5,0
   1027c:	00078513          	mv	a0,a5
   10280:	01813083          	ld	ra,24(sp)
   10284:	01013403          	ld	s0,16(sp)
   10288:	02010113          	addi	sp,sp,32
   1028c:	00008067          	ret

0000000000010290 <exit>:
   10290:	ff010113          	addi	sp,sp,-16
   10294:	00000593          	li	a1,0
   10298:	00813023          	sd	s0,0(sp)
   1029c:	00113423          	sd	ra,8(sp)
   102a0:	00050413          	mv	s0,a0
   102a4:	194000ef          	jal	ra,10438 <__call_exitprocs>
   102a8:	f701b503          	ld	a0,-144(gp) # 11e50 <_global_impure_ptr>
   102ac:	05853783          	ld	a5,88(a0)
   102b0:	00078463          	beqz	a5,102b8 <exit+0x28>
   102b4:	000780e7          	jalr	a5
   102b8:	00040513          	mv	a0,s0
   102bc:	3bc000ef          	jal	ra,10678 <_exit>

00000000000102c0 <__libc_init_array>:
   102c0:	fe010113          	addi	sp,sp,-32
   102c4:	00813823          	sd	s0,16(sp)
   102c8:	01213023          	sd	s2,0(sp)
   102cc:	00011437          	lui	s0,0x11
   102d0:	00011937          	lui	s2,0x11
   102d4:	6c840793          	addi	a5,s0,1736 # 116c8 <__init_array_start>
   102d8:	6c890913          	addi	s2,s2,1736 # 116c8 <__init_array_start>
   102dc:	40f90933          	sub	s2,s2,a5
   102e0:	00113c23          	sd	ra,24(sp)
   102e4:	00913423          	sd	s1,8(sp)
   102e8:	40395913          	srai	s2,s2,0x3
   102ec:	02090063          	beqz	s2,1030c <__libc_init_array+0x4c>
   102f0:	6c840413          	addi	s0,s0,1736
   102f4:	00000493          	li	s1,0
   102f8:	00043783          	ld	a5,0(s0)
   102fc:	00148493          	addi	s1,s1,1
   10300:	00840413          	addi	s0,s0,8
   10304:	000780e7          	jalr	a5
   10308:	fe9918e3          	bne	s2,s1,102f8 <__libc_init_array+0x38>
   1030c:	00011437          	lui	s0,0x11
   10310:	00011937          	lui	s2,0x11
   10314:	6c840793          	addi	a5,s0,1736 # 116c8 <__init_array_start>
   10318:	6d890913          	addi	s2,s2,1752 # 116d8 <__do_global_dtors_aux_fini_array_entry>
   1031c:	40f90933          	sub	s2,s2,a5
   10320:	40395913          	srai	s2,s2,0x3
   10324:	02090063          	beqz	s2,10344 <__libc_init_array+0x84>
   10328:	6c840413          	addi	s0,s0,1736
   1032c:	00000493          	li	s1,0
   10330:	00043783          	ld	a5,0(s0)
   10334:	00148493          	addi	s1,s1,1
   10338:	00840413          	addi	s0,s0,8
   1033c:	000780e7          	jalr	a5
   10340:	fe9918e3          	bne	s2,s1,10330 <__libc_init_array+0x70>
   10344:	01813083          	ld	ra,24(sp)
   10348:	01013403          	ld	s0,16(sp)
   1034c:	00813483          	ld	s1,8(sp)
   10350:	00013903          	ld	s2,0(sp)
   10354:	02010113          	addi	sp,sp,32
   10358:	00008067          	ret

000000000001035c <memset>:
   1035c:	00f00313          	li	t1,15
   10360:	00050713          	mv	a4,a0
   10364:	02c37a63          	bgeu	t1,a2,10398 <memset+0x3c>
   10368:	00f77793          	andi	a5,a4,15
   1036c:	0a079063          	bnez	a5,1040c <memset+0xb0>
   10370:	06059e63          	bnez	a1,103ec <memset+0x90>
   10374:	ff067693          	andi	a3,a2,-16
   10378:	00f67613          	andi	a2,a2,15
   1037c:	00e686b3          	add	a3,a3,a4
   10380:	00b73023          	sd	a1,0(a4)
   10384:	00b73423          	sd	a1,8(a4)
   10388:	01070713          	addi	a4,a4,16
   1038c:	fed76ae3          	bltu	a4,a3,10380 <memset+0x24>
   10390:	00061463          	bnez	a2,10398 <memset+0x3c>
   10394:	00008067          	ret
   10398:	40c306b3          	sub	a3,t1,a2
   1039c:	00269693          	slli	a3,a3,0x2
   103a0:	00000297          	auipc	t0,0x0
   103a4:	005686b3          	add	a3,a3,t0
   103a8:	00c68067          	jr	12(a3)
   103ac:	00b70723          	sb	a1,14(a4)
   103b0:	00b706a3          	sb	a1,13(a4)
   103b4:	00b70623          	sb	a1,12(a4)
   103b8:	00b705a3          	sb	a1,11(a4)
   103bc:	00b70523          	sb	a1,10(a4)
   103c0:	00b704a3          	sb	a1,9(a4)
   103c4:	00b70423          	sb	a1,8(a4)
   103c8:	00b703a3          	sb	a1,7(a4)
   103cc:	00b70323          	sb	a1,6(a4)
   103d0:	00b702a3          	sb	a1,5(a4)
   103d4:	00b70223          	sb	a1,4(a4)
   103d8:	00b701a3          	sb	a1,3(a4)
   103dc:	00b70123          	sb	a1,2(a4)
   103e0:	00b700a3          	sb	a1,1(a4)
   103e4:	00b70023          	sb	a1,0(a4)
   103e8:	00008067          	ret
   103ec:	0ff5f593          	andi	a1,a1,255
   103f0:	00859693          	slli	a3,a1,0x8
   103f4:	00d5e5b3          	or	a1,a1,a3
   103f8:	01059693          	slli	a3,a1,0x10
   103fc:	00d5e5b3          	or	a1,a1,a3
   10400:	02059693          	slli	a3,a1,0x20
   10404:	00d5e5b3          	or	a1,a1,a3
   10408:	f6dff06f          	j	10374 <memset+0x18>
   1040c:	00279693          	slli	a3,a5,0x2
   10410:	00000297          	auipc	t0,0x0
   10414:	005686b3          	add	a3,a3,t0
   10418:	00008293          	mv	t0,ra
   1041c:	f98680e7          	jalr	-104(a3)
   10420:	00028093          	mv	ra,t0
   10424:	ff078793          	addi	a5,a5,-16
   10428:	40f70733          	sub	a4,a4,a5
   1042c:	00f60633          	add	a2,a2,a5
   10430:	f6c374e3          	bgeu	t1,a2,10398 <memset+0x3c>
   10434:	f3dff06f          	j	10370 <memset+0x14>

0000000000010438 <__call_exitprocs>:
   10438:	fb010113          	addi	sp,sp,-80
   1043c:	03413023          	sd	s4,32(sp)
   10440:	f701ba03          	ld	s4,-144(gp) # 11e50 <_global_impure_ptr>
   10444:	03213823          	sd	s2,48(sp)
   10448:	04113423          	sd	ra,72(sp)
   1044c:	1f8a3903          	ld	s2,504(s4)
   10450:	04813023          	sd	s0,64(sp)
   10454:	02913c23          	sd	s1,56(sp)
   10458:	03313423          	sd	s3,40(sp)
   1045c:	01513c23          	sd	s5,24(sp)
   10460:	01613823          	sd	s6,16(sp)
   10464:	01713423          	sd	s7,8(sp)
   10468:	01813023          	sd	s8,0(sp)
   1046c:	04090063          	beqz	s2,104ac <__call_exitprocs+0x74>
   10470:	00050b13          	mv	s6,a0
   10474:	00058b93          	mv	s7,a1
   10478:	00100a93          	li	s5,1
   1047c:	fff00993          	li	s3,-1
   10480:	00892483          	lw	s1,8(s2)
   10484:	fff4841b          	addiw	s0,s1,-1
   10488:	02044263          	bltz	s0,104ac <__call_exitprocs+0x74>
   1048c:	00349493          	slli	s1,s1,0x3
   10490:	009904b3          	add	s1,s2,s1
   10494:	040b8463          	beqz	s7,104dc <__call_exitprocs+0xa4>
   10498:	2084b783          	ld	a5,520(s1)
   1049c:	05778063          	beq	a5,s7,104dc <__call_exitprocs+0xa4>
   104a0:	fff4041b          	addiw	s0,s0,-1
   104a4:	ff848493          	addi	s1,s1,-8
   104a8:	ff3416e3          	bne	s0,s3,10494 <__call_exitprocs+0x5c>
   104ac:	04813083          	ld	ra,72(sp)
   104b0:	04013403          	ld	s0,64(sp)
   104b4:	03813483          	ld	s1,56(sp)
   104b8:	03013903          	ld	s2,48(sp)
   104bc:	02813983          	ld	s3,40(sp)
   104c0:	02013a03          	ld	s4,32(sp)
   104c4:	01813a83          	ld	s5,24(sp)
   104c8:	01013b03          	ld	s6,16(sp)
   104cc:	00813b83          	ld	s7,8(sp)
   104d0:	00013c03          	ld	s8,0(sp)
   104d4:	05010113          	addi	sp,sp,80
   104d8:	00008067          	ret
   104dc:	00892783          	lw	a5,8(s2)
   104e0:	0084b703          	ld	a4,8(s1)
   104e4:	fff7879b          	addiw	a5,a5,-1
   104e8:	06878263          	beq	a5,s0,1054c <__call_exitprocs+0x114>
   104ec:	0004b423          	sd	zero,8(s1)
   104f0:	fa0708e3          	beqz	a4,104a0 <__call_exitprocs+0x68>
   104f4:	31092783          	lw	a5,784(s2)
   104f8:	008a96bb          	sllw	a3,s5,s0
   104fc:	00892c03          	lw	s8,8(s2)
   10500:	00d7f7b3          	and	a5,a5,a3
   10504:	0007879b          	sext.w	a5,a5
   10508:	02079263          	bnez	a5,1052c <__call_exitprocs+0xf4>
   1050c:	000700e7          	jalr	a4
   10510:	00892703          	lw	a4,8(s2)
   10514:	1f8a3783          	ld	a5,504(s4)
   10518:	01871463          	bne	a4,s8,10520 <__call_exitprocs+0xe8>
   1051c:	f8f902e3          	beq	s2,a5,104a0 <__call_exitprocs+0x68>
   10520:	f80786e3          	beqz	a5,104ac <__call_exitprocs+0x74>
   10524:	00078913          	mv	s2,a5
   10528:	f59ff06f          	j	10480 <__call_exitprocs+0x48>
   1052c:	31492783          	lw	a5,788(s2)
   10530:	1084b583          	ld	a1,264(s1)
   10534:	00d7f7b3          	and	a5,a5,a3
   10538:	0007879b          	sext.w	a5,a5
   1053c:	00079c63          	bnez	a5,10554 <__call_exitprocs+0x11c>
   10540:	000b0513          	mv	a0,s6
   10544:	000700e7          	jalr	a4
   10548:	fc9ff06f          	j	10510 <__call_exitprocs+0xd8>
   1054c:	00892423          	sw	s0,8(s2)
   10550:	fa1ff06f          	j	104f0 <__call_exitprocs+0xb8>
   10554:	00058513          	mv	a0,a1
   10558:	000700e7          	jalr	a4
   1055c:	fb5ff06f          	j	10510 <__call_exitprocs+0xd8>

0000000000010560 <__libc_fini_array>:
   10560:	fe010113          	addi	sp,sp,-32
   10564:	00813823          	sd	s0,16(sp)
   10568:	000117b7          	lui	a5,0x11
   1056c:	00011437          	lui	s0,0x11
   10570:	6d840413          	addi	s0,s0,1752 # 116d8 <__do_global_dtors_aux_fini_array_entry>
   10574:	6e078793          	addi	a5,a5,1760 # 116e0 <result>
   10578:	408787b3          	sub	a5,a5,s0
   1057c:	00913423          	sd	s1,8(sp)
   10580:	00113c23          	sd	ra,24(sp)
   10584:	4037d493          	srai	s1,a5,0x3
   10588:	02048063          	beqz	s1,105a8 <__libc_fini_array+0x48>
   1058c:	ff878793          	addi	a5,a5,-8
   10590:	00878433          	add	s0,a5,s0
   10594:	00043783          	ld	a5,0(s0)
   10598:	fff48493          	addi	s1,s1,-1
   1059c:	ff840413          	addi	s0,s0,-8
   105a0:	000780e7          	jalr	a5
   105a4:	fe0498e3          	bnez	s1,10594 <__libc_fini_array+0x34>
   105a8:	01813083          	ld	ra,24(sp)
   105ac:	01013403          	ld	s0,16(sp)
   105b0:	00813483          	ld	s1,8(sp)
   105b4:	02010113          	addi	sp,sp,32
   105b8:	00008067          	ret

00000000000105bc <atexit>:
   105bc:	00050593          	mv	a1,a0
   105c0:	00000693          	li	a3,0
   105c4:	00000613          	li	a2,0
   105c8:	00000513          	li	a0,0
   105cc:	0040006f          	j	105d0 <__register_exitproc>

00000000000105d0 <__register_exitproc>:
   105d0:	f701b703          	ld	a4,-144(gp) # 11e50 <_global_impure_ptr>
   105d4:	1f873783          	ld	a5,504(a4)
   105d8:	06078063          	beqz	a5,10638 <__register_exitproc+0x68>
   105dc:	0087a703          	lw	a4,8(a5)
   105e0:	01f00813          	li	a6,31
   105e4:	08e84663          	blt	a6,a4,10670 <__register_exitproc+0xa0>
   105e8:	02050863          	beqz	a0,10618 <__register_exitproc+0x48>
   105ec:	00371813          	slli	a6,a4,0x3
   105f0:	01078833          	add	a6,a5,a6
   105f4:	10c83823          	sd	a2,272(a6)
   105f8:	3107a883          	lw	a7,784(a5)
   105fc:	00100613          	li	a2,1
   10600:	00e6163b          	sllw	a2,a2,a4
   10604:	00c8e8b3          	or	a7,a7,a2
   10608:	3117a823          	sw	a7,784(a5)
   1060c:	20d83823          	sd	a3,528(a6)
   10610:	00200693          	li	a3,2
   10614:	02d50863          	beq	a0,a3,10644 <__register_exitproc+0x74>
   10618:	00270693          	addi	a3,a4,2
   1061c:	00369693          	slli	a3,a3,0x3
   10620:	0017071b          	addiw	a4,a4,1
   10624:	00e7a423          	sw	a4,8(a5)
   10628:	00d787b3          	add	a5,a5,a3
   1062c:	00b7b023          	sd	a1,0(a5)
   10630:	00000513          	li	a0,0
   10634:	00008067          	ret
   10638:	20070793          	addi	a5,a4,512
   1063c:	1ef73c23          	sd	a5,504(a4)
   10640:	f9dff06f          	j	105dc <__register_exitproc+0xc>
   10644:	3147a683          	lw	a3,788(a5)
   10648:	00000513          	li	a0,0
   1064c:	00c6e633          	or	a2,a3,a2
   10650:	00270693          	addi	a3,a4,2
   10654:	00369693          	slli	a3,a3,0x3
   10658:	0017071b          	addiw	a4,a4,1
   1065c:	30c7aa23          	sw	a2,788(a5)
   10660:	00e7a423          	sw	a4,8(a5)
   10664:	00d787b3          	add	a5,a5,a3
   10668:	00b7b023          	sd	a1,0(a5)
   1066c:	00008067          	ret
   10670:	fff00513          	li	a0,-1
   10674:	00008067          	ret

0000000000010678 <_exit>:
   10678:	00000593          	li	a1,0
   1067c:	00000613          	li	a2,0
   10680:	00000693          	li	a3,0
   10684:	00000713          	li	a4,0
   10688:	00000793          	li	a5,0
   1068c:	05d00893          	li	a7,93
   10690:	00000073          	ecall
   10694:	00054463          	bltz	a0,1069c <_exit+0x24>
   10698:	0000006f          	j	10698 <_exit+0x20>
   1069c:	ff010113          	addi	sp,sp,-16
   106a0:	00813023          	sd	s0,0(sp)
   106a4:	00050413          	mv	s0,a0
   106a8:	00113423          	sd	ra,8(sp)
   106ac:	4080043b          	negw	s0,s0
   106b0:	00c000ef          	jal	ra,106bc <__errno>
   106b4:	00852023          	sw	s0,0(a0)
   106b8:	0000006f          	j	106b8 <_exit+0x40>

00000000000106bc <__errno>:
   106bc:	f801b503          	ld	a0,-128(gp) # 11e60 <_impure_ptr>
   106c0:	00008067          	ret
