#!/usr/bin/python3

def seq(*args):
    mixHistoryMaxLength = 0
    ret = ["LIST_SEQ", len(args)]
    for arg in args:
        if(isinstance(arg,list)):
            # do not choke if a tuple comes...
            ret += list(arg)
        elif(isinstance(arg,tuple)):
            ret += arg[0]
            # if we wanted to use the real minimum history length we
            # would use the follow line.
            #mixHistoryMaxLength = max(mixHistoryMaxLength, arg[1])
            mixHistoryMaxLength += arg[1]
        else:
            ret.append(arg)
    return ret, mixHistoryMaxLength

def alt(*args):
    mixHistoryMaxLength = 0
    ret = ["LIST_ALT", len(args)]
    for arg in args:
        if(isinstance(arg,list)):
            ret += arg
        elif(isinstance(arg,tuple)):
            ret += arg[0]
            # if we wanted to use the real minimum history length we
            # would use the follow line.
            #mixHistoryMaxLength = max(mixHistoryMaxLength, arg[1])
            mixHistoryMaxLength += arg[1]
        else:
            ret.append(arg)
    return ret, mixHistoryMaxLength

def prob(probability, arg):
    mixHistoryMaxLength = 0
    # PROB is only allowed to take 1 element!
    # we misuse the "length" byte as a probability.
    ret = ["PROB", int(probability*100),]
    #ret = ["LIST_SEQ", 1, ] ##for debugging, show all
    if(isinstance(arg,list)):
        ret += list(arg)
    elif(isinstance(arg,tuple)):
        ret += arg[0]
        # if we wanted to use the real minimum history length we
        # would use the follow line.
        #mixHistoryMaxLength = max(mixHistoryMaxLength, arg[1])
        mixHistoryMaxLength += arg[1]
    else:
        ret.append(arg)
    return ret, mixHistoryMaxLength

def esc(arg):
    # ESC is only allowed to take 1 element!
    ret = ["ESC_SPECIAL", 1, ]
    mixHistoryMaxLength = 0
    if(isinstance(arg,list)):
        ret += list(arg)
    elif(isinstance(arg,tuple)):
        ret += arg[0]
        mixHistoryMaxLength += arg[1]
    else:
        ret.append(arg)
    return ret, mixHistoryMaxLength

def mix(*args):
    ret = ["LIST_MIX", len(args),]
    mixHistoryMaxLength = len(args)
    for arg in args:
        if(isinstance(arg,list)):
            ret += list(arg)
        elif(isinstance(arg,tuple)):
            ret += arg[0]
            mixHistoryMaxLength += arg[1]
        else:
            ret.append(arg)
    return ret, mixHistoryMaxLength

if(__name__ == "__main__"):
    call1 = esc("CALL1_MAGIC")
    call2 = esc("CALL2_MAGIC")
    de = 50
    r = 174
    es = 58
    ur = 215
    _pse = 145
    pse = prob(0.8, _pse)
    k = 99
    my = prob(0.2,126)
    _is = prob(0.2,91)
    op = alt(65, 136, 137, 81, 237, 141)# frd om op ham yl ow*/
    dr = alt(52,104)#dr lbr
    nw = 134
    hr = prob(0.2, 85)
    group5 = esc("GROUPOF5_MAGIC")
    repeat = esc("REPEAT_MAGIC")
    pause = esc("PAUSE_MAGIC")
    stop = esc("BT_IDX")
    ar = esc("AR_IDX")
    c = 70 # this is not numbered in order in the abbrevs
    buro = alt(35,29)
    comma = 198

    fer = alt(61,64)
    abt = prob(0.2,9)
    cq = 43

    antenna = alt(
        113, #lw
        77, #gp
        seq(prob(0.3,108),219), #vert
        group5,
        seq(esc("NUM1_MAGIC"),59), #123 el
        seq(esc("NUM2_MAGIC"),124), #123 mtr
    )

    sota_chaser_says_bye =seq(alt(203, 204,206), #tnx, tks, tu
                        prob(0.5, seq(3, de,)),#73 de
                        call2, esc("SK_IDX"))

    qso, mixHistoryMaxLength = seq(
        prob(0.1, seq(152, pause, 152, pause)),
        #qrl? qrl?
        #FIXME alt(qrl,c) pause

        #contest, sota, or regular or...?
        alt(
            #regular qso
        seq(cq, cq, de, call1, call1, alt(seq(pse, k,),ar ),
        pause,
        # cq cq
        seq(call1, de, call2, call2,
        prob(0.1, seq(pse, alt(170,155,156,158))),# qsy qro qrp qrs
            alt(seq(pse, k), esc('KN_IDX'))),
        pause,
        #
        alt(
            #####english qso
            seq(
            ### 1st durchgang
            seq(
                alt(r,218), # r v
                alt(68, 71, 72, 74,# ga gd ge gm
                    alt(68,74, 79),#german: ga, gm, gt
                ),
                dr,
                op,
                es, prob(0.5,alt(224,121,)), alt(203, 204,), fer,
                # es (vy,mni) (tks,tnx) (fer,fr)
                alt(18, 37),
                # ans call
                stop,
                mix(
                    seq(prob(0.5,seq(ur, prob(0.2,188))), 183,
                        # ur sigs rst
                        esc("RST_MAGIC"), repeat,
                        alt(60, 210,
                            # fb ufb
                            seq(
                                alt(153, 154, 164, 179, 26),repeat,#qrm qrn qsb rfi bci
                                prob(0.2,seq(40,224,28,))#conds vy bd
                            )),stop,
                    ),
                    # prob(0.1,seq(prob(0.2,seq(call1, 91, 81, 39, 195,stop)) #is ham cl stn
                    #              ,137, group5, repeat, stop, #op
                    #              prob(0.5, seq(149, group5, repeat, stop)))), #qra
                    seq(my,238,_is, group5, repeat,stop,),
                    # my name is
                    seq(my,172,_is, prob(0.3, 132), esc("NAME1_MAGIC"), esc("NAME1_MAGIC"),
                        prob(0.1, seq(prob(0.5,195), 137, group5)),#op
                        prob(0.1,seq(149, group5, group5, 195))#qra stn
                        ,stop,),
                    # my qth is nr
                    seq(my,239,_is,esc("DOK_MAGIC"),repeat,stop,),
                    # my dok is
                    # prob(0.1, seq(alt(40,41),r,80,stop)),#conds condx r gud
                ),
                nw,90,ar,
                # nw hw?
                call2, de, call1,
                pse, k,
            ),
            pause,
            ### 2. durchgang
            seq(call1, de, call2, stop,
                alt(r, 60, esc("VE_IDX"), # r fb <ve>
                    seq(prob(0.5, 12), 135) #all ok
                ),
                dr, op, es,
                prob(0.5,alt(224,121,)), alt(203, 204,), fer,
                # vy mni tks tnx fr fer
                alt(182,182,95,123),stop,#rpt rprt info msg
                mix(
                    seq(alt(seq(prob(0.5,seq(ur, prob(0.2,188))), 183,),
                            ur),
                        # ur sigs rst
                    esc("RST_MAGIC"), repeat,
                        alt(60, 210, # fb ufb
                            seq(prob(0.1, 89),#hvy
                                alt(153, 154, 164,))),#qrm qrn qsb
                        stop,
                    ),
                    seq(my,238,_is,group5,repeat,stop,),
                    # my name is
                    seq(my,172,_is,esc("NAME2_MAGIC"),esc("NAME2_MAGIC"),stop,),
                    # my qth is
                    seq(my,239,_is,esc("DOK_MAGIC"),repeat,stop,),
                    # my dok is
                    prob(0.10,seq(esc("AS_IDX"), pause, pause,)),
                    # make <as> a second break
                ),
                mix(
                    seq(hr, alt(
                        seq(alt(180,205,235),group5),#(rig trx xcvr)
                        seq(185, group5, 208, group5)),#rx tx
                        prob(0.1, seq(227,34)), # wid bug
                        prob(0.9,seq(
                            alt(227,es,stop), 147,#pwr
                            abt, alt(seq(esc("NUM3_MAGIC"),alt(225, 226)),#w,wtts
                                     seq(esc("NUM1_MAGIC"), 102),#kw
                            ),
                            prob(0.5,143), #pep
                        alt(147,es,stop),19,group5,stop #ant
                        ))),
                    seq(233,hr,alt(seq(group5, group5), 49, 30, 111), #wx sunny cldy rain
                        200, esc("NUM2_MAGIC"), c, stop, #temp
                    ),
                ),
                    84, 135, 231, ar, #hpe ok ?
                    call1, de, call2, pse, k,
                ),
                pause,
                ### kurze wechsel
                prob(0.1, seq(pse, ur, 19, 14, esc('QUESTION_IDX'),esc('BK_IDX'),#ant agn?
                              pause,
                              92, 88, antenna, repeat, hr, esc('BK_IDX'), #i 88
                              pause,
                )),
                prob(0.1, seq(236, 91, ur, 13, esc('QUESTION_IDX'),esc('BK_IDX'),#what is ur age ?
                              pause,
                              13, hr, 91, esc('NUM2_MAGIC'), repeat, esc('BK_IDX'), #age hr is
                              pause,
                )),
                ### 3. durchgang
                seq(
                    call2, de, call1, stop,
                    alt(r, 60,esc("VE_IDX"),# r fb
                        seq(prob(0.5, 12), 135) #all ok
                    ),
                    dr, alt(op,esc("NAME2_MAGIC")), es,
                    alt(206, prob(0.5,alt(224,121,)), alt(203, 204,)), fer,
                    # tu / vy mni tks tnx fr fer
                    alt(182,95,123),stop,#rprt info msg
                    prob(0.1,seq(224,146,comma,88,131,148,120,228,36)),#vy psed hv ur px not wkd b4
                    mix(
                        seq(hr, alt(180,205,235),group5,#(rig trx xcvr)
                                prob(0.1, seq(227,alt(34,58,seq(56,103)))), # wid bug elbug (el ky)
                            prob(0.9,seq(
                                alt(227,es,stop), 147,#pwr
                                abt, alt(seq(esc("NUM3_MAGIC"),alt(225, 226)),#w,wtts
                                         seq(esc("NUM1_MAGIC"), 102),#kw
                                ),
                                prob(0.5,143), #pep
                            alt(227,es,stop),19,antenna,stop #ant
                        ))),
                        seq(233, hr, alt(seq(group5, group5), 49, 30, 111), es, #wx sunny cldy rain
                            200, esc("NUM2_MAGIC"), c, stop, #temp
                        ),
                        seq(33,88,alt(228,86), #btw hv wkd hrd
                            group5,
                            prob(0.5,191), #sp
                            36,#b4
                            alt(132,151),#nr qrg
                            esc("NUM2_MAGIC"),118, stop #99 mhz

                        ),
                        seq(227,213,alt(65,197),hr,238,group5,stop), #wid unlis frd name
                    ),
                    nw, 160, stop,# qru
                    prob(0.1, seq(230,170,fer,189,prob(0.5,190),stop)),# wl qsy fer fr sked sn
                    mix(
                    seq(_pse, ur, 166, 221, #qsl via
                        alt(buro, seq(117, group5)),#buro mgr
                                      prob(0.5,202) , stop), #tia
                    seq(alt(206, seq(prob(0.5,alt(224,121,)), alt(203, 204,))), fer,
                    # tu / vy mni tks tnx fr fer
                        132, 167), #nice qso
                    seq(es, alt(seq(84, 45,), #hpe cuagn
                            seq(alt(44,46),14),#cu agn
                            27,#bcnu
                            23, 24)), # awdh awds
                    seq(60, 114, 12, esc('NAME2_MAGIC')), #fb on all
                    seq(146, 88, 228, 209) #psed hv wkd u
                    ),
                    seq(prob(0.2, seq(2, es)), # 55 es
                        prob(0.2, 224,), alt(3,4)),# vy (72 73)
                        es, alt(seq(73, 73),# es gl gl
                                69, 75),#gb gn
                    esc("SK_IDX"),
                    call1, de, call2,pse, k
                ),
                pause,
                ### 4. durchgang
                seq(call1, de, call2, stop,
                    alt(r, 60,esc("VE_IDX"),# r fb
                        seq(prob(0.5, 12), 135) #all ok
                    ),
                    dr, alt(op,esc("NAME1_MAGIC")), stop,
                    prob(0.2, seq(42, fer, alt(# congrats fer
                        seq(183, 227, 147),#rst wid pwr
                        107,#lis
                    ))),
                    alt(seq(166, prob(0.5, 196), 221, buro, prob(0.5,135), stop),
                        #qsl (sure) via (buro bureau) (ok)
                        seq(131, 192, _pse, 59, #no sri pse excus
                            alt(seq(131, 166, buro, hr), #no qsl buro hr
                            ),
                            alt(prob(0.5, seq(_pse, 221, 186)), #pse via sase
                                seq(_pse, 59,166, 221,128)#pse el qsl via net
                            ),
                            stop,)
                    ),
                    alt(206, prob(0.5,alt(224,121,)), alt(203, 204,)), fer,
                    # tu / vy mni tks tnx fr fer
                    prob(0.5, 132),167, # nice qso
                    prob(0.2, seq(2, es)), # 55 es
                    prob(0.2, 224,), alt(3,4), es, #vy (72 73) es
                    alt(69, 75, 73,72, #gb gn gl ge
                        seq(80, 54), #gud dx
                        ),
                    dr, op,
                    es, alt(seq(84, 45,), #hpe cuagn
                            seq(alt(44,46),14),#cu agn
                            27,#bcnu
                            23, 24), # awdh awds
                    prob(0.3, seq(93, 47)),# in cw
                    stop,
                    call1, de, call2, esc("SK_IDX"),
                    prob(0.2, alt(159,55,39)), #qrt ee cl
                ),
                #pause,
                ###end
                # seq(206,4), pause, #tu 73
                # #
                # seq(206,55), pause, #tu 73
            ),

            ),
            ),
            ###### SOTA qso
            seq(
                #activator calls
                seq(cq, 216, cq, 216, de, call1, call1, #cq sota cq sota de call1
                    114, esc("SOTA_MAGIC"), repeat, alt(seq(pse, k), ar)), #on
                pause,
                #chaser calls activator
                seq(call1, de, call2, call2,
                    prob(0.1, seq(pse, alt(170,155,156,158))),# qsy qro qrp qrs
                    alt(seq(pse, k), esc('KN_IDX'))),
                pause,
                # caser replies with report
                seq(call2, prob(0.5,seq(ur, prob(0.2,188))), 183,
                    # ur sigs rst
                    esc("RST_MAGIC"), repeat, repeat,
                    de, call1, pse, k),
                pause,
                #chaser confirms
                seq(alt(r, 60, esc("VE_IDX"), # r fb <ve>
                        135), repeat, #ok
                    call2, prob(0.5,seq(ur, prob(0.2,188))), 183,
                    # ur sigs rst
                    esc("RST_MAGIC"), repeat,
                    de, call2, pse, k),
                pause,
                #activator acknowledges rprt and signs off
                seq(alt(r, 60, esc("VE_IDX"), # r fb <ve>
                        135), repeat, #ok
                    alt(203, 204,206), #tnx, tks, tu
                    3, de, call1),#73
                ####alt(
                    seq(alt(55, # ee
                            seq(ar, k),),
                        pause,
                        sota_chaser_says_bye,
                        pause,
                        # If the chaser sends TU 73 etc the activato
                        # r may send a final final, pause and then call  briefly in
                        # anticipation of a pile up.
                        seq(55, cq, 216, de, call1, k),
                        pause,
                ),
                #     # activator ends activation.
                #     seq(esc("SK_IDX"),39,#cl
                #         pause,
                #         sota_chaser_says_bye,
                #         pause
                #     )
                # ),
            ),
            ####### contest qso
            seq(prob(0.5, cq), 201, call1, call1, k, #test
                pause,
                call2, k,
                pause,
                call2, esc("RST_MAGIC"), esc("NUM3_MAGIC"), k,
                pause,
                r, prob(0.5, ur), esc("RST_MAGIC"), esc("NUM3_MAGIC"), k,
                pause,
                alt(203, 204,206), #tnx, tks, tu
                201, call1, call1 #test
                )
        ),
    )

    dbgqso, dbgMixHistoryMaxLength = seq(es,0,1,
                  esc("CALL1_MAGIC"),
                  esc("CALL2_MAGIC"),
                  # esc("NAME1_MAGIC"),
                  # esc("NAME2_MAGIC"),
                  # esc("RANDCALL_MAGIC"),
                  # esc("RANDCALL_MAGIC"),
                  # esc("RST_MAGIC"),
                  # esc("REPEAT_MAGIC"),
                  # esc("PAUSE_MAGIC"),
                  # esc("NUM1_MAGIC"),
                  # esc("NUM2_MAGIC"),
                  # esc("NUM3_MAGIC"),
                  # esc("GROUPOF5_MAGIC"),
                  # esc("LOCATOR_MAGIC"),
                  # esc("DOK_MAGIC"),
                  # esc("AS_IDX"),
                  # esc("SK_IDX"),
    )

    toobig = [elem for elem in qso if isinstance(elem, int) and elem>239]
    if (len(toobig) > 0):
        print(toobig)
        raise ValueError("There are values outside the abbrev index range.")
    with open("qsotree.h","w") as f:
        print("#ifndef QSOTREE_H",file=f)
        print("#define QSOTREE_H",file=f)
        print("const byte qsotree[%d] PROGMEM = {" % len(qso),file=f)
        print("#ifdef DEBUG",file=f)
        print(", ".join([str(elem) for elem in dbgqso]),file=f)
        print("#else",file=f)
        print(", ".join([str(elem) for elem in qso]),file=f)
        print("#endif",file=f)
        print("};",file=f)
        print("// a list which will contain the indices of the mix elements ",file=f)
        print("// which have been printed already.",file=f)
        print("#ifdef DEBUG",file=f)
        print("uint16_t mixHistory[%d];" % dbgMixHistoryMaxLength,file=f)
        print("#else",file=f)
        print("uint16_t mixHistory[%d];" % mixHistoryMaxLength,file=f)
        print("#endif",file=f)
        print("#endif",file=f)
